///////////////////////////////////////////////////////////////////////////////////
//
// DFO_GridExtractIsoPoint.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/SC_RealFormat.h>

#include <objDFO/DFO_GridExtractIsoPoint.h>

DFO_GridExtractIsoPoint :: DFO_GridExtractIsoPoint() :
    FuncObjC("Extract Isopoint from Grid"), isoValMS("Extract Iso Point")
{
    isoOnXLines = true;
    logInterp = false;
    findIsoMax = true;
    InitCommon();
}

DFO_GridExtractIsoPoint::DFO_GridExtractIsoPoint(const DFO_GridExtractIsoPoint& a) :
    FuncObjC(a), isoValMS(a.isoValMS), DSC_GridInput(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_GridExtractIsoPoint :: ~DFO_GridExtractIsoPoint ()
{
}

void DFO_GridExtractIsoPoint::InitCommon()
{
    isoValMS.AddPort(*this);
    DSC_GridInput::AddPort(*this);

    AddOutPort(outputXDO);
    AddOutPort(outputYDO);

    outputXYDC.Alloc(1);
    outputXYDO.xyData = &outputXYDC;
    AddOutPort(outputXYDO);
    DoStatusChk();
}

DFO_GridExtractIsoPoint& DFO_GridExtractIsoPoint::operator= (const DFO_GridExtractIsoPoint& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        DSC_GridInput::operator=(a);
        isoValMS = a.isoValMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_GridExtractIsoPoint::LocalCopy(const DFO_GridExtractIsoPoint& a)
{
    isoOnXLines  = a.isoOnXLines;
    logInterp = a.logInterp;
    findIsoMax = a.findIsoMax;
}

void  DFO_GridExtractIsoPoint::DoStatusChk()
{
    outputXDO.Reset("Grid X value");
    outputYDO.Reset("Grid Y value");

    FuncObjC::DoStatusChk();
    DSC_GridInput::DoCheck(*this);
    isoValMS.DoCheck("Isovalue source", *this);
    if (StatusOK())
    {
        isoVal = isoValMS.GetCurrentValue();
        if (logInterp)
        {
            if (!IsPositive(isoVal))
                SetObjErrMsg("iso value must be +ve for log interp");
            else
                isoVal = log10(isoVal);
        }
    }
}


void DFO_GridExtractIsoPoint:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    const DC_DataCoord* currGrid;
    const DC_DataCoord* currOther;
    if (isoOnXLines)
    {
        currGrid = &(gridInputDC->xData);
        currOther = &(gridInputDC->yData);
    }
    else
    {
        currGrid = &(gridInputDC->yData);
        currOther = &(gridInputDC->xData);
    }

    int nOther = currOther->Size();
    int nGrid = currGrid->Size();

    SC_DoubleArray tempLine(nGrid);
    tempLine.SetSizeToAlloc();

    const SC_DoubleMatrix& gridData = gridInputDC->gData;

    currLineVal = nullReal;
    currOtherVal = nullReal;
    for (int i = 0; i < nOther; i++)
    {
        for (int j = 0; j < nGrid; j++)
            if (isoOnXLines)
                tempLine[j] = gridData[j][i];
            else
                tempLine[j] = gridData[i][j];
        ProcessLine(*currGrid, tempLine, (*currOther)[i]);
    }

    outputXYDC.SetEmpty();
    if (RealIsNull(currLineVal))
    {
        outputXDO.Reset("Isovalue not found");
        outputXDO.InitLabelAndValue(currLineVal);
        outputYDO.Reset("Isovalue not found");
        outputYDO.InitLabelAndValue(currOtherVal);
    }
    else
    {
        const char* typeStr;
        if (findIsoMax)
            typeStr = "Max ";
        else
            typeStr = "Min ";

        char valStr[80];
        MakeString(valStr, typeStr, currGrid->GetID(), 80);
        char otherStr[80];
        MakeString(otherStr, "Assoc. ", currOther->GetID(), 80);

        SC_SciFormat labForm(5);
        if (isoOnXLines)
        {
            outputXDO.Reset(valStr);
            outputXDO.InitLabelAndValue(currLineVal, labForm);
            outputYDO.Reset(otherStr);
            outputYDO.InitLabelAndValue(currOtherVal, labForm);
        }
        else
        {
            outputYDO.Reset(valStr);
            outputYDO.InitLabelAndValue(currLineVal, labForm);
            outputXDO.Reset(otherStr);
            outputXDO.InitLabelAndValue(currOtherVal, labForm);
        }
        outputXYDC.xData += outputXDO.realVal;
        outputXYDC.yData += outputYDO.realVal;
    }

}


void DFO_GridExtractIsoPoint::ProcessLine(const SC_DoubleArray& inGrid,
                                                SC_DoubleArray& inValue,
                                          const double&       otherVal)
{
    if (logInterp)
    {
        if (!inValue.Log10())
            return;
    };

    double nextVal = inValue[0];
    for (int i = 0; i < inValue.UpperBound(); i++)
    {
        double currVal = nextVal;
        nextVal = inValue[i + 1];
        if (RealIsNull(currVal) || RealIsNull(nextVal))
            continue;

        double tVal = (isoVal - currVal) / (nextVal - currVal);

        if ((tVal < 0.0)|| (tVal > 1.0))
            continue;

        double isoPos = inGrid[i] + tVal * (inGrid[i + 1] - inGrid[i]);

        bool doUpdate;
        if (RealIsNull(currLineVal))
            doUpdate = true;
        else
            if (findIsoMax)
                doUpdate = isoPos > currLineVal;
            else
                doUpdate = isoPos < currLineVal;


        if (doUpdate)
        {
            currLineVal = isoPos;
            currOtherVal = otherVal;
        }
    }
}

