///////////////////////////////////////////////////////////////////////////////////
//
// PPO_CubeColorPointPS.cpp
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

#include "PPO_CubeColorPointPS.h"

void PPO_CubeColorPointPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_CubeColorPointPS*>(inObj)->SetAllSensitive();
}

void PPO_CubeColorPointPS::SetAllSensitive()
{
    pointSymbolUI.SetSymbolSize(pointSymSizeUI.GetSymbolSize());
    edgeSpecUI.SetInternalSensitive();
}

// ****************************************

PPO_CubeColorPointPS::PPO_CubeColorPointPS()
{
}

void PPO_CubeColorPointPS::InitMenu(const PPO_CubeColorPoint&   inData)
{
    PPO_CubeColorBasePS::InitMenu(inData);

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
    PPO_CubeColorBasePS::InitMapAndOffset(inData, args);
}

void PPO_CubeColorPointPS::MenuOpen(PPO_CubeColorPoint& inData)
{
    PPO_CubeColorBasePS::MenuOpen(inData);
    edgeSpecUI.edgePenUI.ResetPens(inData.objPenSet);
}

void PPO_CubeColorPointPS::SetMenuData(PPO_CubeColorPoint& inData)
{
    PPO_CubeColorBasePS::SetMenuData(inData, inData);

    pointSymbolUI.SetSymbolType(inData.pointSymbol);
    pointSymSizeUI.SetSymbolSize(inData.pointSymSize);

    edgeSpecUI.SetEdgeSpec(inData.edgeSpec);

    SetAllSensitive();
}

bool PPO_CubeColorPointPS::UIdataOK()
{
    PPO_CubeColorBasePS::UIdataOK();
    edgeSpecUI.UIdataOK(allOK);
    return allOK;
}

void PPO_CubeColorPointPS::GetMenuData(PPO_CubeColorPoint& outData)
{
    PPO_CubeColorBasePS::GetMenuData(outData, outData);

    outData.pointSymbol = pointSymbolUI.GetSymbolType();
    outData.pointSymSize = pointSymSizeUI.GetSymbolSize();
    edgeSpecUI.GetEdgeSpec(outData.edgeSpec);
}


