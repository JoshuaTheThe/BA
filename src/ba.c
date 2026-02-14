
extern int _print(const char *);
extern int _perror(const char *, int);
extern void _exit(int);
extern int _read(int, char *, int);
extern expr(int);

int STDIN, STDOUT, NUMBER, PUSHBACK, EBPOFF;
char ID[32];
char variables[256][32];
char strings[256][32], function[32];
int ebpoff[256], string_count, label_count, variable_count, global_count;

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

createstr()
{
        char i;
        for (i = 0; i < 31; ++i)
                strings[string_count][i] = ID[i];
        string_count++;
}

createvar()
{
        char i;
        for (i = 0; i < 31; ++i)
                variables[variable_count + global_count][i] = ID[i];
        ebpoff[variable_count + global_count] = EBPOFF;
        EBPOFF -= 4;
        variable_count++;
}

createglobal()
{
        char i;
        for (i = 0; i < 31; ++i)
                variables[variable_count + global_count][i] = ID[i];
        global_count++;
}

getvar(buf) char *buf;
{
        int i;
        for (i = 0; i < 255; ++i)
                if (!cmp(buf, variables[i]))
                        return i;
        return -1;
}

init()
{
        label_count = string_count = global_count = variable_count = 0;
        STDIN = 0;
        STDOUT = 1;
        PUSHBACK = -1;
        EBPOFF = -4;
}

getc()
{
        char chr;
        chr = -1;
        if (PUSHBACK != -1)
        {
                chr = PUSHBACK;
                PUSHBACK = -1;
                return chr;
        }
        _read(STDIN, &chr, 1);
        return chr;
}

ungetc(c)
{
        PUSHBACK = c;
}

cmp(a, b) char *a, *b;
{
        while (*a && *b && *a == *b)
        {
                a++, b++;
        }
        return *a - *b;
}

checkkeyword(tk)
{
        if (tk != 32)
                return tk;
        if (!cmp(ID, "if"))
        {
                return 34;
        }
        if (!cmp(ID, "else"))
        {
                return 35;
        }
        if (!cmp(ID, "while"))
        {
                return 36;
        }
        if (!cmp(ID, "static"))
        {
                return 37;
        }
        if (!cmp(ID, "auto"))
        {
                return 38;
        }
        if (!cmp(ID, "goto"))
        {
                return 39;
        }
        if (!cmp(ID, "return"))
        {
                return 40;
        }
        if (!cmp(ID, "extern"))
        {
                return 41;
        }
        if (!cmp(ID, "asm"))
        {
                return 42;
        }
        if (!cmp(ID, "fn"))
        {
                return 43;
        }
        if (!cmp(ID, "EOF"))
        {
                return 0;
        }

        return tk;
}

tok()
{
        int tk, chr, i;
        for (i = 0; i < 31; ++i)
                ID[i] = 0;
        NUMBER = i = tk = 0;
        do
        {
                chr = getc();
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
        case '[':
                tk = 9;
                return tk;
        case ']':
                tk = 10;
                return tk;
        case '!':
                tk = 11;
                return tk;
        case '@':
                tk = 12;
                return tk;
        case '#':
                tk = 13;
                return tk;
        case '$':
                tk = 14;
                return tk;
        case '%':
                tk = 15;
                return tk;
        case '^':
                tk = 16;
                return tk;
        case '&':
                tk = 17;
                return tk;
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
        case '\'':
                tk = 26;
                return tk;
        case '\\':
                tk = 27;
                return tk;
        case '|':
                tk = 28;
                return tk;
        case '?':
                tk = 29;
                return tk;
        case '`':
                tk = 30;
                return tk;
        case '~':
                tk = 31;
                return tk;
        case '"':
                i = 0;
                while (1)
                {
                        chr = getc();
                        if (chr == '"')
                                break;
                        if (chr == -1)
                        {
                                _perror("Unterminated string", 20);
                                _exit(1);
                        }
                        if (i < 31)
                        {
                                ID[i] = chr;
                                i++;
                        }
                }
                ID[i] = 0;
                tk = 25;
                return tk;
        }

        while ((chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z') || chr == '_')
        {
                tk = 32;
                ID[i] = chr;
                i = i + 1;
                chr = getc();
        }

        if (tk)
        {
                if (chr == ':')
                {
                        tk = 64;
                        return tk;
                }
                ungetc(chr);
                tk = checkkeyword(tk);
                return tk;
        }

        while ((chr >= '0' && chr <= '9') || chr == '_')
        {
                tk = 33;
                ID[i] = chr;
                if (chr != '_')
                {
                        NUMBER *= 10;
                        NUMBER += chr - '0';
                }
                i = i + 1;
                chr = getc();
        }

        if (tk)
        {
                ungetc(chr);
                return tk;
        }

        return tk;
}

assignment_typeB(tk, buf) char *buf;
{
        char i;
        tk = expr(tk);
        i = getvar(buf);
        if (i == -1)
                _perror("not found\n", 11), _exit(1);
        if (i < global_count)
        {
                _print("\tlea ");
                _print(buf);
                _print(", %ebx\n");
        }
        else
        {
                _print("\tlea ");
                _print_number(ebpoff[i]);
                _print("(%ebp), %ebx\n");
        }
        _print("\tpopl (%ebx)\n");
        _print("\tpushl (%ebx)\n");
        return tk;
}

primary(tk)
{
        char buf[32], i;
        for (i = 0; i < 31; ++i)
                buf[i] = ID[i];
        if (tk == 5)
        {
                tk = expr(tok());
                if (tk != 6)
                        _perror("syntax error\n", 14), _exit(1);
                return tok();
        }
        if (tk == 25)
        {
                _print("\tlea (string");
                _print_number(string_count);
                _print("), %ebx\n");
                _print("\tpushl %ebx\n");
                createstr();
                return tok();
        }
        if (tk == 33)
        {
                _print("\tpushl $");
                _print(ID);
                _print("\n");
                return tok();
        }
        if (tk == 32)
        {
                tk = tok();
                if (tk == 18)
                        return assignment_typeB(tok(), buf);
                else if (tk == 5)
                {
                        _print("\tpushl %esi\n");
                        _print("\tmovl %esp,%esi\n");
                        tk = expr(tok());
                        _print("\tcall ");
                        _print(buf);
                        _print("\n\tmovl %esi,%esp\n");
                        _print("\tpopl %esi\n");
                        _print("\tpush %eax\n");
                        if (tk != 6)
                                _perror("syntax error\n", 14), _exit(1);
                        tk = tok();
                }
                else
                {
                        i = getvar(buf);
                        if (i == -1)
                                _perror("not found\n", 11), _exit(1);
                        if (i < global_count)
                        {
                                _print("\tpushl (");
                                _print(buf);
                                _print(")\n");
                        }
                        else
                        {
                                _print("\tpushl ");
                                _print_number(ebpoff[i]);
                                _print("(%ebp)\n");
                        }
                }
                return tk;
        }
        return tk;
}

multiplicative(tk)
{
        tk = primary(tk);
        while (tk == 3 || tk == 4 || tk == 15)
        {
                int _ = tk;
                tk = primary(tok());
                _print("\tpopl %ebx\n");
                _print("\tpopl %eax\n");
                if (_ == 3)
                        _print("\timul %ebx\n");
                else if (_ == 4)
                {
                        _print("\tcdq\n");
                        _print("\tidiv %ebx\n");
                }
                else if (_ == 15)
                {
                        _print("\tcdq\n");
                        _print("\tidiv %ebx\n");
                        _print("\tmovl %edx, %eax\n");
                }
                _print("\tpushl %eax\n");
        }
        return tk;
}

additive(tk)
{
        tk = multiplicative(tk);
        while (tk == 1 || tk == 2)
        {
                int _ = tk;
                tk = multiplicative(tok());
                _print("\tpopl %ebx\n");
                _print("\tpopl %eax\n");
                if (_ == 1)
                        _print("\taddl %ebx, %eax\n");
                else
                        _print("\tsubl %ebx, %eax\n");
                _print("\tpushl %eax\n");
        }
        return tk;
}

relational(tk)
{
        tk = additive(tk);
        while (tk == 21 || tk == 22)
        {
                int _ = tk;
                tk = additive(tok());
                _print("\tpopl %ebx\n");
                _print("\tpopl %eax\n");
                _print("\tcmpl %ebx, %eax\n");
                if (_ == 21) /* < */
                        _print("\tsetl %al\n");
                else
                        _print("\tsetg %al\n");
                _print("\tmovzx %al, %eax\n");
                _print("\tpushl %eax\n");
        }
        return tk;
}

assignment_typeA(tk)
{
        tk = relational(tk);
        while (tk == 18)
        {
                tk = relational(tok());
                _print("\tpopl %ebx\n");
                _print("\tpopl %eax\n");
                _print("\tmovl %ebx, (%eax)\n");
        }
        return tk;
}

expr(tk)
{
        tk = assignment_typeA(tk);
        while (tk == 23)
        {
                tk = assignment_typeA(tok());
        }
        return tk;
}

args(tk)
{
        int i, temp;
        EBPOFF = 8;
        if (tk == 5)
        {
                tk = tok();
                while (tk != 6)
                {
                        if (tk == 32)
                        {
                                createvar();
                        }
                        tk = tok();
                        EBPOFF += 4;
                }
                tk = tok();
        }

        int param_start = global_count;
        int param_end = (global_count + variable_count) - 1;

        for (i = 0; i < (param_end - param_start + 1) / 2; i++)
        {
                temp = ebpoff[param_start + i];
                ebpoff[param_start + i] = ebpoff[param_end - i];
                ebpoff[param_end - i] = temp;
        }

        return tk;
}

statement(tk)
{
        char buf[32], i;
        if (tk == 7)
        {
                tk = tok();
                while (tk != 8)
                {
                        tk = statement(tk);
                }
                return tok();
        }
        else if (tk == 41)
        { /* extern */
                do
                {
                        tk = tok();
                        if (tk == 32)
                        {
                                _print("\t.extern ");
                                _print(ID);
                                _print("\n");
                        }
                        else
                        {
                                _perror("syntax error\n", 14);
                                _exit(1);
                        }
                        tk = tok();
                } while (tk == 23);
                if (tk != 19)
                {
                        _perror("syntax error\n", 14);
                        _exit(1);
                }
                return tok();
        }
        else if (tk == 64)
        { /* Label */
                _print("\t");
                _print(ID);
                _print(":\n");
                return tok();
        }
        else if (tk == 42)
        { /* asm */
                tk = tok();
                if (tk == 25)
                {
                        _print("\t");
                        _print(ID);
                        _print("\n");
                }
                else
                {
                        _perror("syntax error\n", 14), _exit(1);
                }
                return tok();
        }
        else if (tk == 34)
        { /* if-else */
                int _then = label_count++;
                int _else = label_count++;
                int _end = label_count++;
                tk = expr(tok());
                _print("\tpopl %eax\n");
                _print("\ttestl %eax,%eax\n");
                _print("\tje m");
                _print_number(_else);
                _print("\nm");
                _print_number(_then);
                _print(":\n");
                tk = statement(tk);
                _print("\tjmp m");
                _print_number(_end);
                _print("\nm");
                _print_number(_else);
                _print(":\n");
                if (tk == 35)
                        tk = statement(tok());
                _print("m");
                _print_number(_end);
                _print(":\n");
        }
        else if (tk == 36)
        { /* while */
                int _cond = label_count++;
                int _end = label_count++;
                _print("m");
                _print_number(_cond);
                _print(":\n");
                tk = expr(tok());
                _print("\tpopl %eax\n");
                _print("\ttestl %eax,%eax\n");
                _print("\tje m");
                _print_number(_end);
                _print("\n");
                tk = statement(tk);
                _print("\tjmp m");
                _print_number(_cond);
                _print("\n");
                _print("m");
                _print_number(_end);
                _print(":\n");
        }
        else if (tk == 40)
        { /* return */
                tk = tok();

                if (tk == 19)
                {
                        _print("\tjmp ");
                        _print(function);
                        _print(".ext\n");
                        return tok();
                }
                else
                {
                        tk = expr(tk);
                        if (tk != 19)
                        {
                                _perror("Expected ; after return expression", 35);
                                _exit(1);
                        }
                        _print("\tpopl %eax\n");
                        _print("\tjmp ");
                        _print(function);
                        _print(".ext\n");
                        return tok();
                }
        }
        else if (tk == 39)
        { /* GOTO */
                tk = tok();
                if (tk == 32 || tk == 33)
                {
                        _print("\tjmp ");
                        _print(ID);
                        _print("\n");
                }
                else
                {
                        _perror("syntax error\n", 14), _exit(1);
                }
                return tok();
        }
        else if (tk == 43)
        { /* fn */
                tk = tok();
                variable_count = 0;
                for (i = 0; i < 31; ++i)
                        function[i] = buf[i] = ID[i];
                _print("\tjmp ");
                _print(ID);
                _print(".aft\n");
                _print(ID);
                _print(":\n");
                _print("\tpushl %ebp\n");
                _print("\tmovl %esp, %ebp\n");
                tk = args(tok());
                EBPOFF = -4;
                tk = statement(tk);
                _print(buf);
                _print(".ext:\n");
                _print("\tmovl %ebp, %esp\n");
                _print("\tpopl %ebp\n");
                _print("\tret\n");
                _print(buf);
                _print(".aft:\n");
        }
        else if (tk == 37) /* static */
        {
                do
                {
                        tk = tok();
                        for (i = 0; i < 31; ++i)
                                buf[i] = ID[i];
                        createglobal();

                        tk = tok();
                        if (tk == 18)
                        {
                                tk = assignment_typeB(tok(), buf);
                        }
                } while (tk == 23);

                if (tk != 19)
                {
                        _perror("syntax error\n", 14);
                        _exit(1);
                }
                return tok();
        }
        else if (tk == 38)
        {
                do
                {
                        tk = tok();
                        for (i = 0; i < 31; ++i)
                                buf[i] = ID[i];
                        createvar();
                        _print("\tsubl $4, %esp\n");
                        tk = tok();
                        if (tk == 18)
                        {
                                tk = assignment_typeB(tok(), buf);
                        }
                } while (tk == 23);

                if (tk != 19)
                {
                        _perror("syntax error\n", 14);
                        _exit(1);
                }
                return tok();
        }
        else if (tk)
        {
                tk = expr(tk);
                if (tk != 19)
                {
                        _perror("syntax error\n", 14);
                        _exit(1);
                }
                _print("\tpopl %eax\n");
                return tok();
        }

        return tk;
}

main(c, v) char **v;
{
        init();
        char tk, i;
        tk = tok();
        _print("\t.global main\n");
        _print("\t.section .text\n");
        _print("main:");
        _print("\tpushl %ebp\n");
        _print("\tmovl %esp, %ebp\n");

        for (i = 0; i < 6; ++i)
                ID[i] = "arg_v"[i];
        createglobal();
        for (i = 0; i < 6; ++i)
                ID[i] = "arg_c"[i];
        createglobal();
        _print("\tmovl 8(%ebp), %eax\n");
        _print("\tmovl %eax, (arg_c)\n");
        _print("\tmovl 12(%ebp), %eax\n");
        _print("\tmovl %eax, (arg_v)\n");
        while (tk)
                tk = statement(tk);
        if (c == 2 && !cmp(v[1], "/Main"))
                _print("\tcall Main\n");
        _print(".ext:\n");
        _print("\tmovl %ebp, %esp\n");
        _print("\tpopl %ebp\n");
        _print("\tpushl %eax\n");
        _print("\tmovl $0x01, %eax\n");
        _print("\tpopl %ebx\n");
        _print("\tint $0x80\n");
        _print("\t.section .data\n");
        for (i = 0; i < global_count; ++i)
        {
                _print(variables[i]);
                _print(":\t.long 0\n");
        }
        _print("\t.section .rodata\n");
        for (i = 0; i < string_count; ++i)
        {
                _print("string");
                _print_number(i);
                _print(":\n\t.ascii \"");
                _print(strings[i]);
                _print("\"\n");
        }
        return 0;
}
