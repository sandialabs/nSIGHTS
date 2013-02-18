///////////////////////////////////////////////////////////////////////////////////
//
// UPO_WriteTablePS.cpp
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

#include <genClass/DO_TableData.h>

#include "UPO_WriteTablePS.h"

UPO_WriteTablePS::UPO_WriteTablePS() :
    dataSource(typeid(DO_TableData))
{
}

void UPO_WriteTablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<UPO_WriteTablePS*>(inObj)->SetAllSensitive();
}

void UPO_WriteTablePS::SetAllSensitive()
{
    outField.SetSensitive(dataSource.CheckDataObj());
}


void UPO_WriteTablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    dataSource.InitFramedUI("Table Data Source", PPform.GetWidget(), args);
    dataSource.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(dataSource);
    args.TopOffset(10);
    outField.InitFramedUI("Table Output File", PPform.GetWidget(), args);

    args.StdTopObjectAttach(outField);
    nextRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    optionsFrame.InitFramedUI("Options", nextRow.GetWidget(), args);

    args.StdToggleSetup(5);
    writeColHeadersUI.StdToggleInit("Col Headers", optionsFrame.GetForm(), args);

    args.StdToggleSetup(50);
    writeRowDescsUI.StdToggleInit("Row Descriptions", optionsFrame.GetForm(), args);

    args.StdSplitFinish();
    mpiFilesUI.nodeFileID += "table file";
    mpiFilesUI.StdInit(nextRow.GetWidget(), args);

}

void UPO_WriteTablePS::MenuOpen(UPO_WriteTable&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    dataSource.StdMenuInit(inData.tableObjRef);
}

void UPO_WriteTablePS::MenuClose()
{
    outField.CloseDialog();
    MenuBaseC::MenuClose();
}

void UPO_WriteTablePS::SetMenuData(UPO_WriteTable&    inData)
{
    MenuBaseC::SetMenuData(inData);

    dataSource.SetFuncObjRef(inData.tableObjRef);
    outField.SetFieldString(inData.outFname);
    writeColHeadersUI.SetValue(inData.writeColHeaders);
    writeRowDescsUI.SetValue(inData.writeRowDescs);
    mpiFilesUI.SetNodeFileArray(inData.mpiFiles);

    SetAllSensitive();
}

bool UPO_WriteTablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataSource.FuncObjRefOK(allOK, "Data source");
    outField.FieldFileOK(allOK, "Output file", false, true);

    return allOK;
}

void UPO_WriteTablePS::GetMenuData(UPO_WriteTable&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outField.GetFieldFile(outData.outFname);
    outData.tableObjRef = dataSource.GetFuncObjRef();
    mpiFilesUI.GetNodeFileArray(outData.mpiFiles);

    outData.writeColHeaders = writeColHeadersUI.GetValue();
    outData.writeRowDescs = writeRowDescsUI.GetValue();
}

