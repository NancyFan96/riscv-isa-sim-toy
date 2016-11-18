	.file	"arith.cpp"
	.globl	g
	.section	.sdata,"aw",@progbits
	.align	2
	.type	g, @object
	.size	g, 4
g:
	.word	1091567616
	.section	.rodata
	.align	3
.LC2:
	.string	"a=%f, b=%f, d=a+b=%f\n"
	.align	3
.LC3:
	.string	"c=%f, g=%f, d=c*g=%f\n"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	add	sp,sp,-32
	.cfi_def_cfa_offset 32
	sd	ra,24(sp)
	sd	s0,16(sp)
	.cfi_offset 1, -8
	.cfi_offset 8, -16
	add	s0,sp,32
	.cfi_def_cfa 8, 0
	lui	a5,%hi(.LC0)
	flw	fa5,%lo(.LC0)(a5)
	fsw	fa5,-20(s0)
	lui	a5,%hi(.LC1)
	flw	fa5,%lo(.LC1)(a5)
	fsw	fa5,-24(s0)
	flw	fa4,-20(s0)
	flw	fa5,-24(s0)
	fadd.s	fa5,fa4,fa5
	fsw	fa5,-28(s0)
	flw	fa5,-20(s0)
	fcvt.d.s	fa4,fa5
	flw	fa5,-24(s0)
	fcvt.d.s	fa3,fa5
	flw	fa5,-28(s0)
	fcvt.d.s	fa5,fa5
	fmv.x.d	a3,fa5
	fmv.x.d	a2,fa3
	fmv.x.d	a1,fa4
	lui	a5,%hi(.LC2)
	add	a0,a5,%lo(.LC2)
	call	printf
	lui	a5,%hi(_ZZ4mainE1c)
	flw	fa4,%lo(_ZZ4mainE1c)(a5)
	lui	a5,%hi(g)
	flw	fa5,%lo(g)(a5)
	fmul.s	fa5,fa4,fa5
	fsw	fa5,-28(s0)
	lui	a5,%hi(_ZZ4mainE1c)
	flw	fa5,%lo(_ZZ4mainE1c)(a5)
	fcvt.d.s	fa4,fa5
	lui	a5,%hi(g)
	flw	fa5,%lo(g)(a5)
	fcvt.d.s	fa3,fa5
	flw	fa5,-28(s0)
	fcvt.d.s	fa5,fa5
	fmv.x.d	a3,fa5
	fmv.x.d	a2,fa3
	fmv.x.d	a1,fa4
	lui	a5,%hi(.LC3)
	add	a0,a5,%lo(.LC3)
	call	printf
	li	a5,0
	mv	a0,a5
	ld	ra,24(sp)
	ld	s0,16(sp)
	add	sp,sp,32
	jr	ra
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.sdata
	.align	2
	.type	_ZZ4mainE1c, @object
	.size	_ZZ4mainE1c, 4
_ZZ4mainE1c:
	.word	1079194419
	.section	.rodata
	.align	2
.LC0:
	.word	1069547520
	.align	2
.LC1:
	.word	1074161254
	.ident	"GCC: (GNU) 5.3.0"
