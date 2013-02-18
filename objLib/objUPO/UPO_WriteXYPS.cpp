///////////////////////////////////////////////////////////////////////////////////
//
// UPO_WriteXYPS.cpp
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

#include <genClass/DO_XYData.h>

#include "UPO_WriteXYPS.h"

UPO_WriteXYPS::UPO_WriteXYPS() :
    dataSource(typeid(DO_XYData))
{
}

void UPO_WriteXYPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<UPO_WriteXYPS*>(inObj)->SetAllSensitive();
}

void UPO_WriteXYPS::SetAllSensitive()
{
    outField.SetSensitive(dataSource.CheckDataObj());
}


void UPO_WriteXYPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    dataSource.InitFramedUI("XY Data Source", PPform.GetWidget(), args);
    dataSource.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(dataSource);
    args.TopOffset(10);
    outField.InitFramedUI("XY Output File", PPform.GetWidget(), args);

    args.StdTopObjectAttach(outField);
    nextRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    optionsFrame.InitFramedUI("Options", nextRow.GetWidget(), args);

    args.StdToggleSetup(5);
    writeColHeadersUI.StdToggleInit("Col Headers", optionsFrame.GetForm(), args);

    args.StdSplitFinish();
    mpiFilesUI.nodeFileID += "XY file";
    mpiFilesUI.StdInit(nextRow.GetWidget(), args);


}

void UPO_WriteXYPS::MenuOpen(UPO_WriteXY&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    dataSource.StdMenuInit(inData.xyObjRef);
}

void UPO_WriteXYPS::MenuClose()
{
    outField.CloseDialog();
    MenuBaseC::MenuClose();
}

void UPO_WriteXYPS::SetMenuData(UPO_WriteXY&    inData)
{
    MenuBaseC::SetMenuData(inData);

    dataSource.SetFuncObjRef(inData.xyObjRef);
    outField.SetFieldString(inData.outFname);
    writeColHeadersUI.SetValue(inData.writeColHeaders);
    mpiFilesUI.SetNodeFileArray(inData.mpiFiles);

    SetAllSensitive();
}

bool UPO_WriteXYPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataSource.FuncObjRefOK(allOK, "Data source");
    outField.FieldFileOK(allOK, "Output file", false, true);
    return allOK;
}

void UPO_WriteXYPS::GetMenuData(UPO_WriteXY&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outField.GetFieldFile(outData.outFname);
    outData.xyObjRef = dataSource.GetFuncObjRef();
    mpiFilesUI.GetNodeFileArray(outData.mpiFiles);

    outData.writeColHeaders = writeColHeadersUI.GetValue();
}

