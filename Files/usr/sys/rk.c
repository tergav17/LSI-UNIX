#
/*
 */

/*
 * RK disk driver
 */

#include "param.h"
#include "buf.h"
#include "conf.h"
#include "user.h"

#define RKADDR  0177400
#define NRK     4
#define NRKBLK  4872

#define RESET   0
#define GO      01
#define DRESET  014
#define IENABLE 0100
#define DRY     0200
#define ARDY    0100
#define WLO     020000
#define CTLRDY  0200

struct {
        int rkds;
        int rker;
        int rkcs;
        int rkwc;
        int rkba;
        int rkda;
};

struct  devtab  rktab;
struct  buf     rrkbuf;

rkstrategy(abp)
struct buf *abp;
{
        register struct buf *bp;
        register *qc, *ql;
        int d;

        bp = abp;

        d = bp->b_dev.d_minor-7;
        if(d <= 0)
                d = 1;
        if (bp->b_blkno >= NRKBLK*d) {
                bp->b_flags =| B_DONE | B_ERROR;
                return;
        }
        bp->b_link = 0;
        spl7();
        if (rktab.d_actf==0)
                rktab.d_actf = bp;
        else
                rktab.d_actl->b_link = bp;
        rktab.d_actl = bp;
        if (rktab.d_active==0)
                rkstart();
        spl0();
}

rkaddr(bp)
struct buf *bp;
{
        register struct buf *p;
        register int b;
        int d, m;

        p = bp;
        b = p->b_blkno;
        m = p->b_dev.d_minor - 7;
        if(m <= 0)
                d = p->b_dev.d_minor;
        else {
                d = 0;
        }
        return(d<<13 | (b/12)<<4 | b%12);
}

rkstart()
{
        register struct buf *bp;

        if ((bp = rktab.d_actf) == 0)
                return;
        rktab.d_active++;
        devstart(bp, &RKADDR->rkda, rkaddr(bp), 0);
}

rkintr()
{
        register struct buf *bp;

        if (rktab.d_active == 0)
                return;
        bp = rktab.d_actf;
        rktab.d_active = 0;
        if (RKADDR->rkcs < 0) {         /* error bit */
                RKADDR->rkcs = RESET|GO;
                while((RKADDR->rkcs&CTLRDY) == 0) ;
                if (++rktab.d_errcnt <= 10) {
                        rkstart();
                        return;
                }
                bp->b_flags =| B_ERROR;
        }
        rktab.d_errcnt = 0;
        rktab.d_actf = bp->b_link;
        bp->b_flags =| B_DONE;
        rkstart();
}

/*
 * Device start routine for disks
 * and other devices that have the register
 * layout of the older DEC controllers (RF, RK, RP, TM)
 */
#define IENABLE 0100
#define WCOM    02
#define RCOM    04
#define GO      01
devstart(bp, devloc, devblk, hbcom)
struct buf *bp;
int *devloc;
{
        register int *dp;
        register struct buf *rbp;
        register int com;

        dp = devloc;
        rbp = bp;
        *dp = devblk;                   /* block address */
        *--dp = rbp->b_addr;            /* buffer address */
        *--dp = rbp->b_wcount;          /* word count */
        com = IENABLE | GO;
        if (rbp->b_flags&B_READ)        /* command + x-mem */
                com =| RCOM;
        else
                com =| WCOM;
        *--dp = com;
}
