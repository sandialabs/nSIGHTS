///////////////////////////////////////////////////////////////////////////////////
//
// LFO_VersionListing.cpp
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
//      Platform independent base classes to allow creation of listings in HTML browser windows.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <time.h>

#include <genClass/U_Str.h>
#include <genApp/AppConfigFile.h>
#include <genApp/G_Version.h>

#include <genListClass/LFO_VersionListing.h>

namespace appVersionListing {


    LFO_VersionListing versionListObj;


    LFO_VersionListing::LFO_VersionListing()
        :ListObjC("VersionListing")
    {
    }

    void  LFO_VersionListing::CreateListing()
    {
        using namespace appVersionGlobals;

        TextBigTitle(versionID);

        AddText("Version date", 0);
        AddText(versionDate, 15);
        AddNextLine();

        AddText("Listing date", 0);
        time_t currTime;
        if (time(&currTime) == -1)
        {
            AddText("n/a", 15);
        }
        else
        {
            tm localTime = *localtime(&currTime);
            char timeLab[80];
            strftime(timeLab, 80, "%d %b %Y", &localTime);
            AddText(timeLab, 15);
        }
        AddNextLine();


        AddText("QA status", 0);
        AddText(versionQA, 15);
        AddNextLine();

        AddText("Config file", 0);
        if (IsNull(currConfigFileName))
            AddText("no file", 15);
        else
            AddText(currConfigFileName, 15);
        AddNextLine();
    };

}

