	.file	"time.cpp"
	.section	.rodata
	.align	3
.LC0:
	.string	"Begin_Time(long%lx) = %lf\n"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	add	sp,sp,-64
	.cfi_def_cfa_offset 64
	sd	ra,56(sp)
	sd	s0,48(sp)
	.cfi_offset 1, -8
	.cfi_offset 8, -16
	add	s0,sp,64
	.cfi_def_cfa 8, 0
	add	a5,s0,-56
	mv	a0,a5
	call	times
	ld	a5,-56(s0)
	sd	a5,-24(s0)
	ld	a2,-24(s0)
	ld	a1,-24(s0)
	lui	a5,%hi(.LC0)
	add	a0,a5,%lo(.LC0)
	call	printf
	li	a5,0
	mv	a0,a5
	ld	ra,56(sp)
	ld	s0,48(sp)
	add	sp,sp,64
	jr	ra
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 5.3.0"
