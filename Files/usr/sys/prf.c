#
/*
 */

#define KL	0177560

/*
 * Address and structure of the
 * KL-11 console device registers.
 */
struct
{
	int	rsr;
	int	rbr;
	int	xsr;
	int	xbr;
};

/*
 * Scaled down version of C Library printf.
 * Only %s %l %d (==%l) %o are recognized.
 * Used to print diagnostic information
 * directly on console tty.
 * Since it is not interrupt driven,
 * all system activities are pretty much
 * suspended.
 * Printf should not be used for chit-chat.
 */
printf(fmt,x1,x2,x3,x4,x5,x6,x7,x8,x9,xa,xb,xc)
char fmt[];
{
	register char *s;
	register *adx, c;

	adx = &x1;
loop:
	while((c = *fmt++) != '%') {
		if(c == '\0')
			return;
		putchar(c);
	}
	c = *fmt++;
	if(c == 'd' || c == 'l' || c == 'o')
		printn(*adx, c=='o'? 8: 10);
	if(c == 's') {
		s = *adx;
		while(c = *s++)
			putchar(c);
	}
	adx++;
	goto loop;
}

/*
 * Print an unsigned integer in base b.
 */
printn(n, b)
{
	register a;
	
	if (n < 0)
		n = -n;

	if(a = (n / b))
		printn(a, b);
	putchar((n % b) + '0');
}

/*
 * Print a character on console.
 * Attempts to save and restore device
 * status.
 */
putchar(c)
{
	register rc, s;

	rc = c;
	while((KL->xsr&0200) == 0)
		;
	if(rc == 0)
		return;
	s = KL->xsr;
	KL->xsr = 0;
	KL->xbr = rc;
	if(rc == '\n') {
		putchar('\r');
		putchar(0177);
		putchar(0177);
	}
	putchar(0);
	KL->xsr = s;
}