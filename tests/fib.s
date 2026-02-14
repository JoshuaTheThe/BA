	.global main
	.section .text
main:	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %eax
	movl %eax, (arg_c)
	movl 12(%ebp), %eax
	movl %eax, (arg_v)
	jmp fib.aft
fib:
	pushl %ebp
	movl %esp, %ebp
	pushl 8(%ebp)
	pushl $2
	popl %ebx
	popl %eax
	cmpl %ebx, %eax
	setl %al
	movzx %al, %eax
	pushl %eax
	popl %eax
	testl %eax,%eax
	je m1
m0:
	pushl 8(%ebp)
	popl %eax
	jmp fib.ext
	jmp m2
m1:
m2:
	pushl %esi
	movl %esp,%esi
	pushl 8(%ebp)
	pushl $1
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	call fib
	movl %esi,%esp
	popl %esi
	push %eax
	pushl %esi
	movl %esp,%esi
	pushl 8(%ebp)
	pushl $2
	popl %ebx
	popl %eax
	subl %ebx, %eax
	pushl %eax
	call fib
	movl %esi,%esp
	popl %esi
	push %eax
	popl %ebx
	popl %eax
	addl %ebx, %eax
	pushl %eax
	popl %eax
	jmp fib.ext
fib.ext:
	movl %ebp, %esp
	popl %ebp
	ret
fib.aft:
	pushl %esi
	movl %esp,%esi
	pushl $10
	call fib
	movl %esi,%esp
	popl %esi
	push %eax
	popl %eax
	jmp fib.ext
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
