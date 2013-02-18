///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CalcConfidenceTable.cpp
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
//      I/O and user interface for all osDFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <objDPO/objDPOConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_CalcConfidenceTablePS.h"
#endif
#include "DPO_CalcConfidenceTable.h"

const char* DPO_CalcConfidenceTable::objLongDesc = "Calculate Joint Confidence for Table Data";

DPO_CalcConfidenceTable :: DPO_CalcConfidenceTable() :
    MenuObjC(this, objLongDesc)
{
}

DPO_CalcConfidenceTable :: DPO_CalcConfidenceTable(const DPO_CalcConfidenceTable& a) :
    DFO_CalcConfidenceTable(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_CalcConfidenceTable& DPO_CalcConfidenceTable::operator= (const DPO_CalcConfidenceTable& a)
{
    if (&a != this)
       *((DFO_CalcConfidenceTable*) this) = a;
    return *this;
}


bool  DPO_CalcConfidenceTable::ReadFromFile()
{
    ReadObjectHeader();

    ReadTableColSel(*this);

    nDataPts = ReadInt();
    nParam = ReadInt();

    bestCaseIndex = ReadInt();

    if (currentObjReadMajorVersion == 0)
        ReadInt();

    autoBestCase = ReadBool();

    distribution = DFO_CalcConfidenceTable::Distribution(ReadInt());

    CheckForInputFailure("Reading DPO_CalcConfidenceTable");

    return true;
}


void DPO_CalcConfidenceTable::WriteToFile()
{
    const int objMajVer = 1;  // removed unused par sseColIndex
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteTableColSel(*this);

    WriteInt(nDataPts);
    WriteInt(nParam);
    WriteInt(bestCaseIndex);
//  WriteInt(sseColIndex);  removed v1
    WriteBool(autoBestCase);
    WriteInt(int(distribution));

    WriteLine();

}

#ifndef ADCONSOLEAPP

// ************************* menu stuff


DPO_CalcConfidenceTablePS& DPO_CalcConfidenceTable::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CalcConfidenceTablePS&>(*assocMenu);
}


void DPO_CalcConfidenceTable::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_CalcConfidenceTable::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CalcConfidenceTablePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_CalcConfidenceTable::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


// ************************* expose support

//void DPO_CalcConfidenceTable::CalcOutput(FOcalcType  calcType)
//{
//    DFO_CalcConfidenceTable::CalcOutput(calcType);
//    DPO_SelectOpt::UpdateUI();
//}

#endif //!ADCONSOLEAPP

