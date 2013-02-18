///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadSATablePS.cpp
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

#include "SaLibAfx.h"

#include <genClassPS/PS_UIBase.h>

#include "DPO_ReadSATablePS.h"



void DPO_ReadSATablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ReadSATablePS*>(inObj)->SetAllSensitive();
}

void DPO_ReadSATablePS::SetAllSensitive()
{
    goldsimIndepVarFrame.SetSensitive(saFileFormatUI.GetChoice() == 2);
    bool readVarNames = (saFileFormatUI.GetChoice() == 2) && translateIndepVarNamesUI.GetValue();
    varNameFnameUI.SetSensitive(readVarNames);
    selectOnlyNewNamesUI.SetSensitive(readVarNames);
    sortByNamesUI.SetSensitive(readVarNames);

    goldsimDepVarUI.SetSensitive(saFileFormatUI.GetChoice() == 1);
}

void DPO_ReadSATablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.StdFormAttach();
    saFnameUI.InitFramedUI("SA Table File ", PPform.GetWidget(), args);
    saFnameUI.SetPromptAndMask("SA table file to read",
        "SA table file (*.txt)|*.txt|All Files (*.*)|*.*|||");

    args.StdTopObjectAttach(saFnameUI);
    static const char* formatLabels[] = {"Sandia WIPP", "GoldSim Dependent", "Goldsim Independent", "Other"};
    saFileFormatUI.PreInitSetUp(formatLabels, 4);
    saFileFormatUI.InitFramedUI("SA Table File Format", PPform.GetWidget(), args);
    saFileFormatUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(saFileFormatUI);
    goldsimIndepVarFrame.InitFramedUI("Goldsim Independent Variable Options", PPform.GetWidget(), args);

    args.StdFormAttach();
    toggleForm.InitWidget(goldsimIndepVarFrame.GetForm(), args);

    args.StdToggleSetup(5);
    translateIndepVarNamesUI.StdToggleInit("translate var names", toggleForm.GetWidget(), args);
    translateIndepVarNamesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(35);
    selectOnlyNewNamesUI.StdToggleInit("select named vars only", toggleForm.GetWidget(), args);

    args.StdToggleSetup(70);
    sortByNamesUI.StdToggleInit("sort by names", toggleForm.GetWidget(), args);

    args.StdTopObjectAttach(toggleForm.GetWidget());
    varNameFnameUI.AttachObjectToLabel();
    varNameFnameUI.InitLabeledUI("Translation file:", goldsimIndepVarFrame.GetForm(), args);

    args.StdTopObjectAttach(goldsimIndepVarFrame);
    goldsimDepVarUI.InitFramedUI("Goldsim Dependent Variable Name", PPform.GetWidget(), args);

    args.StdTopObjectAttach(goldsimDepVarUI);
    statusFrame.InitFramedUI("SA Table Data Status", PPform.GetWidget(), args);

    args.StdSplitSetup(30);
    nsteps.InitLabeledUI("# of steps", statusFrame.GetForm(), args);

    args.StdSplitMiddle(35, 65);
    nvariables.InitLabeledUI("# of variables", statusFrame.GetForm(), args);

    args.StdSplitFinish(30);
    nvectors.InitLabeledUI("# of vectors", statusFrame.GetForm(), args);
}

void  DPO_ReadSATablePS::MenuClose()
{
    saFnameUI.CloseDialog();
    varNameFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadSATablePS::SetMenuData(DPO_ReadSATable&    inData)
{
    MenuBaseC::SetMenuData(inData);

    saFnameUI.SetFieldString(inData.saFname);
    saFileFormatUI.SetChoice(int(inData.saFileFormat));

    translateIndepVarNamesUI.SetValue(inData.translateIndepVarNames);
    selectOnlyNewNamesUI.SetValue(inData.selectOnlyNewNames);
    sortByNamesUI.SetValue(inData.sortByNames);

    varNameFnameUI.SetFieldString(inData.varNameFname);
    goldsimDepVarUI.SetFieldString(inData.goldsimDepVar);

    if (inData.StatusOK())
    {
        nsteps.SetFieldValue(inData.saDataDC.GetNStep());
        nvariables.SetFieldValue(inData.saDataDC.GetNVariable());
        nvectors.SetFieldValue(inData.saDataDC.GetNVector());
    }
    else
    {
        nsteps.SetStatusNA();
        nvariables.SetStatusNA();
        nvectors.SetStatusNA();
    }
    statusFrame.SetSensitive(inData.StatusOK());

    SetAllSensitive();
}

bool DPO_ReadSATablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    saFnameUI.FieldFileOK(allOK, "transfer file", true, false);
    if ((saFileFormatUI.GetChoice() == 2) && translateIndepVarNamesUI.GetValue())
        varNameFnameUI.FieldFileOK(allOK, "indep var name translation file", true, false);
    return allOK;
}

void DPO_ReadSATablePS::GetMenuData(DPO_ReadSATable&  outData)
{
    MenuBaseC::GetMenuData(outData);
    saFnameUI.GetFieldFile(outData.saFname);
    outData.saFileFormat = IO_SATable::SAFileFormat(saFileFormatUI.GetChoice());
    if (outData.saFileFormat == IO_SATable::saffGoldsimIndepVar)
    {
        outData.translateIndepVarNames = translateIndepVarNamesUI.GetValue();
        if (outData.translateIndepVarNames)
        {
            varNameFnameUI.GetFieldFile(outData.varNameFname);
            outData.selectOnlyNewNames = selectOnlyNewNamesUI.GetValue();
            outData.sortByNames = sortByNamesUI.GetValue();
        }
    }
    else if (outData.saFileFormat == IO_SATable::saffGoldsimDepVar)
    {
        goldsimDepVarUI.GetFieldString(outData.goldsimDepVar, DC_SATable::varNameLen);
    }
}

void DPO_ReadSATablePS::MenuClear()
{
    saFnameUI.SetFieldString(NULL);
}

void DPO_ReadSATablePS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    MenuClear();
    saFileFormatUI.SetChoice(0);
}

