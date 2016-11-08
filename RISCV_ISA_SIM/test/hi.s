	.file	"hi.cpp"
	.section	.rodata
	.align	3
.LC0:
	.string	"%s"
	.align	3
.LC1:
	.string	"Hi %s!\n"
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
	ld	a1,-24(s0)
	lui	a5,%hi(.LC0)
	add	a0,a5,%lo(.LC0)
	call	scanf
	ld	a1,-24(s0)
	lui	a5,%hi(.LC1)
	add	a0,a5,%lo(.LC1)
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
