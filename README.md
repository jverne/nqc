NQC Readme
----------

If you have a problem, PLEASE CHECK THE FAQ:
  http://bricxcc.sourceforge.net/nqc/doc/faq.html
  
Send bug reports to bricxcc@comcast.net.  Be sure to include details about what
platform you are running nqc on and a sample file that demonstrates the bug if
possible.

For updates and additional documentation, visit the NQC Web Site:
http://bricxcc.sourceforge.net/nqc.


Note to Windows Users
---------------------

NQC is a command line based tool - normally you run it by typing an
appropriate command into an MS-DOS window.  There is no GUI for it and
if you double-click the nqc.exe file an MS-DOS console will be created
for you, NQC will run within it, then since NQC finishes almost
immediately, the entire window will disappear.

Some people prefer command line based tools because they allow you to
use the text editor of your choice, etc. It also makes for identical
behavior under Windows, Mac, and Linux. In order to use the command line
version of NQC you'll need to do two things:

1) Use some sort of text editor (such as Notepad) to edit and save a
source file for NQC to compile.

2) From an MS-DOS window type the appropriate NQC command. Its usually
best to either put all of your programs and nqc.exe in the same
directory, or make sure the directory containing NQC is in your command
path. 

If any of the above seem either too confusing or too tedious, then you
may want to try the BricxCC which provides a familiar Windows style GUI on top
of the standard NQC compiler.  You can download BricxCC here:

http://bricxcc.sourceforge.net/


Getting started
---------------

Download the appropriate compiler (nqc or nqc.exe) and put it where
your shell can find it as a command.

The IR tower should be connected to your modem port (macintosh) or COM1
(Win32/Linux). The IR tower should be set for "near" mode (small
triangle). The RCX should also be set for this mode, and firmware must
already be downloaded.

Compile and download the test file using the following command line:

nqc -d test.nqc

The test program assumes there's a motor on output A and a touch sensor
on input 1.  It turns on the motor and waits for the switch to be
pressed, then it turns off the motor and plays a sound.

If you are using a USB tower or a different serial port you will need to
specify the port either by adding a -Sx option (where x is the name of the
port) to the command line or by setting the RCX_PORT environment variable.

Here are some examples:

USB tower (where supported)
	nqc -Susb -d test.nqc

Win32 COM2 port:
	set RCX_PORT=COM2

Win32 USB port:
	set RCX_PORT=usb
	
Linux:
	The syntax for setting environment variables is shell specific.  By
	default nqcc uses "/dev/ttyS0" as the device name for the serial
    port.  If you are using the second serial port, then "/dev/ttyS1"
    should work.  Other device drivers may or may not work depending on if
    they implement the expected ioctl's to setup the baud rate, parity,
    etc.

