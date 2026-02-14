
extern int printf(const char *, ...);
extern int perror(const char *, ...);
extern void exit(int);
extern int read(int, char *, int);

static STDIN, STDOUT, NUMBER, PUSHBACK, EBPOFF;
static char ID[32];
static char variables[255][32], variable_count, global_count;
static char strings[8192][32];
static int ebpoff[255], string_count, label_count;

extern expr(int);

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
        variable_count++, global_count++;
}

getvar(buf) char *buf;
{
        char i;
        for (i = 0; i < 255; ++i)
                if (!cmp(buf, variables[i]))
                        return i;
        return 0;
}

init()
{
        label_count = string_count = global_count = variable_count = 0;
        STDIN = 0;
        STDOUT = 1;
        PUSHBACK = -1;
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
        read(STDIN, &chr, 1);
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
                                perror("Unterminated string");
                                exit(1);
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
        if (i < global_count)
        {
                printf("\tlea %s, %%ebx\n", buf);
        }
        else
        {
                printf("\tlea %d(%%ebp), %%ebx\n", ebpoff[i]);
        }
        printf("\tpopl (%%ebx)\n");
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
                        perror("syntax error\n"), exit(1);
                return tok();
        }
        if (tk == 25)
        {
                printf("\tlea (string%d), %%ebx\n", string_count);
                printf("\tpushl %%ebx\n");
                createstr();
                return tok();
        }
        if (tk == 33)
        {
                printf("\tpushl $0x%x\n", NUMBER);
                return tok();
        }
        if (tk == 32)
        {
                tk = tok();
                if (tk == 18)
                        return assignment_typeB(tok(), buf);
                else if (tk == 5)
                {
                        printf("\tpushl %%esi\n");
                        printf("\tmovl %%esp,%%esi\n");
                        tk = expr(tok());
                        printf("\tcall %s\n", buf);
                        printf("\tmovl %%esi,%%esp\n");
                        printf("\tpopl %%esi\n");
                        printf("\tpush %%eax\n");
                        if (tk != 6)
                                perror("syntax error\n"), exit(1);
                        tk = tok();
                }
                else
                {
                        i = getvar(buf);
                        if (i < global_count)
                                printf("\tpushl (%s)\n", buf);
                        else
                                printf("\tpushl %d(%%ebp)\n", ebpoff[i]);
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
                printf("\tpopl %%ebx\n");
                printf("\tpopl %%eax\n");
                if (_ == 3) 
                        printf("\timul %%ebx\n");
                else if (_ == 4)
                {
                        printf("\tcdq\n");
                        printf("\tidiv %%ebx\n");
                }
                else if (_ == 15)
                {
                        printf("\tcdq\n");
                        printf("\tidiv %%ebx\n");
                        printf("\tmovl %%edx, %%eax\n");
                }
                printf("\tpushl %%eax\n");
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
                printf("\tpopl %%ebx\n");
                printf("\tpopl %%eax\n");
                if (_ == 1)
                        printf("\taddl %%ebx, %%eax\n");
                else
                        printf("\tsubl %%ebx, %%eax\n");
                printf("\tpushl %%eax\n");
        }
        return tk;
}

assignment_typeA(tk)
{
        tk = additive(tk);
        while (tk == 18)
        {
                tk = additive(tok());
                printf("\tpopl %%ebx\n");
                printf("\tpopl %%eax\n");
                printf("\tmovl %%ebx, (%%eax)\n");
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
        EBPOFF = 4;
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
                                printf("\t.extern %s\n", ID);
                        }
                        else
                        {
                                perror("syntax error\n");
                                exit(1);
                        }
                        tk = tok();
                } while (tk == 23);
                if (tk != 19)
                {
                        perror("syntax error\n");
                        exit(1);
                }
                return tok();
        }
        else if (tk == 64)
        { /* Label */
                printf("\t%s:\n", ID);
                return tok();
        }
        else if (tk == 42)
        { /* asm */
                tk = tok();
                if (tk == 25)
                        printf("\t%s\n", ID);
                else
                {
                        perror("syntax error\n"), exit(1);
                }
                return tok();
        }
        else if (tk == 34)
        { /* if-else */
                int _then = label_count++;
                int _else = label_count++;
                int _end = label_count++;
                tk = expr(tok());
                printf("\tpopl %%eax\n");
                printf("\ttestl %%eax,%%eax\n");
                printf("\tje m%d\n", _else);
                printf("m%d:\n", _then);
                tk = statement(tk);
                printf("\tjmp m%d\n", _end);
                printf("m%d:\n", _else);
                if (tk == 35)
                        tk=statement(tok());
                printf("m%d:\n", _end);
        }
        else if (tk == 36)
        { /* while */
                int _cond = label_count++;
                int _end = label_count++;
                tk = expr(tok());
                printf("m%d:\n", _cond);
                printf("\tpopl %%eax\n");
                printf("\ttestl %%eax,%%eax\n");
                printf("\tje m%d\n", _end);
                tk = statement(tk);
                printf("\tjmp m%d\n", _cond);
                printf("m%d:\n", _end);
        }
        else if (tk == 40)
        { /* return */
                tk = tok();

                if (tk == 19)
                { 
                        printf("\tjmp .ext\n");
                        return tok();
                }
                else
                {
                        tk = expr(tk);
                        if (tk != 19)
                        {
                                perror("Expected ; after return expression");
                                exit(1);
                        }
                        printf("\tpopl %%eax\n");
                        printf("\tjmp .ext\n");
                        return tok();
                }
        }
        else if (tk == 39)
        { /* GOTO */
                tk = tok();
                if (tk == 32 || tk == 33)
                        printf("\tjmp %s\n", ID);
                else
                {
                        perror("syntax error\n"), exit(1);
                }
                return tok();
        }
        else if (tk == 43)
        { /* fn */
                tk = tok();
                variable_count = 0;
                for (i = 0; i < 31; ++i)
                        buf[i] = ID[i];
                printf("\tjmp %s.aft\n", ID);
                printf("%s:", ID);
                printf("\tpushl %%ebp\n");
                printf("\tmovl %%esp, %%ebp\n");
                tk = args(tok());
                EBPOFF = -4;
                tk = statement(tk);
                printf("%s.ext:\n", buf);
                printf("\tmovl %%ebp, %%esp\n");
                printf("\tpopl %%ebp\n");
                printf("\tret\n");
                printf("%s.aft:\n", buf);
        }
        else if (tk == 37)
        { /* static */
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

                        if (tk == 23)
                        {
                                continue;
                        }
                } while (tk == 23);

                if (tk != 19)
                {
                        perror("syntax error\n");
                        exit(1);
                }
                return tok();
        }
        else if (tk == 38)
        {
                int cnt = 0;
                do
                {
                        tk = tok();
                        for (i = 0; i < 31; ++i)
                                buf[i] = ID[i];
                        createvar();
                        cnt++;

                        tk = tok();
                        if (tk == 18)
                        {
                                tk = assignment_typeB(tok(), buf);
                        }

                        if (tk == 23)
                        {
                                continue;
                        }
                } while (tk == 23);

                printf("\tsub $%d, %%esp\n", cnt*4);

                if (tk != 19)
                {
                        perror("syntax error\n");
                        exit(1);
                }
                return tok();
        }
        else if (tk)
        {
                tk = expr(tk);
                if (tk != 19)
                {
                        perror("syntax error\n");
                        exit(1);
                }
                printf("\tpopl %%eax\n");
                return tok();
        }

        return tk;
}

main(c, v) char **v;
{
        init();
        char tk = tok(), i;
        printf("\t.global main\n");
        printf("\t.section .text\n");
        printf("main:");
        while (tk)
                tk = statement(tk);
        if (c == 2 && !cmp(v[1], "/Main"))
                printf("\tcall Main\n");
        printf(".ext:\n");
        printf("\tpushl %%eax\n");
        printf("\tmovl $0x01, %%eax\n");
        printf("\tpopl %%ebx\n");
        printf("\tint $0x80\n");
        printf("\t.section .data\n");
        for (i = 0; i < global_count; ++i)
        {
                printf("%s:\t.long 0\n", variables[i]);
        }
        printf("\t.section .rodata\n");
        for (i = 0; i < string_count; ++i)
        {
                printf("string%d:\n\t.ascii \"%s\"\n", i, strings[i]);
        }
        return 0;
}
