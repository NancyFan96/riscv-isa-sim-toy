	.file	"ftiny.cpp"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	add	sp,sp,-32
	.cfi_def_cfa_offset 32
	sd	s0,24(sp)
	.cfi_offset 8, -8
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
	flw	fa4,-20(s0)
	flw	fa5,-24(s0)
	fmul.s	fa5,fa4,fa5
	fsw	fa5,-28(s0)
	flw	fa4,-20(s0)
	flw	fa5,-24(s0)
	fsub.s	fa5,fa4,fa5
	fsw	fa5,-28(s0)
	flw	fa4,-20(s0)
	flw	fa5,-24(s0)
	fdiv.s	fa5,fa4,fa5
	fsw	fa5,-28(s0)
	li	a5,0
	mv	a0,a5
	ld	s0,24(sp)
	add	sp,sp,32
	jr	ra
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.rodata
	.align	2
.LC0:
	.word	1069547520
	.align	2
.LC1:
	.word	1074161254
	.ident	"GCC: (GNU) 5.3.0"
