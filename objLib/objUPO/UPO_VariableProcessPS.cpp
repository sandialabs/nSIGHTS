///////////////////////////////////////////////////////////////////////////////////
//
// UPO_VariableProcessPS.cpp
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
//      File I/O and user-interface for objUFO.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <objClass/DO_VariableArray.h>

#include "UPO_VariableProcessPS.h"

UPO_VariableProcessPS::UPO_VariableProcessPS() :
    variableObjRefUI(typeid(DO_VariableArray))
{
}

void UPO_VariableProcessPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<UPO_VariableProcessPS*>(inObj)->SetAllSensitive();
}

void UPO_VariableProcessPS::SetAllSensitive()
{
    outputFileNameUI.SetSensitive(openOutputFileUI.GetValue());
    mpiFilesUI.nodeFileUI[1].SetSensitive(openOutputFileUI.GetValue());
}


void UPO_VariableProcessPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    variableObjRefUI.InitFramedUI("Variable Data Source", PPform.GetWidget(), args);

    args.StdTopObjectAttach(variableObjRefUI);
    args.TopOffset(10);
    directiveFileNameUI.InitFramedUI("$$ Directive File", PPform.GetWidget(), args);

    args.StdTopObjectAttach(directiveFileNameUI);
    outputFileFrame.InitFramedUI("Output File Options", PPform.GetWidget(), args);

    args.StdToggleSetup(2);
    openOutputFileUI.StdToggleInit("open output", outputFileFrame.GetForm(), args);
    openOutputFileUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(80);
    outputFileNameUI.AttachObjectToLabel();
    outputFileNameUI.InitLabeledUI("Output file", outputFileFrame.GetForm(), args);

    mpiFilesUI.nodeFileID += "directive file";
    mpiFilesUI.nodeFileID += "output file";
    args.StdTopObjectAttach(outputFileFrame);
    mpiFilesUI.StdInit(PPform.GetWidget(), args);
}

void UPO_VariableProcessPS::MenuOpen(UPO_VariableProcess&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    variableObjRefUI.StdMenuInit(inData.variableObjRef);
}

void UPO_VariableProcessPS::MenuClose()
{
    directiveFileNameUI.CloseDialog();
    outputFileNameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void UPO_VariableProcessPS::SetMenuData(UPO_VariableProcess&    inData)
{
    MenuBaseC::SetMenuData(inData);

    variableObjRefUI.SetFuncObjRef(inData.variableObjRef);
    directiveFileNameUI.SetFieldString(inData.directiveFileName);
    outputFileNameUI.SetFieldString(inData.outputFileName);
    openOutputFileUI.SetValue(inData.openOutputFile);
    mpiFilesUI.SetNodeFileArray(inData.mpiFiles);

    SetAllSensitive();
}

bool UPO_VariableProcessPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    variableObjRefUI.FuncObjRefOK(allOK, "Data source");
    directiveFileNameUI.FieldFileOK(allOK, "$$ directive File", true, false);
    if (openOutputFileUI.GetValue())
        outputFileNameUI.FieldFileOK(allOK, "Output file", false, true);
    return allOK;
}

void UPO_VariableProcessPS::GetMenuData(UPO_VariableProcess&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.variableObjRef = variableObjRefUI.GetFuncObjRef();
    directiveFileNameUI.GetFieldFile(outData.directiveFileName);

    outData.openOutputFile = openOutputFileUI.GetValue();
    if (outData.openOutputFile)
        outputFileNameUI.GetFieldFile(outData.outputFileName);
    mpiFilesUI.GetNodeFileArray(outData.mpiFiles);
}

