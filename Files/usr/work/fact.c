main(argc, argv)
char *argv[];
{
	int psectr,ptrck,sector;
	
	if (argc<2) return;
	sector = atoi(argv[1]);
	
	rx2factr(sector,&psectr,&ptrck);
		
	printf("LOGSEC: %d SECTOR: %d TRACK: %d\n", sector, psectr, ptrck);
	
}

rx2factr(sectr, psectr, ptrck)
int *psectr, *ptrck;
{
	register int p1, p2;
	
	p1 = sectr/26;
	p2 = sectr%26;
	
	p2 = (2*p2 + (p2 >= 13 ? 1 : 0)) % 26;
	
	p2 = (p2 + 6*p1) % 26;
	if (++p1 >= 77)
		p1 = 0;
	
	*psectr = 1 + p2;
	*ptrck = p1;
}
