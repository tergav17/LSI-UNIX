#
/*
 */

/*
 * extended memory disk
 */

#include "param.h"
#include "buf.h"
#include "conf.h"
#include "user.h"


struct  devtab  xmtab;

xmstrategy(abp)
struct buf *abp;
{
        register struct buf *bp;

        bp = abp;

        bp->b_link = 0;
        spl7();
        if (xmtab.d_actf==0)
                xmtab.d_actf = bp;
        else
                xmtab.d_actl->b_link = bp;
        xmtab.d_actl = bp;
        if (xmtab.d_active==0)
                xmstart();
        spl0();
}


xmstart()
{
        register struct buf *bp;

nextbuf:
        if ((bp = xmtab.d_actf) == 0)
                return;
        xmtab.d_active++;
        bp = xmtab.d_actf;
		
		/* do transfer */
		if (bp->b_flags&B_READ) {
			/* read from memory */
		} else {
			/* write to memory */
		}
		
		/* complete and next transfer */
		xmtab.d_active = 0;
		xmtab.d_errcnt = 0;
        xmtab.d_actf = bp->b_link;
        bp->b_flags =| B_DONE;
        goto nextbuf;
}
