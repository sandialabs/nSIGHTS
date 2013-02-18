///////////////////////////////////////////////////////////////////////////////////
//
// DefaultObjects.cpp
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include <time.h>

#include <genClass/U_Str.h>
#include <genApp/G_Version.h>
#include <genApp/AppConfigFile.h>

#include "DefaultObjects.h"

namespace appDefaultObjects {

    SysInfoObj              sysInfoObj;
    DefaultPens             defaultPenObj;
    ColdHotMap              coldHotMapObj;
    WhiteBlackMap   whiteBlackMapObj;


    DefaultColorMap :: DefaultColorMap(const char* inID) : FuncObjC(inID)
    {
        colorMapDO.colorMapData = &colorMapDC;
        AddOutPort(colorMapDO);
    }

    void  DefaultColorMap::SetColorMap(bool          useHSV,
                                       SC_ColorSpec& mapStart,
                                       SC_ColorSpec& mapEnd)
    {
        colorMapDC.ncolorMap = 50;
        if (useHSV)
            colorMapDC.MakeLinearHSVMap(mapStart, mapEnd);
        else
            colorMapDC.MakeLinearRGBMap(mapStart, mapEnd);
        DoStatusChk();
    }

    void DefaultColorMap::CalcOutput(FOcalcType  calcType)
    {
        DoStatusChk();
    }

    ColdHotMap::ColdHotMap() : DefaultColorMap("Cold->Hot")
    {
        SetColorMap(true, SC_ColorSpec(0.65,  1.0, 1.0), SC_ColorSpec(0.0,  1.0, 1.0));
    }

    WhiteBlackMap::WhiteBlackMap() : DefaultColorMap("GreyScale")
    {
        SetColorMap(false, SC_ColorSpec(0.95, 0.95, 0.95), SC_ColorSpec(0.1,  0.1, 0.1));
    }

    DefaultPens::DefaultPens() : FuncObjC("Default Pens")
    {
        penSet.SetDefault();
        penSetDO.penSetData = &penSet;
        AddOutPort(penSetDO);
        DoStatusChk();
    }


    void DefaultPens::CalcOutput(FOcalcType  calcType)
    {
        DoStatusChk();
    }



    SysInfoObj :: SysInfoObj() : FuncObjC("SysInfo")
    {
        AddOutPort(versionIDDO);
        AddOutPort(versionDateDO);
        AddOutPort(todaysDateDO);
        AddOutPort(currentTimeDO);
        AddOutPort(currentConfigFileOnlyDO);
        AddOutPort(currentConfigFileAndPathDO);
        DoStatusChk();
    }

    void  SysInfoObj:: DoStatusChk()
    {
        FuncObjC::DoStatusChk();

        versionIDDO.SetTypeLabel("Version");
        versionIDDO.SetValueLabel(appVersionGlobals::versionID);

        versionDateDO.SetTypeLabel("Version date");
        versionDateDO.SetValueLabel(appVersionGlobals::versionDate);

        todaysDateDO.SetTypeLabel("Today");
        currentTimeDO.SetTypeLabel("Time");

        time_t currTime;
        if (time(&currTime) == -1)
            {
                todaysDateDO.SetValueLabel("n/a");
                currentTimeDO.SetValueLabel("n/a");
            }
        else
            {
                tm localTime = *localtime(&currTime);
                char timeLab[80];
                strftime(timeLab, 80, "%d %b %Y", &localTime);
                todaysDateDO.SetValueLabel(timeLab);

                strftime(timeLab, 80, "%H:%M", &localTime);
                currentTimeDO.SetValueLabel(timeLab);
            }

        currentConfigFileOnlyDO.SetTypeLabel("Configuration file");
        currentConfigFileAndPathDO.SetTypeLabel("Configuration file and path");
        if (IsNull(currConfigFileName))
            {
                currentConfigFileOnlyDO.SetValueLabel("not set");
                currentConfigFileAndPathDO.SetValueLabel("not set");
            }
        else
            {
                char tempFile[stdFileStrLen];
                ExtractFileName(tempFile, currConfigFileName, stdFileStrLen);
                currentConfigFileOnlyDO.SetValueLabel(tempFile);

                currentConfigFileAndPathDO.SetValueLabel(currConfigFileName);
            }
    }

    void SysInfoObj:: CalcOutput(FOcalcType  calcType)
    {
        DoStatusChk();
    }


};

