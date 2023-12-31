; $Header: RCS/fastknj.asm 1.1 91/08/06 18:52:41 RayMan Exp $
; Modified by Shinra 1996/12/22, 2023/09/18

TVRAM	equ	0a000h

argm	struc
	retadr	dw	?
	x	dw	?
	y	dw	?
	str	dw	?
argm	ends

	.186
	.model	small
	.code

	public	_fast_kanji

;-----------------------------------------------
; ����(�����Ή�)������\��(�A�g���r���[�g�Ȃ�)
;
; void fast_kanji (int x, int y, char *str);
;
; �I���W�i��: �s������
; �ڐA�E����: �q�����l����
;_______________________________________________
_fast_kanji	proc	near
	mov	bx, sp
	push	si
	push	di
	
	mov	al, byte ptr [bx].y
	mov	cl, 80
	mul	cl
	mov	di, [bx].x
	add	di, ax
	shl	di, 1
	mov	si, [bx].str
	
	mov	ax, TVRAM
	mov	es, ax
	;mov	ah, al		; ah=0	; --�C�� �폜
	mov	bx, 0a080h
	mov	cx, 1fa1h
	mov	dx, 7f7fh
even
getchar:
	xor	ax, ax		; --�C�� �ǉ�
	lodsb
	or	al, al	; EOL
	jz	done
	cmp	al, bl	; 80h
	jb	ankput
	cmp	al, 0e0h
	jae	kanji
	cmp	al, bh	; 0a0h
	jae	ankput
kanji:
	mov	ah, al
	lodsb
	shl	ah, 1
	sub	al, ch
	js	_F
	cmp	al, 61h
	adc	al, 0deh
_F:
	add	ax, cx
	and	ax, dx
	xchg	ah, al
kanjiput:
	sub	al, 20h
	stosw			; �����E����
	xor	ah, 80h		; --�C�� xor	ax, ax
ankput:
	stosw			; ����������
	jmp	short getchar
even
done:
	pop	di
	pop	si
	ret
_fast_kanji	endp

	end
