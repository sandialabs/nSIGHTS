///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadGridDataPS.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include "DPO_ReadGridDataPS.h"

void DPO_ReadGridDataPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.StdFormAttach();
    gridDataFnameUI.InitFramedUI("Grid Data File ", PPform.GetWidget(), args);
    gridDataFnameUI.SetPromptAndMask("Grid file to read",
        "Grid Files (*.grd)|*.grd|All Files (*.*)|*.*|||");


    args.Clear();
    args.StdTopObjectAttach(gridDataFnameUI);
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    static const char* formatLabels[] = {"Intera Std", "Surfer", "XYZ List", "ArcInfo Ascii", "Other"};
    gridFileFormatUI.PreInitSetUp(formatLabels, 5);
    gridFileFormatUI.InitFramedUI("File Format", topRowForm.GetWidget(), args);

    args.StdSplitFinish(35);
    decimationModulusUI.InitFramedUI("Decimation", topRowForm.GetWidget(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    setXLogUI.StdToggleInit("X is Log", optionsFrame.GetForm(), args);

    args.StdToggleSetup(25);
    setYLogUI.StdToggleInit("Y is Log", optionsFrame.GetForm(), args);

    args.StdSplitMiddle(40, 70);
    setXIDUI.SetFieldLen(10);
    setXIDUI.InitLabeledUI("X ID: ", optionsFrame.GetForm(), args);

    args.StdSplitFinish(30);
    setYIDUI.SetFieldLen(10);
    setYIDUI.InitLabeledUI("Y ID: ", optionsFrame.GetForm(), args);

    args.StdTopObjectAttach(optionsFrame);
    xcoordStatus.StdInit("Grid X Status", PPform.GetWidget(), args);

    args.StdTopObjectAttach(xcoordStatus.coordFrame);
    ycoordStatus.StdInit("Grid Y Status", PPform.GetWidget(), args);
}

void  DPO_ReadGridDataPS::MenuClose()
{
    gridDataFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadGridDataPS::SetMenuData(DPO_ReadGridData&  inData)
{
    gridDataFnameUI.SetFieldString(inData.gridDataFname);
    gridFileFormatUI.SetChoice(inData.gridFileFormat);

    decimationModulusUI.SetFieldValue(inData.decimationModulus);

    setXLogUI.SetValue(inData.setXLog);
    setYLogUI.SetValue(inData.setYLog);
    setXIDUI.SetFieldString(inData.setXID);
    setYIDUI.SetFieldString(inData.setYID);

    if (inData.StatusOK())
    {
        xcoordStatus.SetOKStatus(inData.gridData.xData);
        ycoordStatus.SetOKStatus(inData.gridData.yData);
    }
    else
    {
        xcoordStatus.SetBadStatus();
        ycoordStatus.SetBadStatus();
    }
    MenuBaseC::SetMenuData(inData);
}

bool DPO_ReadGridDataPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    gridDataFnameUI.FieldFileOK(allOK, "Grid Data file", true, false);
    decimationModulusUI.FieldValueOK(allOK, "decimation", 1, 20);
    return allOK;
}

void DPO_ReadGridDataPS::GetMenuData(DPO_ReadGridData&  outData)
{
    MenuBaseC::GetMenuData(outData);
    gridDataFnameUI.GetFieldFile(outData.gridDataFname);
    outData.gridFileFormat = IO_GridData::GridFileFormat(gridFileFormatUI.GetChoice());
    outData.decimationModulus = decimationModulusUI.GetFieldValue();

    outData.setXLog = setXLogUI.GetValue();
    outData.setYLog = setYLogUI.GetValue();

    setXIDUI.GetFieldString(outData.setXID, DC_GridData::gridIDLen);
    setYIDUI.GetFieldString(outData.setYID, DC_GridData::gridIDLen);
}

void DPO_ReadGridDataPS::MenuClear()
{
    gridDataFnameUI.SetFieldString(NULL);
}

