///////////////////////////////////////////////////////////////////////////////////
//
// PPD_2DPlotAnno.cpp
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
//      axes and increment annotation and formatting for a 2D plot
//
//      derived from PPD_Base.  Adds components for all 2D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "PPD_2DPlotAnnoPS.h"
#include "PPD_2DPlotAnno.h"

const char* PPD_2DPlotAnno::objType     = "2DPlotAnno";
const char* PPD_2DPlotAnno::objLongDesc = "2DPlot Annotation";
const char* PPD_2DPlotAnno::allocID     = "PPD_2DPlotAnno";


PPD_2DPlotAnno :: PPD_2DPlotAnno(PC_2DPlotAnno& anData, PlotDefC& anPD)
    : MenuObjC(objID, objType, objLongDesc, objType), annoData(anData), annoPD(anPD)
{
    AutoObjectSetup(allocID);
}

PPD_2DPlotAnnoPS& PPD_2DPlotAnno::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPD_2DPlotAnnoPS&>(*assocMenu);
}


void PPD_2DPlotAnno::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(annoData, *this);
}


void PPD_2DPlotAnno::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();

    assocMenu = new PPD_2DPlotAnnoPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(annoPD);

    PropUpdate();
}


void PPD_2DPlotAnno::PropApply()
{
    MenuRef().GetMenuData(annoData, *this);
    MenuObjC::PropApply();
    PropApplyAssoc(&annoPD);
}



bool  PPD_2DPlotAnno::ReadFromFile()
{
    ReadDefaultObjectHeader();

    if (currentObjReadMajorVersion > 3)
        annoData.plotAnno    = PC_2DPlotAnno::PlotAnno(ReadInt());
    else
        if (ReadBool())
            annoData.plotAnno  = PC_2DPlotAnno::paXandY;
        else
            annoData.plotAnno  = PC_2DPlotAnno::paNone;


    annoData.axesDataPen     = ReadInt();
    annoData.gridLineDataPen = ReadInt();
    annoData.framePlot       = ReadBool();
    annoData.labelAxes       = ReadBool();
    if (currentObjReadMajorVersion > 2)
        annoData.labelIncrements   = ReadBool();

    annoData.drawAxesLines   = ReadBool();
    annoData.gridOverData    = ReadBool();

    ReadText(annoData.xaxesLabel, PC_2DPlotAnno::maxLabelLen);
    ReadText(annoData.yaxesLabel, PC_2DPlotAnno::maxLabelLen);
    if (currentObjReadMajorVersion == 0)
    {
        char dummy[PC_2DPlotAnno::maxLabelLen];
        ReadText(dummy, PC_2DPlotAnno::maxLabelLen);
    }

    ReadFont(annoData.labelFont);
    ReadFont(annoData.incrementFont);

    annoData.axesLineWidth      = ReadInt();
    annoData.majorTicWidth      = ReadInt();
    annoData.minorTicWidth      = ReadInt();
    annoData.majorGridWidth     = ReadInt();
    annoData.minorGridWidth     = ReadInt();
    annoData.axesOffset         = ReadInt();

    if (currentObjReadMajorVersion > 4)
    {
        annoData.majorGridLineType = ReadInt();
        annoData.minorGridLineType = ReadInt();
    }

    annoData.majorTicLength     = ReadInt();
    annoData.minorTicLength     = ReadInt();

    if (currentObjReadMajorVersion > 1)
    {
        annoData.majorTicOffset = ReadInt();
        annoData.minorTicOffset = ReadInt();
    }
    CheckForInputFailure("Reading PPD_2DPlotAnno");
    return true;
}


void PPD_2DPlotAnno::WriteToFile()
{
    const int objMajVer = 5;              // added grid line types
//    const int objMajVer = 4;            // changed plot anno
//    const int objMajVer = 3;            // added label increment
//    const int objMajVer = 2;            // added syView axes offsets
//    const int objMajVer = 1;            // 3D objects replaced plot anno
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteInt(int(annoData.plotAnno));       // changed v4
    WriteInt (annoData.axesDataPen );
    WriteInt (annoData.gridLineDataPen);
    WriteBool(annoData.framePlot);
    WriteBool(annoData.labelAxes);
    WriteBool(annoData.labelIncrements);
    WriteBool(annoData.drawAxesLines);
    WriteBool(annoData.gridOverData);
    WriteLine();

    WriteText(annoData.xaxesLabel);
    WriteText(annoData.yaxesLabel);

    WriteFont(annoData.labelFont);
    WriteFont(annoData.incrementFont);

    WriteInt(annoData.axesLineWidth  );
    WriteInt(annoData.majorTicWidth  );
    WriteInt(annoData.minorTicWidth  );
    WriteInt(annoData.majorGridWidth );
    WriteInt(annoData.minorGridWidth );
    WriteInt(annoData.axesOffset);

    WriteInt(annoData.majorGridLineType);  // added v5
    WriteInt(annoData.minorGridLineType);  // added v5

    WriteInt(annoData.majorTicLength );
    WriteInt(annoData.minorTicLength );

    WriteInt(annoData.majorTicOffset); // added v2
    WriteInt(annoData.minorTicOffset);

    WriteLine();
}

