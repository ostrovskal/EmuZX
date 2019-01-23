BWT struct
		; буфер индексов слов
		_RT		dq ?
		; буфер индексов букв
		_LT		dq ?
		; указатель на буфер индексов
		_index	dq ?
		; указатель на текущий блок трансформации
		_result	dq ?
		; входной буфер
		_in		dq ?
BWT ends

bwt_get_val macro
		dec rax
		cmp rax, rdx
		cmovge rax, r14
		movzx rax, byte ptr [rax + rdi]
endm

bwt_set_val macro
LOCAL _no
		cmp r10, 1
		cmovz rbx, r12
		lea r15, [rdx - 2]
		lea rax, [r10 + r15]
		cmp rax, rdx
		cmovge rax, r15
		movzx rax, byte ptr [rax + rdi]
		mov [rsi + r12], al
		inc r12
endm

extern malloc:near
extern MultiByteToWideChar:near

.data?

result		dw 128 dup(?)

.data
dbl_znak	dq 1.0, -1.0
			dq 1.0
			dq 10.0
			dq 100.0
			dq 1000.0
			dq 10000.0
			dq 100000.0
			dq 1000000.0
			dq 10000000.0
			dq 100000000.0
			dq 1000000000.0
			dq 10000000000.0
			dq 100000000000.0
hex_sym		dw 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70
is_valid	dw 0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
base64_chars	dw 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90
				dw 97, 98, 99, 100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122
				dw 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 43, 47, 0

.code

; 0 - register(offs), 1 - bit check, 2 - bit set 
asm_ssh_capability proc
		push rbx
		push r10
		xor r10, r10
		mov r9, offset result
		mov r8, offset cpu_caps_1
		mov rax, 1
		xor rcx, rcx
		call _cset
		mov rax, 7
		xor rcx, rcx
		mov r8, offset cpu_caps_7
		call _cset
		mov rax, r10
		pop r10
		pop rbx
		ret
_cset:	cpuid
		mov [r9 + 00], rax
		mov [r9 + 08], rbx
		mov [r9 + 16], rcx
		mov [r9 + 24], rdx
@@:		cmp dword ptr [r8], 0
		jz @f
		movzx rax, byte ptr [r8 + 00]		; offset
		movzx rcx, byte ptr [r8 + 01]		; bit check
		movzx rdx, byte ptr [r8 + 02]		; bit set
		add r8, 4
		mov rax, [r9 + rax * 2]
		bt rax, rcx
		jnc @b
		bts r10, rdx
		jmp @b
@@:		ret
cpu_caps_1	db 8, 0, 4, 0, 8, 1, 8, 0, 8, 9, 5, 0, 8, 12, 9, 0, 8, 13, 10, 0, 8, 19, 6, 0
			db 8, 20, 7, 0, 8, 22, 11, 0, 8, 23, 12, 0, 8, 25, 13, 0, 8, 28, 14, 0, 8, 29, 25, 0, 8, 30, 15, 0
			db 12, 8, 31, 0, 12, 11, 31, 0, 12, 15, 16, 0, 12, 19, 31, 0, 12, 23, 1, 0, 12, 24, 31, 0, 12, 25, 2, 0, 12, 26, 3, 0, 0, 0, 0, 0
cpu_caps_7	db 4, 3, 17, 0, 4, 5, 18, 0, 4, 8, 19, 0, 4, 16, 20, 0, 4, 18, 21, 0, 4, 26, 22, 0, 4, 27, 23, 0, 4, 28, 24, 0, 4, 29, 31, 0, 0, 0, 0, 0
asm_ssh_capability endp

;rcx = buf, rdx = count
asm_ssh_to_base64 proc public
		push rsi
		push rdi
		push rbx
		push r12
		mov r8, 3
		mov rax, rdx
		mov r12, rax				; src_len
		mov rsi, rcx				; src_buf
		xor rdx, rdx
		div r8
		test rdx, rdx
		setnz dl
		lea rcx, [rax + rdx + 2]
		shl rcx, 3					; len_ret
		call malloc
		mov qword ptr [rax], 0
		lea rdi, [rax + 8]			; buf_ret
		push rdi
		mov r8, offset base64_chars
		mov r9, offset result		; char_array_3
_loop:	xor rdx, rdx				; i = 0
@@:		test r12, r12
		jz @f
		dec r12
		lodsb
		mov [r9 + rdx], al
		inc rdx
		cmp rdx, 3
		jnz @b
		call _sub
		jmp _loop
@@:		test rdx, rdx
		jz @f
		mov dword ptr [r9 + rdx], 0
		mov r12, 3
		sub r12, rdx
		call _sub
		sub rdi, r12
		sub rdi, r12
		mov ax, '='
		mov rcx, r12
		rep stosw
@@:		mov word ptr [rdi], 0
		pop rax
		pop r12
		pop rbx
		pop rdi
		pop rsi
		ret
_sub:	movzx rcx, byte ptr [r9 + 00]
		movzx rdx, byte ptr [r9 + 01]
		movzx rbx, byte ptr [r9 + 02]
		mov rax, rcx
		and rax, 0fch
		shr rax, 2
		mov ax, [r8 + rax * 2]
		stosw
		mov rax, rdx
		and rcx, 3
		and rax, 0f0h
		shl rcx, 4
		shr rax, 4
		add rax, rcx
		mov ax, [r8 + rax * 2]
		stosw
		mov rax, rbx
		and rdx, 15
		and rax, 0c0h
		shl rdx, 2
		shr rax, 6
		add rax, rdx
		mov ax, [r8 + rax * 2]
		stosw
		and rbx, 3fh
		mov ax, [r8 + rbx * 2]
		stosw
		ret
asm_ssh_to_base64 endp

; rcx - str, rdx - length, r8 - len_buf
asm_ssh_from_base64 proc
		push rbx
		push rsi
		push rdi
		push r10
		push r12
		mov rsi, rcx			; src
		mov r12, rdx
		mov r10, rsi
		mov rcx, 3dh
		call _wcschr
		cmovc r12, rbx			; in_len
		mov rax, rdx
		shr rax, 2
		imul rax, 3
		sub rdx, r12
		sub rax, rdx			; out_len
		mov [r8], rax
		lea rcx, [rax + 14]		; 8 + 6 корекция на выход за пределы
		call malloc
		mov qword ptr [rax], 0
		lea rdi, [rax + 8]		; out_buf
		push rdi
		mov r10, offset base64_chars
		mov r9, offset result	; char_array_4
_loop:	xor rdx, rdx			; i = 0
@@:		test r12, r12
		jz @f
		dec r12
		lodsw
		mov cx, ax
		call _wcschr
		jnc @f
		mov [r9 + rdx * 2], bx
		inc rdx
		cmp rdx, 4
		jnz @b
		call _sub
		jmp _loop
@@:		test rdx, rdx
		jz @f
		mov qword ptr [r9 + rdx * 2], 0
		call _sub
@@:		mov word ptr [rdi], 0
		pop rax
		pop r12
		pop r10
		pop rdi
		pop rsi
		pop rbx
		ret
_sub:	mov rbx, rdx
		movzx rcx, byte ptr [r9 + 00]
		shl rcx, 2
		movzx rdx, byte ptr [r9 + 02]
		mov rax, rdx
		and rax, 30h
		shr rax, 4
		add rax, rcx
		stosb
		dec rbx
		jle @f
		movzx rcx, byte ptr [r9 + 04]
		and rdx, 15
		shl rdx, 4
		mov rax, rcx
		and rax, 3ch
		shr rax, 2
		add rax, rdx
		stosb
		dec rbx
		jle @f
		mov rax, rcx
		and rax, 3
		shl rax, 6
		add al, [r9 + 06]
		stosb
@@:		ret
_wcschr:mov rbx, -1
@@:		inc rbx
		mov ax, [r10 + rbx * 2]
		test ax, ax
		jz @f
		cmp ax, cx
		jnz @b
		stc
@@:		ret
asm_ssh_from_base64 endp

; преобразовать число в строку, в зависимости от системы счисления 0-decimal, (1-bin, 2-oct, 3-hex, 4-double,5-float,6-bool)
; rcx - указатель на число
; rdx - radix
; r8 - ptr end
; rax - result of whar_t*
asm_ssh_ntow proc public
		push r10
		push r11
		push r12
		push r13
		mov r9, offset result + 128
		mov word ptr [r9], 0
		jrcxz @f
		mov r13, r9
		mov r12, r8
		mov rax, [rcx]
		imul rdx, 24
		mov r10, offset tbl_radix
		add rdx, r10
		mov r8, offset hex_sym
		mov r10, [rdx + 8]
		mov rcx, [rdx + 16]
		call qword ptr [rdx]
		test r12, r12
		jz @f
		mov [r12], r13
@@:		mov rax, r9
		pop r13
		pop r12
		pop r11
		pop r10
		ret
str_bool db 'f', 0, 'a', 0, 'l', 0, 's', 0, 'e', 0, 0, 0, 't', 0, 'r', 0, 'u', 0, 'e', 0, 0, 0, 0, 0
tbl_radix dq nto_dec, 0, 10, nto_ohb, 1, 1, nto_ohb, 7, 3, nto_ohb, 15, 4, nto_dbl, 0, 10, nto_flt, 0, 10, nto_bool, offset str_bool, 12
nto_bool:
		test rax, rax
		cmovnz rax, rcx
		mov rdx, [rax + r10]
		mov eax, [rax + r10 + 8]
		mov [r9], rdx
		mov [r9 + 8], eax
		lea r13, [r9 + 12]
		ret
nto_ohb:sub r9, 2
		mov rdx, rax
		and rdx, r10
		movzx rdx, word ptr [rdx * 2 + r8]
		mov [r9], dx
		shr rax, cl
		jnz nto_ohb
		ret
nto_dec:; определяем знак
		sub r9, 2
		test rax, rax
		jns @f
		mov r10w, '-'
		neg rax
@@:		cmp rax, rcx
		jb @f
		xor rdx, rdx
		div rcx
		or dl, 48
		mov word ptr [r9], dx
		sub r9, 2
		jmp @b
@@:		or al, 48
		mov word ptr [r9], ax
		test r10, r10
		jz @f
		sub r9, 2
		mov [r9], r10w
@@:		ret
nto_flt:movd xmm0, rax
		mov r11, 4
		cvtss2sd xmm0, xmm0
		jmp @f
nto_dbl:; read double from rax
		mov r11, 8
		movd xmm0, rax
		; отбросим дробную часть
@@:		roundsd xmm1, xmm0, 3
		cvttsd2si rax, xmm1
		; преобразуем целую часть в строку
		call nto_dec
		mov rdx, offset result + 128
		mov r8, offset dbl_znak + 24
		mov word ptr [rdx], '.'
		add rdx, 2
		; количество знаков после запятой
		mov rcx, r11
		mov r10, rdx
@@:		; дробная часть
		subsd xmm0, xmm1
		mulsd xmm0, qword ptr [r8]
		roundsd xmm1, xmm0, 3
		cvttsd2si rax, xmm1
		test rax, rax
		cmovnz r10, rdx
		add rax, 48
		mov [rdx], ax
		add rdx, 2
		loop @b
		lea r13, [r10 + 2]
		mov [r13], cx
		ret
asm_ssh_ntow endp

; распарсить спецификатор wprintf
; rcx - val
; rdx - указатель на ptr
; ret - result of whar_t*
asm_ssh_parse_spec proc public
LOCAL @@end:QWORD
		jrcxz _exit
		test rdx, rdx
		jz _exit
		push rdi
		push r12
		mov r10, rcx
		mov r11, rdx
		mov rcx, [rdx]
		cmp word ptr [rcx], '*'
		jnz @f
		lea r12, [rcx + 2]
		mov rcx, [r10]
		cvtsi2sd xmm0, dword ptr [rcx]
		add qword ptr [r10], 8
		jmp _next
@@:		mov rdx, 4
		lea r8, @@end
		call asm_ssh_wton
		movsd xmm0, qword ptr [rax]
		mov r12, @@end
_next:	mov ax, [r12]
		mov rcx, 13
		call f_spec
		jnc @f
		add r12, 2
		call qword ptr [rax + rcx + 8]
@@:		mov [r11], r12
		test rax, rax
		jnz @f
		mov rax, offset result
		mov word ptr [rax], 0
@@:		add qword ptr [r10], 8
		pop r12
		pop rdi
_exit:	ret
OPTION EPILOGUE:NONE
sp_ii:	cmp dword ptr [r12], 00340036h	; I64
		jnz sp_err
		mov ax, [r12 + 4]
		mov rcx, 5
		call f_spec
		jnc sp_err
		add r12, 6
		jmp sp_pp
sp_x:
sp_b:
sp_o:
sp_i:	mov rax, [r10]
		mov ecx, [rax]
		mov [rax], rcx
sp_pp:	mov rcx, [r10]
		xor r8, r8
		call asm_ssh_ntow
		mov rdi, rax
		mov r9, rax
		mov rcx, offset result + 128
		sub rax, rcx
		neg rax
		shr rax, 1				; length number
		cvttsd2si rcx, xmm0		; length spec number
		sub rcx, rax
		jle @f
		sub rdi, rcx
		sub rdi, rcx
		mov r9, rdi
		mov ax, '0'
		rep stosw
@@:		mov rax, r9
		ret
sp_err:	ret
sp_f:	mov rcx, 10
@@:		roundsd xmm1, xmm0, 3
		subsd xmm0, xmm1
		mulsd xmm0, qword ptr [dbl_znak + 24]
		cvttsd2si rax, xmm0
		test rax, rax
		loopz @b
		push rax
		mov rcx, [r10]
		lea r8, @@end
		call asm_ssh_ntow
		mov r9, rax
		mov rax, offset result + 130
		mov rdi, @@end
		sub rax, rdi
		neg rax
		shr rax, 1				; length number
		pop rcx					; length spec number
		sub rcx, rax
		jle @f
		mov ax, '0'
		rep stosw
@@:		mov [rdi], ah
		mov rax, r9
		ret
sp_cc:	sub rsp, 48
		mov r8, [r10]
		mov r9, 1
		xor rcx, rcx
		xor rdx, rdx
		mov rax, offset result
		mov [rax + r9 * 2], dx
		mov qword ptr [rsp + 32], rax
		mov qword ptr [rsp + 40], r9
		call MultiByteToWideChar
		add rsp, 48
		mov rax, offset result
		ret
sp_c:	mov rax, offset result
		mov rcx, [r10]
		movzx rcx, word ptr [rcx]
		mov [rax], rcx
		ret
strlen:	xor rax, rax
@@:		inc rax
		cmp byte ptr [r8 + rax], 0
		jnz @b
		ret
sp_ss:	mov r8, [r10]
		mov r8, [r8]
		call strlen
		sub rsp, 48
		mov r9, rax
		xor rcx, rcx
		xor rdx, rdx
		mov rax, offset result
		mov [rax + r9 * 2], dx
		mov qword ptr [rsp + 32], rax
		mov qword ptr [rsp + 40], r9
		call MultiByteToWideChar
		add rsp, 48
		mov rax, offset result
		ret
sp_s:	mov rax, [r10]
		mov rax, [rax]
		ret
sp_z:	mov rcx, [r10]
		mov rcx, [rcx]
		cmp dword ptr [rcx + rdx], 18
		mov rax, rcx
		cmovg rax, [rcx]
		ret
sp_p:	mov rax, 16
		movsd xmm0, xmm1
		cvtsi2sd xmm0, rax
		jmp sp_pp
f_spec:	mov rdi, offset _spec
		lea rdx, [rcx - 1]
		repnz scasb
		mov rax, rcx
		jnz @f
		sub rcx, rdx
		neg rcx
		mov rax, offset _spec_tbl
		shl rcx, 4
		mov rdx, [rax + rcx]
		stc
		ret
@@:		clc
		ret
OPTION EPILOGUE:EPILOGUEDEF
align 16
_spec	db 'boIixCcSsfzp', 0, 0, 0, 0
_spec_tbl dq 1, sp_b, 2, sp_o, 0, sp_ii, 0, sp_i, 3, sp_x, 0, sp_cc, 0, sp_c, 0, sp_ss, 0, sp_s, 4, sp_f, 40, sp_z, 3, sp_p, 0, 0
asm_ssh_parse_spec endp

; преобразовать строку в число в зависимости от системы счисления
; rcx - ptr
; rdx - radix
; r8 -  &ptr end
asm_ssh_wton proc public
		push rbx
		push r10
		push r11
		push r12
		push r13
		push r14
		mov r14, r8
		xor rax, rax
		jrcxz @f
		mov r10, rcx
		xor r13, r13
		mov r9, offset radix
		imul rdx, 24
		add rdx, r9
		mov r9, [rdx + 8]; множитель порядка
		mov r8, [rdx + 16]; маска
		call qword ptr [rdx]
@@:		mov qword ptr [result], rax
		test r14, r14
		jz @f
		mov [r14], r11
@@:		pop r14
		pop r13
		pop r12
		pop r11
		pop r10
		pop rbx
		mov rax, offset result
		ret
_BIN	= 1
_OCT	= 2
_HEX	= 4
_DEC	= 8
tbl_hex dw 0, _HEX, _HEX, _HEX, _HEX, _HEX, _HEX, 0, 0, 0, 0, 0, 0, 0, 0, 0, _DEC + _BIN + _OCT + _HEX, _DEC + _BIN + _OCT + _HEX, _DEC + _OCT + _HEX, _DEC + _OCT + _HEX, _DEC + _OCT + _HEX
		dw _DEC + _OCT + _HEX, _DEC + _OCT + _HEX, _DEC + _OCT + _HEX, _DEC + _HEX, _DEC + _HEX
radix	dq wto_dec, 10, _DEC, wto_obh, 2, _BIN, wto_obh, 8, _OCT, wto_obh, 16, _HEX, wto_dbl, 10, _DEC, wto_flt, 10, _DEC, wto_bool, 0, 0
wto_bool:
		xor rax, rax
		cmp dword ptr [rcx], 00720074h
		setz al
		cmp dword ptr [rcx + 4], 00650075h
		setz ah
		and al, ah
		movzx rax, al
		ret
wto_obh:sub rcx, 2
		mov rdx, offset tbl_hex
@@:		add rcx, 2
		movzx rax, word ptr [rcx]
		and rax, -33
		cmp rax, 'F'
		ja @f
		cmp rax, 16
		jb @f
		and rax, 00011111b
		movzx rax, word ptr [rax * 2 + rdx]
		and rax, r8
		jnz @b
@@:		mov r11, rcx
		mov r8, 1
		xor rax, rax
		mov rdx, offset is_valid
@@:		sub rcx, 2
		cmp rcx, r10
		jb @f
		movzx rbx, word ptr [rcx]
		and rbx, -97
		movzx rbx, word ptr [rbx * 2 + rdx]
		imul rbx, r8
		add rax, rbx
		imul r8, r9
		jmp @b
@@:		ret
wto_dec:; определяем признак отрицательного значения
		xor r12, r12
		cmp word ptr [rcx], '-'
		setz r12b
		lea rcx, [rcx + r12 * 2]
		call wto_obh
		test r12, r12
		jz @f
		neg rax
@@:		ret
wto_flt:inc r13
wto_dbl:call wto_dec
		mov r10, r11
		cvtsi2sd xmm0, rax
		xorps xmm1, xmm1
		cmp word ptr [r11], '.'
		jnz @f
		mov r8, 8
		lea rcx, [r11 + 2]
		mov r10, rcx
		call wto_obh
		cvtsi2sd xmm1, rax
@@:		mov r9, r11
		sub r9, r10
		mov rcx, 22
		cmp r9, rcx
		cmova r9, rcx
		mov r8, offset dbl_znak
		shl r12, 1
		divsd xmm1, qword ptr [r8 + r9 * 4 + 16]
		addsd xmm0, xmm1
		mulsd xmm0, qword ptr [r8 + r12 * 8]
		test r13, r13
		jz @f
		cvtsd2ss xmm0, xmm0
@@:		movd rax, xmm0
		ret
asm_ssh_wton endp

;rcx - src, rdx - vec
;<[/]tag [attr = value ...][/]>
;vec - 0 - <[/], 1- tag, 2 - [/]>, 3.... - attrs
xml_chars	db 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
			db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2, 0, 0, 1
			db 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1
			db 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
asm_ssh_parse_xml proc USES rbx r10 r11 r12
		mov r11, 002d002d0021003ch
		mov r12, 003e002d002d0020h
		xor r8, r8					; cur src
		xor rbx, rbx				; count attrs
		call _skip_c
		test rax, rax
		jz _fin
		cmp rax, '<'
		jnz _err1
		mov r10, offset xml_chars
		lea r9, [r8 + 1]
		cmp word ptr [rcx + r9 * 2], '/'
		jnz @f
		inc r9
@@:		mov rax, 1
		call _set_v
		mov r8, r9
		call _skip_s
		; tag
		call _word
		mov rax, 2
		call _set_v
		mov r8, r9
@@:		call _skip_s
		cmp rax, '/'
		jz @f
		cmp rax, '>'
		jz @f
		; attrs - name
		call _word
		lea rax, [rbx + 5]
		call _set_v
		mov r8, r9
		call _skip_s
		cmp rax, '='
		jnz _err
		inc r8
		call _skip_s
		; attr - value
		call _str
		jnc _err1
		inc r8
		lea rax, [rbx + 6]
		call _set_v
		lea r8, [r9 + 1]
		add rbx, 2
		jmp @b
@@:		lea r9, [r8 + 1]
		cmp rax, '/'
		jnz @f
		cmp word ptr [rcx + r9 * 2], '>'
		jnz _err1
		inc r9
@@:		mov rax, 3
		call _set_v
		; value tag
		mov r8, r9
		call _skip_c
		call _str
		jnc @f
		inc r8
		mov rax, 4
		call _set_v
		inc r9
		jmp _ex
@@:		mov dword ptr [rdx + 16], 0
_ex:	xor rax, rax
		movzx r8, word ptr [rdx + 4]
		call _set_v
		lea rax, [rbx + 3]
		ret
_fin:	xor rax, rax
		ret
OPTION EPILOGUE:NONE
_skip_c:call _skip_s
;<!-- sergey -->
		cmp qword ptr [rcx + r8 * 2], r11
		jnz _skip_cc
		add r8, 3
@@:		inc r8
		movzx rax, word ptr [rcx + r8 * 2]
		test rax, rax
		jz _err
		cmp rax, '>'
		jnz @b
		cmp qword ptr [rcx + r8 * 2 - 6], r12
		jnz @b
		inc r8
		call _skip_s
_skip_cc:ret
_skip_s:dec r8
@@:		inc r8
		movzx rax, word ptr [rcx + r8 * 2]
		cmp rax, 33
		jae @f
		test rax, rax
		jnz @b
@@:		ret
_set_v: mov [rdx + rax * 4], r8w
		mov [rdx + rax * 4 + 2], r9w
		ret
_word:	lea r9, [r8 - 1]
		cmp rax, 48
		jb _err
		cmp rax, 58
		jb _err
@@:		inc r9
		movzx rax, word ptr [rcx + r9 * 2]
		cmp rax, 128
		jae @b
		movzx rax, byte ptr [rax + r10]
		test rax, rax		; undef
		jz _err
		dec rax				; word
		jz @b
		ret
_err:	add rsp, 8
_err1:	mov rax, -1
		ret
_str:	cmp rax, 34
		jnz _str1
		mov r9, r8
@@:		inc r9
		movzx rax, word ptr [rcx + r9 * 2]
		test rax, rax
		jz _str1
		cmp rax, 34
		jnz @b
		stc
		ret
_str1:	clc
		ret
OPTION EPILOGUE:EPILOGUEDEF
asm_ssh_parse_xml endp

; rcx - this
; rdx - size
asm_ssh_bwt_transform proc USES rbx rsi rdi r12 r13 r14 r15
local @@index:QWORD
		mov r8, [rcx].BWT._RT
		mov r9, [rcx].BWT._LT
		mov rbx, [rcx].BWT._index
		mov rsi, [rcx].BWT._result
		mov r10, [rcx].BWT._in
		mov word ptr [rbx], -1
		add [rcx].BWT._in, rdx			; in += size
		add [rcx].BWT._result, rdx		; _result += size
		add [rcx].BWT._index, 2			; _index += 2
		mov @@index, rbx
		xor r12, r12					; idx_lit
		mov rdi, r9
		imul rcx, rdx, 64
		xor rax, rax
		rep stosq						; ssh_memzero(LT, keys * 512);
		mov rdi, r10
		xor r11, r11
		xor r10, r10					; idx = 0
@@:		mov al, [rdi + r10]				; c = in[idx]
		mov [rsi + r10], al				; _result[idx] = c;
		lea rcx, [r10 + 1]
		xchg cx, [r9 + rax * 2]			; LT[c] = idx;
		mov [r8 + r10 * 2 + 2],	cx		; RT[idx] = LT[c]
		cmp al, [rdi]
		setnz al
		or r11b, al
		inc r10
		cmp r10, rdx
		jl @b
		test r11, r11
		jz @f
		xor r14, r14
		call asm_ssh_bwt_sort
		mov rax, @@index
		mov [rax], bx
@@:		ret
asm_ssh_bwt_transform endp

; rdx - level
asm_ssh_bwt_sort proc
		xor r13, r13
_loop:	cmp r13, 512
		jge _exit
		movzx r10, word ptr [r9 + r13]			; auto idx(LT[i]);
		mov word ptr [r9 + r13], 0				; LT[i] = 0;
		add r13, 2
		test r10, r10
		jz _loop
		cmp word ptr [r8 + r10 * 2], 0			; RT[idx] == 0 ?
		jnz @f
		bwt_set_val
		jmp _loop
@@:		cmp r11, rdx
		jb _next
@@:		bwt_set_val
		movzx r10, word ptr [r8 + r10 * 2]		; idx = RT[idx]
		test r10, r10
		jnz @b
		jmp _loop
_next:	add r9, 512
@@:		mov rcx, r10
		lea rax, [r10 + r11 - 2]
		bwt_get_val								; auto c(get_val(idx, level));
		xchg r10w, [r9 + rax * 2]				; swap(idx, LT[c]);
		xchg r10w, [r8 + rcx * 2]
		test r10, r10
		jnz @b
		inc r11
		push r13
		call asm_ssh_bwt_sort
		pop r13
		sub r9, 512
		dec r11
		jmp _loop
_exit:	ret
asm_ssh_bwt_sort endp

; rcx - this
; rdx - size
; r8 - vec
asm_ssh_bwt_untransform proc
LOCAL @@count[256]:DWORD
		mov r10, [rcx].BWT._in
		mov r11, [rcx].BWT._result
		mov r9, [rcx].BWT._index
		add [rcx].BWT._index, 2
		add [rcx].BWT._in, rdx
		lea rdi, @@count
		mov rcx, 128
		xor rax, rax
		rep stosq								; ssh_memzero(count, 256 * 4);
		xor rcx, rcx
@@:		movzx rax, byte ptr [r10 + rcx]			; tmp = in[i]
		inc dword ptr [rsp + rax * 4]			; count[tmp]++
		inc rcx
		cmp rcx, rdx
		jb @b
		xor r12, r12							; sum = 0
		xor rcx, rcx
@@:		movsxd rax, dword ptr [rsp + rcx * 4]
		add r12, rax
		sub rax, r12
		neg rax
		mov [rsp + rcx * 4], eax
		inc rcx
		cmp ch, 1
		jnz @b
		xor rcx, rcx
@@:		movzx rax, byte ptr [r10 + rcx]			; tmp = in[i]
		movzx r12, word ptr [rsp + rax * 4]		; tmp = in[i
		mov [r8 + r12 * 2], cx
		inc dword ptr [rsp + rax * 4]			; count[tmp]++
		inc rcx
		cmp rcx, rdx
		jb @b
		movzx r12, word ptr [r9]
		xor rcx, rcx
@@:		movzx r12, word ptr [r8 + r12 * 2]
		mov al, [r10 + r12]
		mov [r11], al
		inc r11
		inc rcx
		cmp rcx, rdx
		jb @b
		ret
asm_ssh_bwt_untransform endp

; (void* _this, ssh_u size);
ssh_asm_encode_ari proc
		ret
ssh_asm_encode_ari endp

end
