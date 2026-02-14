
#pragma pack(1)

#define MAX_LENGTH (32)
#define MAX_N (256)

extern int _print(const char *);
extern int _perror(const char *, int);
extern void _exit(int);
extern int _read(int, char *, int);

struct _GLOBAL_
{
        char strings[MAX_LENGTH*MAX_N];
        char variables[MAX_N][MAX_LENGTH];
        int ebpoff[MAX_N];
        char ID[MAX_LENGTH],function[MAX_LENGTH];
        int NUMBER, PUSHBACK, EBPOFF, string_count, label_count, variable_count, global_count;
};

extern expr(struct _GLOBAL_ *,int);

_print_number(n)
{
        char buf[12];
        int i;

        i = 11;
        buf[11] = 0;

        if (n < 0)
        {
                _print("-");
                n = -n;
        }

        do
        {
                buf[--i] = '0' + (n % 10);
                n /= 10;
        } while (n > 0);

        _print(buf + i);
}

createstr(struct _GLOBAL_ *GLOBAL)
{
        char i;
        for (i = 0; i < MAX_LENGTH - 1; ++i)
                GLOBAL->strings[GLOBAL->string_count * 32 + i] = GLOBAL->ID[i];
        GLOBAL->string_count++;
}

createvar(struct _GLOBAL_ *GLOBAL)
{
        char i;
        for (i = 0; i < MAX_LENGTH - 1; ++i)
                GLOBAL->variables[GLOBAL->variable_count + GLOBAL->global_count][i] = GLOBAL->ID[i];
        GLOBAL->ebpoff[GLOBAL->variable_count + GLOBAL->global_count] = GLOBAL->EBPOFF;
        GLOBAL->EBPOFF -= 4;
        GLOBAL->variable_count++;
}

createglobal(struct _GLOBAL_ *GLOBAL)
{
        char i;
        for (i = 0; i < MAX_LENGTH - 1; ++i)
                GLOBAL->variables[GLOBAL->variable_count + GLOBAL->global_count][i] = GLOBAL->ID[i];
        GLOBAL->global_count++;
}

getvar(struct _GLOBAL_ *GLOBAL, char *buf)
{
        int i;
        for (i = 0; i < MAX_N - 1; ++i)
                if (!cmp(buf, GLOBAL->variables[i]))
                        return i;
        return -1;
}

init(struct _GLOBAL_ *GLOBAL)
{
        GLOBAL->label_count = GLOBAL->string_count = GLOBAL->global_count = GLOBAL->variable_count = 0;
        GLOBAL->PUSHBACK = -1;
        GLOBAL->EBPOFF = -4;
}

getc(struct _GLOBAL_ *GLOBAL)
{
        char chr;
        chr = -1;
        if (GLOBAL->PUSHBACK != -1)
        {
                chr = GLOBAL->PUSHBACK;
                GLOBAL->PUSHBACK = -1;
                return chr;
        }
        _read(0, &chr, 1);
        return chr;
}

ungetc(struct _GLOBAL_ *GLOBAL, int c)
{
        GLOBAL->PUSHBACK = c;
}

cmp(a, b) char *a, *b;
{
        while (*a && *b && *a == *b)
        {
                a++, b++;
        }
        return *a - *b;
}

checkkeyword(struct _GLOBAL_ *GLOBAL, int tk)
{
        if (tk != 32)
                return tk;
        if (!cmp(GLOBAL->ID, "if"))
        {
                return 34;
        }
        if (!cmp(GLOBAL->ID, "else"))
        {
                return 35;
        }
        if (!cmp(GLOBAL->ID, "while"))
        {
                return 36;
        }
        if (!cmp(GLOBAL->ID, "static"))
        {
                return 37;
        }
        if (!cmp(GLOBAL->ID, "auto"))
        {
                return 38;
        }
        if (!cmp(GLOBAL->ID, "goto"))
        {
                return 39;
        }
        if (!cmp(GLOBAL->ID, "return"))
        {
                return 40;
        }
        if (!cmp(GLOBAL->ID, "extern"))
        {
                return 41;
        }
        if (!cmp(GLOBAL->ID, "asm"))
        {
                return 42;
        }
        if (!cmp(GLOBAL->ID, "fn"))
        {
                return 43;
        }
        if (!cmp(GLOBAL->ID, "EOF"))
        {
                return 0;
        }

        return tk;
}

tok(struct _GLOBAL_ *GLOBAL)
{
        int tk, chr, i;
        for (i = 0; i < MAX_LENGTH - 1; ++i)
                GLOBAL->ID[i] = 0;
        GLOBAL->NUMBER = i = tk = 0;
        do
        {
                chr = getc(GLOBAL);
        } while (chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r');

        switch (chr)
        {
        case '+':
                tk = 1;
                return tk;
        case '-':
                tk = 2;
                return tk;
        case '*':
                tk = 3;
                return tk;
        case '/':
                tk = 4;
                return tk;
        case '(':
                tk = 5;
                return tk;
        case ')':
                tk = 6;
                return tk;
        case '{':
                tk = 7;
                return tk;
        case '}':
                tk = 8;
                return tk;
        //case '[':
        //        tk = 9;
        //        return tk;
        //case ']':
        //        tk = 10;
        //        return tk;
        //case '!':
        //        tk = 11;
        //        return tk;
        //case '@':
        //        tk = 12;
        //        return tk;
        //case '#':
        //        tk = 13;
        //        return tk;
        //case '$':
        //        tk = 14;
        //        return tk;
        case '%':
                tk = 15;
                return tk;
        //case '^':
        //        tk = 16;
        //        return tk;
        //case '&':
        //        tk = 17;
        //        return tk;
        case '=':
                tk = 18;
                return tk;
        case ';':
                tk = 19;
                return tk;
        case ':':
                tk = 20;
                return tk;
        case '<':
                tk = 21;
                return tk;
        case '>':
                tk = 22;
                return tk;
        case ',':
                tk = 23;
                return tk;
        case '.':
                tk = 24;
                return tk;
        //case '\'':
        //        tk = 26;
        //        return tk;
        //case '\\':
        //        tk = 27;
        //        return tk;
        //case '|':
        //        tk = 28;
        //        return tk;
        //case '?':
        //        tk = 29;
        //        return tk;
        //case '`':
        //        tk = 30;
        //        return tk;
        //case '~':
        //        tk = MAX_LENGTH - 1;
        //        return tk;
        case '"':
                i = 0;
                while (1)
                {
                        chr = getc(GLOBAL);
                        if (chr == '"')
                                break;
                        if (chr == -1)
                        {
                                _perror("Unterminated string", 20);
                                _exit(1);
                        }
                        if (i < MAX_LENGTH - 1)
                        {
                                GLOBAL->ID[i] = chr;
                                i++;
                        }
                }
                GLOBAL->ID[i] = 0;
                tk = 25;
                return tk;
        }

        while ((chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z') || chr == '_')
        {
                tk = 32;
                GLOBAL->ID[i] = chr;
                i = i + 1;
                chr = getc(GLOBAL);
        }

        if (tk)
        {
                if (chr == ':')
                {
                        tk = 64;
                        return tk;
                }
                ungetc(GLOBAL, chr);
                tk = checkkeyword(GLOBAL, tk);
                return tk;
        }

        while ((chr >= '0' && chr <= '9') || chr == '_')
        {
                tk = 33;
                GLOBAL->ID[i] = chr;
                if (chr != '_')
                {
                        GLOBAL->NUMBER *= 10;
                        GLOBAL->NUMBER += chr - '0';
                }
                i = i + 1;
                chr = getc(GLOBAL);
        }

        if (tk)
        {
                ungetc(GLOBAL,chr);
                return tk;
        }

        return tk;
}

assignment_typeB(struct _GLOBAL_ *GLOBAL, int tk, char *buf)
{
        char i;
        tk = expr(GLOBAL,tk);
        i = getvar(GLOBAL, buf);
        if (i == -1)
                _perror("not found\n", 11), _exit(1);
        if (i < GLOBAL->global_count)
        {
                _print("\tlea ");
                _print(buf);
                _print(", %ebx\n");
        }
        else
        {
                _print("\tlea ");
                _print_number(GLOBAL->ebpoff[i]);
                _print("(%ebp), %ebx\n");
        }
        _print("\tpopl (%ebx)\n\tpushl (%ebx)\n");
        return tk;
}

primary(struct _GLOBAL_ *GLOBAL, int tk)
{
        char buf[32], i;
        for (i = 0; i < MAX_LENGTH - 1; ++i)
                buf[i] = GLOBAL->ID[i];
        if (tk == 5)
        {
                tk = expr(GLOBAL,tok(GLOBAL));
                if (tk != 6)
                        _perror("syntax error\n", 14), _exit(1);
                return tok(GLOBAL);
        }
        if (tk == 25)
        {
                _print("\tlea (string");
                _print_number(GLOBAL->string_count);
                _print("), %ebx\n\tpushl %ebx\n");
                createstr(GLOBAL);
                return tok(GLOBAL);
        }
        if (tk == 33)
        {
                _print("\tpushl $");
                _print(GLOBAL->ID);
                _print("\n");
                return tok(GLOBAL);
        }
        if (tk == 32)
        {
                tk = tok(GLOBAL);
                if (tk == 18)
                        return assignment_typeB(GLOBAL, tok(GLOBAL), buf);
                else if (tk == 5)
                {
                        _print("\tpushl %esi\n\tmovl %esp,%esi\n");
                        tk = expr(GLOBAL,tok(GLOBAL));
                        _print("\tcall ");
                        _print(buf);
                        _print("\n\tmovl %esi,%esp\n\tpopl %esi\n\tpush %eax\n");
                        if (tk != 6)
                                _perror("syntax error\n", 14), _exit(1);
                        tk = tok(GLOBAL);
                }
                else
                {
                        i = getvar(GLOBAL, buf);
                        if (i == -1)
                                _perror("not found\n", 11), _exit(1);
                        if (i < GLOBAL->global_count)
                        {
                                _print("\tpushl (");
                                _print(buf);
                                _print(")\n");
                        }
                        else
                        {
                                _print("\tpushl ");
                                _print_number(GLOBAL->ebpoff[i]);
                                _print("(%ebp)\n");
                        }
                }
                return tk;
        }
        return tk;
}

multiplicative(struct _GLOBAL_ *GLOBAL, int tk)
{
        tk = primary(GLOBAL, tk);
        while (tk == 3 || tk == 4 || tk == 15)
        {
                int _ = tk;
                tk = primary(GLOBAL, tok(GLOBAL));
                _print("\tpopl %ebx\n\tpopl %eax\n");
                if (_ == 3)
                        _print("\timul %ebx\n");
                else if (_ == 4)
                {
                        _print("\tcdq\n\tidiv %ebx\n");
                }
                else if (_ == 15)
                {
                        _print("\tcdq\n\tidiv %ebx\n\tmovl %edx, %eax\n");
                }
                _print("\tpushl %eax\n");
        }
        return tk;
}

additive(struct _GLOBAL_ *GLOBAL, int tk)
{
        tk = multiplicative(GLOBAL, tk);
        while (tk == 1 || tk == 2)
        {
                int _ = tk;
                tk = multiplicative(GLOBAL, tok(GLOBAL));
                _print("\tpopl %ebx\n\tpopl %eax\n");
                if (_ == 1)
                        _print("\taddl %ebx, %eax\n");
                else
                        _print("\tsubl %ebx, %eax\n");
                _print("\tpushl %eax\n");
        }
        return tk;
}

relational(struct _GLOBAL_ *GLOBAL, int tk)
{
        tk = additive(GLOBAL, tk);
        while (tk == 21 || tk == 22)
        {
                int _ = tk;
                tk = additive(GLOBAL, tok(GLOBAL));
                _print("\tpopl %ebx\n\tpopl %eax\n\tcmpl %ebx, %eax\n");
                if (_ == 21) /* < */
                        _print("\tsetl %al\n");
                else
                        _print("\tsetg %al\n");
                _print("\tmovzx %al, %eax\n\tpushl %eax\n");
        }
        return tk;
}

assignment_typeA(struct _GLOBAL_ *GLOBAL, int tk)
{
        tk = relational(GLOBAL, tk);
        while (tk == 18)
        {
                tk = relational(GLOBAL, tok(GLOBAL));
                _print("\tpopl %ebx\n\tpopl %eax\n\tmovl %ebx, (%eax)\n");
        }
        return tk;
}

expr(struct _GLOBAL_ *GLOBAL, int tk)
{
        tk = assignment_typeA(GLOBAL, tk);
        while (tk == 23)
        {
                tk = assignment_typeA(GLOBAL, tok(GLOBAL));
        }
        return tk;
}

args(struct _GLOBAL_ *GLOBAL, int tk)
{
        int i, temp;
        GLOBAL->EBPOFF = 8;
        if (tk == 5)
        {
                tk = tok(GLOBAL);
                while (tk != 6)
                {
                        if (tk == 32)
                        {
                                createvar(GLOBAL);
                        }
                        tk = tok(GLOBAL);
                        GLOBAL->EBPOFF += 4;
                }
                tk = tok(GLOBAL);
        }

        int param_start = GLOBAL->global_count;
        int param_end = (GLOBAL->global_count + GLOBAL->variable_count) - 1;

        for (i = 0; i < (param_end - param_start + 1) / 2; i++)
        {
                temp = GLOBAL->ebpoff[param_start + i];
                GLOBAL->ebpoff[param_start + i] = GLOBAL->ebpoff[param_end - i];
                GLOBAL->ebpoff[param_end - i] = temp;
        }

        return tk;
}

statement(struct _GLOBAL_ *GLOBAL, int tk)
{
        char buf[32], i;
        if (tk == 7)
        {
                tk = tok(GLOBAL);
                while (tk != 8)
                {
                        tk = statement(GLOBAL, tk);
                }
                return tok(GLOBAL);
        }
        else if (tk == 41)
        { /* extern */
                do
                {
                        tk = tok(GLOBAL);
                        if (tk == 32)
                        {
                                _print("\t.extern ");
                                _print(GLOBAL->ID);
                                _print("\n");
                        }
                        else
                        {
                                _perror("syntax error\n", 14);
                                _exit(1);
                        }
                        tk = tok(GLOBAL);
                } while (tk == 23);
                if (tk != 19)
                {
                        _perror("syntax error\n", 14);
                        _exit(1);
                }
                return tok(GLOBAL);
        }
        else if (tk == 64)
        { /* Label */
                _print("\t");
                _print(GLOBAL->ID);
                _print(":\n");
                return tok(GLOBAL);
        }
        else if (tk == 42)
        { /* asm */
                tk = tok(GLOBAL);
                if (tk == 25)
                {
                        _print("\t");
                        _print(GLOBAL->ID);
                        _print("\n");
                }
                else
                {
                        _perror("syntax error\n", 14), _exit(1);
                }
                return tok(GLOBAL);
        }
        else if (tk == 34)
        { /* if-else */
                int _then = GLOBAL->label_count++;
                int _else = GLOBAL->label_count++;
                int _end = GLOBAL->label_count++;
                tk = expr(GLOBAL,tok(GLOBAL));
                _print("\tpopl %eax\n\ttestl %eax,%eax\n\tje m");
                _print_number(_else);
                _print("\nm");
                _print_number(_then);
                _print(":\n");
                tk = statement(GLOBAL, tk);
                _print("\tjmp m");
                _print_number(_end);
                _print("\nm");
                _print_number(_else);
                _print(":\n");
                if (tk == 35)
                        tk = statement(GLOBAL, tok(GLOBAL));
                _print("m");
                _print_number(_end);
                _print(":\n");
        }
        else if (tk == 36)
        { /* while */
                int _cond = GLOBAL->label_count++;
                int _end = GLOBAL->label_count++;
                _print("m");
                _print_number(_cond);
                _print(":\n");
                tk = expr(GLOBAL,tok(GLOBAL));
                _print("\tpopl %eax\n\ttestl %eax,%eax\n\tje m");
                _print_number(_end);
                _print("\n");
                tk = statement(GLOBAL, tk);
                _print("\tjmp m");
                _print_number(_cond);
                _print("\n");
                _print("m");
                _print_number(_end);
                _print(":\n");
        }
        else if (tk == 40)
        { /* return */
                tk = tok(GLOBAL);

                if (tk == 19)
                {
                        _print("\tjmp ");
                        _print(GLOBAL->function);
                        _print(".ext\n");
                        return tok(GLOBAL);
                }
                else
                {
                        tk = expr(GLOBAL,tk);
                        if (tk != 19)
                        {
                                _perror("Expected ; after return expression", 35);
                                _exit(1);
                        }
                        _print("\tpopl %eax\n\tjmp ");
                        _print(GLOBAL->function);
                        _print(".ext\n");
                        return tok(GLOBAL);
                }
        }
        else if (tk == 39)
        { /* GOTO */
                tk = tok(GLOBAL);
                if (tk == 32 || tk == 33)
                {
                        _print("\tjmp ");
                        _print(GLOBAL->ID);
                        _print("\n");
                }
                else
                {
                        _perror("syntax error\n", 14), _exit(1);
                }
                return tok(GLOBAL);
        }
        else if (tk == 43)
        { /* fn */
                tk = tok(GLOBAL);
                GLOBAL->variable_count = 0;
                for (i = 0; i < MAX_LENGTH - 1; ++i)
                        GLOBAL->function[i] = buf[i] = GLOBAL->ID[i];
                _print("\tjmp ");
                _print(GLOBAL->ID);
                _print(".aft\n");
                _print(GLOBAL->ID);
                _print(":\n\tpushl %ebp\n\tmovl %esp, %ebp\n");
                tk = args(GLOBAL, tok(GLOBAL));
                GLOBAL->EBPOFF = -4;
                tk = statement(GLOBAL, tk);
                _print(buf);
                _print(".ext:\n\tmovl %ebp, %esp\n\tpopl %ebp\n\tret\n");
                _print(buf);
                _print(".aft:\n");
        }
        else if (tk == 37) /* static */
        {
                do
                {
                        tk = tok(GLOBAL);
                        for (i = 0; i < MAX_LENGTH - 1; ++i)
                                buf[i] = GLOBAL->ID[i];
                        createglobal(GLOBAL);

                        tk = tok(GLOBAL);
                        if (tk == 18)
                        {
                                tk = assignment_typeB(GLOBAL, tok(GLOBAL), buf);
                        }
                } while (tk == 23);

                if (tk != 19)
                {
                        _perror("syntax error\n", 14);
                        _exit(1);
                }
                return tok(GLOBAL);
        }
        else if (tk == 38)
        {
                do
                {
                        tk = tok(GLOBAL);
                        for (i = 0; i < MAX_LENGTH - 1; ++i)
                                buf[i] = GLOBAL->ID[i];
                        createvar(GLOBAL);
                        _print("\tsubl $4, %esp\n");
                        tk = tok(GLOBAL);
                        if (tk == 18)
                        {
                                tk = assignment_typeB(GLOBAL, tok(GLOBAL), buf);
                        }
                } while (tk == 23);

                if (tk != 19)
                {
                        _perror("syntax error\n", 14);
                        _exit(1);
                }
                return tok(GLOBAL);
        }
        else if (tk)
        {
                tk = expr(GLOBAL, tk);
                if (tk != 19)
                {
                        _perror("syntax error\n", 14);
                        _exit(1);
                }
                _print("\tpopl %eax\n");
                return tok(GLOBAL);
        }

        return tk;
}

main(c, v) char **v;
{
        struct _GLOBAL_ GLOBAL;
        init(&GLOBAL);
        char tk, i;
        tk = tok(&GLOBAL);
        _print("\t.global main\n\t.section .text\nmain:\tpushl %ebp\n\tmovl %esp, %ebp\n");
        for (i = 0; i < 6; ++i)
                GLOBAL.ID[i] = "arg_v"[i];
        createglobal(&GLOBAL);
        for (i = 0; i < 6; ++i)
                GLOBAL.ID[i] = "arg_c"[i];
        createglobal(&GLOBAL);
        _print("\tmovl 8(%ebp), %eax\n\tmovl %eax, (arg_c)\n\tmovl 12(%ebp), %eax\n\tmovl %eax, (arg_v)\n");
        while (tk)
                tk = statement(&GLOBAL, tk);
        if (c == 2 && !cmp(v[1], "/Main"))
                _print("\tcall Main\n");
        _print(".ext:\n");
        _print("\tmovl %ebp, %esp\n\tpopl %ebp\n\tpushl %eax\n\tmovl $0x01, %eax\n\tpopl %ebx\n\tint $0x80\n\t.section .data\n");
        for (i = 0; i < GLOBAL.global_count; ++i)
        {
                _print(GLOBAL.variables[i]);
                _print(":\t.long 0\n");
        }
        _print("\t.section .rodata\n");
        for (i = 0; i < GLOBAL.string_count; ++i)
        {
                _print("string");
                _print_number(i);
                _print(":\n\t.ascii \"");
                _print(&GLOBAL.strings[i * 32]);
                _print("\"\n");
        }
        return 0;
}
