        .section .text
        .global _exit
        .global _start
        .global _perror
        .global _read
        .global _print
_start:
        popl    %ecx
        movl    %esp, %edx
        pushl   %edx
        pushl   %ecx
        call    main
        movl    %eax, %ebx
        movl    $1, %eax
        int     $0x80

_exit:
        pushl   %ebp
        movl    %esp, %ebp
        movl    8(%ebp), %ebx
        movl    $1, %eax
        int     $0x80

_read:
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ebx
        pushl   %ecx
        pushl   %edx
        
        movl    8(%ebp), %ebx
        movl    12(%ebp), %ecx
        movl    16(%ebp), %edx
        movl    $3, %eax
        int     $0x80
        
        popl    %edx
        popl    %ecx
        popl    %ebx
        popl    %ebp
        ret

_perror:
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ebx
        pushl   %ecx
        pushl   %edx
        
        movl    8(%ebp), %ecx
        movl    12(%ebp), %edx
        movl    $2, %ebx
        movl    $4, %eax
        int     $0x80
        
        popl    %edx
        popl    %ecx
        popl    %ebx
        popl    %ebp
        ret

_print:
        pushl   %ebp
        movl    %esp, %ebp
        pushl   %ebx
        pushl   %ecx
        pushl   %edx
        pushl   %esi
        
        movl    8(%ebp), %esi
        xorl    %edx, %edx
        
strlen:
        cmpb    $0, (%esi, %edx)
        je      do_write
        incl    %edx
        jmp     strlen
        
do_write:
        movl    %esi, %ecx
        movl    $1, %ebx
        movl    $4, %eax
        int     $0x80
        
        popl    %esi
        popl    %edx
        popl    %ecx
        popl    %ebx
        popl    %ebp
        ret