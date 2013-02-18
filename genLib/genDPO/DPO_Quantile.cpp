///////////////////////////////////////////////////////////////////////////////////
//
// DPO_Quantile.cpp
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

#include "DPO_Quantile.h"

DPO_Quantile :: DPO_Quantile(DFO_Quantile* bPO, const char* longDesc) :
    MenuObjC(bPO, longDesc),
    bpRef(*bPO)
{
}

bool  DPO_Quantile::ReadFromFile()
{
    int stVer = ReadInt();

    bpRef.quantileType  = DC_XYCDF::QuantileType(ReadInt());

    // added qtExcel in middle of enum
    if ((stVer == 0) && (bpRef.quantileType == DC_XYCDF::qtExcel))
        bpRef.quantileType  = DC_XYCDF::qtGeneric;

    bpRef.logData       = ReadBool();
    ReadRealMSSpec(bpRef.quantileMS);

    CheckForInputFailure("Reading DPO_Quantile");
    return true;
}

void DPO_Quantile::WriteToFile()
{
    const int stVer = 1; // added Excel quantile type
    //  const int stVer = 0;
    WriteInt(stVer);

    WriteInt(int(bpRef.quantileType));
    WriteBool(bpRef.logData);
    WriteLine();

    WriteRealMSSpec(bpRef.quantileMS);
}

