	.global main
	.section .text
main:	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %eax
	movl %eax, (arg_c)
	movl 12(%ebp), %eax
	movl %eax, (arg_v)
	.extern printf
	pushl $10
	lea k, %ebx
	popl (%ebx)
	pushl (%ebx)
m0:
	pushl (k)
	popl %eax
	testl %eax,%eax
	je m1
	pushl %esi
	movl %esp,%esi
	pushl (k)
	lea (string0), %ebx
	pushl %ebx
	call printf
	movl %esi,%esp
	popl %esi
	push %eax
	popl %eax
	pushl (k)
	pushl $1
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	lea k, %ebx
	popl (%ebx)
	pushl (%ebx)
	popl %eax
	jmp m0
m1:
	pushl $0
	popl %eax
	jmp .ext
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
k:	.long 0
	.section .rodata
string0:
	.ascii "Hello, World! %d\n"
