///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DLighting.cpp
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
//      lighting specification for 3D plots
//
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "PPD_3DLightingPS.h"
#include "PPD_3DLighting.h"

const char* PPD_3DLighting::objType = "3D Lighting";
const char* PPD_3DLighting::objLongDesc = "3D Light Setup";
const char* PPD_3DLighting::allocID = "PPD_3DLighting";

PPD_3DLighting :: PPD_3DLighting(PC_Lighting& lightData, PlotDefC& lightPD)
    : MenuObjC(objID, objType, objLongDesc, objType), lightingData(lightData), lightingPD(lightPD)
{
    AutoObjectSetup(allocID);
}

PPD_3DLightingPS& PPD_3DLighting::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPD_3DLightingPS&>(*assocMenu);
}


void PPD_3DLighting::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(lightingData, *this);
}


void PPD_3DLighting::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();

    assocMenu = new PPD_3DLightingPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(lightingPD);

    PropUpdate();
}


void PPD_3DLighting::PropApply()
{
    MenuRef().GetMenuData(lightingData, *this);
    MenuObjC::PropApply();
    PropApplyAssoc(&lightingPD);
}



bool  PPD_3DLighting::ReadFromFile()
{
    ReadDefaultObjectHeader();

    lightingData.useLighting = ReadBool();

    lightingData.ambientPen = ReadInt();
    lightingData.ambientIntensity = ReadDouble();
    lightingData.ambientLightAlpha = ReadDouble();

    lightingData.useDefaultLight = ReadBool();
    lightingData.showDefaultLight = ReadBool();
    lightingData.defaultDiffuseLightPen = ReadInt();
    lightingData.defaultDiffuseLightIntensity = ReadDouble();
    lightingData.defaultDiffuseLightAlpha = ReadDouble();

    lightingData.defaultSpecularLightPen = ReadInt();
    lightingData.defaultSpecularLightIntensity = ReadDouble();
    lightingData.defaultSpecularLightAlpha = ReadDouble();

    lightingData.defaultLightIsFixed = ReadBool();
    lightingData.fixedElevation = ReadDouble();
    lightingData.fixedAzimuth = ReadDouble();
    lightingData.relativeElevation = ReadDouble();
    lightingData.relativeAzimuth = ReadDouble();
    lightingData.defaultLightW = ReadDouble();

    CheckForInputFailure("Reading PPD_3DLighting");
    return true;
}

void PPD_3DLighting::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 4));

    WriteBool(lightingData.useLighting);

    WriteInt(lightingData.ambientPen);
    WriteDouble(lightingData.ambientIntensity);
    WriteDouble(lightingData.ambientLightAlpha);
    WriteLine();

    WriteBool(lightingData.useDefaultLight);
    WriteBool(lightingData.showDefaultLight);

    WriteInt(lightingData.defaultDiffuseLightPen);
    WriteDouble(lightingData.defaultDiffuseLightIntensity);
    WriteDouble(lightingData.defaultDiffuseLightAlpha);
    WriteLine();

    WriteInt(lightingData.defaultSpecularLightPen);
    WriteDouble(lightingData.defaultSpecularLightIntensity);
    WriteDouble(lightingData.defaultSpecularLightAlpha);
    WriteLine();

    WriteBool(lightingData.defaultLightIsFixed);

    WriteDouble(lightingData.fixedElevation);
    WriteDouble(lightingData.fixedAzimuth);
    WriteDouble(lightingData.relativeElevation);
    WriteDouble(lightingData.relativeAzimuth);
    WriteDouble(lightingData.defaultLightW);
    WriteLine();

}

