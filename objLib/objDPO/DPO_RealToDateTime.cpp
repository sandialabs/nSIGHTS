///////////////////////////////////////////////////////////////////////////////////
//
// DPO_RealToDateTime.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>
#include <genPPO/genPPOConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_RealToDateTimePS.h"
#endif
#include "DPO_RealToDateTime.h"

const char* DPO_RealToDateTime::objLongDesc = "Convert Real Value to Time/Date Label";

DPO_RealToDateTime :: DPO_RealToDateTime() :
    MenuObjC(this, objLongDesc)
{
}

DPO_RealToDateTime :: DPO_RealToDateTime(const DPO_RealToDateTime& a) :
    DFO_RealToDateTime(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_RealToDateTime& DPO_RealToDateTime::operator= (const DPO_RealToDateTime& a)
{
    if (&a != this)
    {
       *((DFO_RealToDateTime*) this) = a;
    }
    return *this;
}


bool  DPO_RealToDateTime::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(inputRealObjRef);
    ReadFuncObjRef(timeSpecMasterObjRef);
    timeSpecIsMaster = ReadBool();

    dateTimeSpec.timeSpecType = DC_DateTimeSpec::TimeSpecType(ReadInt());
    dateTimeSpec.timeSpecUnits = DC_DateTime::TimeUnits(ReadInt());
    dateTimeSpec.calendarYearStart = ReadInt();

    ReadText(arbitraryTimeStr, dateTimeStrLen);
    ReadText(arbitraryDateStr, dateTimeStrLen);

    labelType = LabelType(ReadInt());
    timeFormat = DC_DateTime::TimeFormat(ReadInt());
    ReadDateFormat(dateFormat);

    ReadTextWithTrailingBlanks(dateTimeDelimStr, dateTimeStrLen);

    ReadLegendOverride(labelOverride);

    CheckForInputFailure("Reading DPO_RealToDateTime");
    return true;
}

void DPO_RealToDateTime::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(inputRealObjRef);
    WriteFuncObjRef(timeSpecMasterObjRef);
    WriteBool(timeSpecIsMaster);

    WriteInt(int(dateTimeSpec.timeSpecType));
    WriteInt(int(dateTimeSpec.timeSpecUnits));
    WriteInt(dateTimeSpec.calendarYearStart);
    WriteLine();

    WriteText(arbitraryTimeStr);
    WriteText(arbitraryDateStr);


    WriteInt(int(labelType));
    WriteInt(int(timeFormat));
    WriteLine();
    WriteDateFormat(dateFormat);
    WriteTextWithTrailingBlanks(dateTimeDelimStr);

    WriteLegendOverride(labelOverride);
}


#ifndef ADCONSOLEAPP

DPO_RealToDateTimePS& DPO_RealToDateTime::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_RealToDateTimePS&>(*assocMenu);
}

void DPO_RealToDateTime::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_RealToDateTime::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_RealToDateTimePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_RealToDateTime::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

