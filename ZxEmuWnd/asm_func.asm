
.xmm
.model flat

.code
_asm_example proc
		xor eax, eax
		mov al, 79h
		mov bl, 35h
		add al, bl

		setc ch
		seta bh

		mov cl, al
		mov bl, 0

		mov dl, al
		and dl, 15

		cmp bh, 1
		jz @f
		cmp dl, 9
		jbe _1
@@:		add al, 6
		mov bh, 1
		cmp al, 255
		setc ah
		mov bl, ch
		or bl, ah
		jmp @f
_1:		mov bh, 0
@@:		cmp ch, 1
		jz @f
		cmp cl, 99h
		jbe _2
@@:		add al, 60h
		mov bl, 1
		jmp @f
_2:		mov bl, 0
@@:





		sub al, 36
		xor eax, eax
		mov al, 50
		sub al, 116

		xor eax, eax
		mov al, 128
		sub al, 127
		xor al, 129

		mov ax, 32768
		mov bx, 32768
		sub ax, bx
		adc ax, 0
		mov al, 128
		sub al, 1
		mov al, 1
		sub al, 2
		add al, 2
		mov al, 127
		inc al
		dec al
		add al, 127
@@:		add al, 1
		jnz @b
		ret
_asm_example endp

end
