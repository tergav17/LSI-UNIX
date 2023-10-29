/*
 * LSI-UNIX device configuration file
 */

int	(*bdevsw[])()
{
	&rxstrategy,	&rxtab,	/* rx */
	&rkstrategy,	&rktab,	/* rk */
	0
};
