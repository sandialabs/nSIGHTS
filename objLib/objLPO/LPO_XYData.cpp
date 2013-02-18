///////////////////////////////////////////////////////////////////////////////////
//
// LPO_XYData.cpp
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

#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "LPO_XYDataPS.h"
#include "LPO_XYData.h"

const char* LPO_XYData::objLongDesc = "Listing of XY Data";

LPO_XYData :: LPO_XYData(ListDefC* assLst) :
    LPO_Base(this, objLongDesc), LFO_XYData(*assLst)
{
}

LPO_XYData :: LPO_XYData(ListDefC* assLst, const LPO_XYData& a) :
    LFO_XYData(a, *assLst),
    LPO_Base(this, objLongDesc)
{
}

LPO_XYData& LPO_XYData::operator= (const LPO_XYData& a)
{
    if (&a != this)
       *((LFO_XYData*) this) = a;

    return *this;
}

// ************************* menu ops

LPO_XYDataPS& LPO_XYData::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<LPO_XYDataPS&>(*assocMenu);
}

void LPO_XYData::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void LPO_XYData::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new LPO_XYDataPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void LPO_XYData::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  LPO_XYData::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(xyDataObjRef);

    listXYDataID = ReadBool();
    listHeaders = ReadBool();
    ReadRealFormat(xyDataFormat);
    xyDataColWidth = ReadInt();

    CheckForInputFailure("Reading LPO_XYData");
    return true;
}

void LPO_XYData::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(xyDataObjRef);

    WriteBool(listXYDataID);
    WriteBool(listHeaders);
    WriteRealFormat(xyDataFormat);
    WriteInt(xyDataColWidth);
    WriteLine();

}

