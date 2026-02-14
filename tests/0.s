	.global main
	.section .text
main:	pushl %ebp
	movl %esp, %ebp
	.extern printf
	pushl $0xa
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
	pushl $0x1
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
	pushl $0x0
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
k:	.long 0
	.section .rodata
string0:
	.ascii "Hello, World! %d\n"
