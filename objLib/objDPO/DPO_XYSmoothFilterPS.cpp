///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYSmoothFilterPS.cpp
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

#include <genClassPS/PS_UIBase.h>

#include "DPO_XYSmoothFilter.h"
#include "DPO_XYSmoothFilterPS.h"

DPO_XYSmoothFilterPS::DPO_XYSmoothFilterPS()
{
}

//  The following is a kludge, but at least it's a readable one.
//  AFter ripping out the FFT smoothing options, the UI now only has two options:  0  =  median
//  and 1 = mean.  This corresponds to sfMedian = 1 and sfMean = 4, in the enumeration in DFO_XYSmoothFilter.
//  The reason I don't change the enumeration is because we would then have an issue with older .nPre files
//  which already specify the old enumeration.  The reason I don't rev the version number of all of this is
//  because it is such a minor detail that I don't think it is necessary.  That is the robust fix, however.
int DPO_XYSmoothFilterPS::MapFilterChoice(int UIChoice)
{
    if(UIChoice == 0)
        return(DFO_XYSmoothFilter::sfMedian);
    if(UIChoice == 1)
        return(DFO_XYSmoothFilter::sfMean);
    //default (should never be here)
    return(DFO_XYSmoothFilter::sfMedian);
}

int DPO_XYSmoothFilterPS::MapFilterChoiceBack(int filterChoice)
{
    if(filterChoice == DFO_XYSmoothFilter::sfMedian);
        return(0);
    if(filterChoice == DFO_XYSmoothFilter::sfMean);
        return(1);
    //default (should never be here)
    return(0);
}

void DPO_XYSmoothFilterPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_XYSmoothFilterPS*>(inObj)->SetAllSensitive();
}

void DPO_XYSmoothFilterPS::SetAllSensitive()
{
    bool isMedianOp = MapFilterChoice(smoothFilterOpUI.GetChoice()) == DFO_XYSmoothFilter::sfMedian;
    bool isMeanOp   = MapFilterChoice(smoothFilterOpUI.GetChoice()) == DFO_XYSmoothFilter::sfMean;

    ptsInHalfWindowField.SetSensitive(isMedianOp || isMeanOp);
}

void DPO_XYSmoothFilterPS::InitMenu()
{

    DPO_XYBasePS::InitMenu("XY Smooth/Filter");

    ArgListC args;

    // frame
    args.Clear();
    args.StdFormAttach();
    smoothFilterFrame.InitFramedUI("Smooth/Filter", PPform.GetWidget(), args);

    // smooth/filter operation
    args.Clear();
    args.StdFormAttach();

    const char* operationStrings[] = {"Median Smooth", "Mean Smooth"};
    smoothFilterOpUI.PreInitSetUp(operationStrings, 2);
    smoothFilterOpUI.InitLabeledUI("Smooth/Filter Operation", smoothFilterFrame.GetForm(), args);
    smoothFilterOpUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(smoothFilterOpUI);
    ptsInHalfWindowField.InitLabeledUI("# of pts in half window", smoothFilterFrame.GetForm(), args);
    // # of pts in half window
    args.Clear();
    args.StdTopObjectAttach(ptsInHalfWindowField);


    args.Clear();
    args.StdTopObjectAttach(smoothFilterFrame);
    SetOutputDescUI(args);
}

void DPO_XYSmoothFilterPS::MenuOpen(DPO_XYSmoothFilter& inData)
{
    DPO_XYBasePS::MenuOpen(inData);
}


void DPO_XYSmoothFilterPS::SetMenuData(DPO_XYSmoothFilter&  inData)
{
    DPO_XYBasePS::SetMenuData(inData, inData);

    smoothFilterOpUI.SetChoice(MapFilterChoiceBack(inData.smoothFilterOp));
    ptsInHalfWindowField.SetFieldValue(inData.ptsInHalfWindow);

    SetAllSensitive();
}

bool DPO_XYSmoothFilterPS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    if ((MapFilterChoice(smoothFilterOpUI.GetChoice()) == DFO_XYSmoothFilter::sfMedian)||
        (MapFilterChoice(smoothFilterOpUI.GetChoice()) == DFO_XYSmoothFilter::sfMean))
        ptsInHalfWindowField.FieldValueOK(allOK, "# of pts in half window", 0, 100);

    return allOK;
}

void DPO_XYSmoothFilterPS::GetMenuData(DPO_XYSmoothFilter&  outData)
{
    DPO_XYBasePS::GetMenuData(outData, outData);

    outData.smoothFilterOp = DFO_XYSmoothFilter::SmoothFilterOp(MapFilterChoice(smoothFilterOpUI.GetChoice()));

    if (outData.smoothFilterOp == DFO_XYSmoothFilter::sfMedian)
        outData.ptsInHalfWindow = ptsInHalfWindowField.GetFieldValue();

    if (outData.smoothFilterOp == DFO_XYSmoothFilter::sfMean)
        outData.ptsInHalfWindow = ptsInHalfWindowField.GetFieldValue();

}

