///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExtendedProfileScaleTransformPS.cpp
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

#include "NsLibAfx.h"

#include <nsClass/DO_ExtendedProfile.h>

#include "DPO_ExtendedProfileScaleTransform.h"
#include "DPO_ExtendedProfileScaleTransformPS.h"


DPO_ExtendedProfileScaleTransformPS::DPO_ExtendedProfileScaleTransformPS() :
    profileInputObjRefUI(typeid(DO_ExtendedProfile))
{
}

void DPO_ExtendedProfileScaleTransformPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ExtendedProfileScaleTransformPS*>(inObj)->SetAllSensitive();
}

void DPO_ExtendedProfileScaleTransformPS::SetAllSensitive()
{
    thresholdUI.SetInternalSensitive();
}


void DPO_ExtendedProfileScaleTransformPS::InitMenu()
{
    DPO_ScaleTransformPS::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    profileInputObjRefUI.InitFramedUI("Scalar To Scale/Transform", URform.GetWidget(), args);

    args.StdFormAttach();
    InitSTCommon(args);

    args.StdTopObjectAttach(nullFrame);
    thresholdUI.StdInit(PPform.GetWidget(), args, true);
    thresholdUI.minThresholdUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
    thresholdUI.maxThresholdUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
}

void DPO_ExtendedProfileScaleTransformPS::SetMenuData(DPO_ExtendedProfileScaleTransform&  inData)
{
    DPO_ScaleTransformPS::SetMenuData(inData);

    profileInputObjRefUI.SetFuncObjRef(inData.profileInputObjRef);
    thresholdUI.SetThreshold(inData.thresholdOp);

    SetAllSensitive();
}

void DPO_ExtendedProfileScaleTransformPS::MenuOpen(DPO_ExtendedProfileScaleTransform& inData)
{
    DPO_ScaleTransformPS::MenuOpen(inData);
    profileInputObjRefUI.StdMenuInit(inData.profileInputObjRef);
}


bool DPO_ExtendedProfileScaleTransformPS::UIdataOK()
{
    DPO_ScaleTransformPS::UIdataOK();
    profileInputObjRefUI.FuncObjRefOK(allOK, "Input scalar");
    thresholdUI.UIdataOK(allOK);
    return allOK;
}

void DPO_ExtendedProfileScaleTransformPS::GetMenuData(DPO_ExtendedProfileScaleTransform&  outData)
{
    DPO_ScaleTransformPS::GetMenuData(outData);
    outData.profileInputObjRef = profileInputObjRefUI.GetFuncObjRef();
    thresholdUI.GetThreshold(outData.thresholdOp);
}


