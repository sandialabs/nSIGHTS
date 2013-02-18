///////////////////////////////////////////////////////////////////////////////////
//
// DFO_EnterColorMap.cpp
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
// DESCRIPTION: Basic functional objects (all derived from FuncObjC)
//              used by all applications, primarily concerned with
//              defining/operating on pen sets and colour maps:
//              Define individual colors in a color map.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genDFO/DFO_EnterColorMap.h>


DFO_EnterColorMap :: DFO_EnterColorMap() : FuncObjC("Enter Color Map")
{
    SetDefault();
    InitCommon();
}

DFO_EnterColorMap::DFO_EnterColorMap(const DFO_EnterColorMap& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_EnterColorMap :: ~DFO_EnterColorMap ()
{
}

void DFO_EnterColorMap::InitCommon()
{
    AddOutPort(colorMapDO);

    colorMapDO.colorMapData = &outputColorMap;
    DoStatusChk();
}

DFO_EnterColorMap& DFO_EnterColorMap::operator= (const DFO_EnterColorMap& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_EnterColorMap::LocalCopy(const DFO_EnterColorMap& a)
{
    useHSVspec  = a.useHSVspec;
    enterColorMap = a.enterColorMap;
}


void  DFO_EnterColorMap::SetDefault()
{
    useHSVspec  = true;
    enterColorMap.ncolorMap = 5;
    enterColorMap.colorMapColors[0] = SC_ColorSpec(0.0,   1.0, 1.0);
    enterColorMap.colorMapColors[1] = SC_ColorSpec(0.15,  1.0, 1.0);
    enterColorMap.colorMapColors[2] = SC_ColorSpec(0.30,  1.0, 1.0);
    enterColorMap.colorMapColors[3] = SC_ColorSpec(0.45,  1.0, 1.0);
    enterColorMap.colorMapColors[4] = SC_ColorSpec(0.60,  1.0, 1.0);
    for (int i = enterColorMap.ncolorMap; i < DC_ColorMap::maxCMcolors; i++)
        enterColorMap.colorMapColors[i] = SC_ColorSpec(-1.0, -1.0, -1.0);

}


void DFO_EnterColorMap::DoStatusChk()
{
    outputColorMap.ncolorMap = 0;
    FuncObjC::DoStatusChk();
    if (enterColorMap.ncolorMap < 3)
        SetObjErrMsg("At least 3 colors must be entered");
}

void DFO_EnterColorMap:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    outputColorMap = enterColorMap;
    for (int i = 0; i < enterColorMap.ncolorMap; i++)
        if (useHSVspec )
            outputColorMap.colorMapColors[i].ToRGB();
}
