///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExtractSequenceInterval.cpp
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

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_ExtractSequenceIntervalPS.h"
#endif

#include "DPO_ExtractSequenceInterval.h"

const char* DPO_ExtractSequenceInterval::objLongDesc = "Extract Sequence Interval";

DPO_ExtractSequenceInterval :: DPO_ExtractSequenceInterval() :
    MenuObjC(this, objLongDesc),
    exposedStartSeqIndex(*this, startSeqMS, 0, seqIndexStrings),
    exposedEndSeqIndex(*this, endSeqMS, 0, seqIndexStrings)
{
}

DPO_ExtractSequenceInterval :: DPO_ExtractSequenceInterval(const DPO_ExtractSequenceInterval& a) :
    DFO_ExtractSequenceInterval(a),
    MenuObjC(this, objLongDesc),
    exposedStartSeqIndex(*this, startSeqMS, 0, seqIndexStrings),
    exposedEndSeqIndex(*this, endSeqMS, 0, seqIndexStrings)
{
}

DPO_ExtractSequenceInterval& DPO_ExtractSequenceInterval::operator= (const DPO_ExtractSequenceInterval& a)
{
    if (&a != this)
       *((DFO_ExtractSequenceInterval*) this) = a;
    return *this;
}


bool  DPO_ExtractSequenceInterval::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(xyInputDataObjRef);
    ReadFuncObjRef(seqTimesObjRef);

    if (currentObjReadMajorVersion > 3)
    {
        ReadIndexMSSpec(startSeqMS);
        ReadIndexMSSpec(endSeqMS);
    }

    if (currentObjReadMajorVersion < 2)
    {
        ReadFuncObjRef(startSeqMS.indexMasterObjRef);
        startSeqMS.indexIsMaster = ReadBool();
        startSeqMS.enteredIndexValue     = ReadInt();
    }

    adjustTimeToStart = ReadBool();
    startTimeOffsetValue = ReadDouble();

    if (currentObjReadMajorVersion > 0)
    {

        if (currentObjReadMajorVersion > 6)
        {
            removeDuplicates = ReadBool();
        }
        else if (currentObjReadMajorVersion > 4)
        {
            removeDuplicates = ReadBool();
            ReadDouble();  // was dup adjust
        }

        adjustPressureToStart = ReadBool();
        fixedPressureOffset = ReadBool();

        startPressureOffsetValue = ReadDouble();
        if (currentObjReadMajorVersion > 2)
            absValuePressure = ReadBool();
        if (currentObjReadMajorVersion > 5)
            log10Pressure = ReadBool();

        if (currentObjReadMajorVersion > 3)
        {
            setStartAndEnd = ReadBool();
            setFinalFlowRate = ReadBool();
            finalFlowRate = ReadDouble();
        }
    }

    if (currentObjReadMajorVersion < 2)
    {
        exposedStartSeqIndex.ReadExposed();
        CheckForInputFailure("Reading DPO_ExtractSequenceInterval");
        return true;
    }

    if (currentObjReadMajorVersion < 4)
    {
        ReadIndexMSSpec(startSeqMS);
        exposedStartSeqIndex.ReadExposed();
        CheckForInputFailure("Reading DPO_ExtractSequenceInterval");
        return true;
    }

    exposedStartSeqIndex.ReadExposed();
    exposedEndSeqIndex.ReadExposed();

    CheckForInputFailure("Reading DPO_ExtractSequenceInterval");
    return true;
}


void DPO_ExtractSequenceInterval::WriteToFile()
{
    const int objMajVer = 7;      // removed dup adjust, added dup removal
//    const int objMajVer = 6;      // log10 pressure
//    const int objMajVer = 5;    // duplicate adjustment
//    const int objMajVer = 4;    // start & end indexes
//    const int objMajVer = 3;    // abs start pressure
//    const int objMajVer = 2;    // index MS
//    const int objMajVer = 1;    // pressure normalization added
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(xyInputDataObjRef);
    WriteFuncObjRef(seqTimesObjRef);
    WriteIndexMSSpec(startSeqMS);
    WriteIndexMSSpec(endSeqMS);

    WriteBool(adjustTimeToStart);
    WriteDouble(startTimeOffsetValue);

    WriteBool(removeDuplicates);

    WriteBool(adjustPressureToStart);
    WriteBool(fixedPressureOffset);
    WriteDouble(startPressureOffsetValue);
    WriteBool(absValuePressure);
    WriteBool(log10Pressure);
    WriteBool(setStartAndEnd);
    WriteBool(setFinalFlowRate);
    WriteDouble(finalFlowRate);
    WriteLine();

    exposedStartSeqIndex.WriteExposed();
    exposedEndSeqIndex.WriteExposed();
    WriteLine();
}

#ifndef ADCONSOLEAPP

// ************************* menu stuff


DPO_ExtractSequenceIntervalPS& DPO_ExtractSequenceInterval::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ExtractSequenceIntervalPS&>(*assocMenu);
}


void DPO_ExtractSequenceInterval::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ExtractSequenceInterval::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ExtractSequenceIntervalPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_ExtractSequenceInterval::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

void DPO_ExtractSequenceInterval::CalcOutput(FOcalcType  calcType)
{
    DFO_ExtractSequenceInterval::CalcOutput(calcType);

    seqIndexStrings.DeAlloc();
    if (StatusOK())
        seqIndexStrings = sequenceTimesDC->sequenceIDs;
    UpdateUI();
}

bool DPO_ExtractSequenceInterval::ContainsExposedObject()
{
    return exposedStartSeqIndex.IsExposed() || exposedEndSeqIndex.IsExposed();
}

void DPO_ExtractSequenceInterval::UpdateUI()
{
    exposedStartSeqIndex.UpdateIfCreated();
    exposedEndSeqIndex.UpdateIfCreated();
    if (MenuUpdateOK())
    {
        MenuRef().startSeqMSUI.UpdateList(startSeqMS);
        MenuRef().endSeqMSUI.UpdateList(endSeqMS);
    }
}


#endif //ADCONSOLEAPP

