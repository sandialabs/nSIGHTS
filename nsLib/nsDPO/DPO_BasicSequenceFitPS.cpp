///////////////////////////////////////////////////////////////////////////////////
//
// DPO_BasicSequenceFitPS.cpp
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

#include "DPO_BasicSequenceFitPS.h"

DPO_BasicSequenceFitPS::DPO_BasicSequenceFitPS() :
    xyInputObjRefUI(typeid(DO_XYData)),
    simResultsUI(typeid(DO_XYData))
{
}

void DPO_BasicSequenceFitPS::SetSensitive()
{
    duplicateEpsUI.SetSensitive(removeFieldDuplicatesUI.GetValue() || removeSimDuplicatesUI.GetValue());
}


void DPO_BasicSequenceFitPS::SetSensitiveCB(void* inObj)
{
    static_cast<DPO_BasicSequenceFitPS*>(inObj)->SetSensitive();
}


void DPO_BasicSequenceFitPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    xyInputObjRefUI.InitFramedUI("XY Field Data", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    simResultsUI.InitFramedUI("Simulation Results", topRowForm.GetWidget(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    timeLimitsUI.StdInit(PPform.GetWidget(), args);

    args.StdTopObjectAttach(timeLimitsUI.mainForm.GetWidget());
    interpPointsUI.StdInit(PPform.GetWidget(), args);

    args.StdTopObjectAttach(interpPointsUI.mainForm);
    secondRowForm.InitWidget(PPform.GetWidget(), args);
    if (OptimizationControlStatics::RequiresChisq())
    {
        args.StdSplitSetup();
        fitSigmaUI.SetFieldLen(10);
        fitSigmaUI.InitFramedUI("Measurement Error Std Dev", secondRowForm.GetWidget(), args);
    }
    else
    {
        args.StdSplitSetup(25);

        static const char* fitCalcTypeLabels[]  = {"Sum Sqr(Resid)","Sum Abs(Resid)"};
        fitCalcTypeUI.PreInitSetUp(fitCalcTypeLabels, 2);
        fitCalcTypeUI.InitFramedUI("Fit Calculation", secondRowForm.GetWidget(), args);

        args.StdSplitMiddle(25, 50);
        optionFrame.InitFramedUI("Fit Options", secondRowForm.GetWidget(), args);

        args.StdToggleSetup(5);
        fitIsNormedUI.StdToggleInit("Normalize", optionFrame.GetForm(), args);
    }

    args.StdSplitFinish();
    duplicateFrame.InitFramedUI("Duplicate Removal Options", secondRowForm.GetWidget(), args);

    args.StdToggleSetup(5);
    removeFieldDuplicatesUI.StdToggleInit("field", duplicateFrame.GetForm(), args);
    removeFieldDuplicatesUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdToggleSetup(25);
    removeSimDuplicatesUI.StdToggleInit("sim", duplicateFrame.GetForm(), args);
    removeSimDuplicatesUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdSplitFinish();
    duplicateEpsUI.AttachLabelToObject();
    duplicateEpsUI.SetFieldFormat(SC_SciFormat(2));
    duplicateEpsUI.SetFieldLen(10);
    duplicateEpsUI.InitLabeledUI("Eps:", duplicateFrame.GetForm(), args);

    args.StdTopObjectAttach(secondRowForm.GetWidget());
    coverageFrame.InitFramedUI("Coverage Requirement", PPform.GetWidget(), args);

    args.StdSplitSetup(45);
    minCoverageUI.SetFieldLen(8);
    minCoverageUI.InitLabeledUI("Minimum coverage ", coverageFrame.GetForm(), args);

    args.StdSplitFinish(45);
    coveragePenaltyUI.SetFieldLen(8);
    coveragePenaltyUI.InitLabeledUI("Coverage penalty", coverageFrame.GetForm(), args);

    args.StdTopObjectAttach(coverageFrame);

    statusFrame.InitFramedUI("Fit Status", PPform.GetWidget(), args);

    args.StdFormAttach(5);
    args.LeftOffset(10);
    args.RightOffset(10);
    fitResults.SetFieldLen(12);

    fitResults.InitLabeledUI("Calculated fit value", statusFrame.GetForm(), args);
}

void DPO_BasicSequenceFitPS::MenuOpen(DPO_BasicSequenceFit& inData)
{
    MenuBaseC::MenuOpen(false, false);
    xyInputObjRefUI.StdMenuInit(inData.xyInputObjRef);
    simResultsUI.StdMenuInit(inData.simResultsObjRef);
    timeLimitsUI.StdMenuInit(inData.timeLimits);
}


void DPO_BasicSequenceFitPS::SetMenuData(DPO_BasicSequenceFit&  inData)
{
    xyInputObjRefUI.SetFuncObjRef(inData.xyInputObjRef);
    simResultsUI.SetFuncObjRef(inData.simResultsObjRef);

    timeLimitsUI.SetTimeLimits(inData.timeLimits);
    interpPointsUI.SetInterp(inData.interpPoints);

    if (OptimizationControlStatics::RequiresChisq())
        fitSigmaUI.SetFieldValue(inData.fitSigma);
    else
    {
        fitCalcTypeUI.SetChoice(int(inData.fitCalcType));
        fitIsNormedUI.SetValue(inData.fitIsNormed);
    }

    removeFieldDuplicatesUI.SetValue(inData.removeFieldDuplicates);
    removeSimDuplicatesUI.SetValue(inData.removeSimDuplicates);
    duplicateEpsUI.SetFieldValue(inData.duplicateEps);

    minCoverageUI.SetFieldValue(inData.minCoverage);
    coveragePenaltyUI.SetFieldValue(inData.coveragePenalty);

    if (inData.StatusOK())
        fitResults.SetFieldValue(inData.fitResultsDC.fitValue);
    else
        fitResults.SetStatusNA();

    SetSensitive();
    MenuBaseC::SetMenuData(inData);
}

bool  DPO_BasicSequenceFitPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    xyInputObjRefUI.FuncObjRefOK(allOK, "Field data source");
    simResultsUI.FuncObjRefOK(allOK, "Sim results source");

    timeLimitsUI.UIdataOK(allOK);
    interpPointsUI.UIdataOK(allOK);

    if (OptimizationControlStatics::RequiresChisq())
        fitSigmaUI.FieldValueOK(allOK, "sigma");

    if (removeFieldDuplicatesUI.GetValue() || removeSimDuplicatesUI.GetValue())
    {
        duplicateEpsUI.FieldValueOK(allOK, "Duplicate epsilon", 1.0E-20, 1.0E+05);
    }

    minCoverageUI.FieldValueOK(allOK, "Min coverage", 0.01, 1.0);
    coveragePenaltyUI.FieldValueOK(allOK, "Coverage penalty", 0.0, 100.0);

    return allOK;
}

void DPO_BasicSequenceFitPS::GetMenuData(DPO_BasicSequenceFit&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.xyInputObjRef = xyInputObjRefUI.GetFuncObjRef();
    outData.simResultsObjRef = simResultsUI.GetFuncObjRef();

    timeLimitsUI.GetTimeLimits(outData.timeLimits);
    interpPointsUI.GetInterp(outData.interpPoints);

    outData.removeFieldDuplicates = removeFieldDuplicatesUI.GetValue();
    outData.removeSimDuplicates = removeSimDuplicatesUI.GetValue();
    if (outData.removeFieldDuplicates || outData.removeSimDuplicates)
        outData.duplicateEps = duplicateEpsUI.GetFieldValue();

    if (OptimizationControlStatics::RequiresChisq())
        outData.fitSigma = fitSigmaUI.GetFieldValue();
    else
    {
        outData.fitCalcType    = DFO_BasicSequenceFit::FitCalcType(fitCalcTypeUI.GetChoice());
        outData.fitIsNormed = fitIsNormedUI.GetValue();
    }

    outData.minCoverage = minCoverageUI.GetFieldValue();
    outData.coveragePenalty = coveragePenaltyUI.GetFieldValue();
}

