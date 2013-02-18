///////////////////////////////////////////////////////////////////////////////////
//
// PPO_CubeColorBlockPS.cpp
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

#include "PPO_CubeColorBlockPS.h"

void PPO_CubeColorBlockPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_CubeColorBlockPS*>(inObj)->SetAllSensitive();
}

void PPO_CubeColorBlockPS::SetAllSensitive()
{
    edgeSpecUI.SetInternalSensitive();
}

// ****************************************

PPO_CubeColorBlockPS::PPO_CubeColorBlockPS()
{
}

void PPO_CubeColorBlockPS::InitMenu(const PPO_CubeColorBlock&   inData)
{
    PPO_CubeColorBasePS::InitMenu(inData);

    ArgListC args;
    args.Clear();
    args.StdTopObjectAttach(outsideForm.GetWidget());

    reductionFactorUI.SetFieldLen(6);
    reductionFactorUI.format = SC_RealFormat(5);
    reductionFactorUI.InitFramedUI("Reduction Factor", PPform.GetWidget(), args);

    //  edges
    args.Clear();
    args.StdTopObjectAttach(reductionFactorUI);
    edgeSpecUI.StdInit(PPform.GetWidget(), args, true);
    edgeSpecUI.plotEdgesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(edgeSpecUI);
    PPO_CubeColorBasePS::InitMapAndOffset(inData, args);
}

void PPO_CubeColorBlockPS::MenuOpen(PPO_CubeColorBlock& inData)
{
    PPO_CubeColorBasePS::MenuOpen(inData);
    edgeSpecUI.edgePenUI.ResetPens(inData.objPenSet);
}

void PPO_CubeColorBlockPS::SetMenuData(PPO_CubeColorBlock& inData)
{
    PPO_CubeColorBasePS::SetMenuData(inData, inData);

    reductionFactorUI.SetFieldValue(inData.reductionFactor);

    edgeSpecUI.SetEdgeSpec(inData.edgeSpec);

    SetAllSensitive();
}

bool PPO_CubeColorBlockPS::UIdataOK()
{
    PPO_CubeColorBasePS::UIdataOK();
    reductionFactorUI.FieldValueOK(allOK, "Area Reduction", 0.01, 1.0);
    edgeSpecUI.UIdataOK(allOK);
    return allOK;
}

void PPO_CubeColorBlockPS::GetMenuData(PPO_CubeColorBlock& outData)
{
    PPO_CubeColorBasePS::GetMenuData(outData, outData);

    outData.reductionFactor = reductionFactorUI.GetFieldValue();
    edgeSpecUI.GetEdgeSpec(outData.edgeSpec);
}


