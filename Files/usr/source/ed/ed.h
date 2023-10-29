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
char	savedfile[FNSIZE];
char	file[FNSIZE];
char	linebuf[LBSIZE];
char	rhsbuf[LBSIZE/2];
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
int	vflag;
int	listf;
int	col;
char	*globp;
int	tfile;
int	tline;
char	*tfname;
char	*loc1;
char	*loc2;
char	*locs;
char	ibuff[512];
int	iblock;
char	obuff[512];
int	oblock;
int	ichanged;
int	nleft;
int	errfunc();
int	*errlab;
char	*TMPERR;
int	names[26];
char	*braslist[NBRA];
char	*braelist[NBRA];
