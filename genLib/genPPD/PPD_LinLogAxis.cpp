///////////////////////////////////////////////////////////////////////////////////
//
// PPD_LinLogAxis.cpp
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
//      a single axis for an 2DXY or 3DXYZ plot
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genPlotClass/PD_3D.h>
#include <genApp/AppConfigFile.h>

#include "PPD_LinLogAxisPS.h"
#include "PPD_LinLogAxis.h"

const char* PPD_LinLogAxis::objType = "1D Axes";
const char* PPD_LinLogAxis::objLongDesc = "Single 1D Axis";
const char* PPD_LinLogAxis::allocID = "PPD_LinLogAxis";

PPD_LinLogAxis :: PPD_LinLogAxis(LinLogAxis1D& axData, LinLogAxis1D& setaxData, PlotDefC& axPD,
                                 const char* axID)
    : MenuObjC(objID, axID, objLongDesc, objType),
    axisData(axData), setaxisData(setaxData), axisPD(axPD)
{
    AutoObjectSetup(allocID);
    is3D = (dynamic_cast<PD_3D*>(&axPD) != 0);
}

// ************************* menu ops

PPD_LinLogAxisPS& PPD_LinLogAxis::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPD_LinLogAxisPS&>(*assocMenu);
}


void PPD_LinLogAxis::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(axisData, *this);
}


void PPD_LinLogAxis::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPD_LinLogAxisPS();
    MenuRef().InitMenu(is3D);
    MenuRef().MenuOpen(true, false);

    PropUpdate();
}


void PPD_LinLogAxis::PropApply()
{
    MenuRef().GetMenuData(axisData, *this);
    axisPD.axesHaveBeenSet = false; // clear old settings
    MenuObjC::PropApply();
    PropApplyAssoc(&axisPD);
}


bool  PPD_LinLogAxis::ReadFromFile()
{
    ReadDefaultObjectHeader();
    axisData.autoScaleAxes   = ReadBool();
    axisData.axesAreLinear   = ReadBool();
    if (currentObjReadMajorVersion > 4)
        axisData.incrFont.fontRotation = PSC_Font::FontRotation(ReadInt());
    axisData.axisLinMin      = ReadDouble();
    axisData.axisLinMax      = ReadDouble();
    axisData.axisLogMin      = ReadDouble();
    axisData.axisLogMax      = ReadDouble();

    axisData.axisMajorIncs   = PC_Axes::AxesIncType(ReadInt());
    axisData.autoMajorIncs   = ReadBool();
    axisData.majorIncVal     = ReadDouble();
    if (currentObjReadMajorVersion > 3)
        axisData.startAtFirst  = ReadBool();

    axisData.axisMinorIncs   = PC_Axes::AxesIncType(ReadInt());
    axisData.autoMinorIncs   = ReadBool();
    axisData.nminorPerMajor  = ReadInt();

    axisData.axisIncFormat.format             = NumConvFormat(ReadInt());
    axisData.axisIncFormat.digitsPrecision    = ReadInt();
    if (currentObjReadMajorVersion > 2)
    {
        axisData.cursorReportFormat.format             = NumConvFormat(ReadInt());
        axisData.cursorReportFormat.digitsPrecision    = ReadInt();
    }

    if (currentObjReadMajorVersion > 1)
    {
        // to get inc settings right
        setaxisData = axisData;

        setaxisData.autoScaleAxes   = false; // by definition
        setaxisData.axesAreLinear   = ReadBool();
        setaxisData.axisLinMin      = ReadDouble();
        setaxisData.axisLinMax      = ReadDouble();
        setaxisData.axisLogMin      = ReadDouble();
        setaxisData.axisLogMax      = ReadDouble();

        if (axisPD.axesHaveBeenSet)  // this should have already been read
            axisData = setaxisData;
    }

    CheckForInputFailure("Reading PPD_LinLogAxis");
    return true;
}

void PPD_LinLogAxis::WriteToFile()
{
    const int objMajVer = 5; //  added increment font rotation
//    const int objMajVer = 4; //  added start at first inc
//    const int objMajVer = 3; //  added cursor report formatting support
//    const int objMajVer = 2; //  added set axes support
//    const int objMajVer = 1; //  change to ver 1 with axes formatting stuff
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    SetRealConvFormat(SC_RealFormat(ncf_Scientific, 8));

    WriteBool  (axisData.autoScaleAxes  );
    WriteBool  (axisData.axesAreLinear  );
    WriteInt(int(axisData.incrFont.fontRotation)); //added v5

    WriteDouble(axisData.axisLinMin   );
    WriteDouble(axisData.axisLinMax   );
    WriteDouble(axisData.axisLogMin   );
    WriteDouble(axisData.axisLogMax   );
    WriteLine();

    WriteInt(int(axisData.axisMajorIncs));
    WriteBool(axisData.autoMajorIncs    );
    WriteDouble(axisData.majorIncVal    );
    WriteBool(axisData.startAtFirst ); // aded v4
    WriteLine();

    WriteInt(int(axisData.axisMinorIncs));
    WriteBool(axisData.autoMinorIncs    );
    WriteInt(axisData.nminorPerMajor    );
    WriteLine();

    WriteInt(axisData.axisIncFormat.format         );
    WriteInt(axisData.axisIncFormat.digitsPrecision);
    WriteInt(axisData.cursorReportFormat.format         ); // added v3
    WriteInt(axisData.cursorReportFormat.digitsPrecision);
    WriteLine();

    // set axes limits/type only -- added v1
    WriteBool  (setaxisData.axesAreLinear  );
    WriteDouble(setaxisData.axisLinMin   );
    WriteDouble(setaxisData.axisLinMax   );
    WriteDouble(setaxisData.axisLogMin   );
    WriteDouble(setaxisData.axisLogMax   );
    WriteLine();

}

