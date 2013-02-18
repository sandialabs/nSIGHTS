///////////////////////////////////////////////////////////////////////////////////
//
// DFO_MergeColorMap.cpp
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
//              Combines two color maps sequentially.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/SC_RealFormat.h>

#include <genClass/U_Str.h>

#include <genDFO/DFO_MergeColorMap.h>

// **************************************** constructor/destructor


DFO_MergeColorMap :: DFO_MergeColorMap() : FuncObjC("Merge Color Maps")
{
    InitCommon();
}

DFO_MergeColorMap::DFO_MergeColorMap(const DFO_MergeColorMap& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_MergeColorMap :: ~DFO_MergeColorMap ()
{
}

void DFO_MergeColorMap::InitCommon()
{
    AddInPort(input1ObjRef,      typeid(DO_ColorMap));
    AddInPort(input2ObjRef,      typeid(DO_ColorMap));

    AddOutPort(outColorMapDO);
    outColorMapDO.colorMapData = &mergeColorMap;
    DoStatusChk();
}

DFO_MergeColorMap& DFO_MergeColorMap::operator= (const DFO_MergeColorMap& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_MergeColorMap::LocalCopy(const DFO_MergeColorMap& a)
{
    input1ObjRef = a.input1ObjRef;
    input2ObjRef = a.input2ObjRef;
}


void DFO_MergeColorMap:: DoStatusChk()
{
    mergeColorMap.ncolorMap = 0;
    FuncObjC::DoStatusChk();

    //  get data
    if ((! CheckInputFO(input1ObjRef, "Input color map #1")) ||
        (! CheckInputFO(input2ObjRef, "Input color map #2")))
        return;

    const DC_ColorMap*    colorMap1 = static_cast<DO_ColorMap*>(GetInPortData(input1ObjRef))->colorMapData;
    const DC_ColorMap*    colorMap2 = static_cast<DO_ColorMap*>(GetInPortData(input2ObjRef))->colorMapData;

    if ((colorMap1->ncolorMap + colorMap2->ncolorMap) > DC_ColorMap::maxCMcolors)
    {
        SetObjErrMsg("Too many colors in result (max 255)");
        return;
    }
    mergeColorMap.AddColorMaps(*colorMap1, *colorMap2);
}


void DFO_MergeColorMap:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

