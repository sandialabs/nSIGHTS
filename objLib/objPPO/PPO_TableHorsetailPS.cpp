///////////////////////////////////////////////////////////////////////////////////
//
// PPO_TableHorsetailPS.cpp
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"


#include "PPO_TableHorsetail.h"
#include "PPO_TableHorsetailPS.h"


PPO_TableHorsetailPS::PPO_TableHorsetailPS()
{
}


void PPO_TableHorsetailPS::InitMenu(const PPO_TableHorsetail& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    tableDataUI.ObjRefInit(topRowForm.GetWidget(), args);

    args.StdSplitFinish(35);
    tableDataUI.SelColInit(topRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    horsetailUI.StdInit(PPform.GetWidget(), args);
}

void PPO_TableHorsetailPS::MenuOpen(PPO_TableHorsetail&   inData)
{
    MenuBaseC::MenuOpen(true, false);
    tableDataUI.MenuOpen(inData);
    horsetailUI.MenuOpen(inData, inData);
}

void PPO_TableHorsetailPS::SetMenuData(PPO_TableHorsetail&    inData)
{
    tableDataUI.SetTableColSel(inData);
    horsetailUI.SetHorsetail(inData);
    PPO_BasePS::SetMenuData(inData);
}

bool PPO_TableHorsetailPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    tableDataUI.UIdataOK(allOK);
    horsetailUI.UIdataOK(allOK);

    return allOK;
}

void PPO_TableHorsetailPS::GetMenuData(PPO_TableHorsetail&    outData)
{
    PPO_BasePS::GetMenuData(outData);

    tableDataUI.GetTableColSel(outData);
    horsetailUI.GetHorsetail(outData);
}


