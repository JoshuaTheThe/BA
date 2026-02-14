	.global main
	.section .text
main:	.extern printf
	jmp foo.aft
foo:	pushl %ebp
	movl %esp, %ebp
	pushl 12(%ebp)
	pushl 8(%ebp)
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	popl %eax
	jmp .ext
foo.ext:
	movl %ebp, %esp
	popl %ebp
	ret
foo.aft:
	pushl %esi
	movl %esp,%esi
	pushl $0x3
	pushl $0x2
	call foo
	movl %esi,%esp
	popl %esi
	push %eax
	popl %eax
.ext:
	pushl %eax
	movl $0x01, %eax
	popl %ebx
	int $0x80
	.section .data
	.section .rodata
