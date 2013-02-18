///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadRangeSimResultsPS.cpp
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

#include "DPO_ReadRangeSimResultsPS.h"


void DPO_ReadRangeSimResultsPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.StdFormAttach();
    rangeSimResultsFnameUI.InitFramedUI("Range Results Data File ", PPform.GetWidget(), args);
    rangeSimResultsFnameUI.SetPromptAndMask("Range file to read",
        "Range Output Files (*.nRng)|*.nRng|All Files (*.*)|*.*|||");

    args.Clear();
    args.StdTopObjectAttach(rangeSimResultsFnameUI);
    statusFrame.InitFramedUI("File Status", PPform.GetWidget(), args);

    nRun.SetFieldLen(8);
    nOutput.SetFieldLen(8);
    dataTypeStatus.SetFieldLen(8);

    args.Clear();
    args.LeftOffset(10);
    args.RightOffset(10);
    args.SetPlaceHolder();
    args.StdFormAttach(5);
    nRun.InitLabeledUI("# of runs", statusFrame.GetForm(), args);

    args.ResetPlace();
    args.StdTopObjectAttach(nRun);
    nOutput.InitLabeledUI("# of grid/cubes", statusFrame.GetForm(), args);

    args.ResetPlace();
    args.StdTopObjectAttach(nOutput);
    dataTypeStatus.InitLabeledUI("Type of data", statusFrame.GetForm(), args);
}

void  DPO_ReadRangeSimResultsPS::MenuClose()
{
    rangeSimResultsFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadRangeSimResultsPS::SetMenuData(DPO_ReadRangeSimResults&  inData)
{
    rangeSimResultsFnameUI.SetFieldString(inData.rangeSimResultsIO.fileName);

    bool isOK = inData.StatusOK();
    if (isOK)
    {
        nRun.SetFieldValue(inData.rangeSimResults.Size());
        nOutput.SetFieldValue(inData.rangeSimResults.GetNOutput());
        dataTypeStatus.SetBoolValue(inData.rangeSimResults.IsCubes(), "Cubes", "Grids");

    }
    else
    {
        nRun.SetStatusNA();
        nOutput.SetStatusNA();
        dataTypeStatus.SetStatusNA();
    }

    statusFrame.SetSensitive(isOK);
    MenuBaseC::SetMenuData(inData);
}

bool DPO_ReadRangeSimResultsPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    rangeSimResultsFnameUI.FieldFileOK(allOK, "Simulation results file", true, false);
    return allOK;
}

void DPO_ReadRangeSimResultsPS::GetMenuData(DPO_ReadRangeSimResults&  outData)
{
    MenuBaseC::GetMenuData(outData);
    rangeSimResultsFnameUI.GetFieldFile(outData.rangeSimResultsIO.fileName);
}

void DPO_ReadRangeSimResultsPS::MenuClear()
{
    rangeSimResultsFnameUI.SetFieldString(NULL);
}

