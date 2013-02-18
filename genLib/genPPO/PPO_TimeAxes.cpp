///////////////////////////////////////////////////////////////////////////////////
//
// PPO_TimeAxes.cpp
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genPPO/genPPOConfigFile.h>

#include "PPO_TimeAxesPS.h"
#include "PPO_TimeAxes.h"

const char* PPO_TimeAxes::objLongDesc = "Time Axis";

PPO_TimeAxes :: PPO_TimeAxes(PlotDefC* assPlt)
     : PFO_TimeAxesGL(*assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_TimeAxes :: PPO_TimeAxes(PlotDefC* assPlt, const PPO_TimeAxes& a)
                            : PFO_TimeAxesGL (a, *assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_TimeAxes& PPO_TimeAxes::operator= (const PPO_TimeAxes& a)
{
    if (&a != this)
        PFO_TimeAxesGL::operator=(a);
    return *this;
}

// ************************* menu ops

PPO_TimeAxesPS& PPO_TimeAxes::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_TimeAxesPS&>(*assocMenu);
}


void PPO_TimeAxes::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_TimeAxes::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_TimeAxesPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_TimeAxes::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


bool  PPO_TimeAxes::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);
    ReadFuncObjRef(timeSpecMasterObjRef);
    timeSpecIsMaster = ReadBool();

    dateTimeSpec.timeSpecType = DC_DateTimeSpec::TimeSpecType(ReadInt());
    dateTimeSpec.timeSpecUnits = DC_DateTime::TimeUnits(ReadInt());
    dateTimeSpec.calendarYearStart = ReadInt();

    ReadText(arbitraryTimeStr, dateTimeStrLen);
    ReadText(arbitraryDateStr, dateTimeStrLen);

    autoIncrements = ReadBool();

    minorIncrementUnits = AxesTimeUnits(ReadInt());
    minorIncrementCount = ReadInt();

    majorIncrementUnits = AxesTimeUnits(ReadInt());
    majorIncrementCount = ReadInt();

    incrementLabelType = IncrementLabelType(ReadInt());
    timeFormat = DC_DateTime::TimeFormat(ReadInt());
    ReadDateFormat(dateFormat);

    ReadTextWithTrailingBlanks(dateTimeDelimStr, dateTimeStrLen);

    ReadText(axesLabel, maxLabelLen);

    CheckForInputFailure("Reading PPO_TimeAxes");
    return true;
}

void PPO_TimeAxes::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(penSetObjRef);
    WriteFuncObjRef(timeSpecMasterObjRef);
    WriteBool(timeSpecIsMaster);

    WriteInt(int(dateTimeSpec.timeSpecType));
    WriteInt(int(dateTimeSpec.timeSpecUnits));
    WriteInt(dateTimeSpec.calendarYearStart);
    WriteLine();

    WriteText(arbitraryTimeStr);
    WriteText(arbitraryDateStr);

    WriteBool(autoIncrements);
    WriteInt(int(minorIncrementUnits));
    WriteInt(minorIncrementCount);

    WriteInt(int(majorIncrementUnits));
    WriteInt(majorIncrementCount);

    WriteInt(int(incrementLabelType));
    WriteInt(int(timeFormat));
    WriteLine();
    WriteDateFormat(dateFormat);
    WriteTextWithTrailingBlanks(dateTimeDelimStr);

    WriteText(axesLabel);
}

