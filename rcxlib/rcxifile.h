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
 */

#ifndef __RCXIFILE_H
#define __RCXIFILE_H

/* RCX Image File Format
 *
 * An RCX Image FIle consists of an RCXIHeader followed by an arbitrary
 * (up to 65535) number of Fragments.  The Fragments are followed by a
 * optional symbol table.  Fragments consist of an
 * RCXIFragmentHeader followed by 0 or more data bytes (as specified within
 * the fragment header).  Fragment data must be padded to a
 * four byte boundry (the unpadded length is stored in the header).  For
 * example, a fragment of 2 bytes would have RCXIFragmentHeader.fDataLength = 2,
 * but actually contain 4 bytes of file data following the fragment header.
 *
 * All multiple-byte values (shorts and longs) are stored in little endian
 * format.
 *
 * All unused or reserved bytes (including pad bytes in the fragment data)
 * should be written as 0's, and ignored during reading.
 *
 * The following values can be used for fragment types
 *
 *	0 : RCX Task bytecodes
 *  1 : RCX Subroutine bytecodes
 *  2-255 : reserved for future use
 *
 *
 * Version 1.02 -
 *	added fTargetType field in RCXIHeader
 */

/* constants for the RCXIHeader */
#define kRCXI_Signature	0x49584352	/* 'RCXI' */
#define kRCXI_CurrentVersion	0x102	/* 1.02 */

/* fragment types */
enum
{
	kRCXI_TaskFragment = 0,
	kRCXI_SubFragment
};


/* symbol types */
enum
{
	kRCXI_TaskSymbol = 0,
	kRCXI_SubSymbol,
	kRCXI_VarSymbol
};


struct RCXIHeader
{
	unsigned long	fSignature;	/* always kRCXI_Signature */
	unsigned short	fVersion;	/* kRCXI_CurrentVersion */
	unsigned short	fFragmentCount;
	unsigned short	fSymbolCount; /* number of symbols defined in symbol table */
	unsigned char	fTargetType; /* target type value as defined in RCX_Target.h */
	unsigned char	fReserved_;	/* should be 0 */
};


struct RCXIFragmentHeader
{
	unsigned char	fType;		/* see above */
	unsigned char	fIndex; 	/* depends on fragment type */
	unsigned short	fDataLength;	/* unpadded data length */
};


struct RCXISymbolHeader
{
	unsigned char	fType;		/* see above */
	unsigned char	fIndex;		/* depends on symbol type */
	unsigned char	fLength;	/* length including terminating 0 */
	unsigned char	fReserved;	/* should be 0 */
};


/* a macro to compute padded data length */
#define RCXI_PADDED_LENGTH(len)	(((len) + 3) & ~3)

/* a macro to compute the number of pad bytes */
#define RCXI_PAD_BYTES(len)		((4 - (len)) & 3)


#endif
