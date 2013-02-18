///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYInterpValPS.cpp
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

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/DO_TableData.h>

#include "DPO_XYInterpVal.h"
#include "DPO_XYInterpValPS.h"


DPO_XYInterpValPS::DPO_XYInterpValPS()
{
}


void DPO_XYInterpValPS::InitMenu()
{
    DPO_InterpValPS::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    xyColSelUI.xyInputObjRefUI.InitFramedUI("XY Input Data", URform.GetWidget(), args);

    args.StdFormAttach();
    xyColSelUI.SelColInit(PPform.GetWidget(), args);

    xyColSelUI.operateOnXUI.frame.SetFrameText("Data to Interpolate On");

    args.StdTopObjectAttach(xyColSelUI.operateOnXUI);
    InitInterpValCommon(args);
}

void DPO_XYInterpValPS::SetMenuData(DPO_XYInterpVal&  inData)
{
    xyColSelUI.SetXYColSel(inData);
    DPO_InterpValPS::SetMenuData(inData);
}


void DPO_XYInterpValPS::MenuOpen(DPO_XYInterpVal& inData)
{
    DPO_InterpValPS::MenuOpen(inData);
    xyColSelUI.MenuOpen(inData);
}


bool DPO_XYInterpValPS::UIdataOK()
{
    DPO_InterpValPS::UIdataOK();
    xyColSelUI.UIdataOK(allOK);
    return allOK;
}

void DPO_XYInterpValPS::GetMenuData(DPO_XYInterpVal&  outData)
{
    DPO_InterpValPS::GetMenuData(outData);
    xyColSelUI.GetXYColSel(outData);
}

