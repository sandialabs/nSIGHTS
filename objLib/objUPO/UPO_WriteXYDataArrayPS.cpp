///////////////////////////////////////////////////////////////////////////////////
//
// UPO_WriteXYDataArrayPS.cpp
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

#include <objClass/DO_XYDataArray.h>

#include "UPO_WriteXYDataArrayPS.h"

UPO_WriteXYDataArrayPS::UPO_WriteXYDataArrayPS() :
        dataSource(typeid(DO_XYDataArray))
{
}


void UPO_WriteXYDataArrayPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC    args;
    args.StdFormAttach();
    args.TopOffset(10);
    dataSource.InitFramedUI("XY Data Array Data Source", PPform.GetWidget(), args);

    args.StdTopObjectAttach(dataSource);
    args.TopOffset(10);
    outField.InitFramedUI("XY data array output file", PPform.GetWidget(), args);
    outField.SetPromptAndMask("XY data array file to create",
        "XY data array files (*.nXYA)|*.nXYA|All Files (*.*)|*.*|||");

    mpiFilesUI.nodeFileID += "XY array file";
    args.StdTopObjectAttach(outField);
    mpiFilesUI.StdInit(PPform.GetWidget(), args);

    args.StdTopObjectAttach(mpiFilesUI);
    writeAsTextUI.PreInitSetUp("Binary", "Text");
    writeAsTextUI.InitFramedUI("File Format", PPform.GetWidget(), args);
}

void UPO_WriteXYDataArrayPS::MenuOpen(UPO_WriteXYDataArray&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    dataSource.StdMenuInit(inData.xyDataArrayObjRef);
}

void UPO_WriteXYDataArrayPS::MenuClose()
{
    outField.CloseDialog();
    MenuBaseC::MenuClose();
}


void UPO_WriteXYDataArrayPS::SetMenuData(UPO_WriteXYDataArray&    inData)
{
    MenuBaseC::SetMenuData(inData);

    dataSource.SetFuncObjRef(inData.xyDataArrayObjRef);
    outField.SetFieldString(inData.outFname);
    writeAsTextUI.SetBoolChoice(inData.writeAsText);
    mpiFilesUI.SetNodeFileArray(inData.mpiFiles);

}

bool UPO_WriteXYDataArrayPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataSource.FuncObjRefOK(allOK, "Data source");
    outField.FieldFileOK(allOK, "Output file", false, true);
    return allOK;
}

void UPO_WriteXYDataArrayPS::GetMenuData(UPO_WriteXYDataArray&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outField.GetFieldFile(outData.outFname);
    outData.xyDataArrayObjRef = dataSource.GetFuncObjRef();
    outData.writeAsText = writeAsTextUI.GetBoolChoice();
    mpiFilesUI.GetNodeFileArray(outData.mpiFiles);

}

