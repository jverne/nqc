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
#include <cstring>
#include <cstdio>
#include "RCX_Image.h"
#include "RCX_Disasm.h"
#include "RCX_Link.h"
#include "RCX_Cmd.h"

#include "rcxifile.h"

using std::sprintf;
using std::fopen;
using std::memcpy;

void Write4(ULong d, FILE *fp);
void Write2(UShort d, FILE *fp);
ULong Read4(FILE *fp);
UShort Read2(FILE *fp);
void WriteSymbol(UByte type, UByte index, const char *name, FILE *fp);

static bool IsCodeChunkType(RCX_ChunkType type);
static void GetChunkTypeName(char *dst, RCX_ChunkType type);
static void HexDump(RCX_Printer *dst, bool genLASM, string name,
  RCX_ChunkType type, int ChunkNum, const UByte *data, int count);


RCX_Image::RCX_Image()
{
}


void RCX_Image::Clear()
{
        for(int i=0; i<(int)fChunks.size(); ++i) {
                delete fChunks[i];
        }
        fChunks.resize(0);

        fVars.resize(0);
}



void RCX_Image::AddChunk(RCX_ChunkType type, UByte number, const UByte *data,
                                                        int length, const char *name,
                                                        const RCX_SourceTag *tags, int tagCount)
{
        Chunk *f = new Chunk();

        f->fData = new UByte[length];
        f->fLength = length;
        f->fType = type;
        f->fNumber = number;
        memcpy(f->fData, data, (size_t)length);

        f->fName = name;

        if (tags && tagCount) {
                f->fTagCount = tagCount;
                f->fTags = new RCX_SourceTag[tagCount];
                memcpy(f->fTags, tags, tagCount * sizeof(RCX_SourceTag));
        }
        else {
                f->fTags = 0;
                f->fTagCount = 0;
        }

        fChunks.push_back(f);
}


void RCX_Image::SetVariable(int index, const char *name)
{
        int i = fVars.size();
        fVars.resize(i+1);
        fVars[i].fName = name;
        fVars[i].fIndex = index;
}


void RCX_Image::Print(RCX_Printer *dst, RCX_SourceFiles *sf, bool genLASM) const
{
        RCX_Disasm disasm(fTargetType);
        char line[256];
        const Chunk **index = BuildIndex();

        for(size_t i=0; i<fVars.size(); i++)
        {
                sprintf(line, "%s Var %d = %s\n", genLASM ? ";" : "***", fVars[i].fIndex, fVars[i].fName.c_str());
                dst->Print(line);
        }

        for(int i=0; i<(int)fChunks.size(); i++)
        {
                const Chunk &f = *index[i];

                char typeName[10];

                GetChunkTypeName(typeName, f.fType);
                sprintf(line, "\n%s %s %d", genLASM ? ";" : "***", typeName, f.fNumber);
                dst->Print(line);

                sprintf(line, " = %s, size: %d bytes\n", f.fName.c_str(), f.fLength);
                dst->Print(line);

                if (IsCodeChunkType(f.fType))
                {
                        disasm.Print(dst, genLASM, f.fName, f.fType, f.fNumber, f.fData, f.fLength, sf, f.fTags, f.fTagCount);
                }
                else
                {
                        HexDump(dst, genLASM, f.fName, f.fType, f.fNumber, f.fData, f.fLength);
                }
        }

        sprintf(line, "\n%sTotal size: %d bytes\n", genLASM ? ";" : "", GetSize());
        dst->Print(line);

        delete [] index;
}



const RCX_Image::Chunk **RCX_Image::BuildIndex() const
{
        // build a sorted index for fragments
        const Chunk **index;

        // start with order in image
        index = new const Chunk*[fChunks.size()];
        for(int i=0; i<(int)fChunks.size(); ++i)
                index[i] = fChunks[i];

        // now bubble sort
        bool dirty;
        do
        {
                dirty = false;
                for(int i=0; i<(int)fChunks.size()-1; ++i)
                {
                        if (*index[i+1] < *index[i])
                        {
                                const Chunk *f = index[i+1];
                                index[i+1] = index[i];
                                index[i] = f;
                                dirty = true;
                        }
                }
        } while(dirty);

        return index;
}


bool RCX_Image::Chunk::operator<(const RCX_Image::Chunk &rhs) const
{
        if (fType == rhs.fType)
        {
                return fNumber < rhs.fNumber;
        }
        else
        {
                return fType < rhs.fType;
        }
}


RCX_Result RCX_Image::Download(RCX_Link *link, int programNumber) const
{
        return link->Download(*this, programNumber);
}


RCX_Result RCX_Image::Read(const char *filename)
{
        FILE *fp;
        UShort count;
        UShort symbolCount;
        int i;

        Clear();

        fp = fopen(filename, "rb");
        if (!fp) return kRCX_FileError;

        // check signature
        if (Read4(fp) != kRCXI_Signature) goto ErrorReturn;

        // check version
        if (Read2(fp) > kRCXI_CurrentVersion) goto ErrorReturn;

        // get counts
        count = Read2(fp);
        symbolCount = Read2(fp);

        // target type
        fTargetType = (RCX_TargetType)getc(fp);

        // reserved byte
        getc(fp);

        fChunks.reserve(count);

        for(i=0; i<count; i++)
        {
                UByte type;
                UByte number;
                UShort length;
                Chunk *f;

                type = (UByte) getc(fp);
                if (type > 2) goto ErrorReturn;

                number = (UByte)getc(fp);
                length = Read2(fp);

                if (feof(fp) && (length != 0)) goto ErrorReturn;

                f = new Chunk();
                f->fType = (RCX_ChunkType)type;
                f->fNumber = number;
                f->fLength = length;

                if (length)
                {
                        f->fData = new UByte[length];
                        if ((UShort)fread(f->fData, 1, length, fp) != length) goto ErrorReturn;
                        fseek(fp, RCXI_PAD_BYTES(length),SEEK_CUR);
                }
                else
                        f->fData = nil;

                fChunks.push_back(f);
        }

        for(i=0; i<symbolCount; ++i)
        {
                UByte type;
                UByte index;
                UByte length;
                string *stringPtr;

                // read symbol headers
                type = getc(fp);
                index = getc(fp);
                length = getc(fp);
                (void)getc(fp);	// reserved

                if (length)
                {
                        stringPtr = GetNameString(type, index);
                        if (stringPtr)
                        {
                                char buf[256];
                                fread(buf, length, 1, fp);
                                *stringPtr = buf;
                        }
                        else
                        {
                                fseek(fp, length, SEEK_CUR);
                        }
                }
        }

        fclose(fp);
        return kRCX_OK;

ErrorReturn:
        fclose(fp);
        return kRCX_FormatError;
}


string *RCX_Image::GetNameString(UByte type, UByte index)
{
        switch(type)
        {
        case kRCXI_TaskSymbol:
        case kRCXI_SubSymbol:
                for(int i=0; i<(int)fChunks.size(); ++i)
                {
                        if (fChunks[i]->fType == type &&
                                fChunks[i]->fNumber == index)
                        {
                                return &fChunks[i]->fName;
                        }
                }
                return 0;
        case kRCXI_VarSymbol:
        {
                int n = fVars.size();
                fVars.resize(n+1);
                fVars[n].fIndex = index;
                return &fVars[n].fName;
        }
        default:
                return 0;
        }
}


bool RCX_Image::Write(const char *filename)
{
        FILE *fp;
        Chunk *f;
        int pad;
        long zeros = 0;

        fp = fopen(filename, "wb");
        if (!fp) return false;

        // write header
        Write4(kRCXI_Signature, fp);
        Write2(kRCXI_CurrentVersion, fp);
        Write2((UShort)fChunks.size(), fp);
        Write2((UShort)fChunks.size() + fVars.size(), fp);
        putc(fTargetType, fp);
        putc(0, fp);

        // write fragments
        for(int i=0; i<(int)fChunks.size(); i++)
        {
                f = fChunks[i];
                putc(f->fType, fp);	// this assumes that internal fragment types match the file format
                putc(f->fNumber, fp);
                Write2((UShort)f->fLength, fp);
                if (f->fLength)
                        fwrite(f->fData, 1, (size_t)f->fLength, fp);
                pad = RCXI_PAD_BYTES(f->fLength);
                if (pad)
                        fwrite(&zeros, (ULong) pad, 1, fp);
        }

        // write code symbols
        for(int i=0; i<(int)fChunks.size(); i++)
        {
                f = fChunks[i];
                WriteSymbol(f->fType, f->fNumber, f->fName.c_str(), fp); // this assumes that internal fragment types match the file format
        }

        // write var symbols
        for(size_t i=0; i<fVars.size(); i++)
        {
                WriteSymbol(kRCXI_VarSymbol, fVars[i].fIndex, fVars[i].fName.c_str(), fp);
        }

        fclose(fp);
        return true;
}


int RCX_Image::GetSize() const
{
        int size = 0;
        int i;

        for(i=0; i<(int)fChunks.size(); ++i)
                size += fChunks[i]->fLength;

        return size;
}


const RCX_Image::Chunk* RCX_Image::FindChunk(RCX_ChunkType type, UByte number) const
{
        for(int i=0; i<(int)fChunks.size(); ++i) {
                if (fChunks[i]->fType==type &&
                        fChunks[i]->fNumber == number)
                        return fChunks[i];
        }

        return 0;
}


RCX_Image::Chunk::Chunk()
{
        fData = nil;
        fLength = 0;
        fTags = nil;
        fTagCount = 0;
}


RCX_Image::Chunk::~Chunk()
{
        delete [] fData;
        delete [] fTags;
}


void Write4(ULong d, FILE *fp)
{
        putc((UByte)d, fp);
        putc((UByte)(d>>8), fp);
        putc((UByte)(d>>16), fp);
        putc((UByte)(d>>24), fp);
}


void Write2(UShort d, FILE *fp)
{
        putc(d, fp);
        putc(d>>8, fp);
}


void WriteSymbol(UByte type, UByte index, const char *name, FILE *fp)
{
        int length = name ? strlen(name)+1 : 0;
        putc(type, fp);
        putc(index, fp);
        putc((UByte)length, fp);
        putc(0, fp);

        if (length) fwrite(name, length, 1, fp);
}


ULong Read4(FILE *fp)
{
        ULong d;

        d = (UByte)getc(fp);
        d += (UByte)getc(fp) << 8;
        d += (UByte)getc(fp) << 16;
        d += (UByte)getc(fp) << 24;

        return d;
}

UShort Read2(FILE *fp)
{
        UShort d;

        d = (UByte)getc(fp);
        d += (UByte)getc(fp) << 8;

        return d;
}

/*
static char *NewString(const char *s)
{
        if (!s) return nil;

        char *newString = new char[strlen(s) + 1];
        strcpy(newString, s);
        return newString;
}
*/


void GetChunkTypeName(char *dst, RCX_ChunkType type)
{
        switch(type) {
                case kRCX_TaskChunk:
                        strcpy(dst, "Task");
                        break;
                case kRCX_SubChunk:
                        strcpy(dst, "Sub");
                        break;
                case kRCX_SoundChunk:
                        strcpy(dst, "Sound");
                        break;
                case kRCX_AnimationChunk:
                        strcpy(dst, "Animation");
                        break;
//                case kRCX_DataChunk:
//                        strcpy(dst, "Data");
//                        break;
                default:
                        sprintf(dst, "Res %d", type);
                        break;
        }
}


bool IsCodeChunkType(RCX_ChunkType type)
{
        switch(type) {
                case kRCX_TaskChunk:
                case kRCX_SubChunk:
                case kRCX_SoundChunk:
                        return true;
                default:
                        return false;
        }
}

void HexDump(RCX_Printer *dst, bool genLASM, string name,
  RCX_ChunkType type, int ChunkNum, const UByte *data, int count)
{
        char line[256];
        char *ptr;

        int offset = 0;

        if (genLASM)
        {
          // output the header
          sprintf(line, ";%s\n", name.c_str());
          dst->Print(line);
          if (type == kRCX_AnimationChunk)
            sprintf(line, "\tmood\t%d\n", ChunkNum);
//          else if (type == kRCX_DataChunk)
//            sprintf(line, "\tdata\t%d\n", ChunkNum);
          dst->Print(line);

          // output the body

/*
          if (type == kRCX_DataChunk)
          {
              sprintf(line, "");
              ptr = line;
              sprintf(ptr, "\t\t");
              ptr += strlen(ptr);
              // print data bytes (all but one)
              for(int i=0; i<count-1; ++i)
              {
                      sprintf(ptr, "%d,", *data++);
                      ptr += strlen(ptr);
              }
              // print the last byte
              sprintf(ptr, "%d", *data++);
              ptr += strlen(ptr);
              strcat(ptr, "\n");
              dst->Print(line);
          }
          else
*/
          {
              // output 2 bytes per line
              while(count)
              {
                  // max of 2 bytes per line
                  int n = count;
                  if (n > 2)
                      n = 2;

                  // print tab
                  sprintf(line, "\t");
                  ptr = line + strlen(line);

                  // print data bytes
                  for(int i=0; i<n; ++i)
                  {
                          sprintf(ptr, "%d ", *data++);
                          ptr += strlen(ptr);
                  }

                  strcat(ptr, "\n");
                  dst->Print(line);

                  count -= n;
              }
          }
          // output the footer
          if (type == kRCX_AnimationChunk)
            sprintf(line, "\tendm\n");
//          else if (type == kRCX_DataChunk)
//            sprintf(line, "\tendd\n");
          dst->Print(line);
        }
        else
        {
            while(count)
            {
                    // max of 16 bytes per line
                    int n = count;
                    if (n > 16)
                    {
                            n = 16;
                    }

                    // print offset
                    sprintf(line, "%03x  ", offset);
                    ptr = line + strlen(line);

                    // print data bytes
                    for(int i=0; i<n; ++i)
                    {
                            sprintf(ptr, "%02x ", *data++);
                            ptr += strlen(ptr);
                    }

                    strcat(ptr, "\n");
                    dst->Print(line);

                    count -= n;
                    offset += count;
            }
        }

}

