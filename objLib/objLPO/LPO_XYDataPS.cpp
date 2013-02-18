///////////////////////////////////////////////////////////////////////////////////
//
// LPO_XYDataPS.cpp
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

#include "ObjLibAfx.h"

#include <genClass/U_Str.h>

#include <genClass/DO_XYData.h>

#include "LPO_XYData.h"
#include "LPO_XYDataPS.h"

LPO_XYDataPS::LPO_XYDataPS() :
    xyDataObjRefUI(typeid(DO_XYData))
{
}


void LPO_XYDataPS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    xyDataObjRefUI.InitFramedUI("XYData Data To List", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    optionsFrame.InitFramedUI("Data To Write", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    listXYDataIDUI.StdToggleInit("XYData ID", optionsFrame.GetForm(), args);

    args.StdToggleSetup(30);
    listHeadersUI.StdToggleInit("column headers", optionsFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(optionsFrame);
    xyDataFormatUI.InitFramedUI("XY Data Format", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(xyDataFormatUI);
    xyDataColWidthUI.SetFieldLen(6);
    xyDataColWidthUI.InitFramedUI("Column width", PPform.GetWidget(), args);
}


void LPO_XYDataPS::MenuOpen(LPO_XYData&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    xyDataObjRefUI.StdMenuInit(inData.xyDataObjRef);
}


void LPO_XYDataPS::SetMenuData(LPO_XYData&    inData)
{
    MenuBaseC::SetMenuData(inData);
    xyDataObjRefUI.SetFuncObjRef(inData.xyDataObjRef);

    listXYDataIDUI.SetValue(inData.listXYDataID);
    listHeadersUI.SetValue(inData.listHeaders);
    xyDataFormatUI.SetRealFormatSettings(inData.xyDataFormat);
    xyDataColWidthUI.SetFieldValue(inData.xyDataColWidth);
}

bool LPO_XYDataPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    xyDataObjRefUI.FuncObjRefOK(allOK, "XYData data source");
    xyDataColWidthUI.FieldValueOK(allOK, "Col width", 5, 30);
    return allOK;
}

void LPO_XYDataPS::GetMenuData(LPO_XYData&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.xyDataObjRef = xyDataObjRefUI.GetFuncObjRef();

    outData.listXYDataID = listXYDataIDUI.GetValue();
    outData.listHeaders = listHeadersUI.GetValue();

    xyDataFormatUI.GetRealFormatSettings(outData.xyDataFormat);
    outData.xyDataColWidth = xyDataColWidthUI.GetFieldValue();
}

