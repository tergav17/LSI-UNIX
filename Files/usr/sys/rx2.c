#
/*
 * Simplified RX02 driver
 */

#include "param.h"
#include "buf.h"
#include "user.h"

#define ERR	0100000
#define	INIT	040000
#define DD	0400
#define TR	0200
#define IENABLE	0100
#define DONE	040
#define UNIT	020
#define FILLBUF	0
#define EMPBUF	02
#define RXWRITE	04
#define RXREAD	06
#define GO	01

#define RXVECT	0264
#define RXPS	0266

#define NRX	2
#define NRXBLK	1001
#define RXADR	0177170

/* unamed struct for RX211 registers */
struct {
	int	rxcs;
	int	rxdb;
};

/* driver status variables */
struct devtab rxtab;
int sect;

rxstrategy(abp)
struct buf *abp;
{
	register struct buf *bp;
	
	bp = abp;
	if (bp->b_blkno >= NRXBLK) {
		bp->b_flags =| B_DONE | B_ERROR;
		return;
	}
	bp->b_link = 0;
	
	spl7();
	if (rxtab.d_actf==0)
		rxtab.d_actf = bp;
	else
		rxtab.d_actl->b_link = bp;
	rxtab.d_actl = bp;
	if (rxtab.d_active==0) {
		rxstart();
	}
	spl0();
}

rxstart()
{
	register struct buf *bp;
	register int *RX;
	register char *cp;
	int sectr,track;
	
	if ((bp = rxtab.d_actf) == 0)
		return;
	rxtab.d_active++;
	RX = RXADR;
	
	/* if we are writing, then the buffer should be filled */
	if ((bp->b_flags&B_READ) == 0) {
		cp = bp->b_addr+sect*256;
		
		RX->rxcs = FILLBUF | GO | DD;
		
		/* send word count */
		rxwait();
		RX->rxdb = 128;
		
		/* send address */
		rxwait();
		RX->rxdb = cp;
		
		/* check for errors */
		while ((RX->rxcs&DONE) == 0);
		if (RX->rxcs & ERR)
			panic();
	}
	while ((RX->rxcs&DONE) == 0);
	
	/* execute the command */
	/* RXV31 2 head support is included on minors 2-3 */
	RX->rxcs = ((bp->b_dev&1)<<4) | ((bp->b_dev&2)<<9) | IENABLE | ((bp->b_flags&B_READ)?RXREAD:RXWRITE) | DD | GO;
	
	/* calculate sector / track */
	rxfact(bp->b_blkno*2 + sect, &sectr, &track);
	
	/* wait and transfer sector */
	rxwait();
	RX->rxdb = sectr;
	
	/* wait and transfer track */
	rxwait();
	RX->rxdb = track;
	
	/* now we wait for the interrupt */
}

rxintr()
{
	register struct buf *bp;
	register int *RX;
	register char *cp;
	
	if (rxtab.d_active == 0)
		return;
	bp = rxtab.d_actf;
	rxtab.d_active = 0;
	RX = RXADR;
	
	if (RX->rxcs&ERR) {
		if (++rxtab.d_errcnt <= 10) {
			rxstart();
			return;
		}
		bp->b_flags =| B_ERROR;
	}
	rxtab.d_errcnt = 0;
	
	if (bp->b_flags&B_READ) {
		cp = bp->b_addr+sect*256;
		
		RX->rxcs = EMPBUF | GO | DD;
		
		/* send word count */
		rxwait();
		RX->rxdb = 128;
		
		/* send address */
		rxwait();
		RX->rxdb = cp;
		
		/* check for errors */
		while ((RX->rxcs&DONE) == 0);
		if (RX->rxcs & ERR)
			panic();
	}
	
	if ((bp->b_wcount =+ 128) == 0) {
		rxtab.d_actf = bp->b_link;
		bp->b_flags =| B_DONE;
		
		sect = 0;
	} else {
		if (++sect == 2) {
			sect = 0;
			bp->b_blkno++;
			bp->b_addr =+ 512;
		}
	}
	rxstart();
}

/*
 * waits for TR to come back on
 */
rxwait()
{
	register int *RX;
	
	RX = RXADR;
	
	while ((RX->rxcs&TR) == 0);
	return;
}
/*
 * RX02 sect/track mapping function
 * borrowed from the V7 rx2.c driver
 */
rxfact(sectr, psectr, ptrck)
int *psectr, *ptrck;
{
	register int p1, p2;
	
	p1 = sectr/26;
	p2 = sectr%26;
	
	/* 2 to 1 interleave */
	p2 = (2*p2 + (p2 >= 13?1:0))%26;
	
	/* 6 sector per track slew */
	p2 = (p2 + 6*p1)%26;
	if (++p1 >= 77)
		p1 = 0;
	
	*psectr = 1+p2;
	*ptrck = p1;
}
