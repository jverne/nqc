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
