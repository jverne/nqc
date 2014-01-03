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

#ifndef __RCX_Image_h
#define __RCX_Image_h

#ifndef __RCX_Result_h
#include "RCX_Result.h"
#endif

#ifndef __PTypes_h
#include "PTypes.h"
#endif

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

#ifndef __RCX_Target_h
#include "RCX_Target.h"
#endif

#ifndef _STDIO_H
#include <cstdio>
#endif

#ifndef __RCX_SourceTag_h
#include "RCX_SourceTag.h"
#endif

#include <vector>
#include <string>

using std::vector;
using std::string;

class RCX_Link;
class RCX_Printer;
class RCX_SourceFiles;

class RCX_Image
{
public:
        class Chunk;

        RCX_Image();
        ~RCX_Image()	{ Clear(); }

        RCX_Result	Read(const char *filename);
        bool		Write(const char *filename);

        RCX_Result	Download(RCX_Link *link, int programNumber=0) const;
        void		Print(RCX_Printer *dst, RCX_SourceFiles *sf=0, bool genLASM=false) const;

        int		GetChunkCount() const	{ return fChunks.size(); }
        void		AddChunk(RCX_ChunkType type, UByte number,
                                 const UByte *data, int length, const char *name,
                                 const RCX_SourceTag *tags, int tagCount);

        const Chunk&	GetChunk(int i) const { return *fChunks[i]; }

        void		SetVariable(int index, const char *name);
        void		SetTargetType(RCX_TargetType t) { fTargetType = t; }

        void		Clear();
        int		GetSize() const;

        class	Chunk
        {
        public:

                const UByte*	GetData() const { return fData; }
                int		GetLength() const { return fLength; }
                UByte		GetNumber() const { return fNumber; }
                RCX_ChunkType GetType() const { return fType; }

        private:
		Chunk();
		~Chunk();

                // partial ordering for sorting
                bool	operator<(const Chunk &rhs) const;

                int		fLength;
                UByte*		fData;
                UByte		fNumber;
                RCX_ChunkType fType;
                string		fName;
                RCX_SourceTag*	fTags;
                int		fTagCount;

                friend class RCX_Image;
        };

private:
        struct	Variable
        {
                int	fIndex;
                string	fName;
        };

        const Chunk*	FindChunk(RCX_ChunkType type, UByte number) const;
        std::string*	GetNameString(UByte type, UByte index);

        const Chunk**	BuildIndex() const;

        vector<Chunk*>		fChunks;
        vector<Variable>	fVars;
        RCX_TargetType		fTargetType;
};


#endif
