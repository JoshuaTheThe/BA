	.global main
	.section .text
main:	.extern printf
	pushl $0xa
	lea k, %ebx
	popl (%ebx)
	pushl (k)
m0:
	popl %eax
	testl %eax,%eax
	je m1
	pushl %esi
	movl %esp,%esi
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
	popl %eax
	jmp m0
m1:
.ext:
	pushl %eax
	movl $0x01, %eax
	popl %ebx
	int $0x80
	.section .data
k:	.long 0
	.section .rodata
string0:
	.ascii "Hello, World!\n"
