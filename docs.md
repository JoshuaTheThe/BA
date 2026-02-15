BA(1)                   BA User Manual                    BA(1)

NAME
       BA - Basic Applied Language, a word-oriented system
       implementation language, based on B

SYNOPSIS
       ba [options] file.b

DESCRIPTION
       BA is a minimal systems programming language for the
       PDP-11 and similar word-oriented machines.  All data
       types are one word.  Storage is explicitly controlled
       with 'static' and 'auto' declarations.

       keywords list:
       if       else    while   static  auto     goto    return  extern
       asm      fn      byte    short

EXAMPLE
        fn main(){
                return 0;
        }
INTERACTIVE SHELL
     BA can be used interactively by running without a file argument.
     Type BA statements directly, one per line.  To exit, type "EOF"
     on a line by itself or press Ctrl-D (which generates an actual
     EOF, also recognized).

     Example session:
          $ ./ba
          > extern printf;
          > printf("Hello, world!\n");
          Hello, world!
          > EOF
          $

     The special "EOF" keyword signals the end of input and is not
     compiled as part of the program.