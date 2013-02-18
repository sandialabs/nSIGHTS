///////////////////////////////////////////////////////////////////////////////////
//
// DPO_BEETCompensationPS.cpp
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

#include "nsLibAfx.h"

#include <nsClass/DO_TimeProcess.h>

#include <genClassPS/PS_UIBase.h>
#include <genApp/ExposedReal.h>

#include "DPO_BEETCompensation.h"
#include "DPO_BEETCompensationPS.h"

DPO_BEETCompensationPS::DPO_BEETCompensationPS() :
	responseFunctionObjRefUI(typeid(DO_XYResponseFunction)),
	pressureInputObjRefUI(typeid(DO_XYData)),
    barometricInputObjRefUI(typeid(DO_XYData)),
	earthTideInputObjRefUI(typeid(DO_XYData))
{
}

void DPO_BEETCompensationPS::SetFlags()
{
	useBE = false;
	useET = false;
    if (responseFunctionObjRefUI.CheckDataObj())
    {
        DO_XYResponseFunction* tempDO = static_cast<DO_XYResponseFunction*>(responseFunctionObjRefUI.GetDataObj());
		if (tempDO->DataOK())
		{
			useBE = tempDO->GetResponseFunction()->useBE;
			useET = tempDO->GetResponseFunction()->useET;
		}
	}
}
void DPO_BEETCompensationPS::SetAllSensitive()
{
	SetFlags();
	barometricInputObjRefUI.SetSensitive(useBE);
	earthTideInputObjRefUI.SetSensitive(useET);
}

void DPO_BEETCompensationPS::SetAllSensitiveCB(void* inObj)
{
	static_cast<DPO_BEETCompensationPS*>(inObj)->SetAllSensitive();
}


void DPO_BEETCompensationPS::InitMenu()
{
	MenuBaseC::InitMenu();
 
    ArgListC args;
    args.AllFormAttach();
	pressureInputObjRefUI.InitFramedUI("P(t)Data to Compensate", URform.GetWidget(), args);
    
	args.StdFormAttach();
	responseFunctionObjRefUI.InitFramedUI("Response Function", PPform.GetWidget(), args);
	responseFunctionObjRefUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(responseFunctionObjRefUI);
	barometricInputObjRefUI.InitFramedUI("Barometric P(t) Data", PPform.GetWidget(), args);

	args.StdTopObjectAttach(barometricInputObjRefUI);	
	earthTideInputObjRefUI.InitFramedUI("Earth Tide Data", PPform.GetWidget(), args);

	//status
	args.StdTopObjectAttach(earthTideInputObjRefUI);
    statusFrame.InitFramedUI("Status", PPform.GetWidget(), args);
    args.StdFormAttach(5);
    args.LeftOffset(10);
    args.RightOffset(10);
	timeSpacingStatus.InitLabeledUI("Time Spacing", statusFrame.GetForm(), args);

    args.StdTopObjectAttach(timeSpacingStatus);
    args.LeftOffset(10);
    args.RightOffset(10);
	memoryStatus.InitLabeledUI("Memory (# Time Delay Increments)", statusFrame.GetForm(), args);
}

void DPO_BEETCompensationPS::MenuOpen(DPO_BEETCompensation& inData)
{
	MenuBaseC::MenuOpen(false, false);
	responseFunctionObjRefUI.StdMenuInit(inData.responseFuncInputObjRef);
	pressureInputObjRefUI.StdMenuInit(inData.pressureInputObjRef);
    barometricInputObjRefUI.StdMenuInit(inData.barometricInputObjRef);
	earthTideInputObjRefUI.StdMenuInit(inData.earthTideInputObjRef);
}

void DPO_BEETCompensationPS::SetMenuData(DPO_BEETCompensation&  inData)
{
	MenuBaseC::SetMenuData(inData);

	responseFunctionObjRefUI.SetFuncObjRef(inData.responseFuncInputObjRef);
    pressureInputObjRefUI.SetFuncObjRef(inData.pressureInputObjRef);
    barometricInputObjRefUI.SetFuncObjRef(inData.barometricInputObjRef);
	earthTideInputObjRefUI.SetFuncObjRef(inData.earthTideInputObjRef);

	if (inData.StatusOK())
    {
		timeSpacingStatus.SetFieldValue(inData.timeSpacing);
		memoryStatus.SetFieldValue(inData.m);
    }
    else
    {
        timeSpacingStatus.SetStatusNA();
        memoryStatus.SetStatusNA();
	}
 
	statusFrame.SetSensitive(inData.StatusOK());
	
	SetAllSensitive();
}

bool DPO_BEETCompensationPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
	SetFlags();
	responseFunctionObjRefUI.FuncObjRefOK(allOK, "Response Function input");
	if (allOK && useBE)
		barometricInputObjRefUI.FuncObjRefOK(allOK, "Barometric input");
	if (allOK && useET)
		earthTideInputObjRefUI.FuncObjRefOK(allOK, "Earth Tide input");
	pressureInputObjRefUI.FuncObjRefOK(allOK, "P(t) input");
	
	return allOK;
}

void DPO_BEETCompensationPS::GetMenuData(DPO_BEETCompensation&  outData)
{
    MenuBaseC::GetMenuData(outData);
	SetFlags();

	outData.pressureInputObjRef = pressureInputObjRefUI.GetFuncObjRef();
	outData.responseFuncInputObjRef = responseFunctionObjRefUI.GetFuncObjRef();
	if (useBE)
		outData.barometricInputObjRef = barometricInputObjRefUI.GetFuncObjRef();
	if (useET)
		outData.earthTideInputObjRef = earthTideInputObjRefUI.GetFuncObjRef();
	
}

