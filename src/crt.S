        .section .text
        .global _exit
        .global _start
        .global _read
        .global _fd_print
        .global _print
        .global _perror
        .extern main
_start:
        popl    %ecx
        movl    %esp, %edx
        pushl   %edx
        pushl   %ecx
        call    main
_exit:
        movl    4(%esp), %ebx
        movl    $1, %eax
        int     $0x80
_read:
        pushal
        movl    $0, %ebx
        movl    36(%esp), %ecx
        movl    40(%esp), %edx
        movl    $3, %eax
        int     $0x80
        popal
        ret

_perror:
        pushl   %ebx
        movl    $2, %ebx
        jmp     _fd_print
_print:
        pushl   %ebx
        movl    $1, %ebx
_fd_print:
        pushl   %ebp
        movl    %esp, %ebp
        pushal
        #       We need to skip the pushed EBX, so its the second arg, not first
        movl    12(%ebp), %esi
        xorl    %edx, %edx
strlen:
        cmpb    $0, (%esi, %edx)
        je      do_write
        incl    %edx
        jmp     strlen
do_write:
        movl    %esi, %ecx
        movl    $4, %eax
        int     $0x80
        popal
        popl    %ebp
        popl    %ebx
        ret