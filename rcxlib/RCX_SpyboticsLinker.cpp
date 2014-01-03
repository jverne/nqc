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

#include <cstring>
#include "RCX_SpyboticsLinker.h"
#include "RCX_Image.h"
#include "RCX_Target.h"
#include "PDebug.h"

using std::memcpy;

static const RCX_ChunkType sChunkOrder[] = {
	kRCX_SubChunk,
	kRCX_TaskChunk,
	kRCX_SoundChunk,
	kRCX_AnimationChunk
};


struct EmptyChunkDef
{
	UByte	fLength;
	UByte	fData[3];	// max of 3 bytes
};

static const EmptyChunkDef sEmptyChunkDefs[] =
{
	{ 0 },			// empty task (not used - tasks are special cases)
	{ 1, 0xf6 },	// empty sub
	{ 1, 0 }, 		// empty sound
	{ 2, 0, 0 }, 	// empty animation
};


RCX_SpyboticsLinker::RCX_SpyboticsLinker()
{
	int i;

	fTarget = getTarget(kRCX_SpyboticsTarget);

	// clear the section start array
	for(i=0; i<kRCX_ChunkTypeCount; ++i) fSectionStarts[i] = -1;

	// allocate slots
	fSlotCount = 0;
	for(i=0; i<sizeof(sChunkOrder) / sizeof(RCX_ChunkType); ++i)
	{
		fSlotCount += fTarget->fRanges[sChunkOrder[i]].fCount;
	}
	fSlots = new Slot[fSlotCount];

	// initialize slots
	int index = 0;
	for(i=0; i<sizeof(sChunkOrder) / sizeof(RCX_ChunkType); ++i)
	{
		RCX_ChunkType type = sChunkOrder[i];
		fSectionStarts[type] = index;

		int end = index + fTarget->fRanges[type].fCount;
		while(index < end)
		{
			fSlots[index].fType = type;
			fSlots[index].fChunk = 0;
			++index;
		}
	}
}


RCX_SpyboticsLinker::~RCX_SpyboticsLinker()
{
	delete [] fSlots;
}


void RCX_SpyboticsLinker::Generate(const RCX_Image &image, std::vector<UByte> &output)
{
	int tocSize = (fSlotCount+2) * 2;

	fTocOffset = 0;
	fContentOffset = tocSize;
	fOutput = &output;
	fOutput->reserve(tocSize + 1000);
	fOutput->resize(fContentOffset);

	PlaceChunks(image);
	GenerateSlots();

	// data area
	AddEntry();

	// end marker
	AddEntry();


	// dump the binary image for debugging
/*
	int length = fOutput->size();
	const UByte* data = &(*fOutput)[0];
	for(int i=0; i<length; ++i) {
		if ((i%16)==0) printf("\n%04x: ", i);
		printf("%02x ", data[i]);
	}
	printf("\n");
/**/

}


void RCX_SpyboticsLinker::PlaceChunks(const RCX_Image &image)
{
	int i;

	for(i=0; i<image.GetChunkCount(); ++i)
	{
		const RCX_Image::Chunk &c = image.GetChunk(i);
		RCX_ChunkType type = c.GetType();

		PASSERT(type >= 0 && type < kRCX_ChunkTypeCount);
		PASSERT(fSectionStarts[type] >=0);

		// compute slot index
		int index = c.GetNumber() - fTarget->fRanges[type].fBase + fSectionStarts[type];

		PASSERT(index>=0 && index<fSlotCount);
		PASSERT(fSlots[index].fType == type);

		// add chunk to index
		fSlots[index].fChunk = &c;
	}
}


void RCX_SpyboticsLinker::GenerateSlots()
{
	for(int i=0; i<fSlotCount; ++i)
	{
		const RCX_Image::Chunk* c = fSlots[i].fChunk;

		if (c)
		{
			AddEntry(c->GetData(), c->GetLength());
                        // end non-empty subroutines with rets (0xF6)
                        if (fSlots[i].fType == kRCX_SubChunk)
                        {
                                UByte rets[1] = {0xF6};
                                AddData(rets, 1);
                        }
		}
		else
		{
			RCX_ChunkType type = fSlots[i].fType;

			const EmptyChunkDef &def = sEmptyChunkDefs[type];
			AddEntry(def.fData, def.fLength);

			// only add endTask for empty tasks
			if (fSlots[i].fType == kRCX_TaskChunk)
			{
				UByte endTask[2] = {0x81, 0};
                                // The MindScript compiler does not offset
                                // task numbers by 8
				endTask[1] = i - fSectionStarts[kRCX_TaskChunk]/*+8*/;
				AddData(endTask, 2);
			}
		}
	}
}


void RCX_SpyboticsLinker::AddEntry(const UByte *data, int length)
{
	// add TOC entry
	int address = fContentOffset + 0x8100;
	(*fOutput)[fTocOffset++] = (address) & 0xff;
	(*fOutput)[fTocOffset++] = (address >> 8) & 0xff;

	if (length) AddData(data, length);
}


void RCX_SpyboticsLinker::AddData(const UByte *data, int length)
{
	if (!length) return;

	// add data
	fOutput->resize(fContentOffset + length);
	memcpy(&(*fOutput)[fContentOffset], data, length);
	fContentOffset += length;
}
