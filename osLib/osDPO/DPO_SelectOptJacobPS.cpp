///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectOptJacobPS.cpp
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
//      I/O and user interface for all osDFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClassPS/PS_UIBase.h>

#include "DPO_SelectOptJacobPS.h"

DPO_SelectOptJacobPS::DPO_SelectOptJacobPS()
{
    showOKonly = true;
}


void DPO_SelectOptJacobPS::SetAllSensitive()
{
    DPO_SelectOptPS::SetAllSensitive();
}

void DPO_SelectOptJacobPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
/*
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftPosAttach(5);
    includeSubFitsUI.StdToggleInit("Fit components", optionsFrame.GetForm(), args);
    includeSubFitsUI.AddOtherObjectCallback(ToggleSubFitsCB, this);

    args.ResetPlace();
    args.LeftPosAttach(40);
    args.RightPosAttach(80);
    selectActualCovarUI.PreInitSetUp("Estimated", "Actual", true);
    selectActualCovarUI.InitWidget(optionsFrame.GetForm(), args);

    args.Clear();
    args.TopUICAttach(optionsFrame);
 */
    DPO_SelectOptPS::InitMenu(args, true, false);
}

void DPO_SelectOptJacobPS::MenuOpen(DPO_SelectOptJacob& inData)
{
    DPO_SelectOptPS::MenuOpen(inData);
}


void DPO_SelectOptJacobPS::SetMenuData(DPO_SelectOptJacob&  inData)
{
    DPO_SelectOptPS::SetMenuData(inData);

}

bool DPO_SelectOptJacobPS::UIdataOK()
{
    DPO_SelectOptPS::UIdataOK();
    return allOK;
}

void DPO_SelectOptJacobPS::GetMenuData(DPO_SelectOptJacob&  outData)
{
    DPO_SelectOptPS::GetMenuData(outData);
}

