///////////////////////////////////////////////////////////////////////////////////
//
// DPO_BasicSequenceFit.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "nsLibAfx.h"

#include <objDPO/objDPOConfigFile.h>
#include <nsDPO/nsDPOConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_BasicSequenceFitPS.h"
#endif

#include "DPO_BasicSequenceFit.h"

static const char* objLongDesc = "Sequence Fit";

DPO_BasicSequenceFit :: DPO_BasicSequenceFit() :
    MenuObjC(this, objLongDesc)
{
}

DPO_BasicSequenceFit :: DPO_BasicSequenceFit(const DPO_BasicSequenceFit& a) :
    DFO_BasicSequenceFit(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_BasicSequenceFit& DPO_BasicSequenceFit::operator= (const DPO_BasicSequenceFit& a)
{
    if (&a != this)
       *((DFO_BasicSequenceFit*) this) = a;
    return *this;
}


bool  DPO_BasicSequenceFit::ReadFromFile()
{
    ReadObjectHeader();

    // base class
    ReadFuncObjRef(simResultsObjRef);
    fitCalcType = FitCalcType(ReadInt());
    fitSigma = ReadDouble();
    fitIsNormed = ReadBool();

    if (currentObjReadMajorVersion > 0)
    {
        minCoverage = ReadDouble();
        coveragePenalty = ReadDouble();
    }

    if (currentObjReadMajorVersion > 1)
    {
        removeFieldDuplicates = ReadBool();
        removeSimDuplicates = ReadBool();
        duplicateEps = ReadDouble();
    }

    // derived current
    ReadFuncObjRef(xyInputObjRef);
    ReadTimeLimits(timeLimits);

    ReadInterpPoints(interpPoints);

    CheckForInputFailure("Reading DPO_BasicSequenceFit");
    return true;
}


void DPO_BasicSequenceFit::WriteToFile()
{
     const int objMajVer = 2;             // added field & sim reduce
//   const int objMajVer = 1;            // added coverage test & multiplier
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    // base class
    WriteFuncObjRef(simResultsObjRef);
    WriteInt(int(fitCalcType));
    WriteDouble(fitSigma);
    WriteBool(fitIsNormed);

    WriteDouble(minCoverage);           // added v1
    WriteDouble(coveragePenalty);       // added v1

    WriteBool(removeFieldDuplicates);   // v2
    WriteBool(removeSimDuplicates);
    WriteDouble(duplicateEps);


    WriteLine();

    // derived current
    WriteFuncObjRef(xyInputObjRef);
    WriteTimeLimits(timeLimits);

    WriteInterpPoints(interpPoints);
}

#ifndef ADCONSOLEAPP

DPO_BasicSequenceFitPS& DPO_BasicSequenceFit::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_BasicSequenceFitPS&>(*assocMenu);
}


void DPO_BasicSequenceFit::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_BasicSequenceFit::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_BasicSequenceFitPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_BasicSequenceFit::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


#endif //ADCONSOLEAPP

