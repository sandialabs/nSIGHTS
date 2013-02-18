///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadMiniTroll.cpp
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
#include "DPO_ReadMiniTrollPS.h"
#endif

#include "DPO_ReadMiniTroll.h"


const char* DPO_ReadMiniTroll::objLongDesc = "Read MiniTroll/Westbay Data";

DPO_ReadMiniTroll :: DPO_ReadMiniTroll() : MenuObjC(this, objLongDesc)
{
    RegisterFileName(mtFname);
}

DPO_ReadMiniTroll :: DPO_ReadMiniTroll(const DPO_ReadMiniTroll& a) : DFO_ReadMiniTroll(a), MenuObjC(this, objLongDesc)
{
    RegisterFileName(mtFname);
}

DPO_ReadMiniTroll::~DPO_ReadMiniTroll()
{
}

DPO_ReadMiniTroll& DPO_ReadMiniTroll::operator= (const DPO_ReadMiniTroll& a)
{
    if (&a != this)
       *((DFO_ReadMiniTroll*) this) = a;
    return *this;
}


bool  DPO_ReadMiniTroll::ReadFromFile()
{
    ReadObjectHeader();

    ReadFileName(mtFname);

    if (currentObjReadMajorVersion > 0)
    {
        timeSpecDC.timeSpecUnits = DC_DateTime::TimeUnits(ReadInt());
        if (currentObjReadMajorVersion > 2)
        {
            timeSpecDC.timeSpecType = DC_DateTimeSpec::TimeSpecType(ReadInt());
        }
        else
        {
            if (ReadBool())
                timeSpecDC.timeSpecType = DC_DateTimeSpec::tstExcelDays;
            else
                timeSpecDC.timeSpecType = DC_DateTimeSpec::tstArbitraryZero;
        }
        timeSpecDC.calendarYearStart = ReadInt();
    }
    else
    {
        ReadInt();
        ReadInt();
        ReadInt();
    }

    if (currentObjReadMajorVersion > 1)
    {
        if (currentObjReadMajorVersion > 4)
            fileType = FileType(ReadInt());
        else
        {
            if (ReadBool())
                fileType = mtCSV;
            else
                fileType = mtRawText;
//          mfFileIsCSV = ReadBool();
        }

        readAllColumns = ReadBool();
        yColumn = ReadInt();

        if ((fileType == mtRawText) && (currentObjReadMajorVersion < 4))  
            // reset to consistent usage
            readAllColumns = true;

        if (currentObjReadMajorVersion > 5)  
            minimumDeltaT = ReadDouble();
        if (currentObjReadMajorVersion > 6)
        {
            dasReadID = ReadBool();
            dasReadKey = ReadBool();
        }
    }

    CheckForInputFailure("Reading DPO_ReadMiniTroll");
    return true;
}


void DPO_ReadMiniTroll::WriteToFile()
{
    const int objMajVer = 7;    // FB 257 - add key & ID top OPG DAS
//    const int objMajVer = 6;    // Westbay
//    const int objMajVer = 5;    // CSV DAS
//    const int objMajVer = 4;    // column processing for text - no new vars, however, must be read differently
//    const int objMajVer = 3;  // new el time format
//    const int objMajVer = 2;  // read CSV format
//    const int objMajVer = 1;  // restructured
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteText(mtFname);

    WriteInt(int(timeSpecDC.timeSpecUnits));
    WriteInt(int(timeSpecDC.timeSpecType));
    WriteInt(timeSpecDC.calendarYearStart);
    WriteLine();

//  WriteBool(mfFileIsCSV); removed V4
    WriteInt(int(fileType));
    WriteBool(readAllColumns);
    WriteInt(yColumn);
    WriteDouble(minimumDeltaT);  // added v6
    WriteBool(dasReadID);   // added v7
    WriteBool(dasReadKey);
    WriteLine();
}

#ifndef ADCONSOLEAPP


// ************************* menu stuff


DPO_ReadMiniTrollPS& DPO_ReadMiniTroll::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ReadMiniTrollPS&>(*assocMenu);
}


void DPO_ReadMiniTroll::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_ReadMiniTroll::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ReadMiniTrollPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(true, true);

    PropUpdate();
}


void DPO_ReadMiniTroll::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


#endif //ADCONSOLEAPP

