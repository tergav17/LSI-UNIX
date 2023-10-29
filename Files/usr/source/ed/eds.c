#

/*
 * Editor, Screen
 */

#define	SIGHUP	1
#define	SIGINTR	2
#define	SIGQUIT	3
#define	FNSIZE	64
#define	LBSIZE	512
#define	ESIZE	128
#define	GBSIZE	256
#define	NBRA	5
#define	EOF	-1

#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12

#define	STAR	01

#define	error	goto errlab
#define	READ	0
#define	WRITE	1

int	peekc;
int	lastc;
char	file[FNSIZE];
char	linebuf[LBSIZE];
char	expbuf[ESIZE+4];
int	circfl;
int	*zero;
int	*dot;
int	*dol;
int	*endcore;
int	*fendcore;
int	*addr1;
int	*addr2;
char	genbuf[LBSIZE];
int	count[2];
char	*nextip;
char	*linebp;
int	ninbuf;
int	io;
int	pflag;
int	onhup;
int	onquit;
int	vflag	1;
int	listf;
int	col;
char	*globp;
int	tfile	-1;
int	tline;
char	*tfname;
char	*loc1;
char	*loc2;
char	*locs;
char	ibuff[512];
int	iblock	-1;
char	obuff[512];
int	oblock	-1;
int	ichanged;
int	nleft;
int	errfunc();
int	*errlab	errfunc;
char	TMPERR[] "TMP";
int	names[26];
char	*braslist[NBRA];
char	*braelist[NBRA];

/* display buffer varaibles */
char	dispbuf[82];
char	doeof;
int	split;
char	null;
int	displine;
int	xscroll;
char	ledbuf[LBSIZE];

/* line state */
int	ledline;
int	ledlen;

/* cursor state */
int	curpos;
int	curinx;
int	curdes;

/* TTY mode state */
int	mode[3];
int	newmode[3];


main(argc, argv)
char **argv;
{
	register char *p1, *p2;
	int i;
	int getfile();
	
	null = 0;
	curdes = 0;
	
	/* handle args */
	argv++;
	if (argc>1) 
		p1 = *argv;
	else
		p1 = "untitled";
	
	p2 = file;
	while (*p2++ = *p1++);
	
	/* init stuff */
	fendcore = sbrk(0);
	init();
	
	/* read in the file if it exists */
	addr2 = 0;
	setall();
	ninbuf = 0;
	if ((io = open(file, 0)) > 0) {
		append(getfile, addr2);
		close(io);
		io = -1;
	}
	

	/* prepare screen for graphics */
	displine = ledline = 1;
	xscroll = curpos = 0;
	
	clears();
	mkstat();
	
	/* get mode of terminal */
	gtty(1, mode);
	for (i = 0; i < 3; i++) newmode[i] = mode[i];
	
	/* set terminal to raw no echo */
	newmode[2] = (newmode[2] | 000040) & ~000010;
	stty(1, newmode);

	/* set up line editor */
	openline(ledline);
	setcur(0);

	/* draw screen */
	mklines(0);

	/* enter command mode */
	gotos(0, 0);
	command();
	
	/* exit conditions */
	clears();
	gotos(0, 0);
	clean();
	
}

/* enters the command mode */
command()
{
	char c, com, op, doline;
	int n;
	
	doline = com = 0;
	
	while (read(0, &c, 1) > 0) {
		
		/* handle goto line */
		if (doline) {
			if (c >= '0' && c <= '9') {
				n = n * 10;
				n =+ c - '0';

				if (n > 9999) n = 0;
				upstat(n);
			} else {
				if (n < 1) n = 1;
				
				/* hack for the pointer related bs */
				while (zero + n > dol) n--;
				
				/* go to that line */
				closeline();
				openline(n);
				
				/* and exit */
				doline = 0;
			}
		}
		
		/* escape command */
		else if (com) {
			/* 2nd stage command */
			if (com == 2) {
				com = 0; 
				
				/* Right Arrow */
				if (op == '[' && c == 'C') {
					if (curinx >= ledlen && zero + ledline < dol) {
						closeline();
						openline(++ledline);
						setcur(0);
						
					}
					else
						setcur(++curinx);
					curdes = curpos;
				}
				
				/* Left Arrow */
				if (op == '[' && c == 'D') {
					if (!curinx && ledline > 1) {
						closeline();
						openline(--ledline);
						setcur(999);
					}	
					else 
						setcur(--curinx);
					curdes = curpos;
				}
				
				/* Up Arrow */
				if (op == '[' && c == 'A' && ledline > 1) {
					closeline();
					openline(--ledline);
				}
				
				/* Down Arrow */
				if (op == '[' && c == 'B' && zero + ledline < dol) {
					closeline();
					openline(++ledline);
				}
			} 
			/* 1st stage command */
			else {
				op = c;
				com++;
			}
		} 
		/* normal level command */
		else {
			/* CTRL-Q */
			if (c == 17)
				return;
			
			/* CTRL-R */
			if (c == 18) {
				mkstat();
				upstat(ledline);
				mklines(0);
				setcur(curinx);
			}
			
			/* CTRL-A */
			if (c == 1) {
				setcur(0);
				curdes = curpos;
			}
			
			/* CRTL-Z */
			if (c == 26) {
				setcur(999);
				curdes = curpos;
			}
			
			/* CTRL-G */
			if (c == 7) {
				n = 0;
				doline = 1;
			}
			
			/* CTRL-W */
			if (c == 23) {
				/* save operation */
				closeline();
				
				addr2 = 0;
				setall();
				nonzero();
				if ((io = creat(file, 0666)) < 0)
					error;
				
				putfile();
				close(io);
				io = -1;
				
				/* reopen line */
				openline(ledline);
				
				/* print a little message */
				gotos(76, 23);
				puts("DONE");
				
				/* reset cursor */
				setcur(curinx);
			}
			
			/* RETURN */
			if (c == 10) 
				enter();
			
			/* DEL or BS */
			if (c == 127 || c == 8)
				remove();
			
			
			/* Insert Character */
			if ((c >= ' ' &&  c <= '~') || c == 9) {
				insert(c); 
				curdes = curpos;
			}
		}
		
		/* ESC */
		if (c == 27) {
			com = 1;
			op = 0;
		}
		
	}
	
}

/* creates a new line, and splits lines */
enter()
{
	register i, t;
	int getdbuf();
	
	/* count tabs */
	t = 0;
	while (ledbuf[t] == 9) t++;

	/* add break, and save */
	insert(0);
	split = ++curinx;
	
	addr1 = addr2 = zero + ledline;
	doeof = 0;
	
	/* replace line */
	if (addr1 <= dol) {
		delete();
		append(getdbuf, addr1-1);
	}
	/* append line */
	else {
		append(getdbuf, dol);
	}
	
	mklines(ledline - displine);
	openline(++ledline);
	
	setcur(0);

	/* add the tabs */
	while (t--) insert(9);
	curdes = curpos;
}

/* removes a character behind the cursor */
remove()
{
	register i;
	register char *p;
	int o;
	
	/* check if a line deletion should occur */
	if (!curinx) {
		if (ledline <= 1) return;
		
		closeline();
		openline(--ledline);
		
		i = 0;
		while (ledbuf[i]) i++;
		o = i;
		
		p = getline(*(zero + ledline + 1));
		
		while (*p) {
			ledbuf[i++] = *p;
			ledlen++;
			p++;
		}
		ledbuf[i] = 0;
		
		/* delete line */
		addr1 = addr2 = zero + ledline + 1;
		delete();
		
		/* update screen */
		mklines(ledline - displine);
		setcur(o);
		curdes = curpos;
		
		return;
	}
	
	i = curinx;
	
	while (ledbuf[i-1]) {
		ledbuf[i-1] = ledbuf[i];
		i++;
	}
	
	setcur(--curinx);
	drawline(ledline, curpos+xscroll, 80-curpos);
	gotos(curpos, ledline - displine);
	curdes = curpos;
}

/* inserts a char at the cursor */
insert(c)
char c;
{
	register i;
	
	i = ++ledlen;
	
	while (i > curinx) {
		ledbuf[i] = ledbuf[i-1];
		i--;
	}
	
	ledbuf[i] = c;
	
	drawline(ledline, curpos+xscroll, 80-curpos);
	setcur(++curinx);
}

/* sets the cursor to a specific index */
setcur(c)
{
	register p, i;
	int line;
	char redraw;
	
	line = ledline - displine;
	i = p = 0;
	redraw = 0;
	
	/* no negatives */
	if (c < 0) c = 0;
	
	while (ledbuf[i] && i != c) {
		
		/* handle tabs */
		if (ledbuf[i] == 9)
			p =+ 8 - (p % 8);
		else
			p++;
		
		i++;
	}
	
	/* correct position for scrolling */
	curpos = p - xscroll;
	
	while (curpos < 0) {
		xscroll =- 40;
		curpos = p - xscroll;
		redraw = 1;
	}
	
	while (curpos >= 80) {
		xscroll =+ 40;
		curpos = p - xscroll;
		redraw = 1;
	}

	/* redraw if scrolled */
	if (redraw) mklines(0);	
	
	curinx = i;
	
	gotos(curpos, line);
}

/* calculates an index based on desired position */
calcinx(p)
{
	register i, l;
	
	l = i = 0;
	while (ledbuf[i]) {
		if (l >= p) return i;
		
		/* handle tabs */
		if (ledbuf[i] == 9)
			l =+ 8 - (l % 8);
		else
			l++;
		i++;
	}
	
	/* go to the end of the line */
	return 1024;
}

/* returns the buffer, then EOF */
getbuf() {
	register i;
	if (doeof) return EOF;
	
	doeof = 1;
	i = 0;
	while (ledbuf[i]) {
		linebuf[i] = ledbuf[i];
		i++;
	}
	linebuf[i] = 0;
	return 0;
}

/* returns the buffer, then a second buffer, then EOF */
getdbuf() {
	register i, p;
	if (doeof == 2) return EOF;
	
	p = 0;
	if (!doeof) {
		doeof = 1;
		i = 0;
	} else {
		doeof = 2;
		i = split;
	}
	
	while (ledbuf[i]) {
			linebuf[p++] = ledbuf[i++];
	}
	linebuf[p] = 0;
	
	return 0;
}

/* generates a blank line for append */
getblank() {
	if (doeof) return EOF;
	
	doeof = 1;
	linebuf[0] = 0;
	return 0;
}

/* takes the current line, and puts it into editor buffer */
closeline(s)
{	
	addr1 = addr2 = zero + ledline;
	doeof = 0;
	
	/* replace line */
	if (addr1 <= dol) {
		delete();
		append(getbuf, addr1-1);
	}
	/* append line */
	else {
		append(getbuf, dol);
	}
}

/* sets the cursor to a line, and buffers it */
openline(l)
{
	register char *p;
	register i;
	
	ledline = l;
	ledlen = 0;
	upstat(l);
	
	/* scan line into buffer */
	if (zero + l <= dol) {
		p = getline(*(zero + l));
		for (i = 0; *(p + i); i++) ledbuf[i] = *(p + i);
		ledbuf[i] = 0;
		ledlen = i;
	}
	
	if (zero + l == dol + 1) {
		ledbuf[0] = 0;
	}
	
	/* if it is offsceen, scroll to it */
	if (l > displine + 21) {
		displine = l - 21;
		mklines(0);
	}
	
	if (l < displine) {
		displine = l;
		mklines(0);
	}
	
	/* go to the new line */
	i = calcinx(curdes + xscroll);
	setcur(i);
}

/* draw all of the lines at once */
mklines(s)
{
	register i;
	
	for (i = s; i < 22; i++) {
		gotos(0, i);
		drawline(displine + i, xscroll, 80);
	}
}

/* draws a line at the cursor */
drawline(l, o, e)
{
	register char *p, c;
	register i;
	int pos;

	p = 0;
	
	if (l == ledline)
		p = ledbuf;
	
	else if (zero + l <= dol)
		p = getline(*(zero + l));
	
	i = 1;
	
	if (p) {
		
		i = -o;
		pos = 0;
		
		while (*p && i < e) {
			c = *p;
			
			/* tab handling */
			if (c == 9) {
				c = ' ';
				if (pos % 8 == 7) p++;
				
			} 
			else 
				p++;
			
			if (i >= 0) dispbuf[i] = c;
			pos++;
			i++;
		}
		
		/* ensure index is above 0 */
		if (i < 0) 
			i = 0;
	}
	else 
		dispbuf[0] = '~';
	
	/* clear out rest of line if contents do not fill it */
	if (i < e) {
		dispbuf[i++] = 27;
		dispbuf[i++] = '[';
		dispbuf[i++] = 'K';
	}
	
	write(1, dispbuf, i);
}

/* init line editor code */
init()
{
	register char *p;
	register pid;

	close(tfile);
	tline = 0;
	iblock = -1;
	oblock = -1;
	tfname = "/tmp/exxxxx";
	ichanged = 0;
	pid = getpid();
	for (p = &tfname[11]; p > &tfname[6];) {
		*--p = (pid&07) + '0';
		pid =>> 3;
	}
	close(creat(tfname, 0600));
	tfile = open(tfname, 2);
	brk(fendcore);
	dot = zero = dol = fendcore;
	endcore = fendcore - 2;
}

/*
 * general addressing operations
 */

setdot()
{
	if (addr2 == 0)
		addr1 = addr2 = dot;
	if (addr1 > addr2)
		error;
}

setall()
{
	if (addr2==0) {
		addr1 = zero+1;
		addr2 = dol;
		if (dol==zero)
			addr1 = zero;
	}
	setdot();
}

setnoaddr()
{
	if (addr2)
		error;
}

nonzero()
{
	if (addr1<=zero || addr2>dol)
		error;
}

/* delete a line */ 
delete()
{
	register *a1, *a2, *a3;

	setdot();
	/* newline(); */
	nonzero();
	a1 = addr1;
	a2 = addr2+1;
	a3 = dol;
	dol =- a2 - a1;
	do
		*a1++ = *a2++;
	while (a2 <= a3);
	a1 = addr1;
	if (a1 > dol)
		a1 = dol;
	dot = a1;
}

/* error handler */
errfunc()
{
	clears();
	gotos(0, 0);
	puts("catastrophic error\n");

	if (io > 0) {
		close(io);
		io = -1;
	}
	clean();
}

/* gets a line from magic land */
getline(tl)
{
	register char *bp, *lp;
	register nl;

	lp = linebuf;
	bp = getblock(tl, READ);
	nl = nleft;
	tl =& ~0377;
	while (*lp++ = *bp++)
		if (--nl == 0) {
			bp = getblock(tl=+0400, READ);
			nl = nleft;
		}
	return(linebuf);
}

/* reads in files for append() */
getfile()
{
	register c;
	register char *lp, *fp;

	lp = linebuf;
	fp = nextip;
	do {
		if (--ninbuf < 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) < 0)
				return(EOF);
			fp = genbuf;
		}
		if (lp >= &linebuf[LBSIZE])
			error;
		if ((*lp++ = c = *fp++ & 0177) == 0) {
			lp--;
			continue;
		}
		if (++count[1] == 0)
			++count[0];
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
	return(0);
}

/* puts stuff in the file */
putfile()
{
	int *a1;
	register char *fp, *lp;
	register nib;

	nib = 512;
	fp = genbuf;
	a1 = addr1;
	do {
		lp = getline(*a1++);
		for (;;) {
			if (--nib < 0) {
				write(io, genbuf, fp-genbuf);
				nib = 511;
				fp = genbuf;
			}
			if (++count[1] == 0)
				++count[0];
			if ((*fp++ = *lp++) == 0) {
				fp[-1] = '\n';
				break;
			}
		}
	} while (a1 <= addr2);
	write(io, genbuf, fp-genbuf);
}

/* appends lines and stuff */
append(f, a)
int (*f)();
{
	register *a1, *a2, *rdot;
	int nline, tl;
	struct { int integer; };

	nline = 0;
	dot = a;
	while ((*f)() == 0) {
		if (dol >= endcore) {
			if (sbrk(1024) == -1)
				error;
			endcore.integer =+ 1024;
		}
		tl = putline();
		nline++;
		a1 = ++dol;
		a2 = a1+1;
		rdot = ++dot;
		while (a1 > rdot)
			*--a2 = *--a1;
		*rdot = tl;
	}
	return(nline);
}

/* puts a line in the buffer */
putline()
{
	register char *bp, *lp;
	register nl;
	int tl;

	lp = linebuf;
	tl = tline;
	bp = getblock(tl, WRITE);
	nl = nleft;
	tl =& ~0377;
	while (*bp = *lp++) {
		if (*bp++ == '\n') {
			*--bp = 0;
			linebp = lp;
			break;
		}
		if (--nl == 0) {
			bp = getblock(tl=+0400, WRITE);
			nl = nleft;
		}
	}
	nl = tline;
	tline =+ (((lp-linebuf)+03)>>1)&077776;
	return(nl);
}

/* get block from tmp file */
getblock(atl, iof)
{
	extern read(), write();
	register bno, off;
	
	bno = (atl>>8)&0377;
	off = (atl<<1)&0774;
	if (bno >= 255) {
		puts(TMPERR);
		error;
	}
	nleft = 512 - off;
	if (bno==iblock) {
		ichanged =| iof;
		return(ibuff+off);
	}
	if (bno==oblock)
		return(obuff+off);
	if (iof==READ) {
		if (ichanged)
			blkio(iblock, ibuff, write);
		ichanged = 0;
		iblock = bno;
		blkio(bno, ibuff, read);
		return(ibuff+off);
	}
	if (oblock>=0)
		blkio(oblock, obuff, write);
	oblock = bno;
	return(obuff+off);
}

/* block I/O */
blkio(b, buf, iofcn)
int (*iofcn)();
{
	seek(tfile, b, 3);
	if ((*iofcn)(tfile, buf, 512) != 512) {
		puts(TMPERR);
		error;
	}
}

/* does a clean exit, fixes everything */
clean()
{
	stty(1, mode);
	unlink(tfname);
	exit();
}

/* draw status bar */
mkstat()
{
	int i;
	
	gotos(0, 22);
	for (i = 0; i < 8; i++) puts("----------");
	gotos(0, 23);
	puts(file);
	upstat(0);
}

/* update status bar */
upstat(l)
{
	register *p;
	p = "0000";
	puti(p, l);
	
	gotos(76, 23);
	puts(p);
}

/* clear screen */
clears()
{
	puts("\033[2J");
}

/* goto on screen */
gotos(x, y)
char x;
char y;
{
	char *p;
	
	p = "\033[00;00H";
	x++; y++;
	
	putb(p+2, y);
	putb(p+5, x);
	puts(p);
}

/* puts byte in string */
putb(s, v)
char *s;
{
	*(s+1) = '0' + (v % 10);
	*s = '0' + (v / 10);
}

/* puts int in string */
puti(s, v)
char *s;
{
	putb(s+2, v % 100);
	putb(s, v / 100);
}

/* puts string onto terminal */
puts(s)
char *s;
{
	register i;
	char *p;
	
	i = 0;
	p = s;
	
	while (*(p++)) i++;
	write (1, s, i);
}
