;
; Example Program - CS 4331 Assembler Project
;
		org		100h
cr		equ		0dh
Start	LDA		a,cr
		sta		a,[buff1]
		lda		A,[buff2]
		jmp		Begin
;
buff1	ds		48
Check	ds		1
;
		org		200h
BEGIN	ldx		x,[message1]
		ldx		x,message1
		lda		b,9					; Print String DOS
		int		21h					 ;	function call
		jl		next
		lda		B,0Ah
		sta		b,[buff1]
		lda		b,[buff2]
Next	Add		B,10
		ADD		B,[Buff2]
		LDX		Y,[message1]
		neg		b
		JG		Cont
		aDD		A,-2
		Add		A,B
		add		a,[bUFF1]
Cont	ldx		y,message1
		LDA		A,[Y]				; Get first operand
		INX
		add		a,[Y]
		NEG		A					; Negate result
		INX
		STA		A,[Y]				; Store amswer
lf		equ		0ah
		LDA		b,4ch				; Terminate Process (EXIT)
		lda		a,0
		int		21h					; function call
;
buff2	DS		10h
MESSAGE1		db	cr,0Ah,'CS 4331 Example'
		DB		' Program',0Dh,LF,'$'
		END
