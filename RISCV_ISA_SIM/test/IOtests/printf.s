	.file	"print.cpp"
	.section	.rodata
	.align	3
.LC0:
	.string	"%d, %hd, %lx\n"
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
	li	a5,3
	sw	a5,-20(s0)
	li	a5,5
	sh	a5,-22(s0)
	li	a5,38178816
	add	a5,a5,-1329
	sll	a5,a5,7
	sd	a5,-32(s0)
	lh	a5,-22(s0)
	ld	a3,-32(s0)
	mv	a2,a5
	lw	a1,-20(s0)
	lui	a5,%hi(.LC0)
	add	a0,a5,%lo(.LC0)
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
	.ident	"GCC: (GNU) 5.3.0"
