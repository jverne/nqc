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

#include "SRecord.h"
#include <cstring>
#include <cctype>

using std::memset;
using std::strncmp;
using std::memcpy;
using std::isspace;

#define kMaxLine	256

typedef struct
{
  int length;
  int offset;
} segment_t;

typedef struct
{
  unsigned short entry;
  segment_t *segments;
} image_t;

#ifdef SREC_STRICT
#define SREC_DATA_SIZE 32
#else
#define SREC_DATA_SIZE 256
#endif

typedef struct {
    unsigned char type;
    unsigned long addr;
    unsigned char count;
    unsigned char data[SREC_DATA_SIZE];
} srec_t;

#define SREC_OK               0
#define SREC_NULL            -1
#define SREC_INVALID_HDR     -2
#define SREC_INVALID_CHAR    -3
#define SREC_INVALID_TYPE    -4
#define SREC_TOO_SHORT       -5
#define SREC_TOO_LONG        -6
#define SREC_INVALID_LEN     -7
#define SREC_INVALID_CKSUM   -8

#define BUFFERSIZE      4096
#define RETRIES         5
#define WAKEUP_TIMEOUT  4000

#define IMAGE_START     0x8000
#define IMAGE_MAXLEN    0x8000
#define TRANSFER_SIZE   200

#define SEGMENT_BREAK	1024

static signed char ctab[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
     0, 1, 2, 3, 4, 5, 6, 7,   8, 9,-1,-1,-1,-1,-1,-1,
    -1,10,11,12,13,14,15,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,10,11,12,13,14,15,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,-1,-1,-1,-1,-1,
};

static int ltab[10] = {4,4,6,8,0,4,0,8,6,4};

/* Macros */

#define C1(l,p)    (ctab[l[p]])
#define C2(l,p)    ((C1(l,p)<<4)|C1(l,p+1))

/* Static functions */

int srec_decode(srec_t *srec, char *_line)
{
    int len, pos, count, alen, sum = 0;
    unsigned char *line = (unsigned char *)_line;

    if (!srec || !line)
        return SREC_NULL;

    for (len = 0; line[len]; len++)
        if (line[len] == '\n' || line[len] == '\r')
            break;

    if (len < 4)
        return SREC_INVALID_HDR;

    if (line[0] != 'S')
        return SREC_INVALID_HDR;

    for (pos = 1; pos < len; pos++) {
        if (C1(line, pos) < 0)
            return SREC_INVALID_CHAR;
    }

    srec->type = C1(line, 1);
    count = C2(line, 2);

    if (srec->type > 9)
        return SREC_INVALID_TYPE;
    alen = ltab[srec->type];
    if (alen == 0)
        return SREC_INVALID_TYPE;
    if (len < alen + 6)
        return SREC_TOO_SHORT;
    if (count > alen + SREC_DATA_SIZE + 2)
        return SREC_TOO_LONG;
    if (len != count * 2 + 4)
        return SREC_INVALID_LEN;

    sum += count;

    len -= 4;
    line += 4;

    srec->addr = 0;
    for (pos = 0; pos < alen; pos += 2) {
        unsigned char value = C2(line, pos);
        srec->addr = (srec->addr << 8) | value;
        sum += value;
    }

    len -= alen;
    line += alen;

    for (pos = 0; pos < len - 2; pos += 2) {
        unsigned char value = C2(line, pos);
        srec->data[pos / 2] = value;
        sum += value;
    }

    srec->count = count - (alen / 2) - 1;

    sum += C2(line, pos);

    if ((sum & 0xff) != 0xff)
        return SREC_INVALID_CKSUM;

    return SREC_OK;
}

int srec_load (FILE *file, UByte *image, int maxlen, image_t *image_def, int numimage_def)
{
    char buf[256];
    srec_t srec;
    int segStartAddr = 0;
    int prevAddr = -SEGMENT_BREAK;
    int prevCount = SEGMENT_BREAK;
    int segIndex = -1;
    int strip = 0;
    int imageIndex = -SEGMENT_BREAK;
    int i;
    int length = 0;

    /* Clear image to zero */
    memset(image, 0, maxlen);

    /* Read image file */
    while (fgets(buf, sizeof(buf), file)) {
        int error, i;
        /* Skip blank lines */
        for (i = 0; buf[i]; i++)
            if (!isspace(buf[i]))
                break;
        if (!buf[i])
            continue;
        /* Decode line */
        if ((error = srec_decode(&srec, buf)) < 0) {
          return error;
        }
        /* Detect Firm0309.lgo header, set strip=1 if found */
        if (srec.type == 0) {
            if (srec.count == 16)
                if (!strncmp((const char*)srec.data, "?LIB_VERSION_L00", 16))
                    strip = 1;
        }
        /* Process s-record data */
        else if (srec.type == 1) {
                /* Start of a new segment? */
                if (srec.addr - prevAddr >= SEGMENT_BREAK) {
                    segIndex++;
                    if (segIndex >= numimage_def)
                    {
                      return 0;
                    }

                        image_def->segments[segIndex].length = 0;
                        segStartAddr = srec.addr;
                        prevAddr = srec.addr - prevCount;
                        image_def->segments[segIndex].offset = imageIndex + prevCount;
                }

            if (srec.addr < IMAGE_START ||
                srec.addr + srec.count > (unsigned long) IMAGE_START + maxlen) {
                  return 0;
            }

                // Data is not necessarily contiguous so can't just accumulate srec.counts.
            image_def->segments[segIndex].length = srec.addr - segStartAddr + srec.count;

            imageIndex += srec.addr - prevAddr;
            memcpy(&image[imageIndex], &srec.data, srec.count);
            prevAddr = srec.addr;
            prevCount = srec.count;
        }
        /* Process image entry point */
        else if (srec.type == 9) {
            if (srec.addr < IMAGE_START ||
                srec.addr > (unsigned long) IMAGE_START + maxlen) {
                  return 0;
            }
                image_def->entry = srec.addr;
        }
    }

    if (strip) {
                int pos;
                for (pos = IMAGE_MAXLEN - 1; pos >= 0 && image[pos] == 0; pos--)
                {} // do nothing
                image_def->segments[segIndex].length = pos+1;
    }

        for (i = 0; i <= segIndex; i++)
                length += image_def->segments[segIndex].length;

    if (length == 0) {
      return 0;
    }

    return segIndex+1;
}

bool SRecord::Read(FILE *fp, int maxLength)
{
	//char line[kMaxLine];
	//char *ptr;
	//int dataLen;
	//int byte;

	delete [] fData;
	fData = new UByte[maxLength];

        segment_t segments[2];
        image_t image_def = { 0, &segments[0] };
        int num_image_def = srec_load(fp, fData, IMAGE_MAXLEN, &image_def, 2);
        if (num_image_def > 0) {
           fStart = image_def.entry;
           fLength = image_def.segments[num_image_def-1].offset +
                     image_def.segments[num_image_def-1].length;
           return true;
        }
        return false;
}


int SRecord::ReadHexByte(const char *ptr)
{
	int i;
	int v = 0;
	char c;
	int n;

	for(i=0; i<2; i++)
	{
		c = *ptr++;

		if (c >= '0' && c <= '9')
			n = c - '0';
		else if (c >= 'A' && c <= 'F')
			n = c - 'A' + 10;
		else if (c >= 'a' && c <= 'f')
			n = c - 'a' + 10;
		else
			return -1;

		v = (v << 4) + n;
	}

	return v;
}
