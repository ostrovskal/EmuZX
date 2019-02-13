
; перейти по относительному адресу процедуры
jmp_tbl macro dst, src, offs
	movzx dst, word ptr [src * 2 + r12 + offs]
	add dst, r12
	jmp dst
endm

; получить значение флага
get_flag macro dst, flag
	test byte ptr [r10 + RF], flag
	setnz dst
endm

; проверить на наличие флага
is_cond macro 
	and rax, 7
	test al, 1
	setz ah
	shr al, 1
	mov cl, [r13 + rax + condFlags]
	mov al, [r10 + RF]
	test al, cl
	setnz al
	cmp al, ah
endm

update_flags macro mask, val
	and byte ptr [r10 + RF], NOT mask
	or byte ptr [r10 + RF], val
endm

; преобразуем 2 битный номер регистровой пары в указатель на регистр(с учетом AF)
from_rp macro dst, src, offs
		and src, 6
		add src, pref
		movzx dst, byte ptr [r13 + src + offs]
		add dst, r10
endm

; читаем 8 бит из PC
read_mem8PC macro reg
		movzx reg, word ptr [r11 + rsi]
		inc rsi
endm

; читаем 16 бит из PC
read_mem16PC macro reg
		movzx reg, word ptr [r11 + rsi]
		add rsi, 2
endm

; читаем 8 бит из памяти
read_mem8 macro dst, src
		movzx dst, byte ptr [r11 + src]
endm

; читаем 16 бит из памяти
read_mem16 macro dst, src
		movzx dst, word ptr [r11 + src]
endm

ZX_INT			= 1
ZX_MEMORY		= 2
ZX_DEBUG		= 4
ZX_EXEC			= 8
ZX_TERMINATE	= 16
ZX_WRITE_ROM	= 32
ZX_TRAP			= 64
ZX_RESET		= 128
ZX_NMI			= 256
ZX_BUFFER_GPU	= 512

RC		= 0
RB		= 1
RE		= 2
RD		= 3
RL		= 4
RH		= 5
RF		= 6
RA		= 7
RC_		= 8
RB_		= 9
RE_		= 10
RD_		= 11
RL_		= 12
RH_		= 13
RF_		= 14
RA_		= 15
RXL		= 16
RXH		= 17
RYL		= 18
RYH		= 19
RSPL	= 20
RSPH	= 21
RPCL	= 22
RPCH	= 23
RI		= 24
RR		= 25
IFF1	= 26
IFF2	= 27
IM		= 28
TRAP	= 29
FE		= 30
FD		= 31
RAM		= 32
ROM		= 33
VID		= 34
SCAN	= 35
CALL1	= 36
CALL2	= 37
MODEL	= 38
TSTATE1	= 39
TSTATE2	= 40

_FC		= 1
_FN		= 2
_FPV	= 4
_FH		= 16
_FZ		= 64
_FS		= 128

PREFIX_NO	= 0
PREFIX_CB	= 1
PREFIX_ED	= 2
PREFIX_IX	= 1
PREFIX_IY	= 2

extern modifyTSTATE:near
extern writePort:near

prefSP		= 0
prefAF		= 24
prefRON		= 48
condFlags	= 72

.data? 
regZX		dq ?
memZX		dq ?
portZX		dq ?
check		dq ?
pref		dq ?
preg		dq ?
limMem		dq ?
limMemROM	dq ?

.data
cnv		db RC, RB, RE, RD, RL, RH, RSPL, RSPH, RC, RB, RE, RD, RXL, RXH, RSPL, RSPH, RC, RB, RE, RD, RYL, RYH, RSPL, RSPH
		db RC, RB, RE, RD, RL, RH, RF, RA, RC, RB, RE, RD, RXL, RXH, RF, RA, RC, RB, RE, RD, RYL, RYH, RF, RA
		db RB, RC, RD, RE, RH, RL, RF, RA, RB, RC, RD, RE, RXH, RXL, RF, RA, RB, RC, RD, RE, RYH, RYL, RF, RA
		db _FZ, _FC, _FPV, _FS

.code

; regsZX
; memZX
; portsZX
; checkBPS
asm_ssh_zx_init proc
		mov regZX, rcx
		mov memZX, rdx
		mov portZX, r8
		mov check, r9
		ret
asm_ssh_zx_init endp

asm_ssh_calc_fh proc
		ret
asm_ssh_calc_fh endp

asm_ssh_calc_fp proc
		ret
asm_ssh_calc_fp endp

asm_ssh_calc_fo proc
		ret
asm_ssh_calc_fo endp

; cx - op1
; dx - op2
; al - fc | fn
asm_accum2 proc
		push r8
		mov r8w, cx
		mov bl, al
		and rbx, 1				; FC
		push rbx
		and al, 2				; FN
		test al, 2
		jz @f
		add bx, 0ffffh
		sbb cx, dx
		jmp _f
@@:		add bx, 0ffffh
		adc cx, dx
_f:		lahf
		seto bh
		shl bh, 2				; FO
		and ah, 11000001b		; FLAGS (S Z 0 0 0 0 0 C)
		or ah, al				; FN
		or ah, bh				; FO
		mov r8w, cx
		; half carry
		pop rbx
		mov bh, ah
		test al, 2
		jz @f
		add bl, 255
		sbb ch, dh
		jmp _f1
@@:		add bl, 255
		adc ch, dh
_f1:	lahf
		mov al, ah
		and al, 00010000b
		or al, bh
		mov [r10 + RF], al
		mov ax, r8w
		pop r8
		ret
asm_accum2 endp

; cl - op1
; dl - op2
; r14 - ops
; al - fc
asm_accum1 proc
		mov dh, 255
		mov ch, 11010001b
		add al, dh
		mov rax, offset ops
		call qword ptr [r14 * 8 + rax]
		lahf
		mov al, ah
		shl dl, 2
		and al, ch				; FLAGS (S Z 0 H 0 0 N? C)
		or al, dl				; O/P
		cmp dh, 255
		jz @f
		and al, 11000111b
		or al, dh				; H
@@:		mov [r10 + RF], al
		mov al, cl
		ret
ops	dq _add, _adc, _sub, _sbc, _and, _xor, _or, _sub
_adc:	adc cl, dl
		seto dl
		ret
_add:	add cl, dl
		seto dl
		ret
_sbc:	sbb cl, dl
		mov ch, 11010011b
		seto dl
		ret
_sub:	sub cl, dl
		mov ch, 11010011b
		seto dl
		ret
_and:	and cl, dl
		mov dh, 16
		setp dl
		ret
_xor:	xor cl, dl
		mov dh, 0
		setp dl
		ret
_or:	or cl, dl
		mov dh, 0
		setp dl
		ret
asm_accum1 endp

; al - val
; dl - mask
; r14 - ops
asm_rotate proc
		mov rcx, 128
		mov r8, r14
		and r8, 1
		cmovz r8, rcx
		mov cl, al
		mov ch, al
		jz @f
		shr ch, 1
		jmp _f
	@@:	shl ch, 1				; v1
	_f: and cl, r8b
		test cl, cl
		setnz cl				; fc
		get_flag ah, _FC
		mov r8, offset ops1
		call qword ptr [r14 * 8 + r8]
		or al, ch
		lahf
		and ah, 11000100b
		or ah, cl
		mov cl, ah
		and cl, dl
		not dl
		and [r10 + RF], dl
		or [r10 + RF], cl
		ret
ops1 dq _rlc, _rrc, _rl, _rr, _sla, _sra, _sli, _sla
_rlc:	mov al, cl
		ret
_rrc:	mov al, cl
		shl al, 7
		ret
_rl:	mov al, ah
		ret
_rr:	mov al, ah
		shl al, 7
		ret
_sla:	xor al, al
		ret
_sra:	and al, 128
		ret
_sli:	mov al, 1
		ret
asm_rotate endp

; rax - address
asm_ssh_read_port proc
		push rbx
		mov rbx, portZX
		cmp al, 31
		jnz @f
		mov rax, 31
@@:		mov al, [rbx + rax]
		pop rbx
		ret
asm_ssh_read_port endp

; ax - address
asm_ssh_exec_call proc
		mov [r10 + CALL1], si
		sub si, 2
		mov rcx, rsi
		mov rsi, rax
		mov rax, rdi
		jmp asm_ssh_write_mem16
asm_ssh_exec_call endp

; rax - address
; cl - val
asm_ssh_write_mem8 proc
		cmp rax, limMem
		jae _w_no
		test word ptr [r10 + TSTATE1], ZX_DEBUG
		jz _n_d
;		call debug
_n_d:	cmp rax, limMemROM
		jae @f
		; ROM
		test word ptr [r10 + TSTATE1], ZX_WRITE_ROM
		jz _w_no
		; MEM
@@:		mov [rax], cl
		ret
_w_no:	ret
asm_ssh_write_mem8 endp

; rax - address
; cx - val
asm_ssh_write_mem16 proc
		cmp rax, limMem
		jae _w_no
		test word ptr [r10 + TSTATE1], ZX_DEBUG
		jz _n_d
;		call debug
_n_d:	cmp rax, limMemROM
		jae @f
		; ROM
		test word ptr [r10 + TSTATE1], ZX_WRITE_ROM
		jz _w_no
		; MEM
@@:		mov [rax], cx
		ret
_w_no:	ret
asm_ssh_write_mem16 endp

asm_ssh_inc_r proc
		mov al, [r10 + RR]
		mov ah, al
		and ah, 128
		inc al
		and al, 127
		or al, ah
		mov [r10 + RR], al
		ret
asm_ssh_inc_r endp

asm_ssh_read_ops proc
		lodsb
		movzx r15, al
		movzx r14, al
		and r15, 00000111b		; tOps
		and r14, 00111000b
		shr r14, 3				; cOps
		and rax, 11000000b
		shr rax, 4				; gOps
		ret
asm_ssh_read_ops endp

asm_ssh_pref_ron proc
		cmp al, 6
		jz @f
		add rax, pref
		movzx rax, byte ptr [r13 + rax + prefRON]
		add rax, r10
		ret
@@:		mov rax, RL
		from_rp rax, rax, prefSP
		mov r9, pref
		test r9, r9
		jz @f
		read_mem8PC r9
		movsx r9, r9b
@@:		movzx rax, word ptr [rax]
		add rax, r9
		add rax, r11
		mov preg, rax
		ret
asm_ssh_pref_ron endp

; r10 - regZX, rsi - PC, rdi - SP
asm_ssh_zx_step proc
		push rbx
		push rsi
		push rdi
		push r10
		push r11
		push r12
		push r13
		push r14
		push r15
		mov r10, regZX
		mov r11, memZX
		movzx rsi, word ptr [r10 + RPCL]
		movzx rdi, word ptr [r10 + RSPL]
		mov r12, offset grp_ops
		mov r13, offset cnv
		xor rcx, rcx
		xor rdx, rdx
		call asm_ssh_exec
		mov [r10 + RPCL], si
		mov [r10 + RSPL], di
		pop r15
		pop r14
		pop r13
		pop r12
		pop r11
		pop r10
		pop rdi
		pop rsi
		pop rbx
		ret
asm_ssh_zx_step endp

; pref1 - rcx
; pref2 - rdx
asm_ssh_exec proc
		call asm_ssh_inc_r
		; читаем операцию
		call asm_ssh_read_ops
		shl rcx, 3
		mov pref, rcx
		jz @f
		cmp rdx, 1
		jnz @f
		mov rax, 6
		call asm_ssh_pref_ron
		call asm_ssh_read_ops
@@:		add rax, rdx
		jmp_tbl r8, rax, 0
grp_ops	dw opsNN00 - grp_ops, opsCB00 - grp_ops, opsEDXX - grp_ops, 0
		dw opsNN01 - grp_ops, opsCB01 - grp_ops, opsED01 - grp_ops, 0
		dw opsNN10 - grp_ops, opsCB10 - grp_ops, opsED10 - grp_ops, 0
		dw opsNN11 - grp_ops, opsCB11 - grp_ops, opsEDXX - grp_ops, 0
		; NN00 16 * 2 = 32
		dw opsNN00_000 - grp_ops, opsNN00_LD_RP - grp_ops, opsNN00_010 - grp_ops, opsNN00_ID_RP - grp_ops
		dw opsNN00_ID_R - grp_ops, opsNN00_ID_R - grp_ops, opsNN00_LD_R - grp_ops, opsNN00_111 - grp_ops
		; NN11 24 * 2 = 48
		dw opsNN11_XXX - grp_ops, opsNN11_001 - grp_ops, opsNN11_XXX - grp_ops, opsNN11_011 - grp_ops
		dw opsNN11_XXX - grp_ops, opsNN11_101 - grp_ops, opsNN11_XXX - grp_ops, opsNN11_XXX - grp_ops
		; ED01 32 * 2 = 64
		dw opsED01_000 - grp_ops, opsED01_000 - grp_ops, opsED01_000 - grp_ops, opsED01_000 - grp_ops
		dw opsED01_000 - grp_ops, opsED01_000 - grp_ops, opsED01_000 - grp_ops, opsED01_111 - grp_ops
		; ED10 40 * 2 = 80
		dw opsED10_LDI - grp_ops, opsED10_CPI - grp_ops, opsED10_INI - grp_ops, opsED10_OTI - grp_ops
		dw opsEDXX - grp_ops, opsEDXX - grp_ops, opsEDXX - grp_ops, opsEDXX - grp_ops
		; NN00_000_XX 48 * 2 = 96
		dw opsNN00_NOP - grp_ops, opsNN00_EX_AF - grp_ops, opsNN00_DJNZ - grp_ops, opsNN00_JR - grp_ops
		dw opsNN00_JR_CC - grp_ops, opsNN00_JR_CC - grp_ops, opsNN00_JR_CC - grp_ops, opsNN00_JR_CC - grp_ops
		; NN00_111_XX 56 * 2 = 112
		dw opsNN00_ROT - grp_ops, opsNN00_ROT - grp_ops, opsNN00_ROT - grp_ops, opsNN00_ROT - grp_ops
		dw opsNN00_DAA - grp_ops, opsNN00_CPL - grp_ops, opsNN00_SCF - grp_ops, opsNN00_CCF - grp_ops
		; 64 * 2 = 128
		dw opsNN11_RET_CC - grp_ops, opsNN00_NOP - grp_ops, opsNN11_JP_CC - grp_ops, opsNN00_NOP - grp_ops
		dw opsNN11_CALL_CC - grp_ops, opsNN00_NOP - grp_ops, opsNN11_ARIFTH - grp_ops, opsNN11_RST - grp_ops
		; 72 * 2 = 144
		dw opsNN11_POP - grp_ops, opsNN11_RET - grp_ops, opsNN11_POP - grp_ops, opsNN11_EXX - grp_ops
		dw opsNN11_POP - grp_ops, opsNN11_JP_HL - grp_ops, opsNN11_POP - grp_ops, opsNN11_LD_SP - grp_ops
		; 80 * 2 = 160
		dw opsNN11_JP - grp_ops, opsNN11_CB - grp_ops, opsNN11_OUTN - grp_ops, opsNN11_INN - grp_ops
		dw opsNN11_EX_SP - grp_ops, opsNN11_EX_DE - grp_ops, opsNN11_EI_DI - grp_ops, opsNN11_EI_DI - grp_ops
		; 88 * 2 = 176
		dw opsNN11_PUSH - grp_ops, opsNN11_CALL - grp_ops, opsNN11_PUSH - grp_ops, opsNN11_PREF - grp_ops
		dw opsNN11_PUSH - grp_ops, opsNN11_ED - grp_ops, opsNN11_PUSH - grp_ops, opsNN11_PREF - grp_ops
		; 96 * 2 = 192
		dw opsED01_INC - grp_ops, opsED01_OUTC - grp_ops, opsED01_ACCUM2 - grp_ops, opsED01_RP_NN - grp_ops
		dw opsED01_NEG - grp_ops, opsED01_RETI - grp_ops, opsED01_IM - grp_ops, opsNN00_NOP - grp_ops
		; 104 * 2 = 208

opsNN00:jmp_tbl r8, r15, 32
opsNN01:cmp r15, r14
		jnz @f
		cmp r15, 6
		jnz @f
		; HALT
		test word ptr [r10 + TSTATE1], ZX_TRAP
		setz al
		sub rsi, rax
		ret
@@:		cmp r14, 6
		jnz @f
		; LD [HL/IX], SSS
		mov rax, r14
		call asm_ssh_pref_ron
		movzx rcx, byte ptr [r13 + r15 + prefRON]
		mov cl, [r10 + rcx]
		call asm_ssh_write_mem8
		ret
@@:		mov rax, r15
		call asm_ssh_pref_ron
		mov cl, [rax]
		cmp r15, 6
		jnz @f
		; LD DDD, [HL/IX]
		movzx rax, byte ptr [r13 + r14 + prefRON]
		mov [rax + r10], cl
		ret
@@:		; LD DDD, SSS
		add r14, pref
		movzx rax, byte ptr [r13 + r14 + prefRON]
		mov [rax + r10], cl
		ret
opsNN10:mov rax, r15
		call asm_ssh_pref_ron
		mov cl, [r10 + RA]
		mov dl, [rax]
		get_flag al, _FC
		jmp asm_accum1
opsNN11:jmp_tbl r8, r15, 48
opsED01:jmp_tbl r8, r15, 64
opsED01_000:jmp_tbl r8, r15, 192
ed01_data db RI, RR, RA, RA, RA, RA, RI, RR
opsED01_111:cmp r14, 6
			jae opsNN00_NOP
			cmp r14, 4
			jae opsED01_ROT
			; LD I/R/A, A/I/R
			mov rax, offset ed01_data
			movzx rcx, byte ptr [r14 + rax]			; dst
			movzx rdx, byte ptr [r14 + rax + 4]		; src
			mov al, [r10 + rdx]
			mov [r10 + rcx], al
			cmp r14, 2
			jbe @f
			test al, al
			sets cl
			setz ch
			mov al, [r10 + IFF2]
			shl al, 2
			shl cl, 7
			shl ch, 6
			or cl, ch
			or cl, al
			update_flags _FS or _FZ or _FH or _FPV or _FN, cl
@@:			ret
opsED01_ROT:mov rax, RF
			call asm_ssh_pref_ron
			mov cl, [rax]			; vreg
			mov ch, cl
			mov dl, [r10 + RA]
			mov dh, dl
			and dh, 15				; a
			cmp r14, 4
			jnz @f
			and ch, 15				; h
			shr cl, 4
			shl dl, 4
			jmp _opsED01_R0
@@:			shr ch, 4				; h
			shl cl, 4
_opsED01_R0:or dl, cl
			mov [rax], dl
			mov al, [r10 + RA]
			and al, 240
			or al, ch
			mov [r10 + RA], al
			test al, al
			sets cl
			setz ch
;			call asm_ssh_fp
			shl cl, 7
			shl ch, 6
			or cl, ch
			or cl, dl
			update_flags _FS or _FZ or _FH or _FPV or _FN, cl
			ret
opsED10:cmp r14, 4
		jb opsEDXX
		movzx r8, word ptr [r15 * 2 + r12 + 80]
		mov rax, 3
		mov r15, r14
		and r15, 1
		cmovz r15, rax
		sub r15, 2		; direct
		movzx r9, word ptr  [r10 + RC]	; BC
		movzx r8, word ptr [r10 + RL]	; HL
		movzx rdx, word ptr [r10 + RE]	; DE
		movzx rax, byte ptr [r10 + RA];	; A
		mov rdi, portZX
		add r8, r12
		jmp r8
; LD[D/I][R]
opsED10_LDI:read_mem8 rcx, r8
			mov rax, rdx
			call asm_ssh_write_mem8
			add dx, r15w
			add r8w, r15w
			dec r9w
			jz @f
			test r14, 2
			jnz opsED10_LDI
@@:			mov [r10 + RL], r8w
			mov [r10 + RE], dx
			mov [r10 + RC], r9w
			test r9, r9
			setnz cl
			shl cl, 2
			update_flags _FH or _FPV or _FN, cl
			ret
; CP[D/I][R]
opsED10_CPI:read_mem8 rcx, r8
			add r8w, r15w
			cmp al, cl
			jz @f
			dec r9
			jz @f
			test r14, 2
			jnz opsED10_CPI
@@:			mov [r10 + RC], r9w
			mov [r10 + RL], r8w
			; al,cl,0|1
			mov dl, 2		; fc | fn
;			call asm_ssh_fh
;			dl = fh
			sub al, cl
			setz al
			sets ah
			test r9, r9
			setnz cl
			shl al, 6		; fz
			shl ah, 7		; fs
			shl cl, 2		; fpv
			or al, ah
			or al, dl		; fh
			or al, 2		; fn
			or cl, al
			update_flags _FS or _FZ or _FH or _FPV or _FN, cl
			ret
; IN[D/I][R]
opsED10_INI:mov rdx, r9
			shr rdx, 8
@@:			mov cl, [rdi + r9]
			mov rax, r8
			call asm_ssh_write_mem8
			add r8w, r15w
			sub r9w, 256
			dec dl
			jz @f
			test r14, 2
			jnz @b
@@:			mov [r10 + RB], dl
			mov [r10 + RL], r8w
			test dl, dl
			setz cl
			shl cl, 6			; fz
			or cl, 2			; fn
			update_flags _FS or _FZ or _FH or _FPV or _FN, cl
			ret
; OUT[D/I][R]
opsED10_OTI:mov rdx, r9
			shr rdx, 8
@@:			read_mem8 rcx, r8
			mov [rdi + r9], cl
			add r8w, r15w
			sub r9w, 256
			dec dl
			jz @f
			test r15, 2
			jnz @b
@@:			mov [r10 + RB], dl
			mov [r10 + RL], r8w
			test dl, dl
			setz cl
			shl cl, 6			; fz
			or cl, 2			; fn
			update_flags _FS or _FZ or _FH or _FPV or _FN, cl
			ret
opsEDXX:
		ret


opsCB00:
		ret
opsCB01:
		ret
opsCB10:
		ret
opsCB11:
		ret


opsNN00_000:
		xor rcx, rcx
		cmp r14, 1
		jbe @f
		read_mem8PC rcx
		movsx rcx, cl
@@:		movzx r8, word ptr [r14 * 2 + r12 + 96]
		add r8, r12
		jmp r8
opsNN00_NOP:ret
opsNN00_EX_AF:
			mov ax, [r10 + RF]
			mov cx, [r10 + RF_]
			mov [r10 + RF], cx
			mov [r10 + RF_], ax
			ret
opsNN00_DJNZ:
			dec byte ptr [r10 + RB]
			jnz @f
			add rsi, rcx
@@:			ret
opsNN00_JR_CC:
			mov rax, r14
			and rax, 3
			is_cond
			jnz @f
opsNN00_JR: add rsi, rcx
@@:			ret
opsNN00_LD_RP:
			from_rp rdx, r14, prefSP
			test r14, 1
			jnz @f
			; LD RP, NN
			read_mem16PC rax
			mov [rdx], ax
			ret
@@:			; ADD HL, RP
			mov rax, RL
			from_rp r15, rax, prefSP
			mov ax, [r15]		; valHL
			mov cx, [rdx]		; *reg
			add ax, cx
			setc dl
			mov dh, dl
			mov [r15], ax
			shr ax, 8
			shr cx, 8
			;call asm_ssh_fh
			or dl, dh
			update_flags _FH or _FN or _FC, dl
			ret
opsNN00_010:lea rdx, [r10 + RA]
			cmp r14, 4
			jae _opsNN00_0
			; LD (BC/DE), A | LD A, (BC/DE)
			from_rp rax, r14, prefAF
			test r14, 1
			jz @f
			read_mem8 rax, rax
			mov [rdx], al
			ret
@@:			mov cl, [rdx]
			call asm_ssh_write_mem8
			ret
_opsNN00_0:; LD [nn], HL/A | LD A/HL, [nn]
			read_mem16PC rcx	; NN
			from_rp r8, rax, prefAF	; HL/IX/IY
			test r14, 1
			jz _opsNN00_1
			read_mem16 rax, rcx
			test r14, 2
			jz @f
			mov [rdx], al
			ret
@@:			mov [r8], ax
			ret
_opsNN00_1: mov rax, rcx
			test r14, 2
			jz @f
			mov cl, [rdx]
			call asm_ssh_write_mem8
			ret
@@:			mov cx, [r8]
			call asm_ssh_write_mem16
			ret
opsNN00_ID_RP:
			mov rdx, 1
			mov rcx, -1
			test r14, 1
			cmovnz rdx, rcx
			from_rp rcx, r14, prefSP
			add [rcx], dl
			ret
opsNN00_ID_R:
			mov rax, r15
			mov rdx, 1
			mov rcx, -1
			test rax, 1
			cmovnz rdx, rcx				; op2
			setnz r8b
			shl r8b, 1					; fn
			mov rax, r14
			call asm_ssh_pref_ron
			movzx rcx, byte ptr [rax]
			mov r15, rcx				; op1
			add rcx, rdx				; op1 + op2
			call asm_ssh_write_mem8
			test cl, cl
			sets al
			setz ah
			;call asm_ssh_fv1
			mov al, r15b
			mov cl, dl
			mov dl, r8b
			;call asm_ssh_fh
			update_flags _FS or _FZ or _FH or _FPV or _FN, cl
			ret
opsNN00_LD_R:
			mov rax, r14
			call asm_ssh_pref_ron
			read_mem8PC rcx
			call asm_ssh_write_mem8
			ret
opsNN00_111:lea r9, [r10 + RA]
			get_flag r15b, _FC
			movzx r8, word ptr [r14 * 2 + r12 + 112]
			add r8, 12
			jmp r8
opsNN00_ROT:movzx rax, byte ptr [r9]
			mov rdx, _FH or _FN or _FC
			call asm_rotate
			mov [r9], al
			ret
opsNN00_DAA:movzx ax, byte ptr [r9] ; A
			mov dh, al				; oldA
			get_flag cl, _FH
			get_flag ch, _FN
			xor dl, dl				; fc
			mov ah, al
			and ah, 15
			test cl, cl
			mov cl, 0
			jnz @f
			cmp ah, 9
			jbe _opsNN00_2
@@:			mov r8, 6
			mov r12, -6
			test ch, ch
			cmovnz r8, r12
			add ax, r8w
			setc dl
			or dl, r15b				; fc = oldFc | (val > 255 ? 1 : 0);
			movzx ax, al
			mov cl, 16
_opsNN00_2:	test r15b, r15b
			mov dl, 0
			jnz @f
			cmp dh, 99h
			jbe _opsNN00_3
@@:			mov dl, 1
			mov r12, -96
			mov r8, 96
			test ch, ch
			cmovnz r8, r12
			add ax, r8w
_opsNN00_3:	mov [r9],al
			test al, al
			sets dl
			setz dh
			setc cl
			shl dl, 7
			shl dh, 6
			or cl, dl
			or cl, dh
;			call asm_ssh_fp
			or cl, dl
			update_flags _FS or _FZ or _FH or _FPV or _FC, cl
			pop r11
			ret
opsNN00_CPL:update_flags _FH or _FN, 18
			not byte ptr [r9]
			ret
opsNN00_SCF:update_flags _FH or _FN or _FC, 1
			ret
opsNN00_CCF:movzx rax, cl
			shl rax, 4
			test cl, cl
			setz cl
			or cl, al
			update_flags _FH or _FN or _FC, cl
			ret
opsNN11_XXX:xor rcx, rcx
			cmp r15, 7
			jz _opsNN11_X
			test r15, r15
			jz @f
			read_mem16PC rcx
@@:			mov rax, r14
			is_cond
			jz _opsNN11_X
			ret
_opsNN11_X:	movzx r8, word ptr [r15 * 2 + r12 + 128]
			add r8, r12
			jmp r8
opsNN11_RET_CC:
			read_mem16 rcx, rdi
			add rdi, 2
opsNN11_JP_CC:
			mov rsi, rcx
			ret
opsNN11_CALL_CC:
			mov [r10 + CALL1], si
opsNN11_RST:mov rdx, rcx
			sub rdi, 2
			mov rax, rdi
			mov rcx, rsi
			call asm_ssh_write_mem16
			shl r14, 3
			cmp r15, 4
			cmovz rdx, r14
			mov rsi, rdx
			ret
opsNN11_ARIFTH:
			read_mem8PC rcx
			jmp asm_accum1
opsNN11_001:cmp r14, 5
			jz @f
			cmp r14, 7
			jnz _opsNN11_1
@@:			from_rp rcx, rcx, prefAF
			jmp @f
_opsNN11_1:	cmp r14, 3
			jz @f
			read_mem16 rcx, rdi
			add rdi, 2
@@:			movzx r8, word ptr [r14 * 2 + r12 + 144]
			add r8, r12
			jmp r8
opsNN11_POP:from_rp rax, r14, prefAF
			mov [rax], cx
			ret
opsNN11_JP_HL:
opsNN11_RET:mov rsi, rcx
			ret
opsNN11_EXX:mov rax, [r10 + RC]
			mov rcx, [r10 + RC_]
			mov [r10 + RC], rcx
			mov [r10 + RC_], rax
			ret
opsNN11_LD_SP:
			mov rdi, rcx
			ret
opsNN11_011:lea r9, [r10 + RA]
			movzx r8, word ptr [r14 * 2 + r12 + 160]
			add r8, r12
			jmp r8
opsNN11_JP:	read_mem16PC rsi
			ret
opsNN11_CB:	xor rcx, rcx
			mov rdx, PREFIX_CB
			jmp asm_ssh_exec
opsNN11_OUTN:
			movzx rax, byte ptr [r9]
			mov rdx, rax
			shl rdx, 3
			read_mem8PC rcx
			or rcx, rdx
			jmp writePort
opsNN11_INN:movzx ax, byte ptr [r9]
			mov dx, ax
			shl dx, 3
			read_mem8PC rcx
			or cx, dx
			call asm_ssh_read_port
			mov [r9], al
			ret
opsNN11_EX_SP:
			mov rax, RL
			from_rp rdx, rax, prefSP
			read_mem16 r9, rdi
			mov cx, [rdx]
			mov rax, rdi
			call asm_ssh_write_mem16
			mov [rdx], r9w
			ret
opsNN11_EX_DE:
			mov ax, [r10 + RL]
			mov cx, [r10 + RL_]
			mov [r10 + RL], cx
			mov [r10 + RL_], ax
			ret
opsNN11_EI_DI:
			test r14, 1
			setnz al
			mov [r10 + IFF1], al
			mov [r10 + IFF2], al
			; modifyTSTATE(!(ops & 1), ZX_INT);
			ret
opsNN11_101:movzx r8, word ptr [r14 * 2 + r12 + 176]
			add r8, r12
			jmp r8
opsNN11_PUSH:
			sub rdi, 2
			from_rp rax, r14, prefAF
			mov cx, [rax]
			mov rax, rdi
			call asm_ssh_write_mem16
			ret
opsNN11_CALL:
			read_mem16PC rax
			jmp asm_ssh_exec_call
opsNN11_PREF:
			read_mem8 rax, rsi
			cmp rax, 0ddh
			jz opsEDXX
			cmp rax, 0fdh
			jz opsEDXX
			cmp rax, 0edh
			jz opsEDXX
			mov rdx, PREFIX_CB
			cmp rax, 0cbh
			mov rax, 0
			cmovnz rdx, rax
			mov rcx, r14
			and rcx, 4
			shr rcx, 2
			inc rcx
			jmp asm_ssh_exec
opsNN11_ED:	xor rcx, rcx
			mov rdx, PREFIX_ED
			jmp asm_ssh_exec
;*IN F, [C] / IN DDD, [C]; SZ503P0-
opsED01_INC:mov ax, [r10 + RC]
			call asm_ssh_read_port
			mov rcx, rax
			cmp r14, 6
			jz @f
			mov rax, r14
			call asm_ssh_pref_ron
			mov [rax], cl
@@:			test cl, cl
			sets cl
			setz ch
;			call asm_ssh_fp
			or cl, ch
			or cl, dl
			update_flags _FS or _FZ or _FH or _FPV or _FN, cl
			ret
; *OUT[C], 0 / OUT[C], SSS
opsED01_OUTC:
			mov rax, r14
			call asm_ssh_pref_ron
			mov cl, [rax]
			lea rax, [r10 + RC]
			jmp writePort
; ADC/SBC HL, RP
opsED01_ACCUM2:
			jmp asm_accum2
; LD [NN], RP / LD RP, [nn]
opsED01_RP_NN:
			from_rp rcx, r14, prefSP
			read_mem16PC rdx
			test r14, 1
			jz @f
			read_mem16 rax, rdx
			mov [rcx], ax
			ret
@@:			mov rax, rdx
			mov cx, [rcx]
			call asm_ssh_write_mem16
			ret
; NEG
opsED01_NEG:xor cl, cl
			mov al, [r10 + RA]		; a
			mov cl, al				; val
			mov [r10 + RA], cl
			cmp al, 80h
			setae dl
			test al, al
			setnz al
			shl dl, 2
			test cl, cl
			sets cl
			setz ch
			shl ch, 6
			shl cl, 7
			or cl, ch
			or cl, al
			or cl, dl
;			call asm_ssh_fh; _FH(0, a, 0, 1)
			update_flags _FS or _FZ or _FH or _FPV or _FN or _FC, cl
			ret
; RETI
opsED01_RETI:
			mov al, [r10 + IFF2]
			mov [r10 + IFF1], al
			read_mem16 rsi, rdi
			add rdi, 2
			ret
; IM X
ed01_im db 0, 0, 1, 2, 0, 0, 1, 2
opsED01_IM: mov rax, offset ed01_im
			mov al, [r14 + rax]
			mov [r10 + IM], al
			ret
asm_ssh_exec endp

end
