Communication with the RCX (specifically the RCX_Link class) has gotten
a lot more complicated in order to support USB IR Towers.  The big
problem is that every OS has a different way of dealing with the USB
tower and it has been difficult to abstract it in a single place:

Windows: Lego driver shims the tower into a regular I/O device
named \\.\LEGOTOWERn where n is the index of the tower (first
tower is 1, etc).  For the most part, the tower acts like an
ordinary serial port except that the speed cannot be set
and for some reason the leading 0x55 of received packets
never shows up.

Mac OS 9: Lego driver uses a proprietary interface (no shim as a
serial device).  The official way to access the tower is via
Lego's GHOST API, which actually encompasses the entire
communications stack (packet formatting, retries, etc).

Mac OS X: No Lego driver, rcxlib talks directly to the tower
via OS dependent calls.  USB tower appears to sometimes absorb
the leading 0x55 here too.

So the old architecture of RCX_Link looked like this:

RCX_Link
  |
PSerial

PSerial is an abstract base class for serial communication, and
each OS had its own derived class (PSerial_win, PSerial_mac,
PSerial_unix).


The new architecture has many more potential pieces:

RCX_Link: top level class for communication with the RCX.  The public
interface for this class has been changed only slightly.  Internally,
the class only does session-level processing such as breaking a
download into individual commands.

RCX_Transport: abstract class that represents a means to send a packet
to an RCX and get back a reply.

RCX_GhostTransport: implementation of RCX_Transport that uses the GHOST
API to send packets to the RCX.  Presently, it ignores any device name
and tries to attach to the first available USB tower.

RCX_PipeTransport: the old transport code from RCX_Link has been pulled
into this class.  It formats a command into a packet of bytes, waits
for a reply, performs retries, etc.

RCX_Pipe: an abstract class that represents a means to send/receive
byte streams.

RCX_SerialPipe: a simple wrapper that uses a PSerial implementation
as an RCX_Pipe.


Here's how the various pieces get wired together:

Serial based communication (any OS)
  RCX_Link
  RCX_PipeTransport
  RCX_SerialPipe
  P_Serial subclass - platform specific

USB tower, GHOST defined at build time (Mac OS 9)
  RCX_Link
  RCX_GhostTransport

USB tower, GHOST not defined at build time (Win32, OS X, etc)
  RCX_Link
  RCX_PipeTransport
  RCX_Pipe subclass - platform specific


There are three current implementations for USB towers without ghost:

RCX_USBTowerPipe_none - stub file when there is no support
RCX_USBTowerPipe_osx - Mac OS X (direct access to USB endpoints)
RCX_USBTowerPipe_win - Windows (using the serial port shim)


Adding USB Support on other platforms
-------------------------------------

There are three basic options:

1) Write a driver for the USB tower that makes it look like a serial
port (e.g. shows up in /dev on a Unix system, etc).  If the emulation is
complete, then no change is required for NQC (just use it as a serial port).
In cases of partial emulation a new RCX_Pipe class that acts as appropriate
glue may be required (see RCX_USBTowerPipe_win.cpp as an example).

2) Talk to the USB hardware directly from NQC.  See RCX_USBTowerPipe_osx.cpp
as an example of how this was done under Mac OSX.

3) Port Lego's Ghost library to the platform.  Lego is probably the only
group that can do this.  But if they do, then building NQC is pretty easy...
just #define GHOST.

Note that for options 1 and 2, any additional object files should be listed
in the platform specific P_USBOBJ definition in the Makefile.  The default
is for no USB support (RCX_USBTowerPipe_none.o).
