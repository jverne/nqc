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

#ifndef __RCX_SpyboticsLinker_h
#define __RCX_SpyboticsLinker_h

#ifndef __PTypes_h
#include "PTypes.h"
#endif

#ifndef __RCX_Target_h
#include "RCX_Target.h"
#endif

#ifndef __RCX_Image_h
#include "RCX_Image.h"
#endif

#include <vector>

using std::vector;

class RCX_Image;

class RCX_SpyboticsLinker
{
public:
	RCX_SpyboticsLinker();
	~RCX_SpyboticsLinker();

	void	Generate(const RCX_Image &image, vector<UByte> &output);

private:
	struct Slot
	{
		RCX_ChunkType		fType;
		const RCX_Image::Chunk*	fChunk;
	};

	void	PlaceChunks(const RCX_Image &image);
	void	GenerateSlots();

	void	AddEntry(const UByte *data=0, int length=0);
	void	AddData(const UByte *data, int length);

	const RCX_Target*	fTarget;

	int	fSlotCount;	// number of chunks in TOC
	Slot*	fSlots;
	int	fSectionStarts[kRCX_ChunkTypeCount];

	vector<UByte>*	fOutput;
	int	fTocOffset;	// current offset into TOC
	int	fContentOffset;	// current offset for new data
};


#endif
