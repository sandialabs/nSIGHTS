///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableHistogramPS.cpp
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

#include "DPO_TableHistogram.h"
#include "DPO_TableHistogramPS.h"


DPO_TableHistogramPS::DPO_TableHistogramPS()
{
}


void DPO_TableHistogramPS::InitMenu()
{
    DPO_HistogramPS::InitMenu();

    tableColSelUI.StdInit(URform.GetWidget(), PPform.GetWidget());

    ArgListC args;
    args.StdTopObjectAttach(tableColSelUI.selectedColUI);
    InitHistogramCommon(args);
}

void DPO_TableHistogramPS::SetMenuData(DPO_TableHistogram&  inData)
{
    tableColSelUI.SetTableColSel(inData);
    DPO_HistogramPS::SetMenuData(inData);
}

void DPO_TableHistogramPS::MenuOpen(DPO_TableHistogram& inData)
{
    DPO_HistogramPS::MenuOpen(inData);
    tableColSelUI.MenuOpen(inData);
}


bool DPO_TableHistogramPS::UIdataOK()
{
    DPO_HistogramPS::UIdataOK();
    tableColSelUI.UIdataOK(allOK);
    return allOK;
}

void DPO_TableHistogramPS::GetMenuData(DPO_TableHistogram&  outData)
{
    DPO_HistogramPS::GetMenuData(outData);
    tableColSelUI.GetTableColSel(outData);
}


