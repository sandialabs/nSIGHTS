///////////////////////////////////////////////////////////////////////////////////
//
// DPO_Histogram.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              File I/O for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "DPO_Histogram.h"

DPO_Histogram :: DPO_Histogram(DFO_Histogram* bPO, const char* longDesc) :
    MenuObjC(bPO, longDesc),
    bpRef(*bPO)
{
}

bool  DPO_Histogram::ReadFromFile()
{
    int stVer = ReadInt();

    if (stVer > 0)
        bpRef.autoBinWidth = ReadBool(); // added v1

    if (stVer > 1)
        bpRef.intBinWidth = ReadBool(); // added v2


    bpRef.autoLimits        = ReadBool();
    bpRef.logHistogram      = ReadBool();
    bpRef.numberOfBins      = ReadInt();
    bpRef.cumulative        = ReadBool();
    bpRef.outputXisIndex    = ReadBool();
    bpRef.normalizeY        = ReadBool();

    ReadRealMSSpec(bpRef.minValueMS);
    ReadRealMSSpec(bpRef.maxValueMS);

    CheckForInputFailure("Reading DPO_Histogram");
    return true;
}

void DPO_Histogram::WriteToFile()
{
    const int stVer = 2; //added integer bin width
//    const int stVer = 1; // added auto bin width
//    const int stVer = 0;
    WriteInt(stVer);

    WriteBool(bpRef.autoBinWidth); // added v1
    WriteBool(bpRef.intBinWidth); // added v1

    WriteBool(bpRef.autoLimits);
    WriteBool(bpRef.logHistogram);
    WriteInt(bpRef.numberOfBins);
    WriteBool(bpRef.cumulative);
    WriteBool(bpRef.outputXisIndex);
    WriteBool(bpRef.normalizeY);
    WriteLine();

    WriteRealMSSpec(bpRef.minValueMS);
    WriteRealMSSpec(bpRef.maxValueMS);
}

