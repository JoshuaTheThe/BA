	.global main
	.section .text
main:	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %eax
	movl %eax, (arg_c)
	movl 12(%ebp), %eax
	movl %eax, (arg_v)
	.extern printf
	jmp foo.aft
foo:
	pushl %ebp
	movl %esp, %ebp
	pushl 12(%ebp)
	pushl 8(%ebp)
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	popl %eax
	jmp foo.ext
foo.ext:
	movl %ebp, %esp
	popl %ebp
	ret
foo.aft:
	pushl %esi
	movl %esp,%esi
	pushl %esi
	movl %esp,%esi
	pushl $3
	pushl $2
	call foo
	movl %esi,%esp
	popl %esi
	push %eax
	lea (string0), %ebx
	pushl %ebx
	call printf
	movl %esi,%esp
	popl %esi
	push %eax
	popl %eax
	pushl (arg_c)
	popl %eax
	jmp foo.ext
.ext:
	movl %ebp, %esp
	popl %ebp
	pushl %eax
	movl $0x01, %eax
	popl %ebx
	int $0x80
	.section .data
arg_v:	.long 0
arg_c:	.long 0
	.section .rodata
string0:
	.ascii "Difference: %d\n"
