///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3D.cpp
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
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genApp/AppConfigFile.h>

#include "PPD_3D.h"

PPD_3D :: PPD_3D(PD_3D* bPD, const char* objLongDesc) : PPD_Base(bPD, objLongDesc), base3DPD(*bPD)
{
}

bool  PPD_3D::ReadFromFile()
{
    PPD_Base::ReadFromFile();
    //  didnt start 2D/3D split until ver 4
    if (basePDReadMaj < 4)
        return true;

    int base3DReadMaj = ReadInt();
    int base3DReadMin = ReadInt();

    //  perpective and view
    base3DPD.plotProjection = PD_3D::ThreeDProjection(ReadInt());
//    base3DPD.fieldOfView    = ReadDouble();
    if (base3DReadMaj < 5)
        base3DPD.fieldOfViewMS.enteredRealValue = ReadDouble();
    else
        ReadRealMSSpec(base3DPD.fieldOfViewMS);


    if (base3DReadMaj > 3)
    {
        base3DPD.useStereo3DIfAvailable = ReadBool();   // added v4
//      base3DPD.eyeSeparationDivisor = ReadDouble();
        if (base3DReadMaj < 5)
            base3DPD.eyeSeparationDivisorMS.enteredRealValue = ReadDouble();
        else
            ReadRealMSSpec(base3DPD.eyeSeparationDivisorMS);

        if (base3DReadMaj > 5)
            base3DPD.optimizeRedraw = ReadBool();

    }

    if ( base3DReadMaj > 0)
    {
        PC_View initialView;
        initialView.translation.cX = ReadDouble();
        initialView.translation.cY = ReadDouble();
        initialView.translation.cZ = ReadDouble();

        initialView.azimuth   = ReadDouble();
        initialView.elevation = ReadDouble();
        initialView.scale     = ReadDouble();
        base3DPD.SetInitialView(initialView);
    }

    if (base3DReadMaj > 2)
    {
        PC_TransparencySpec& currSpec = base3DPD.transSpec;
        currSpec.useTransparency = ReadBool();
        currSpec.ntransSteps = ReadInt();
        for (int i = 0; i < PC_TransparencySpec::maxTransparencyGroup; i++)
        {
            PC_TransparencySpec::TransparencyGroup& currGroup = currSpec.transparencyGroups[i];
            currGroup.groupIsTransparent = ReadBool();
            currGroup.transparencyIsConstant = ReadBool();
            currGroup.constantTransFactor = ReadDouble();
            currGroup.startTransFactor = ReadDouble();
            currGroup.endTransFactor = ReadDouble();
        }
    }

    if ( base3DReadMaj > 1)
    {
        PC_VRMLOutData& vrmlData = base3DPD.vrmlOutData;

        ReadFileName(vrmlData.outputFile);

        vrmlData.viewpointDistance = ReadDouble();
        vrmlData.viewpointFOV = ReadDouble();
        vrmlData.polyEdgeFactor = ReadDouble();

    }

    return true;
}


void PPD_3D::WriteToFile()
{
    PPD_Base::WriteToFile();

    // ver 0
//    const int objMajVer = 1;          // add current view
//    const int objMajVer = 2;          // add VRML output file
//    const int objMajVer = 3;          // add transparency spec
//    const int objMajVer = 4;          // add stereo support
//    const int objMajVer = 5;          // linked perspective settings to MS
    const int objMajVer = 6;            // save opt redraw button setting
    const int objMinVer = 0;

    WriteInt(objMajVer);
    WriteInt(objMinVer);
    WriteLine();

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));

    WriteInt(int(base3DPD.plotProjection));
//    WriteDouble(base3DPD.fieldOfView);  removed v5
    WriteRealMSSpec(base3DPD.fieldOfViewMS); // added v5


    WriteBool(base3DPD.useStereo3DIfAvailable); // added v4
//  WriteDouble(base3DPD.eyeSeparationDivisor); // removed v5
    WriteRealMSSpec(base3DPD.eyeSeparationDivisorMS); // added v5

    WriteBool(base3DPD.optimizeRedraw); // added v6

    WriteLine();

    PC_View initialView = base3DPD.GetCurrentView();
    //  store 3D view param
    WriteDouble(initialView.translation.cX);
    WriteDouble(initialView.translation.cY);
    WriteDouble(initialView.translation.cZ);
    WriteLine();

    WriteDouble(initialView.azimuth);
    WriteDouble(initialView.elevation);
    WriteDouble(initialView.scale);
    WriteLine();

    // transparency -- added v3
    PC_TransparencySpec& currSpec = base3DPD.transSpec;
    WriteBool(currSpec.useTransparency);
    WriteInt(currSpec.ntransSteps);
    WriteLine();
    for (int i = 0; i < PC_TransparencySpec::maxTransparencyGroup; i++)
    {
        PC_TransparencySpec::TransparencyGroup& currGroup = currSpec.transparencyGroups[i];
        WriteBool(currGroup.groupIsTransparent);
        WriteBool(currGroup.transparencyIsConstant);
        WriteDouble(currGroup.constantTransFactor);
        WriteDouble(currGroup.startTransFactor);
        WriteDouble(currGroup.endTransFactor);
        WriteLine();
    }


    // VRML output
    PC_VRMLOutData& vrmlData = base3DPD.vrmlOutData;
    WriteText(vrmlData.outputFile);
    WriteDouble(vrmlData.viewpointDistance);
    WriteDouble(vrmlData.viewpointFOV);
    WriteDouble(vrmlData.polyEdgeFactor);
    WriteLine();


}

