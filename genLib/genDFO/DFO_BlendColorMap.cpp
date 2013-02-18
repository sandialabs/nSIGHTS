///////////////////////////////////////////////////////////////////////////////////
//
// DFO_BlendColorMap.cpp
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
//              Combines two color maps as a linear combination.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/SC_RealFormat.h>

#include <genClass/U_Str.h>

#include <genDFO/DFO_BlendColorMap.h>

// **************************************** constructor/destructor


DFO_BlendColorMap :: DFO_BlendColorMap() :
    FuncObjC("Blend Color Maps"),
    tValMS("Blend T")
{
    tValMS.enteredRealValue = 0.5;
    InitCommon();
}

DFO_BlendColorMap::DFO_BlendColorMap(const DFO_BlendColorMap& a) :
    FuncObjC(a),
    tValMS(a.tValMS)

{
    LocalCopy(a);
    InitCommon();
}

DFO_BlendColorMap :: ~DFO_BlendColorMap ()
{
}

void DFO_BlendColorMap::InitCommon()
{
    tValMS.AddPort(*this);

    AddInPort(input1ObjRef,      typeid(DO_ColorMap));
    AddInPort(input2ObjRef,      typeid(DO_ColorMap));

    AddOutPort(outColorMapDO);
    outColorMapDO.colorMapData = &blendColorMap;
    DoStatusChk();
}

DFO_BlendColorMap& DFO_BlendColorMap::operator= (const DFO_BlendColorMap& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        tValMS  = a.tValMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_BlendColorMap::LocalCopy(const DFO_BlendColorMap& a)
{
    input1ObjRef = a.input1ObjRef;
    input2ObjRef = a.input2ObjRef;
}


void DFO_BlendColorMap:: DoStatusChk()
{
    blendColorMap.ncolorMap = 0;
    FuncObjC::DoStatusChk();

    tValMS.Reset("Blend T value");
    tValMS.DoCheck("Blend T value master", *this);


    //  get data
    if ((! CheckInputFO(input1ObjRef, "Color map A")) ||
        (! CheckInputFO(input2ObjRef, "Color map B")))
        return;

    colorMap1 = static_cast<DO_ColorMap*>(GetInPortData(input1ObjRef))->colorMapData;
    colorMap2 = static_cast<DO_ColorMap*>(GetInPortData(input2ObjRef))->colorMapData;

    if (colorMap1->ncolorMap != colorMap2->ncolorMap)
    {
        SetObjErrMsg("Both input color maps must be the same size");
        return;
    }
    blendColorMap.AddColorMaps(*colorMap1, *colorMap2);
}


void DFO_BlendColorMap:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    double tvalVal  = tValMS.GetCurrentValue();

    blendColorMap.ncolorMap = colorMap1->ncolorMap;
    for (int i = 0; i < blendColorMap.ncolorMap; i++)
        blendColorMap.colorMapColors[i].Blend(colorMap1->colorMapColors[i], colorMap2->colorMapColors[i], tvalVal);

}
