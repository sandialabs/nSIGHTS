///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadVariableArrayPS.cpp
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

#include <genClass/U_Str.h>

#include <genApp/U_Menu.h>

#include "DPO_ReadVariableArrayPS.h"
#include "DPO_ReadVariableArray.h"

DPO_ReadVariableArrayPS::DPO_ReadVariableArrayPS()
{
}

void DPO_ReadVariableArrayPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.StdFormAttach();
    dataFile.InitFramedUI("Variable Input File ", PPform.GetWidget(), args);

    args.StdTopObjectAttach(dataFile);
    static const char* formatLabels[] = {"PaCalc", "Other"};
    varArrayFileFormatUI.PreInitSetUp(formatLabels, 2);
    varArrayFileFormatUI.InitFramedUI("File Format", PPform.GetWidget(), args);

    args.StdTopObjectAttach(varArrayFileFormatUI);
    mpiFilesUI.nodeFileID += "variable array file";
    mpiFilesUI.StdInit(PPform.GetWidget(), args);

    args.StdTopObjectAttach(mpiFilesUI);
    statusFrame.InitFramedUI("Data Status", PPform.GetWidget(), args);

    args.StdFormAttach(5);
    args.LeftOffset(10);
    args.RightOffset(10);
    nVars.InitLabeledUI("# of variables read", statusFrame.GetForm(), args);
}

void DPO_ReadVariableArrayPS::MenuOpen(DPO_ReadVariableArray& inData)
{
    MenuBaseC::MenuOpen(true, false);
}

void DPO_ReadVariableArrayPS::MenuClose()
{
    dataFile.CloseDialog();
    MenuBaseC::MenuClose();
}


void DPO_ReadVariableArrayPS::SetMenuData(DPO_ReadVariableArray&  inData)
{
    MenuBaseC::SetMenuData(inData);

    dataFile.SetFieldString(inData.varArrayFname);
    varArrayFileFormatUI.SetChoice(int(inData.varArrayFileFormat));
    mpiFilesUI.SetNodeFileArray(inData.mpiFiles);

    bool isOK = (inData.StatusOK());
    if (isOK)
    {
        nVars.SetFieldValue(inData.GetNVars());
    }
    else
    {
        nVars.SetStatusNA();
    }
    statusFrame.SetSensitive(isOK);
}

bool DPO_ReadVariableArrayPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataFile.FieldFileOK(allOK, "Variable array", true, false);
    return allOK;
}

void DPO_ReadVariableArrayPS::GetMenuData(DPO_ReadVariableArray&  outData)
{
    MenuBaseC::GetMenuData(outData);
    dataFile.GetFieldFile(outData.varArrayFname);
    outData.varArrayFileFormat = IO_VariableArray::VariableFileFormat(varArrayFileFormatUI.GetChoice());
    mpiFilesUI.GetNodeFileArray(outData.mpiFiles);
}


void DPO_ReadVariableArrayPS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    dataFile.SetFieldString(NULL);
}

