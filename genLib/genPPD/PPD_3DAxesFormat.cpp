///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DAxesFormat.cpp
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
//      increment/grid line formatting for 3D plots
//
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "PPD_3DAxesFormatPS.h"
#include "PPD_3DAxesFormat.h"

const char* PPD_3DAxesFormat::objType       = "3DAxesFormat";
const char* PPD_3DAxesFormat::objLongDesc   = "3D Axes Formatting";
const char* PPD_3DAxesFormat::allocID       = "PPD_3DAxesFormat";

PPD_3DAxesFormat :: PPD_3DAxesFormat(PC_3DAxesFormat& axData, PD_3D& axPD)
    : MenuObjC(objID, objType, objLongDesc, objType), axesData(axData), axesPD(axPD)
{
    AutoObjectSetup(allocID);
}



PPD_3DAxesFormatPS& PPD_3DAxesFormat::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPD_3DAxesFormatPS&>(*assocMenu);
}


void PPD_3DAxesFormat::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(axesData, *this);
}


void PPD_3DAxesFormat::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();

    assocMenu = new PPD_3DAxesFormatPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(axesPD);

    PropUpdate();
}


void PPD_3DAxesFormat::PropApply()
{
    MenuRef().GetMenuData(axesData, *this);
    MenuObjC::PropApply();
    PropApplyAssoc(&axesPD);
}

void PPD_3DAxesFormat::PropClear()
{
    axesData.Clear();
    PropUpdate();
}



bool  PPD_3DAxesFormat::ReadFromFile()
{
    ReadDefaultObjectHeader();

    for (int i = 0; i < PC_3DAxesFormat::naxes3DPlanes; i++)
    {
        PC_AxesFormat& uForm = axesData.axesUFormat[i];
        uForm.axesMajorInc = PC_Axes::AxesIncType(ReadInt());
        uForm.axesMinorInc = PC_Axes::AxesIncType(ReadInt());
        uForm.axesLinePos  = PC_AxesFormat::AxesLinePos(ReadInt());
        uForm.axesTicPos   = PC_AxesFormat::AxesTicPos(ReadInt());

        PC_AxesFormat& vForm = axesData.axesVFormat[i];
        vForm.axesMajorInc = PC_Axes::AxesIncType(ReadInt());
        vForm.axesMinorInc = PC_Axes::AxesIncType(ReadInt());
        vForm.axesLinePos  = PC_AxesFormat::AxesLinePos(ReadInt());
        vForm.axesTicPos   = PC_AxesFormat::AxesTicPos(ReadInt());
    }

    axesData.autoFormat      = ReadBool();

    axesData.axesLinePen     = ReadInt();
    axesData.majorPen        = ReadInt();
    axesData.minorPen        = ReadInt();

    axesData.axesLineWidth      = ReadInt();
    axesData.majorTicWidth      = ReadInt();
    axesData.minorTicWidth      = ReadInt();
    axesData.majorGridWidth     = ReadInt();
    axesData.minorGridWidth     = ReadInt();
    axesData.axesOffset         = ReadInt();
    axesData.majorTicLength     = ReadInt();
    axesData.minorTicLength     = ReadInt();

    if (currentObjReadMajorVersion > 0)
    {
        axesData.majorGridLineType = ReadInt();
        axesData.minorGridLineType = ReadInt();
    }

    CheckForInputFailure("Reading PPD_3DAxesFormat");
    return true;
}


void PPD_3DAxesFormat::WriteToFile()
{
    const int objMajVer = 1;  // added grid line types
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    for (int i = 0; i < PC_3DAxesFormat::naxes3DPlanes; i++)
    {
        PC_AxesFormat& uForm = axesData.axesUFormat[i];
        WriteInt(uForm.axesMajorInc);
        WriteInt(uForm.axesMinorInc);
        WriteInt(uForm.axesLinePos);
        WriteInt(uForm.axesTicPos);

        PC_AxesFormat& vForm = axesData.axesVFormat[i];
        WriteInt(vForm.axesMajorInc);
        WriteInt(vForm.axesMinorInc);
        WriteInt(vForm.axesLinePos);
        WriteInt(vForm.axesTicPos);

        WriteLine();
    }


    WriteBool(axesData.autoFormat);
    WriteInt (axesData.axesLinePen );
    WriteInt (axesData.majorPen );
    WriteInt (axesData.minorPen );
    WriteLine();

    WriteInt(axesData.axesLineWidth  );
    WriteInt(axesData.majorTicWidth  );
    WriteInt(axesData.minorTicWidth  );
    WriteInt(axesData.majorGridWidth );
    WriteInt(axesData.minorGridWidth );
    WriteInt(axesData.axesOffset  );
    WriteInt(axesData.majorTicLength );
    WriteInt(axesData.minorTicLength );

    WriteInt(axesData.majorGridLineType);  // added v1
    WriteInt(axesData.minorGridLineType);  // added v1

    WriteLine();

}

