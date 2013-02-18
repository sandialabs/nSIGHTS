///////////////////////////////////////////////////////////////////////////////////
//
// PC_DumpTGA.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sandia Corporation nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////
//
// DESCRIPTION:
//
//      Platform independent base classes for plot definitions and plot functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <genClass/U_Str.h>
#include <genPlotClass/PC_DumpTGA.h>

PC_DumpTGA :: PC_DumpTGA()
{
    SetDefault();
}

PC_DumpTGA :: PC_DumpTGA(const PC_DumpTGA& a)
{
    LocalCopy(a);
}

PC_DumpTGA& PC_DumpTGA::operator= (const PC_DumpTGA& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void PC_DumpTGA::LocalCopy(const PC_DumpTGA& a)
{
    CopyString(tgaRootFile, a.tgaRootFile, stdFileStrLen);
    dumpFormat = a.dumpFormat;
    dumpMethod   = a.dumpMethod;
    nextIncrement = a.nextIncrement;
    dumpFrameCount  = a.dumpFrameCount;
    qualityJPEG = a.qualityJPEG;
}

void PC_DumpTGA::SetDefault()
{
    CopyString(tgaRootFile, "app", stdFileStrLen);
    dumpFormat = dumpJPEG;
    dumpMethod = dtga_SemiAuto;
    nextIncrement   = 0;
    dumpFrameCount  = 1;
    qualityJPEG = 100;
}

void PC_DumpTGA::GetNextFileName(char  nextFileName[],
                                 int   maxLen)
{
    CopyString(nextFileName, tgaRootFile, maxLen);
    if (dumpMethod != dtga_Manual)
    {
        char intStr[8];
        IntToString(nextIncrement, intStr, 4, 8);
        ConcatString(nextFileName, intStr, maxLen);
        const char* fileSuffix[] = {".TGA", ".JPG", ".BMP"};
        ConcatString(nextFileName, fileSuffix[dumpFormat], maxLen);
        nextIncrement++;
    }
}

