# LSI-UNIX
This is my personal copy of LSI-UNIX that I have been working on recently. It contains a few modifications from the original source, but is also faithful to it. The kernel is 10KW in size, and adds back a number of features from UNIX v6. The current system setup can compile its kernel and all user programs locally.

This version of LSX has been modified to support the following:

- Full use of system memory
- Multiple block device drivers
- Booting from the RX02 and RK05
- The mount and umount commands
- TTY raw mode for screen editor use
- Proper inode allocation for large file systems
- Various utilities like cat, dd, etc..
