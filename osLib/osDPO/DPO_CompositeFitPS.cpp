///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CompositeFitPS.cpp
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

#include <osClass/DO_FitResults.h>

#include "DPO_CompositeFitPS.h"


DPO_CompositeFitPS::FitDataUIC::FitDataUIC() : inputFitObjRefUI(typeid(DO_FitResults))
{
}

void DPO_CompositeFitPS::FitDataUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<DPO_CompositeFitPS::FitDataUIC*>(currObj)->SetAllSensitive();
}


void DPO_CompositeFitPS::FitDataUIC::InitFitData()
{
    ArgListC args;

    args.TopFormAttach();
    args.LeftFormAttach();
    useFitUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    useFitUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopFormAttach();
    if (OptimizationControlStatics::RequiresChisq())
    {
        args.RightPosAttach(60);
        inputFitObjRefUI.InitFormUI(outsideForm.GetWidget(), args);

        args.RightFormAttach();
    }
    else
    {
        static const int specFormwidth = 550;
        args.RightPosAttach(45);
        inputFitObjRefUI.InitFormUI(outsideForm.GetWidget(), args);

        args.Clear();
        args.TopFormAttach();
        args.SpecifiedWidthAttach(50, 70, specFormwidth);
        fitScalingUI.SetFieldLen(10);
        fitScalingUI.InitWidget(outsideForm.GetWidget(), args);

        args.Clear();
        args.TopFormAttach();
        args.LeftPosAttach(75);
        logFitUI.StdToggleInit("log10", outsideForm.GetWidget(), args);

        args.StdSplitSetup();
        args.StdSplitFinish(15);
    }

    fitResults.SetFieldLen(15);
    fitResults.SetFieldFormat(SC_SciFormat(6));
    fitResults.InitWidget(outsideForm.GetWidget(), args);


}

void DPO_CompositeFitPS::FitDataUIC::SetFitData(const DFO_CompositeFit::FitData& fitData)
{
    useFitUI.SetValue(fitData.useFit);
    inputFitObjRefUI.SetFuncObjRef(fitData.inputFitObjRef);
    if (!OptimizationControlStatics::RequiresChisq())
    {
        fitScalingUI.SetFieldValue(fitData.fitScaling);
        logFitUI.SetValue(fitData.logFit);
    }
    SetAllSensitive();
}

void DPO_CompositeFitPS::FitDataUIC::GetFitData(DFO_CompositeFit::FitData& fitData)
{
    fitData.useFit = useFitUI.GetValue();
    fitData.inputFitObjRef = inputFitObjRefUI.GetFuncObjRef();
    if (!OptimizationControlStatics::RequiresChisq())
    {
        fitData.fitScaling = fitScalingUI.GetFieldValue();
        fitData.logFit = logFitUI.GetValue();
    }
}

bool DPO_CompositeFitPS::FitDataUIC::FitDataOK(bool& prevOK)
{
    if (useFitUI.GetValue())
    {
        inputFitObjRefUI.FuncObjRefOK(prevOK, "Fit input");
        if (!OptimizationControlStatics::RequiresChisq())
            fitScalingUI.FieldValueOK(prevOK, "scaling", 1.0E-10, 1.0E+10);
    }
    return prevOK;
}


void DPO_CompositeFitPS::FitDataUIC::SetAllSensitive()
{
    useFitUI.SetSensitive(true);
    bool doFit = useFitUI.GetValue();
    inputFitObjRefUI.SetSensitive(doFit);

    if (!OptimizationControlStatics::RequiresChisq())
    {
        fitScalingUI.SetSensitive(doFit);
        logFitUI.SetSensitive(doFit);
    }
}

CSize DPO_CompositeFitPS::FitScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 600);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

DPO_CompositeFitPS::DPO_CompositeFitPS()
{
}

void DPO_CompositeFitPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomFormAttach();
    statusFrame.InitFramedUI("Fit Status", PPform.GetWidget(), args);

    args.Clear();
    args.LeftOffset(10);
    args.RightOffset(10);
    args.StdFormAttach(5);
    fitResults.SetFieldLen(15);
    fitResults.SetFieldFormat(SC_SciFormat(6));
    fitResults.InitLabeledUI("Calculated fit value", statusFrame.GetForm(), args);

    args.Clear();
    args.StdFormAttach();
    args.BottomUICAttach(statusFrame);

    args.LeftOffset(3);
    args.RightOffset(3);

    //scrollArea.needHorizontal = false;
    scrollArea.InitWidget(PPform.GetWidget(), args);
    args.Clear();
    args.StdFormAttach();

    for (int i = 0; i < DFO_CompositeFit::maxFitData; i++)
    {
        fitDataUI[i].outsideForm.InitWidget(scrollArea, args);
        fitDataUI[i].InitFitData();

        args.Clear();
        args.StdTopObjectAttach(fitDataUI[i].outsideForm.GetWidget());
        fitDataUI[i].hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(fitDataUI[i].hSep.GetWidget());
    }

}

void DPO_CompositeFitPS::MenuOpen(DPO_CompositeFit& inData)
{
    MenuBaseC::MenuOpen(false, false);
    for (int i = 0; i < DFO_CompositeFit::maxFitData; i++)
    {
        fitDataUI[i].inputFitObjRefUI.StdMenuInit(inData.fitData[i].inputFitObjRef);
    }
}


void DPO_CompositeFitPS::SetMenuData(DPO_CompositeFit&  inData)
{
    for (int i = 0; i < DFO_CompositeFit::maxFitData; i++)
    {
        fitDataUI[i].SetFitData(inData.fitData[i]);
        if (inData.StatusOK() && inData.fitData[i].useFit)
        {
            double inputFitVal = static_cast<DO_FitResults*>(inData.GetInPortData(inData.fitData[i].inputFitObjRef))->realVal;
            fitDataUI[i].fitResults.SetFieldValue(inputFitVal);
        }
        else
            fitDataUI[i].fitResults.SetStatusNA();
    }

    if (inData.StatusOK())
        fitResults.SetFieldValue(inData.GetFitReal().realVal);
    else
        fitResults.SetStatusNA();


    MenuBaseC::SetMenuData(inData);
}

bool DPO_CompositeFitPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    for (int i = 0; i < DFO_CompositeFit::maxFitData; i++)
        if (!fitDataUI[i].FitDataOK(allOK))
            return false;
    return allOK;
}


void DPO_CompositeFitPS::GetMenuData(DPO_CompositeFit&  outData)
{
    MenuBaseC::GetMenuData(outData);
    for (int i = 0; i < DFO_CompositeFit::maxFitData; i++)
        fitDataUI[i].GetFitData(outData.fitData[i]);
}

