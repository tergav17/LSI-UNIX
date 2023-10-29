#
/*
 * This program is designed to load a file into a specific block
 */

char	buf[512];

main(argc, argv)
char *argv[];
{
	register int i,j,k;
	
	if (argc != 4) {
		printf("USAGE: ldblk [source] [driver] [block]\n");
		return;
	}
	
	/* open source */
	if ((i = open(argv[1], 0)) == -1) {
		printf("can't open: %s\n", argv[1]);
		return;
	}
	
	/* read into buffer */
	if (read(i, buf, 512) < 0) {
		printf("can't read: %s\n", argv[1]);
		return;
	}
	
	/* open driver */
	if ((j = open(argv[2], 1)) == -1) {
		printf("can't open: %s\n", argv[2]);
		return;
	}
	
	/* write to driver */
	seek(j, atoi(argv[3]), 3);
	if (write(j, buf, 512) < 0) {
		printf("can't write: %s\n", argv[2]);
	}
}
