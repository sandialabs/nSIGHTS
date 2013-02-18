///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadOptSimResultsPS.cpp
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

#include "DPO_ReadOptSimResultsPS.h"


void DPO_ReadOptSimResultsPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.StdFormAttach();
    optSimResultsFnameUI.InitFramedUI("Optimizer Results Data File ", PPform.GetWidget(), args);
    optSimResultsFnameUI.SetPromptAndMask("Optimizer file to read",
        "Optimizer Output Files (*.nOpt)|*.nOpt|All Files (*.*)|*.*|||");

    args.Clear();
    args.StdTopObjectAttach(optSimResultsFnameUI);
    statusFrame.InitFramedUI("File Status", PPform.GetWidget(), args);

    nRun.SetFieldLen(8);
    nCase.SetFieldLen(8);
    nFits.SetFieldLen(8);
    runTypeStatus.SetFieldLen(8);
    containsResidualsStatus.SetFieldLen(8);
    containsCovarianceStatus.SetFieldLen(8);
    containsJacobianStatus.SetFieldLen(8);

    args.Clear();
    args.LeftOffset(10);
    args.RightOffset(10);
    args.SetPlaceHolder();
    args.StdFormAttach(5);
    nRun.InitLabeledUI("# of runs", statusFrame.GetForm(), args);

    args.ResetPlace();
    args.StdTopObjectAttach(nRun);
    nCase.InitLabeledUI("# of cases", statusFrame.GetForm(), args);

    args.ResetPlace();
    args.StdTopObjectAttach(nCase);
    nFits.InitLabeledUI("# of fits", statusFrame.GetForm(), args);

    args.ResetPlace();
    args.StdTopObjectAttach(nFits);
    runTypeStatus.InitLabeledUI("Type of optimization runs", statusFrame.GetForm(), args);

    args.ResetPlace();
    args.StdTopObjectAttach(runTypeStatus);
    containsResidualsStatus.InitLabeledUI("Contains residuals", statusFrame.GetForm(), args);

    args.ResetPlace();
    args.StdTopObjectAttach(containsResidualsStatus);
    containsCovarianceStatus.InitLabeledUI("Contains covariance matrices", statusFrame.GetForm(), args);

    args.ResetPlace();
    args.StdTopObjectAttach(containsCovarianceStatus);
    containsJacobianStatus.InitLabeledUI("Contains Jacobian data", statusFrame.GetForm(), args);
}

void  DPO_ReadOptSimResultsPS::MenuClose()
{
    optSimResultsFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadOptSimResultsPS::SetMenuData(DPO_ReadOptSimResults&  inData)
{
    optSimResultsFnameUI.SetFieldString(inData.optSimResultsIO.fileName);

    bool isOK = inData.StatusOK();
    if (isOK)
    {
        nRun.SetFieldValue(inData.optSimResults.Size());
        int ncase = 0;
        for (int i = 0; i < inData.optSimResults.Size(); i++)
            ncase += inData.optSimResults[i].runResults.Size();
        nCase.SetFieldValue(ncase);
        nFits.SetFieldValue(inData.optSimResults[0].fitDescriptions.Size());

        static const char* runTypeStrs[] = {"Single", "Perturb", "Multiple", "Sampled"};
        runTypeStatus.SetFieldString(runTypeStrs[inData.optSimResults.runType]);
        containsResidualsStatus.SetBoolValue(inData.optSimResults.containsResiduals);
        containsCovarianceStatus.SetBoolValue(inData.optSimResults.containsCovariance);
        containsJacobianStatus.SetBoolValue(inData.optSimResults.containsJacobian);
    }
    else
    {
        nRun.SetStatusNA();
        nCase.SetStatusNA();
        nFits.SetStatusNA();
        runTypeStatus.SetStatusNA();
        containsResidualsStatus.SetStatusNA();
        containsCovarianceStatus.SetStatusNA();
        containsJacobianStatus.SetStatusNA();
    }

    statusFrame.SetSensitive(isOK);
    MenuBaseC::SetMenuData(inData);
}

bool DPO_ReadOptSimResultsPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    optSimResultsFnameUI.FieldFileOK(allOK, "Simulation results file", true, false);
    return allOK;
}

void DPO_ReadOptSimResultsPS::GetMenuData(DPO_ReadOptSimResults&  outData)
{
    MenuBaseC::GetMenuData(outData);
    optSimResultsFnameUI.GetFieldFile(outData.optSimResultsIO.fileName);
}

void DPO_ReadOptSimResultsPS::MenuClear()
{
    optSimResultsFnameUI.SetFieldString(NULL);
}

