///////////////////////////////////////////////////////////////////////////////////
//
// PPO_GridColorPointPS.cpp
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

#include "PPO_GridColorPointPS.h"

void PPO_GridColorPointPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_GridColorPointPS*>(inObj)->SetAllSensitive();
}

void PPO_GridColorPointPS::SetAllSensitive()
{
    pointSymbolUI.SetSymbolSize(pointSymSizeUI.GetSymbolSize());
    edgeSpecUI.SetInternalSensitive();
}

// ****************************************

PPO_GridColorPointPS::PPO_GridColorPointPS()
{
}

void PPO_GridColorPointPS::InitMenu(const PPO_GridColorPoint&   inData)
{
    PPO_GridColorBasePS::InitMenu(inData);

    ArgListC args;
    args.StdTopObjectAttach(outsideForm.GetWidget());
    symFrame.InitFramedUI("Symbol", PPform.GetWidget(), args);

    //  area to fill
    args.Clear();
    args.TopFormAttach();
    args.LeftPosAttach(20);
    pointSymbolUI.InitWidget(symFrame.GetForm(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftPosAttach(40);
    pointSymSizeUI.InitWidget(symFrame.GetForm(), args);
    pointSymSizeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    //  edges
    args.StdTopObjectAttach(symFrame);
    edgeSpecUI.StdInit(PPform.GetWidget(), args, inData.is3DObject);
    edgeSpecUI.plotEdgesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(edgeSpecUI);
    PPO_GridColorBasePS::InitMapAndOffset(inData, args);
}

void PPO_GridColorPointPS::MenuOpen(PPO_GridColorPoint& inData)
{
    PPO_GridColorBasePS::MenuOpen(inData);
    edgeSpecUI.edgePenUI.ResetPens(inData.objPenSet);
}

void PPO_GridColorPointPS::SetMenuData(PPO_GridColorPoint& inData)
{
    PPO_GridColorBasePS::SetMenuData(inData, inData);

    pointSymbolUI.SetSymbolType(inData.pointSymbol);
    pointSymSizeUI.SetSymbolSize(inData.pointSymSize);

    edgeSpecUI.SetEdgeSpec(inData.edgeSpec);

    SetAllSensitive();
}

bool PPO_GridColorPointPS::UIdataOK()
{
    PPO_GridColorBasePS::UIdataOK();
    edgeSpecUI.UIdataOK(allOK);
    return allOK;
}

void PPO_GridColorPointPS::GetMenuData(PPO_GridColorPoint& outData)
{
    PPO_GridColorBasePS::GetMenuData(outData, outData);

    outData.pointSymbol = pointSymbolUI.GetSymbolType();
    outData.pointSymSize = pointSymSizeUI.GetSymbolSize();
    edgeSpecUI.GetEdgeSpec(outData.edgeSpec);
}


