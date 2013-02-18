///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DAxesLabel.cpp
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
//      axes and increment labeling for 3D plot
//
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "PPD_3DAxesLabelPS.h"
#include "PPD_3DAxesLabel.h"

const char* PPD_3DAxesLabel::objType = "3DAxesLabel";
const char* PPD_3DAxesLabel::objLongDesc = "3D Axes Labelling";
const char* PPD_3DAxesLabel::allocID = "PPD_3DAxesLabel";

PPD_3DAxesLabel :: PPD_3DAxesLabel(PC_3DAxesLabel& laData, PlotDefC& laPD)
    : MenuObjC(objID, objType, objLongDesc, objType), labelData(laData), labelPD(laPD)
{
    AutoObjectSetup(allocID);
}


PPD_3DAxesLabelPS& PPD_3DAxesLabel::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPD_3DAxesLabelPS&>(*assocMenu);
}


void PPD_3DAxesLabel::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(labelData, *this);
}


void PPD_3DAxesLabel::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();

    assocMenu = new PPD_3DAxesLabelPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(true, false);

    PropUpdate();
}


void PPD_3DAxesLabel::PropApply()
{
    MenuRef().GetMenuData(labelData, *this);
    MenuObjC::PropApply();
    PropApplyAssoc(&labelPD);
}



bool  PPD_3DAxesLabel::ReadFromFile()
{
    ReadDefaultObjectHeader();

    labelData.plotAxesLabel         = ReadBool();
    labelData.plotIncrementLabel    = ReadBool();
    labelData.autoPositionLabels    = ReadBool();

    ReadText(labelData.xaxesLabel, PC_3DAxesLabel::maxLabelLen);
    ReadText(labelData.yaxesLabel, PC_3DAxesLabel::maxLabelLen);
    ReadText(labelData.zaxesLabel, PC_3DAxesLabel::maxLabelLen);

    ReadFont(labelData.labelFont);
    ReadFont(labelData.incrementFont);

    labelData.xyLabIsVert       = ReadBool();
    labelData.xLabYPosIsMax     = ReadBool();
    labelData.yLabXPosIsMax     = ReadBool();
    labelData.zLabOrientation   = PC_3DAxesLabel::ZLabOrientation(ReadInt());
    labelData.zLabYPosIsMax     = ReadBool();
    labelData.zLabXPosIsMax     = ReadBool();

    CheckForInputFailure("Reading PPD_3DAxesLabel");
    return true;
}


void PPD_3DAxesLabel::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteBool(labelData.plotAxesLabel);
    WriteBool(labelData.plotIncrementLabel);
    WriteBool(labelData.autoPositionLabels);
    WriteLine();

    WriteText(labelData.xaxesLabel);
    WriteText(labelData.yaxesLabel);
    WriteText(labelData.zaxesLabel);

    WriteFont(labelData.labelFont);
    WriteFont(labelData.incrementFont);

    WriteBool(labelData.xyLabIsVert);
    WriteBool(labelData.xLabYPosIsMax);
    WriteBool(labelData.yLabXPosIsMax);
    WriteInt(labelData.zLabOrientation);
    WriteBool(labelData.zLabYPosIsMax);
    WriteBool(labelData.zLabXPosIsMax);
    WriteLine();
}

