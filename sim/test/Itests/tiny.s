	.file	"tiny.cpp"
	.globl	global
	.section	.sdata,"aw",@progbits
	.align	2
	.type	global, @object
	.size	global, 4
global:
	.word	9
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
	li	a5,1
	sw	a5,-20(s0)
	li	a5,2
	sw	a5,-24(s0)
	lw	a5,-24(s0)
	addw	a5,a5,99
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	addw	a5,a5,1
	sw	a5,-20(s0)
	lui	a5,%hi(global)
	lw	a5,%lo(global)(a5)
	lw	a4,-20(s0)
	addw	a5,a4,a5
	sw	a5,-20(s0)
	lui	a5,%hi(global)
	lw	a4,%lo(global)(a5)
	lw	a5,-24(s0)
	addw	a4,a4,a5
	lui	a5,%hi(_ZZ4mainE1c)
	sw	a4,%lo(_ZZ4mainE1c)(a5)
	li	a5,0
	mv	a0,a5
	ld	s0,24(sp)
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
	.word	3
	.ident	"GCC: (GNU) 5.3.0"
