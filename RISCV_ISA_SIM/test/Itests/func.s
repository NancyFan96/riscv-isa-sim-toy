	.file	"func.cpp"
	.text
	.align	2
	.globl	_Z4funcii
	.type	_Z4funcii, @function
_Z4funcii:
.LFB0:
	.cfi_startproc
	add	sp,sp,-32
	.cfi_def_cfa_offset 32
	sd	s0,24(sp)
	.cfi_offset 8, -8
	add	s0,sp,32
	.cfi_def_cfa 8, 0
	sw	a0,-20(s0)
	sw	a1,-24(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	addw	a5,a4,a5
	mv	a0,a5
	ld	s0,24(sp)
	add	sp,sp,32
	jr	ra
	.cfi_endproc
.LFE0:
	.size	_Z4funcii, .-_Z4funcii
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	add	sp,sp,-48
	.cfi_def_cfa_offset 48
	sd	ra,40(sp)
	sd	s0,32(sp)
	.cfi_offset 1, -8
	.cfi_offset 8, -16
	add	s0,sp,48
	.cfi_def_cfa 8, 0
	li	a5,1
	sw	a5,-20(s0)
	li	a5,2
	sw	a5,-24(s0)
	li	a5,3
	sw	a5,-28(s0)
	li	a5,4
	sw	a5,-32(s0)
	li	a5,5
	sw	a5,-36(s0)
	lw	a1,-28(s0)
	lw	a0,-24(s0)
	call	_Z4funcii
	sw	a0,-20(s0)
	lw	a1,-32(s0)
	lw	a0,-20(s0)
	call	_Z4funcii
	sw	a0,-32(s0)
	li	a5,0
	mv	a0,a5
	ld	ra,40(sp)
	ld	s0,32(sp)
	add	sp,sp,48
	jr	ra
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 5.3.0"
