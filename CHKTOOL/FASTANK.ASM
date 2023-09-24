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

	public	_fast_ank

;-----------------------------------------------
; 高速(ANK限定)文字列表示(アトリビュートなし)
;
; void fast_ank (int x, int y, char *str);
;
; オリジナル: Ｔａｋａ, ＲａｙＭａｎ
;_______________________________________________
_fast_ank	proc	near
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
	mov	ah, al
even
getchar:
	lodsb
	or	al, al	; EOL
	jz	done
putank:
	stosw
	jmp	short getchar
even
done:
	pop	di
	pop	si
	ret
_fast_ank	endp

	end
