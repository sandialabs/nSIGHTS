///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ResidualHistogramPS.cpp
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

#include "DPO_ResidualHistogramPS.h"

DPO_ResidualHistogramPS::DPO_ResidualHistogramPS() :
    residInputObjRefUI(typeid(DO_ResidData))
{
}

void DPO_ResidualHistogramPS::SetAllSensitive()
{
    bool notStd = !standardizeResidualsUI.GetValue();
    normalizeResidualsUI.SetSensitive(notStd);
    normalizationLimitsUI.SetSensitive(notStd && normalizeResidualsUI.GetValue());
}


void DPO_ResidualHistogramPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ResidualHistogramPS*>(inObj)->SetAllSensitive();
}


void DPO_ResidualHistogramPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    residInputObjRefUI.InitFramedUI("Residual Input Data", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    cumulativeDistributionUI.StdToggleInit("Cumulative", optionsFrame.GetForm(), args);

    args.StdToggleSetup(35);
    standardizeResidualsUI.StdToggleInit("Standardize", optionsFrame.GetForm(), args);
    standardizeResidualsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(70);
    normalizeResidualsUI.StdToggleInit("Normalize", optionsFrame.GetForm(), args);
    normalizeResidualsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(optionsFrame);
    entryRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    nhistoBinsUI.InitFramedUI("Number of Bins", entryRow.GetWidget(), args);

    args.StdSplitFinish();
    normalizationLimitsUI.InitFramedUI("Nomalization Limit", entryRow.GetWidget(), args);
}


void DPO_ResidualHistogramPS::MenuOpen(DPO_ResidualHistogram& inData)
{
    MenuBaseC::MenuOpen(false, false);
    residInputObjRefUI.StdMenuInit(inData.residInputObjRef);
}


void DPO_ResidualHistogramPS::SetMenuData(DPO_ResidualHistogram&  inData)
{
    MenuBaseC::SetMenuData(inData);

    residInputObjRefUI.SetFuncObjRef(inData.residInputObjRef);
    nhistoBinsUI.SetFieldValue(inData.nhistoBins);
    cumulativeDistributionUI.SetValue(inData.cumulativeDistribution);
    standardizeResidualsUI.SetValue(inData.standardizeResiduals);
    normalizeResidualsUI.SetValue(inData.normalizeResiduals);
    normalizationLimitsUI.SetFieldValue(inData.normalizationLimits);

    SetAllSensitive();
}

bool  DPO_ResidualHistogramPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    residInputObjRefUI.FuncObjRefOK(allOK, "Residual data source");
    nhistoBinsUI.FieldValueOK(allOK, "# of histogram bins", 5, 300);

    if ((!standardizeResidualsUI.GetValue()) && (normalizeResidualsUI.GetValue()))
        normalizationLimitsUI.FieldValueOK(allOK, "normalization limits", 0.001, 1.0E+06);

    return allOK;
}

void DPO_ResidualHistogramPS::GetMenuData(DPO_ResidualHistogram&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.residInputObjRef = residInputObjRefUI.GetFuncObjRef();
    outData.nhistoBins = nhistoBinsUI.GetFieldValue();
    outData.cumulativeDistribution = cumulativeDistributionUI.GetValue();
    outData.standardizeResiduals = standardizeResidualsUI.GetValue();
    outData.normalizeResiduals = normalizeResidualsUI.GetValue();
    if ((!outData.standardizeResiduals) && outData.normalizeResiduals)
        outData.normalizationLimits = normalizationLimitsUI.GetFieldValue();



}

