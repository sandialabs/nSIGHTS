///////////////////////////////////////////////////////////////////////////////////
//
// UPO_WriteLabelArrayPS.cpp
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

#include <genClass/DO_LabelArray.h>

#include "UPO_WriteLabelArrayPS.h"

UPO_WriteLabelArrayPS::UPO_WriteLabelArrayPS() :
    dataSource(typeid(DO_LabelArray))
{
}

void UPO_WriteLabelArrayPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<UPO_WriteLabelArrayPS*>(inObj)->SetAllSensitive();
}

void UPO_WriteLabelArrayPS::SetAllSensitive()
{
    outField.SetSensitive(dataSource.CheckDataObj());
}


void UPO_WriteLabelArrayPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    dataSource.InitFramedUI("XY Data Source", PPform.GetWidget(), args);
    dataSource.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(dataSource);
    args.TopOffset(10);
    outField.InitFramedUI("Table Output File", PPform.GetWidget(), args);

}

void UPO_WriteLabelArrayPS::MenuOpen(UPO_WriteLabelArray&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    dataSource.StdMenuInit(inData.labelObjRef);
}

void UPO_WriteLabelArrayPS::MenuClose()
{
    outField.CloseDialog();
    MenuBaseC::MenuClose();
}

void UPO_WriteLabelArrayPS::SetMenuData(UPO_WriteLabelArray&    inData)
{
    MenuBaseC::SetMenuData(inData);

    dataSource.SetFuncObjRef(inData.labelObjRef);
    outField.SetFieldString(inData.outFname);

    SetAllSensitive();
}

bool UPO_WriteLabelArrayPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataSource.FuncObjRefOK(allOK, "Data source");
    outField.FieldFileOK(allOK, "Output file", false, true);
    return allOK;
}

void UPO_WriteLabelArrayPS::GetMenuData(UPO_WriteLabelArray&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outField.GetFieldFile(outData.outFname);
    outData.labelObjRef = dataSource.GetFuncObjRef();

}

