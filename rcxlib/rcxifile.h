/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The Initial Developer of this code is David Baum.
 * Portions created by David Baum are Copyright (C) 1999 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */
#ifndef __RCXIFILE_H
#define __RCXIFILE_H

/**
 * @file rcxifile.h
 * @brief RCX Image File Format
 *
 * An RCX Image file consists of an RCXIHeader followed by an arbitrary
 * (up to 65535) number of Fragments. The Fragments are followed by a
 * optional symbol table.  Fragments consist of an RCXIFragmentHeader 
 * followed by 0 or more data bytes (as specified within the fragment
 * header). Fragment data must be padded to a four byte boundry (the
 * unpadded length is stored in the header). For example, a fragment
 * of 2 bytes would have RCXIFragmentHeader.fDataLength = 2, but actually
 * contain 4 bytes of file data following the fragment header.
 *
 * All multiple-byte values (shorts and longs) are stored in little endian
 * format.
 *
 * All unused or reserved bytes (including pad bytes in the fragment data)
 * should be written as 0's, and ignored during reading.
 *
 * The following values can be used for fragment types
 *
 *  + 0: RCX Task bytecodes
 *  + 1: RCX Subroutine bytecodes
 *  + 2-255: reserved for future use
 *
 * @version 1.02 -
 *  added fTargetType field in RCXIHeader
 */

/* Constants for the RCXIHeader */
#define kRCXI_Signature         0x49584352  ///< "RCXI"
#define kRCXI_CurrentVersion    0x102       ///< Version 1.02

/* Fragment types */
enum
{
    kRCXI_TaskFragment = 0,
    kRCXI_SubFragment
};


/* Symbol types */
enum
{
    kRCXI_TaskSymbol = 0,
    kRCXI_SubSymbol,
    kRCXI_VarSymbol
};

/* RCX Image Header */
struct RCXIHeader
{
    unsigned long   fSignature;     ///< Always kRCXI_Signature @see kRCXI_Signature
    unsigned short  fVersion;       ///< @see kRCXI_CurrentVersion
    unsigned short  fFragmentCount; ///< Number of fragments
    unsigned short  fSymbolCount;   ///< Number of symbols defined in symbol table
    unsigned char   fTargetType;    ///< Target type value as defined in RCX_Target.h
    unsigned char   fReserved_;     ///< Should be 0
};

/* RCX Image Fragment Header */
struct RCXIFragmentHeader
{
    unsigned char   fType;          // Fragment types
    unsigned char   fIndex;         ///< Depends on fragment type
    unsigned short  fDataLength;    ///< Unpadded data length
};


/* RCX Image Symbol Header */
struct RCXISymbolHeader
{
    unsigned char   fType;          // Symbol types
    unsigned char   fIndex;         ///< Depends on symbol type
    unsigned char   fLength;        ///< Length including terminating null
    unsigned char   fReserved_;     ///< Should be 0
};


/** A macro to compute padded data length. */
#define RCXI_PADDED_LENGTH(len) (((len) + 3) & ~3)

/** A macro to compute the number of pad bytes. */
#define RCXI_PAD_BYTES(len)     ((4 - (len)) & 3)


#endif
