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
 * The Initial Developer of this code is David Baum.
 * Portions created by David Baum are Copyright (C) 1998 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 * Portions adapted from BrickEmu TCP patches by Matthew Sheets
 * 	BrickEmu TCP Portions Copyright (C) 2003-2005 Jochen Hoenicke
 * 	<http://hoenicke.ath.cx/rcx/brickemu.html>.
 *
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "RCX_Pipe.h"

#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <limits.h>

#include <cstdio>

class RCX_TcpPipe_linux : public RCX_Pipe
{
public:
	        RCX_TcpPipe_linux() : fd(-1) {}
        virtual	~RCX_TcpPipe_linux() { Close(); }

	virtual RCX_Result	Open(const char *name, int mode);
	virtual void		Close();

	virtual int		GetCapabilities() const;
	virtual RCX_Result	SetMode(int mode);

	virtual long		Read(void *ptr, long count, long timeout_ms);
	virtual long		Write(const void *ptr, long count);
        virtual bool            IsUSB() const { return true; };

private:
	void		SetTimeout(long timeout_ms);
	int		fd;
	fd_set		fds;
	struct timeval	tv;
};


RCX_Pipe* RCX_NewTcpPipe()
{
	return new RCX_TcpPipe_linux();
}

#ifndef DEFAULT_HOST
#define DEFAULT_HOST "localhost"
#endif

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 50637
#endif

RCX_Result RCX_TcpPipe_linux::Open(const char *name, int mode)
{
	char name_buf[PATH_MAX];
	int port = 0;
	struct hostent *h;
	struct sockaddr_in localAddr, ttyAddr;

	if (0 == name || 0 == *name)
	{
		h = gethostbyname(DEFAULT_HOST);
		port = DEFAULT_PORT;
	}
	else
	{
		strcpy(name_buf, name);
		char *portStr = strchr(name_buf, ':');
		if (portStr)
		{
			port = atoi(portStr + 1);

			// Since we have the port, we can remove that from the name string
			// Change the ':' in name to the null (end-of-string) character
			*portStr = 0;
		}
		else
		{
			port = DEFAULT_PORT;
		}

		h = gethostbyname(name_buf);
	}

	// Verify the host
	if (!h)
	{
		return kRCX_UnknownTcpHostError;
	}

	// Create the socket
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		return kRCX_OpenSocketError;
	}

	// Bind any port number
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *) &localAddr, sizeof(localAddr)) < 0)
	{
		return kRCX_BindPortError;
	}

	ttyAddr.sin_family = h->h_addrtype;
	memcpy((char *) &ttyAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	ttyAddr.sin_port = htons(port);

	if (connect(fd, (struct sockaddr *) &ttyAddr, sizeof(ttyAddr)) < 0)
	{
		return kRCX_TcpConnectError;
	}


        RCX_Result err = SetMode(mode);
        if (err != kRCX_OK)
        {
          Close();
          return err;
        }
        return kRCX_OK;
}


void RCX_TcpPipe_linux::Close()
{
	if (fd < 0) return;

	close(fd);
	fd = -1;
}


long RCX_TcpPipe_linux::Write(const void *ptr, long count)
{
	ssize_t actual;
	ssize_t i;

	actual = 0;
	while(count > 0) {
		if ((i = write(fd, (void *) &((char *)ptr)[actual], (size_t)count)) < 0)
		{
			return -1;
		}
		count -= i;
		actual += i;
	}

	return actual;
}


long RCX_TcpPipe_linux::Read(void *ptr, long count, long timeout_ms)
{
	int selectResult;
	ssize_t actual = 0;

	FD_ZERO(&fds);
	FD_SET(fd, &fds);

	tv.tv_sec = (timeout_ms / 1000);
	tv.tv_usec = (timeout_ms % 1000) * 1000;

	selectResult = select(fd+1, &fds, NULL, NULL, &tv);

	if (!FD_ISSET(fd, &fds))
	{
		// fprintf(stderr, "NOT SET ERROR\n");
	}


	// selectResult  < 0 is a SELECT ERROR
	// selectResult == 0 is a TIMEOUT
	// selectResult >  0 is BYTES DETECTED
	if (selectResult > 0)
	{
		// Bytes detected
		if ((actual = read(fd, ptr, count)) < 0)
		{
			return 0;
		}
	}

	return actual;
}


int RCX_TcpPipe_linux::GetCapabilities() const
{
	// TODO: Is kTxEchoFlag needed here??
	// only normal IR mode is supported, and
	// the ABSORB_0x55_FLAG is needed to tell the
	// transport that initial 0x55 bytes don't make
	// it through the USB/driver shim
	// return kNormalIrMode + kAbsorb55Flag;
	return kNormalIrMode + kFastIrMode + kCyberMasterMode + kSpyboticsMode;
}


RCX_Result RCX_TcpPipe_linux::SetMode(int mode)
{
	switch(mode)
	{
		case kNormalIrMode:
                case kFastIrMode:
		case kCyberMasterMode:
                case kSpyboticsMode:
			return kRCX_OK;
		default:
			return kRCX_PipeModeError;
	}
}

/*
void RCX_TcpPipe_linux::SetTimeout(long timeout_ms)
{
fprintf(stderr, "TCP Set Timeout\n");
	FD_ZERO(&fds);
	FD_SET(fd, &fds);

	tv.tv_sec = (timeout_ms / 1000) + 2;
	tv.tv_usec = (timeout_ms % 1000) * 1000;

	if (select(fd+1, &fds, NULL, NULL, &tv) < 0)
	{
fprintf(stderr, "SELECT ERROR\n");
	}

	if (!FD_ISSET(fd, &fds))
	{
fprintf(stderr, "NOT SET ERROR\n");
	}
}
*/
