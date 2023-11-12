# LSI-UNIX
This is my personal copy of LSI-UNIX that I have been working on recently. It contains a few modifications from the original source, but is also faithful to it. The kernel is 10KW in size, and adds back a number of features from UNIX v6. The current system setup can compile its kernel and all user programs locally.

This version of LSX has been modified to support the following:

- Full use of system memory
- Multiple block device drivers
- Booting from the RX02 and RK05
- The mount and umount commands
- TTY raw mode for screen editor use
- Proper inode allocation for large file systems
- Execution of `/etc/rc` script on startup
- Swap partition on the root disk
- Dynamic use of swap space depending on program size
- Various utilities like cat, dd, etc..

The included system images for the RK05 and RX02 are both built to run on a PDP-11/03 with 56Kb of RAM and no EIS. Having a CPU with EIS will make things run faster, however.

Also included in the distribution is the `eds` editor. This is a VT100 screen editor that I wrote off of the bones of the original `ed` editor. It works a little like an extremely cut down version of nano. The commands are:

- CTRL+Q: Exit the editor
- CTRL+W: Write to the current file
- CTRL+R: Redraw the screen
- CRTL+G: Goto line (type number of line and hit enter)
- CTRL+A: Goto beginning of line
- CTRL+Z: Goto end of line

Modifying the editor to work with a VT52 or ADM-3A shouldn't be too difficult.
