/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Initial Developer of this code is Rodd Zurcher.
 * Portions created by Rodd Zurcher are Copyright (C) 1998 Rodd Zurcher.
 * All Rights Reserved.
 */

/*
 * NOTE - this is only a partial implementation of the PSerial abstract
 * class.  It defaults to 2400 bps, 8 data bits, 1 stop, odd parity.  It
 * does not support changing the baud rate or other attributes.  It does
 * now support manual control of RTS and DTR lines.
 */

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <termios.h>

// need another #include for Solaris - Richard Mitchell <gpsphoto@designprofessionals.com>
#ifdef SOLARIS
#include <sys/filio.h>
#endif

#include "PSerial.h"

#ifndef DEFAULT_SERIAL_NAME
#define DEFAULT_SERIAL_NAME "/dev/ttyS0"
#endif


class PSerial_unix : public PSerial
{
public:
			PSerial_unix(void);
	virtual	bool	Open(const char *name);
	virtual void	Close();
	virtual long	Write(const void *ptr, long count);
	virtual void	FlushWrite();

	virtual long	Read(void *ptr, long count);
	virtual bool	SetTimeout(long timeout_ms);
	virtual bool	SetSpeed(int speed, int opts = 0);
	virtual bool	SetDTR(bool state);
	virtual bool	SetRTS(bool state);


private:
	bool	SetRaw(void);
	bool	SetLineData(int bits, bool state);

	int		fTerm;
	long	fTimeout;
    bool	fUseTcDrain;
};


static int tv_ge(const struct timeval *a, const struct timeval *b);
static void tv_sub(const struct timeval *a, struct timeval *b);
static bool SetTermiosSpeed(termios &tios, int speed);


PSerial* PSerial::NewSerial()
{
	return new PSerial_unix();
}


const char *PSerial::GetDefaultName()
{
	return DEFAULT_SERIAL_NAME;
}


PSerial_unix::PSerial_unix(void) :
	fTerm(-1),
	fTimeout(kPStream_NeverTimeout)
{
}


bool PSerial_unix::Open(const char *name)
{
    bool ok;
    char *buf = 0;

    fUseTcDrain = true;

    // check for option
    const char *ptr = (const char *)strchr(name, ':');
    if (ptr)
    {
        // extract basename
        int n = ptr - name;
        buf = new char[n+1];
        memcpy(buf, name, n);
        buf[n] = 0;
        if (strcmp(ptr+1, "nodrain")==0)
        {
            fUseTcDrain = false;
        }
        name = buf;
    }

    ok = ((fTerm = open(name, O_RDWR)) >= 0);
    if (ok)
    {
        ok = SetRaw();
    }

    delete [] buf;
    return ok;
}


// Put the tty into raw mode
// Adapted from tty_raw, "Advanced Programing in the UNIX
// Environment", W. Richard Stevens, pp354-355
bool PSerial_unix::SetRaw(void)
{
	termios tios;

	if (tcgetattr(fTerm, &tios) < 0)
		return false;

	// echo off, canonical mode off, extended input processing off
	// signal chars off
	tios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

	// no SIGINT on BREAK, CR-to-NL off, input parity check off,
	// don't strip 8th bit on input, output flow control off
	tios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

	// clear size bits, 1 stop bit, disable parity
	tios.c_cflag &= ~(CSIZE | CSTOPB | PARENB | PARODD);

	// set 8 bits/char
	tios.c_cflag |= (CS8);

	// output processing off
	tios.c_oflag &= ~(OPOST);

	// 1 byte at a time, no timer
	tios.c_cc[VMIN] = 1;
	tios.c_cc[VTIME] = 0;


	if (tcsetattr(fTerm, TCSAFLUSH, &tios) < 0)
		return false;


	SetSpeed(9600, 0);

	return true;
}


void PSerial_unix::Close()
{
	if (fTerm != -1)
	{
		close(fTerm);
	}
	fTerm = -1;
}


bool PSerial_unix::SetSpeed(int speed, int opts)
{
	termios tios;

	if (tcgetattr(fTerm, &tios) < 0)
		return false;

	// set the speed
	if (!SetTermiosSpeed(tios, speed))
		return false;

	// set the parity
	switch(opts & kPSerial_ParityMask)
	{
		case kPSerial_ParityNone:
			tios.c_cflag &= ~(PARENB);
			break;
		case kPSerial_ParityOdd:
			tios.c_cflag |= (PARENB | PARODD);
			break;
		case kPSerial_ParityEven:
			tios.c_cflag |= (PARENB);
			tios.c_cflag &= ~(PARODD);
			break;
	};

	if (tcsetattr(fTerm, TCSAFLUSH, &tios) < 0)
		return false;

	return true;
}


long PSerial_unix::Write(const void *ptr, long count)
{
	return write(fTerm, ptr, count);
}

#include <cstdio>
void PSerial_unix::FlushWrite()
{
#ifndef NO_TCDRAIN
    if (fUseTcDrain)
    {
        tcdrain(fTerm);
    }
#endif
}


bool PSerial_unix::SetTimeout(long timeout_ms)
{
	fTimeout = timeout_ms;
	return true;
}


long PSerial_unix::Read(void *ptr, long count)
{
	// Blocking read
	if (fTimeout == kPStream_NeverTimeout)
	{
		long rval = read(fTerm, ptr, count);
		return rval;
	}

	// time limited read
	char *cur = (char *) ptr;
	struct timeval expire;
	struct timezone tz;

	if (gettimeofday(&expire, &tz) < 0)
	{
		return -1;
	}

	expire.tv_sec += fTimeout / 1000;
	expire.tv_usec += (fTimeout % 1000) * 1000;

	while (count)
	{
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(fTerm, &rfds);
		struct timeval delay;
		long nread;

		if (gettimeofday(&delay, &tz) < 0)
		{
			return -1;
		}

		if (tv_ge(&delay, &expire))
		{
			// delay (current time) >= expire
		  	break;
		}

		tv_sub(&expire, &delay);	// delay = expire - delay

		if (select(fTerm + 1, &rfds, NULL, NULL, &delay))
		{
			if (ioctl(fTerm, FIONREAD, &nread) < 0)
			{
				return -1;
		  	}

		  	nread = (count < nread) ? count : nread;
		  	if ((nread = read(fTerm, cur, nread)) < 0)
		  	{
				return -1;
		  	}
			count -= nread;
			cur += nread;
		}
		else
		{
		  // timeout
		  break;
		}
	} // while

	return (cur - (char *)ptr);
}

//
// This part implemented by Peter Ljungstrand (SetDTR and SetRTS)
// modifided by Dave Baum
//

bool PSerial_unix::SetDTR(bool state)
{
	return SetLineData(TIOCM_DTR, state);
}


bool PSerial_unix::SetRTS(bool state)
{
	return SetLineData(TIOCM_RTS, state);
}


bool PSerial_unix::SetLineData(int bits, bool state)
{
	int lineData;

	if (ioctl(fTerm, TIOCMGET, &lineData))
	{
		return false;
	}
	else
	{
		if (state)
		{
			lineData |= bits;
		}
		else
		{
			lineData &= ~bits;
		}
		return !ioctl(fTerm, TIOCMSET, &lineData);
	}
}



bool SetTermiosSpeed(termios &tios, int speed)
{
	speed_t s;

	switch(speed)
	{
		case 2400:
			s = B2400;
			break;
		case 4800:
			s = B4800;
			break;
		case 9600:
			s = B9600;
			break;
		default:
			return false;
	}

	if (cfsetispeed(&tios, s) < 0)
		return false;

	if (cfsetospeed(&tios, s) < 0)
		return false;

	return true;
}


int tv_ge(const struct timeval *a, const struct timeval *b)
{
	if (a->tv_sec < b->tv_sec)
	return 0;			// asec < bsec

	if (a->tv_sec == b->tv_sec)
	{
  		if (a->tv_usec < b->tv_usec)
  		{
			return 0;		// ausec < busec
  		}
		return 1;			// ausec >= busec
	}
	return 1;			// asec > bsec
}


void tv_sub(const struct timeval *a, struct timeval *b)
{
	b->tv_sec = a->tv_sec - b->tv_sec;
	b->tv_usec = a->tv_usec - b->tv_usec;
	if (b->tv_usec < 0)
	{
		b->tv_usec += 1000000;
		b->tv_sec--;
	}
}


