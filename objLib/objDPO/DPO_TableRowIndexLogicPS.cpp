///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableRowIndexLogicPS.cpp
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

#include <genClass/DO_Indx.h>

#include "DPO_TableRowIndexLogicPS.h"

DPO_TableRowIndexLogicPS::DPO_TableRowIndexLogicPS() :
    dataASource(typeid(DO_Indx)),
    dataBSource(typeid(DO_Indx))
{
}


void DPO_TableRowIndexLogicPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(50);
    dataASource.InitFramedUI("Row Index Source A", topRowForm.GetWidget(), args);

    args.StdSplitFinish(50);
    dataBSource.InitFramedUI("Row Index Source B", topRowForm.GetWidget(), args);


    static const char* logicOpLabels[]  = { "    A  AND  B      ",
                                            "    A  OR  B       ",
                                            "    A  XOR  B      "};

    logicOpUI.PreInitSetUp(logicOpLabels, 3, true);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    args.RightOffset(50);
    logicOpUI.InitFramedUI("Operation", PPform.GetWidget(), args);
}

void DPO_TableRowIndexLogicPS::MenuOpen(DPO_TableRowIndexLogic& inData)
{
    MenuBaseC::MenuOpen(false, false);
    dataASource.StdMenuInit(inData.dataInputAObjRef);
    dataBSource.StdMenuInit(inData.dataInputBObjRef);
}


void DPO_TableRowIndexLogicPS::SetMenuData(DPO_TableRowIndexLogic&  inData)
{
    MenuBaseC::SetMenuData(inData);

    dataASource.SetFuncObjRef(inData.dataInputAObjRef);
    dataBSource.SetFuncObjRef(inData.dataInputBObjRef);
    logicOpUI.SetChoice(int(inData.logicOp));
}

bool DPO_TableRowIndexLogicPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataASource.FuncObjRefOK(allOK, "Data A source");
    dataBSource.FuncObjRefOK(allOK, "Data B source");
    return allOK;
}

void DPO_TableRowIndexLogicPS::GetMenuData(DPO_TableRowIndexLogic&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.dataInputAObjRef  = dataASource.GetFuncObjRef();
    outData.dataInputBObjRef  = dataBSource.GetFuncObjRef();
    outData.logicOp    = DFO_TableRowIndexLogic::OpType(logicOpUI.GetChoice());
}

