	.file	"convert.cpp"
	.section	.rodata
	.align	3
.LC0:
	.string	"convert from int to float: number = %d, fnumber = %f\n"
	.align	3
.LC1:
	.string	"convert from long to float: longnum = %ld, flongnum = %f\n"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	add	sp,sp,-48
	.cfi_def_cfa_offset 48
	sd	ra,40(sp)
	sd	s0,32(sp)
	.cfi_offset 1, -8
	.cfi_offset 8, -16
	add	s0,sp,48
	.cfi_def_cfa 8, 0
	li	a5,8192
	addw	a5,a5,1808
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	fcvt.s.w	fa5,a5
	fsw	fa5,-24(s0)
	li	a5,15
	sd	a5,-32(s0)
	ld	a5,-32(s0)
	fcvt.s.l	fa5,a5
	fsw	fa5,-36(s0)
	flw	fa5,-24(s0)
	fcvt.d.s	fa5,fa5
	fmv.x.d	a2,fa5
	lw	a1,-20(s0)
	lui	a5,%hi(.LC0)
	add	a0,a5,%lo(.LC0)
	call	printf
	flw	fa5,-36(s0)
	fcvt.d.s	fa5,fa5
	fmv.x.d	a2,fa5
	ld	a1,-32(s0)
	lui	a5,%hi(.LC1)
	add	a0,a5,%lo(.LC1)
	call	printf
	li	a5,0
	mv	a0,a5
	ld	ra,40(sp)
	ld	s0,32(sp)
	add	sp,sp,48
	jr	ra
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 5.3.0"
