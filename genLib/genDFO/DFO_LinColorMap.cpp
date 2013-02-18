///////////////////////////////////////////////////////////////////////////////////
//
// DFO_LinColorMap.cpp
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
//              Defines a basic color map.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/SC_RealFormat.h>

#include <genClass/U_Str.h>

#include <genDFO/DFO_LinColorMap.h>

// **************************************** constructor/destructor


DFO_LinColorMap :: DFO_LinColorMap() : FuncObjC("Linear Color Map")
{
    SetDefault();
    InitCommon();
}

DFO_LinColorMap::DFO_LinColorMap(const DFO_LinColorMap& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_LinColorMap :: ~DFO_LinColorMap ()
{
}

void DFO_LinColorMap::InitCommon()
{
    AddOutPort(colorMapDO);
    AddOutPort(penSetDO);

    colorMapDO.colorMapData = &linColorMap;
    penSetDO.penSetData     = &linPenSet;
    objStatus               = os_OK;
    CalcOutput(foc_Full);
}

DFO_LinColorMap& DFO_LinColorMap::operator= (const DFO_LinColorMap& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_LinColorMap::LocalCopy(const DFO_LinColorMap& a)
{
    useHSVspec  = a.useHSVspec;
    nmapShades  = a.nmapShades;
    mapStart    = a.mapStart  ;
    mapEnd      = a.mapEnd    ;
}


void  DFO_LinColorMap::SetDefault()
{
//  initializes to standard rainbow
    useHSVspec  = true;
    nmapShades  = 50;
    mapStart    = SC_ColorSpec(0.0,  1.0, 1.0);
    mapEnd      = SC_ColorSpec(0.85, 1.0, 1.0);
}


void DFO_LinColorMap:: CalcOutput(FOcalcType  calcType)
{
    if (calcType == foc_PortOnly)
        return;

    linColorMap.ncolorMap = nmapShades;
    if (useHSVspec )
        linColorMap.MakeLinearHSVMap(mapStart, mapEnd);
    else
        linColorMap.MakeLinearRGBMap(mapStart, mapEnd);


    SC_RealFormat RGBform(2);

    int CSindx;
    char rStr[50], gStr[50], bStr[50];
    double CSinc = double(nmapShades) / double(DC_PenSet::maxPenSet);
    for (int i = 0; i < DC_PenSet::maxPenSet; i++)
    {
        CSindx = int(CSinc * double(i));

        SC_ColorSpec tmpSpec = linColorMap.GetColor(CSindx);
        linPenSet.penColor[i] = tmpSpec;

        if (useHSVspec)
            tmpSpec.ToHSV();

        RGBform.RealToString(tmpSpec.RH, rStr, 50);
        RGBform.RealToString(tmpSpec.GS, gStr, 50);
        RGBform.RealToString(tmpSpec.BV, bStr, 50);
        MakeString(linPenSet.penLabel[i], rStr, "/", gStr, "/", bStr, DC_PenSet::maxPenLabelLen);
    }
}
