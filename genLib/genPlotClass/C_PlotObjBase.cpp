///////////////////////////////////////////////////////////////////////////////////
//
// C_PlotObjBase.cpp
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
//      derived from PlotObjBaseC.  Adds plot layering and
//  reporting support for 2D objects and polygon offset
//  support for 3D objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genPlotClass/PD_3D.h>

#include <genPlotClass/C_PlotObjBase.h>

PlotObjBaseC :: PlotObjBaseC(const char* pltType, PlotDefC& assPlt)
    : InteractiveObj(pltType), assocPlot(&assPlt)
{
    InitCommon(assPlt);
}


PlotObjBaseC :: PlotObjBaseC(const PlotObjBaseC& a, PlotDefC& assPlt)
    : InteractiveObj(a), assocPlot(&assPlt)
{
    InitCommon(assPlt);
    LocalCopy(a);
}


PlotObjBaseC :: ~PlotObjBaseC ()
{
}


void PlotObjBaseC::InitCommon(PlotDefC& assPlt)
{
    doPlot                  = true;
    // pen set link
    AddInPort(penSetObjRef, typeid(DO_PenSet));
    penSetObjRef.objRef = &assPlt;

    //  input associated data
    objPenSet       = 0;          // pen set
}

PlotObjBaseC& PlotObjBaseC::operator= (const PlotObjBaseC& a)
{
    if (&a != this)
    {
        InteractiveObj::operator =(a);
        LocalCopy(a);
    }

    return *this;
}



void  PlotObjBaseC::LocalCopy(const PlotObjBaseC& a)
{
    doPlot      = a.doPlot;
}

void  PlotObjBaseC::DoStatusChk()
{
    objPenSet       = 0;          // pen set
    FuncObjC::DoStatusChk();

    // kluge for pasting
    penSetObjRef.objRef = assocPlot;
//    if (CheckInputFO(penSetObjRef, "Pen set ??"))   // this should always be true
        objPenSet = static_cast<DO_PenSet*>(GetInPortData(penSetObjRef))->penSetData;
}


bool PlotObjBaseC::IsInUse() const
{
    return doPlot || DownStreamObjectsExist();
}


void PlotObjBaseC::GetAxesTypes(bool& xIsLinear,
                                bool& yIsLinear,
                                bool& zIsLinear)
{
    assocPlot->GetAxesTypes(xIsLinear, yIsLinear, zIsLinear);
}

void PlotObjBaseC::GetAxesTypes(bool& xIsLinear,
                                bool& yIsLinear)
{
    assocPlot->GetAxesTypes(xIsLinear, yIsLinear);
}



