/*
	キーボード入力チェックツール
*/

#include <dir.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <textva.h>	/* s88va250 */
#include <grphva.h>	/* s88va250 */
#include <mouseva.h>	/* s88va250 */
#include <kybdva.h>	/* s88va250 */

void	_Cdecl	fast_ank (int x, int y, char *str);

#define VERSION "v0.2"

#define KEY_COUNT_TO_EXIT 5     /* 2つのキーを交互に5回入力すると終了 */

/* I/O port */
#define ICU_MASTER_PORT_0	0x188
#define ICU_MASTER_PORT_1	0x18a
#define SCAN_CODE_PORT		0x1c1

/* 表示位置 */
#define CUR_SCAN_Y    0
#define REMAIN_Y      1
#define STATUS_AREA_X 2
#define STATUS_AREA_Y 4
#define STATUS_WIDTH  9
#define LEGEND_X     60
#define LEGEND_Y      0
#define LEGEND2_X    36
#define LEGEND2_Y     0
#define NOTE_X        0
#define NOTE_Y       20

/* 表示アトリビュート */
#define TESTED_ATTR 0x70	/* color 7, normal */
#define TBD_ATTR 0x74		/* color 7, reverse */

/* キー関連 */
#define SCANCODES  0x80
#define NOT_KEY    0x7f		/* 該当するキーがないことを表す仮想のｷｰｺｰﾄﾞ */

/* sc_statuses[code] */
#define PRESS_OK   0x01
#define RELEASE_OK 0x02
#define NOT_USED   0x80



static BYTE scan_code=0;
static int  scan_count=0;

static BYTE sc_statuses[SCANCODES];
static char *sc_symbols[SCANCODES] = {
/* 00*/ "ESC  ", "1    ", "2    ", "3    ", "4    ", "5    ", "6    ", "7    ",
/* 08*/ "8    ", "9    ", "0    ", "-=   ", "^    ", "\\|   ","BS   ", "TAB  ",
/* 10*/ "Q    ", "W    ", "E    ", "R    ", "T    ", "Y    ", "U    ", "I    ",
/* 18*/ "O    ", "P    ", "@~   ", "[{   ", "RETRN", "A    ", "S    ", "D    ",
/* 20*/ "F    ", "G    ", "H    ", "J    ", "K    ", "L    ", ";+   ", ":*   ",
/* 28*/ "]}   ", "Z    ", "X    ", "C    ", "V    ", "B    ", "N    ", "M    ",
/* 30*/ ",<   ", ".>   ", "/?   ", "_    ", "SPACE", "変換 ", "ROLUP", "ROLDN",
/* 38*/ "INS  ", "DEL  ", "↑   ", "←   ", "→   ", "↓   ", "CLR  ", "HELP ",
/* 40*/ "TK - ", "TK / ", "TK 7 ", "TK 8 ", "TK 9 ", "TK * ", "TK 4 ", "TK 5 ",
/* 48*/ "TK 6 ", "TK + ", "TK 1 ", "TK 2 ", "TK 3 ", "TK = ", "TK 0 ", "TK , ",
/* 50*/ "TK . ", "決定 ", "     ", "     ", "     ", "     ", "     ", "     ",
/* 58*/ "     ", "     ", "     ", "     ", "     ", "     ", "     ", "     ",
/* 60*/ "STOP ", "COPY ", "f.1  ", "f.2  ", "f.3  ", "f.4  ", "f.5  ", "f.6  ",
/* 68*/ "f.7  ", "f.8  ", "f.9  ", "f.10 ", "     ", "     ", "     ", "     ",
/* 70*/ "左SFT", "CAPS ", "かな ", "GRPH ", "CTRL ", "     ", "     ", "     ",
/* 78*/ "右SFT", "TK RT", "PC   ", "全角 ", "     ", "     ", "     ", "     ",
};


static BOOL is_tested(int code)
{
	return (sc_statuses[code] & (PRESS_OK | RELEASE_OK)) 
		== (PRESS_OK | RELEASE_OK);
}

static void init_statuses()
{
	int code;
	int x,y;

	for (code=0; code<SCANCODES; code++) {
		sc_statuses[code]=
			strcmp(sc_symbols[code],"     ")==0 ? NOT_USED : 0;
	}
}


/*

012345678

左SFT||
-----   キー名
     -  押下確認
      - 開放確認

*/

static void code_to_pos(BYTE code, int *x, int *y)
{
	*x=STATUS_AREA_X+(code/16)*STATUS_WIDTH;
	*y=STATUS_AREA_Y+(code%16);
}

static void draw_keys()
{
	int code;
	int x,y;

	for (code=0; code<SCANCODES; code++) {
		if (! (sc_statuses[code] & NOT_USED)) {
			code_to_pos(code,&x,&y);
			fast_kanji(x, y, sc_symbols[code]);
			/* fast_kanji(x+5, y, "\x1f\x1e"); */ /* "↓↑" */
		}
	}
}

static void draw_status(BYTE code)
{
	int x,y;
	int attr;

	code_to_pos(code,&x,&y);

	locate(x, y);
	attr = is_tested(code) ? TESTED_ATTR : TBD_ATTR;
	set_atrn(attr, 7);

	if (sc_statuses[code] & PRESS_OK) {
		fast_kanji(x+5, y, "\x1f"); /* ↓ */
	}
	if (sc_statuses[code] & RELEASE_OK) {
		fast_kanji(x+6, y, "\x1e"); /* ↑ */
	}
}

static void draw_cur_scan_code(BYTE code, int pressed)
{
	locate(14,CUR_SCAN_Y);
	wstr("[%s] (%02Xh) %s", sc_symbols[code], code, pressed ? "on " : "off");
}


static int cur_scan_elapse=0;

static void draw_cur_scan_elapse()
{
	char *str=" ";
	int level;
	
	if (cur_scan_elapse>0) {
		cur_scan_elapse--;
	}

	level=cur_scan_elapse>>9;
	str[0]= level==0 ? ' ' : (0x80 + level); 
		/* 0x81～0x87 グラフィックキャラクタ 罫線文字 */

	fast_ank(32,CUR_SCAN_Y,str);
}

static void reset_cur_scan_elapse()
{
	cur_scan_elapse=0xfff;
}

static void draw_remain()
{
	int remain=0;
	int code;

	for (code=0; code<SCANCODES; code++) {
		if (! (sc_statuses[code] & NOT_USED)) {
			if (! is_tested(code)) {
				remain++;
			}
		}
	}
	locate(14,REMAIN_Y);
	wstr("残り %3d", remain);
}

static void draw()
{
	BYTE code;
	int x,y;

	draw_keys();

	for (code=0; code<SCANCODES; code++) {
		if (! (sc_statuses[code] & NOT_USED)) {
			draw_status(code);
		}
	}

	for (y=0; y<16; y++) {
		locate(0, STATUS_AREA_Y+y);
		wstr("%1X", y);
	}
	for (x=0; x<8; x++) {
		locate(STATUS_AREA_X+(x*STATUS_WIDTH), STATUS_AREA_Y-1);
		wstr("%1X", x);
	}
	
	fast_kanji(0,CUR_SCAN_Y, "入力したキー: ");
	fast_kanji(0,REMAIN_Y,   "未確認キー　: ");

	fast_kanji(LEGEND_X, LEGEND_Y, "白反転: 確認未完了");
	fast_kanji(LEGEND_X, LEGEND_Y+1, "\x1f     : 押下を確認"); /* ↓ */
	fast_kanji(LEGEND_X, LEGEND_Y+2, "\x1e     : 開放を確認"); /* ↑ */

	locate(LEGEND2_X,LEGEND2_Y+1);
	_wstr("2つのキーを交互に");
	locate(LEGEND2_X,LEGEND2_Y+2);
	wstr("%d回入力で終了",KEY_COUNT_TO_EXIT);

	fast_kanji(NOTE_X, NOTE_Y,  
	  "備考: リピートしないキー: SFT, CTRL, GRPH, 全角, PC, COPY, INS, f.*, CAPS, かな");
	fast_kanji(NOTE_X, NOTE_Y+1, 
	  "      トグルキー:         CAPS, かな");

	draw_remain();
}


static int old_atr_mode;

static void init_screen()
{
	old_atr_mode=get_atr(0); /* モード取得 */
	system_line(OFF);
	set_atr(1, 1); /* モード = 1 (foreground color + attributes) */
	set_atr(0, TESTED_ATTR);
	cls();
	c_off(); /* カーソル off */
}

static void restore_screen()
{
	system_line(ON);
	set_atr(1, old_atr_mode);
	cls();
}


static BYTE prev_exit_check_code;
static BYTE prev_prev_exit_check_code;
static int exit_check_count;

static int init_exit_check()
{
	prev_exit_check_code=NOT_KEY;
	prev_prev_exit_check_code=NOT_KEY;
	exit_check_count=1;
}

static void check_exit(BYTE code, int pressed)
{
	if (pressed) {
		return;
	}

	if (
		code != prev_exit_check_code && 
		code == prev_prev_exit_check_code
	) {
		exit_check_count++;
	}
	else {
		exit_check_count=1;
	}
	prev_prev_exit_check_code=prev_exit_check_code;
	prev_exit_check_code=code;
}

static int should_exit()
{
	return exit_check_count >= ((KEY_COUNT_TO_EXIT-1)*2+1);
}

static void main_loop()
{
	BYTE cur_scan_code;
	int cur_scan_count;
	BYTE code;
	int pressed;
	int last_scan_count=scan_count;
	
	init_statuses();
	init_exit_check();

	draw();

	while(! should_exit()) {
		do {
			draw_cur_scan_elapse();
			cur_scan_count=scan_count;
			cur_scan_code=scan_code;
		} while(cur_scan_count==last_scan_count);
		last_scan_count=cur_scan_count;
		
		code=cur_scan_code & 0x7f;
		pressed=(cur_scan_code & 0x80)==0;

		reset_cur_scan_elapse();
		check_exit(code,pressed);

		if (pressed) {
			sc_statuses[code] |= PRESS_OK;
		}
		else {
			sc_statuses[code] |= RELEASE_OK;
		}
		draw_status(code);
		draw_cur_scan_code(code, pressed);
		draw_remain();
	}
}

static void interrupt(*old_int09_vect)();


static void interrupt keyboard_interrupt()
{
	BYTE code;

	/* 割り込み終了 */
	outportb(ICU_MASTER_PORT_0, 0x20); 

	scan_code=inportb(SCAN_CODE_PORT);
	scan_count++;
}

static void install()
{
	disable();
	old_int09_vect=getvect(0x09);
	setvect(0x09,keyboard_interrupt);
	enable();
}
static void uninstall()
{
	disable();
	setvect(0x09,old_int09_vect);
	enable();
}

static void usage()
{
	_wstr("キーボード入力チェックツール ");
	_wstr(VERSION);
	_wstr("  for PC-88VA\r\n");
	_wstr("By Shinra  2023\r\n");
	_wstr("\r\n");
	_wstr("引数なしで実行してください。\r\n");
	 wstr("2つのキーを交互に%d回入力すると終了します。\r\n", 
	 	KEY_COUNT_TO_EXIT);
}

static void wait_a_while()
{
	uint i;
	for (i=0; i<60000; i++) {
	}
}


void main(int argc, char *argv[])
{
	int i;

	if (argc > 1) {
		usage();
		return;
	}

	init_screen();
	wait_a_while();

	install();

	main_loop();

	uninstall();

	restore_screen();
}


