///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SingleFitPS.cpp
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

#include "DPO_SingleFitPS.h"

DPO_SingleFitPS::DPO_SingleFitPS() :
    fieldDataUI(typeid(DO_XYData)),
    simResultsUI(typeid(DO_XYData))
{
}

void DPO_SingleFitPS::SetSensitive()
{
    bool isLimited = limitInputUI.GetValue();
    limitMinUI.SetSensitive(isLimited);
    limitMaxUI.SetSensitive(isLimited);
    duplicateEpsUI.SetSensitive(removeFieldDuplicatesUI.GetValue() || removeSimDuplicatesUI.GetValue());
}


void DPO_SingleFitPS::SetSensitiveCB(void* inObj)
{
    static_cast<DPO_SingleFitPS*>(inObj)->SetSensitive();
}


void DPO_SingleFitPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    fieldDataUI.InitFramedUI("Field Data", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    simResultsUI.InitFramedUI("Simulation Results", topRowForm.GetWidget(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
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
    limitFrame.InitFramedUI("Field X Limits", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    limitInputUI.StdToggleInit("limited", limitFrame.GetForm(), args);
    limitInputUI.AddOtherObjectCallback(SetSensitiveCB, this);

    args.StdSplitMiddle(30, 65);
    limitMinUI.InitLabeledUI("X min", limitFrame.GetForm(), args);

    args.StdSplitFinish(35);
    limitMaxUI.InitLabeledUI("X max", limitFrame.GetForm(), args);

    args.StdTopObjectAttach(limitFrame);
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
    fitResults.SetFieldLen(15);
    fitResults.SetFieldFormat(SC_SciFormat(6));
    fitResults.InitLabeledUI("Calculated fit value", statusFrame.GetForm(), args);

    args.StdTopObjectAttach(statusFrame);
    infoForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    fieldFrame.InitFramedUI("Field Data Info", infoForm.GetWidget(), args);

    args.StdSplitFinish();
    simFrame.InitFramedUI("Simulation Data Info", infoForm.GetWidget(), args);

    // field data
    args.StdFormAttach();
    nfieldPtsUI.InitLabeledUI("Total points", fieldFrame.GetForm(), args);

    args.StdTopObjectAttach(nfieldPtsUI);
    nfieldPreUI.InitLabeledUI("Points before sim", fieldFrame.GetForm(), args);

    args.StdTopObjectAttach(nfieldPreUI);
    nfieldPostUI.InitLabeledUI("Points after sim", fieldFrame.GetForm(), args);

    args.StdTopObjectAttach(nfieldPostUI);
    fieldMinXUI.InitLabeledUI("Minimum X", fieldFrame.GetForm(), args);

    args.StdTopObjectAttach(fieldMinXUI);
    fieldMaxXUI.InitLabeledUI("Maximum X", fieldFrame.GetForm(), args);

    // sim data
    args.StdFormAttach();
    nsimPtsUI.InitLabeledUI("Total points", simFrame.GetForm(), args);

    args.StdTopObjectAttach(nsimPtsUI);
    nsimPreUI.InitLabeledUI("Points before field", simFrame.GetForm(), args);

    args.StdTopObjectAttach(nsimPreUI);
    nsimPostUI.InitLabeledUI("Points after field", simFrame.GetForm(), args);

    args.StdTopObjectAttach(nsimPostUI);
    simMinXUI.InitLabeledUI("Minimum X", simFrame.GetForm(), args);

    args.StdTopObjectAttach(simMinXUI);
    simMaxXUI.InitLabeledUI("Maximum X", simFrame.GetForm(), args);

}

void DPO_SingleFitPS::MenuOpen(DPO_SingleFit& inData)
{
    MenuBaseC::MenuOpen(false, false);
    fieldDataUI.StdMenuInit(inData.fieldDataObjRef);
    simResultsUI.StdMenuInit(inData.simResultsObjRef);
}


void DPO_SingleFitPS::SetMenuData(DPO_SingleFit&  inData)
{
    MenuBaseC::SetMenuData(inData);

    fieldDataUI.SetFuncObjRef(inData.fieldDataObjRef);
    simResultsUI.SetFuncObjRef(inData.simResultsObjRef);

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

    limitInputUI.SetValue(inData.limitInput);
    limitMinUI.SetFieldValue(inData.limitMin);
    limitMaxUI.SetFieldValue(inData.limitMax);

    minCoverageUI.SetFieldValue(inData.minCoverage);
    coveragePenaltyUI.SetFieldValue(inData.coveragePenalty);

    if (inData.StatusOK())
    {
        fitResults.SetFieldValue(inData.fitResultsDC.fitValue);
        inData.UpdateOverlapStatus();
    }
    else
        fitResults.SetStatusNA();

    fieldFrame.SetSensitive(inData.fieldDataOK);
    if (inData.fieldDataOK)
    {
        nfieldPtsUI.SetFieldValue(inData.nfieldPts);
        nfieldPreUI.SetFieldValue(inData.nfieldPre);
        nfieldPostUI.SetFieldValue(inData.nfieldPost);
        fieldMinXUI.SetFieldValue(inData.fieldMinX);
        fieldMaxXUI.SetFieldValue(inData.fieldMaxX);
    }

    simFrame.SetSensitive(inData.simDataOK);
    if (inData.simDataOK)
    {
        nsimPtsUI.SetFieldValue(inData.nsimPts);
        nsimPreUI.SetFieldValue(inData.nsimPre);
        nsimPostUI.SetFieldValue(inData.nsimPost);
        simMinXUI.SetFieldValue(inData.simMinX);
        simMaxXUI.SetFieldValue(inData.simMaxX);
    }

    SetSensitive();
}

bool  DPO_SingleFitPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    fieldDataUI.FuncObjRefOK(allOK, "Field data source");
    simResultsUI.FuncObjRefOK(allOK, "Sim results source");
    if (OptimizationControlStatics::RequiresChisq())
        fitSigmaUI.FieldValueOK(allOK, "sigma");

    if (removeFieldDuplicatesUI.GetValue() || removeSimDuplicatesUI.GetValue())
    {
        duplicateEpsUI.FieldValueOK(allOK, "Duplicate epsilon", 1.0E-20, 1.0E+05);
    }

    if (limitInputUI.GetValue())
    {
        limitMinUI.FieldValueOK(allOK, "X minimum");
        limitMaxUI.FieldValueOK(allOK, "X maximum");
    }

    minCoverageUI.FieldValueOK(allOK, "Min coverage", 0.01, 1.0);
    coveragePenaltyUI.FieldValueOK(allOK, "Coverage penalty", 0.0, 100.0);

    return allOK;
}

void DPO_SingleFitPS::GetMenuData(DPO_SingleFit&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.fieldDataObjRef = fieldDataUI.GetFuncObjRef();
    outData.simResultsObjRef = simResultsUI.GetFuncObjRef();

    if (OptimizationControlStatics::RequiresChisq())
        outData.fitSigma = fitSigmaUI.GetFieldValue();
    else
    {
        outData.fitCalcType    = DFO_SingleFit::FitCalcType(fitCalcTypeUI.GetChoice());
        outData.fitIsNormed = fitIsNormedUI.GetValue();
    }

    outData.removeFieldDuplicates = removeFieldDuplicatesUI.GetValue();
    outData.removeSimDuplicates = removeSimDuplicatesUI.GetValue();
    if (outData.removeFieldDuplicates || outData.removeSimDuplicates)
        outData.duplicateEps = duplicateEpsUI.GetFieldValue();

    outData.limitInput = limitInputUI.GetValue();
    if (outData.limitInput)
    {
        outData.limitMin = limitMinUI.GetFieldValue();
        outData.limitMax = limitMaxUI.GetFieldValue();
    }

    outData.minCoverage = minCoverageUI.GetFieldValue();
    outData.coveragePenalty = coveragePenaltyUI.GetFieldValue();

}

