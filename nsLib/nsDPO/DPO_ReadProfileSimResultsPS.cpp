///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadProfileSimResultsPS.cpp
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

#include <genClassPS/PS_UIBase.h>

#include "DPO_ReadProfileSimResultsPS.h"

void DPO_ReadProfileSimResultsPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.StdFormAttach();
    profileSimResultsFnameUI.InitFramedUI("Profile Simulation Results Data File ", PPform.GetWidget(), args);
    profileSimResultsFnameUI.SetPromptAndMask("Results file to read",
        "Simulation Profile Output Files (*.nPro)|*.nPro|All Files (*.*)|*.*|||");

    args.Clear();
    args.StdTopObjectAttach(profileSimResultsFnameUI);
    statusFrame.InitFramedUI("File Status", PPform.GetWidget(), args);

    args.Clear();
    args.LeftOffset(10);
    args.RightOffset(10);
    args.StdFormAttach(5);
    nRun.InitLabeledUI("# of runs", statusFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(nRun);
    args.LeftOffset(10);
    args.RightOffset(10);
    nCase.InitLabeledUI("# of cases", statusFrame.GetForm(), args);
}

void  DPO_ReadProfileSimResultsPS::MenuClose()
{
    profileSimResultsFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadProfileSimResultsPS::SetMenuData(DPO_ReadProfileSimResults&  inData)
{
    profileSimResultsFnameUI.SetFieldString(inData.profileSimResultsIO.fileName);

    bool isOK = inData.StatusOK();
    if (isOK)
    {
        nRun.SetFieldValue(inData.profileSimResults.Size());
        int ncase = 0;
        for (int i = 0; i < inData.profileSimResults.Size(); i++)
            ncase += inData.profileSimResults[i].Size();
        nCase.SetFieldValue(ncase);
    }
    else
    {
        nRun.SetStatusNA();
        nCase.SetStatusNA();
    }
    statusFrame.SetSensitive(isOK);
    MenuBaseC::SetMenuData(inData);
}

bool DPO_ReadProfileSimResultsPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    profileSimResultsFnameUI.FieldFileOK(allOK, "Simulation results file", true, false);
    return allOK;
}

void DPO_ReadProfileSimResultsPS::GetMenuData(DPO_ReadProfileSimResults&  outData)
{
    MenuBaseC::GetMenuData(outData);
    profileSimResultsFnameUI.GetFieldFile(outData.profileSimResultsIO.fileName);
}

void DPO_ReadProfileSimResultsPS::MenuClear()
{
    profileSimResultsFnameUI.SetFieldString(NULL);
}

