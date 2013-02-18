///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ExtendedProfileColorRangeFillPS.cpp
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
//      File I/O and user interface for all nsPFOGL functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#include "NsLibAfx.h"

#include <nsClass/DO_ExtendedProfile.h>

#include "PPO_ExtendedProfileColorRangeFillPS.h"

void PPO_ExtendedProfileColorRangeFillPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_ExtendedProfileColorRangeFillPS*>(inObj)->SetAllSensitive();
}

void PPO_ExtendedProfileColorRangeFillPS::SetAllSensitive()
{
    edgeSpecUI.SetInternalSensitive();
    if (useReport)
        reportFormatUI.SetSensitive(reportToggle.GetValue());
}

// ****************************************

PPO_ExtendedProfileColorRangeFillPS::PPO_ExtendedProfileColorRangeFillPS()
{
}

void PPO_ExtendedProfileColorRangeFillPS::InitMenu(const PPO_ExtendedProfileColorRangeFill& inData)
{
    PPO_ExtendedProfileColorBasePS::InitMenu(inData);

    ArgListC    args;
    args.StdTopObjectAttach(outsideForm.GetWidget());
    edgeSpecUI.StdInit(PPform.GetWidget(), args, inData.is3DObject);
    edgeSpecUI.plotEdgesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(edgeSpecUI);
    if (useReport)
    {
        reportFormatUI.InitFramedUI("Cursor Reporting Format", PPform.GetWidget(), args);
        reportToggle.AddOtherObjectCallback(SetAllSensitiveCB, this);
        args.Clear();
        args.StdTopObjectAttach(reportFormatUI);
    }

    PPO_ExtendedProfileColorBasePS::InitMapAndOffset(inData, args);
}

void PPO_ExtendedProfileColorRangeFillPS::MenuOpen(PPO_ExtendedProfileColorRangeFill& inData)
{
    PPO_ExtendedProfileColorBasePS::MenuOpen(inData);

    edgeSpecUI.edgePenUI.ResetPens(inData.objPenSet);
}

void PPO_ExtendedProfileColorRangeFillPS::SetMenuData(PPO_ExtendedProfileColorRangeFill& inData)
{
    PPO_ExtendedProfileColorBasePS::SetMenuData(inData, inData);
    edgeSpecUI.SetEdgeSpec(inData.edgeSpec);
    if (useReport)
        reportFormatUI.SetRealFormatSettings(inData.reportFormat);
    SetAllSensitive();
}

bool PPO_ExtendedProfileColorRangeFillPS::UIdataOK()
{
    PPO_ExtendedProfileColorBasePS::UIdataOK();
    edgeSpecUI.UIdataOK(allOK);
    return allOK;
}

void PPO_ExtendedProfileColorRangeFillPS::GetMenuData(PPO_ExtendedProfileColorRangeFill& outData)
{
    PPO_ExtendedProfileColorBasePS::GetMenuData(outData, outData);

    edgeSpecUI.GetEdgeSpec(outData.edgeSpec);
    if (useReport)
        reportFormatUI.GetRealFormatSettings(outData.reportFormat);
}

