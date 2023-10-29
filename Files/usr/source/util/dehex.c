char    buf[256];
char    wbuf[256];

main()
{
        register i, o, p;
        char c;

        p = 0;
        /* read 256 byte blocks till EOF from stdin */
        while((i = read(0, buf, 256)) > 0) {
                for (o = 0; o < (i-1); o = o + 2) {
                        if (buf[o] < ' ' || buf[o+1] < ' ') continue;

                        wbuf[p++] = (fchar(buf[o]) * 16) + fchar(buf[o+1]);

                        if (p == 256) {
                                write(1, wbuf, 256);
                                p = 0;
                        }
                }
        }

        if (p > 0) write(1, wbuf, p);
}

fchar(ch)
{
        if (48 <= ch && ch <= 57) return (ch - 48);
        if (65 <= ch && ch <= 70) return ((ch + 10) - 65);
        if (97 <= ch && ch <= 102) return ((ch + 10) - 97);

        return 0;
}
