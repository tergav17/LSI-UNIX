char shell[] "/bin/sh";
char minus[] "-";
char ctty[] "/dev/tty8";
char runc[] "/etc/rc";

main()
{
	register i;

	/* dup voodoo */
	open(ctty, 2);
	dup(0);
	dup(0);
	chdir("/");
	open("/bin", 4);


	/* execute /etc/rc */
	i = fork();
	if(i == 0) {
		execl(shell, shell, runc, 0);
		exit();
	}

	/* exec the shell */
	execl(shell, minus, 0);
	exit();
}
