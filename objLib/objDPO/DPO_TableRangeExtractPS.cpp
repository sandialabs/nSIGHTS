///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableRangeExtractPS.cpp
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

#include "DPO_TableRangeExtract.h"
#include "DPO_TableRangeExtractPS.h"

DPO_TableRangeExtractPS::DPO_TableRangeExtractPS()
{
}

void DPO_TableRangeExtractPS::InitMenu()
{
    DPO_RangeExtractPS::InitMenu();

    tableColSelUI.StdInit(URform.GetWidget(), PPform.GetWidget());

    ArgListC args;
    args.StdTopObjectAttach(tableColSelUI.selectedColUI);
    InitDataUseUI(args, PPform.GetWidget());
    InitRECommon();
}

void DPO_TableRangeExtractPS::SetMenuData(DPO_TableRangeExtract&  inData)
{
    tableColSelUI.SetTableColSel(inData);
    DPO_RangeExtractPS::SetMenuData(inData);
}

void DPO_TableRangeExtractPS::MenuOpen(DPO_TableRangeExtract& inData)
{
    DPO_RangeExtractPS::MenuOpen(inData);
    tableColSelUI.MenuOpen(inData);
}


bool DPO_TableRangeExtractPS::UIdataOK()
{
    DPO_RangeExtractPS::UIdataOK();
    tableColSelUI.UIdataOK(allOK);
    return allOK;
}

void DPO_TableRangeExtractPS::GetMenuData(DPO_TableRangeExtract&  outData)
{
    DPO_RangeExtractPS::GetMenuData(outData);
    tableColSelUI.GetTableColSel(outData);
}


