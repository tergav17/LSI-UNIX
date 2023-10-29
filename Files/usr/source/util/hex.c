char    buf[256];

main()
{
        register i, o, c;
        int k;

        /* read 256 byte blocks till EOF from stdin */
        k = 0;
        while((i = read(0, buf, 256)) > 0) {
                for (o = 0; o < i; o++) {
                        c = buf[o];
                        if (0 > c) c = 256 + c;
                        printf("%c%c", tchar(c / 16), tchar(c % 16));
                        if (++k == 40) {
                                printf("\n");
                                k = 0;
                        }
                }
        }
}

tchar(ch)
{
        if (0 <= ch && ch <= 9) return (ch + 48);
        if (10 <= ch && ch <= 15) return ((ch - 10) + 65);

        return 63;
}
