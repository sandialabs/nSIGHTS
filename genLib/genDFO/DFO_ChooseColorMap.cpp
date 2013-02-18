///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ChooseColorMap.cpp
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
//              defining/operating on pen sets and colour maps
//
///////////////////////////////////////////////////////////////////////////////////

#include <genDFO/DFO_ChooseColorMap.h>

DFO_ChooseColorMap :: DFO_ChooseColorMap() : FuncObjC("Choose Color Map")
{
    InitCommon();
}

DFO_ChooseColorMap::DFO_ChooseColorMap(const DFO_ChooseColorMap& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}


void DFO_ChooseColorMap::InitCommon()
{
    AddOutPort(outputColorMapDO);
    AddInPort(inputColorMapObjRef,  typeid(DO_ColorMap));
    DoStatusChk();
}

DFO_ChooseColorMap& DFO_ChooseColorMap::operator= (const DFO_ChooseColorMap& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}

void  DFO_ChooseColorMap::LocalCopy(const DFO_ChooseColorMap& a)
{
    inputColorMapObjRef  = a.inputColorMapObjRef;
}

void  DFO_ChooseColorMap::DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (CheckInputFO(inputColorMapObjRef, "Index source"))
        inputColorMapDC  = static_cast<DO_ColorMap*>(GetInPortData(inputColorMapObjRef))->colorMapData;
}


void DFO_ChooseColorMap::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    outputColorMapDO.colorMapData = inputColorMapDC;
}
