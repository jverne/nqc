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
 * Portions created by David Baum are Copyright (C) 1999 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */


#include <cstdio>
#include <cstring>

#include "Buffer.h"

using std::fopen;
using std::memcpy;
using std::strlen;

#ifdef DEBUG
using std::strcpy;
#endif

#define CR	0x0d
#define LF	0x0a

#define kDefaultBufferSize 10

Buffer::Buffer() :
	fName(0),
	fData(0),
	fLength(0)
{
}


Buffer::~Buffer()
{
	delete [] fName;
	delete [] fData;
}


bool Buffer::Create(const char *name, const char *pathname)
{
	FILE *fp = fopen(pathname, "rb");
	if (!fp) return false;

	Create(name, fp);
	fclose(fp);

	return true;
}


void Buffer::Create(const char *name, FILE *fp)
{
	// figure out length
	if (fseek(fp,0,SEEK_END)==0)
	{
		// if stream supports seeking, then we can determine total size,
		// allcoate enough storage, and read the entire thing at once
		fLength = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// allow room for terminating NL
		int allocate = fLength + 1;
		fData = new char[allocate];

		fread(fData, 1, fLength, fp);
	}
	else
	{
		// This code is for io streams (such as redirected stdin under MPW)
		// which don't support seeking.  The data from the stream is read into
		// a dynamically growing array.  This is a lot less efficient than
		// the previous case which knew the total lenght beforehand.
		int max = kDefaultBufferSize;
		fData = new char[max];
		fLength = 0;

		int c;
		while((c=getc(fp)) != EOF)
		{
			fData[fLength++] = (char)c;

			if (fLength == max)
			{
				// grow buffer - this needs to be done even on the last
				// char in the file since a terminating NL will be added
				// later on
				max *= 2;
				char *newBuf = new char[max];
				memcpy(newBuf, fData, fLength);
				delete [] fData;
				fData = newBuf;
			}
		}
	}
	FinishCreate(name);
}


void Buffer::Create(const char *name, const char *data, int length)
{
	// allcoate memory (allow for trailing NL)
	fData = new char[length + 1];
	fLength = length;
	memcpy(fData, data, length);

	FinishCreate(name);
}


void Buffer::FinishCreate(const char *name)
{
	fName = new char[strlen(name) + 1];
	strcpy(fName, name);

	ConvertLineEndings();

	// do this after ConvertLineEndings() since mac files might
	// end with CR and \n is an LF, so CRLF would get reduced if
	// \n was added before conversion
	fData[fLength++] = '\n';
}


const char *Buffer::GetLine(int line) const
{
	if (line < 1) return 0;

	const char *ptr = fData;
	const char *end = fData + fLength;

	--line;
	while(line)
	{
		if (ptr == end) return 0;
		if (*ptr++ == '\n')
			--line;
	}

	return ptr;
}


/*
 * int Buffer::FindLine(int &offset) const
 *
 * return the line number given an offset.  Also adjust the offset
 * to the start of the line.
 */

int Buffer::FindLine(int &offset) const
{
	const char *ptr = fData;
	const char *lineStart = fData;
	int line = 1;

	// clip the offset to fLength-1 since fData[fLength-1] is implicitly
	// the last line of the buffer and no line can start after it
	const char *end = ptr + ((offset < (fLength-1)) ? offset : fLength-1);

	while(ptr < end)
	{
		if (*ptr++ == '\n')
		{
			++line;
			lineStart = ptr; // ptr is already adjusted past the NL
		}
	}

	offset = lineStart - fData;
	return line;
}


/*
 * int Buffer::FindStartOfLine(int offset) const
 *
 * return the offset of the first character in the line containing the
 * specified offset.
 */

int Buffer::FindStartOfLine(int offset) const
{
	const char *ptr = fData + offset;
	while(ptr > fData)
	{
		--ptr;
		if (*ptr == '\n') return (ptr - fData + 1);
	}

	return 0;
}



/*
 * int Buffer::FindEndOfLine(int offset) const
 *
 * return the offset of the newline ending the line containing the
 * specified offset.
 */

int Buffer::FindEndOfLine(int offset) const
{
	const char *ptr = fData + offset;
	const char *end = fData + fLength;

	while(ptr < end)
	{
		if (*ptr == '\n') return ptr - fData;
		++ptr;
	}

	return fLength - 1;
}


void Buffer::ConvertLineEndings()
{
	const char *src, *end;
	char *dst;

	src = fData;
	end = src + fLength;
	dst = fData;

	while(src < end)
	{
		char c = *src++;

		switch(c)
		{
			case CR:
				*dst++ = '\n';
				// skip second part of CR-LF pair
				if (src < end && *src==LF)
					src++;
				break;
			case LF:
				*dst++ = '\n';
				break;
			default:
				*dst++ = c;
				break;
		}
	}

	fLength = dst - fData;
}


