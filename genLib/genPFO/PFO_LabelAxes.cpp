///////////////////////////////////////////////////////////////////////////////////
//
// PFO_LabelAxes.cpp
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
//      Basic plotting objects used by all applications, primarily concerned with plot annotation:
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genClass/DO_TableData.h>

#include <genPlotClass/C_PlotDef.h>

#include <genPFO/PFO_LabelAxes.h>


PFO_LabelAxes :: PFO_LabelAxes(PlotDefC& assPlt)
    : PlotObjC("Label Axis", assPlt)
{
    labelSource = lsLabelArray;
    isXAxis = true;
    limTxtLen = false;
    maxTxtLen = 10;
    skipCol0 = false;

    SetToNull(axesLabel);


    InitCommon();
}

PFO_LabelAxes::PFO_LabelAxes(const PFO_LabelAxes& a, PlotDefC& assPlt) :
    PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_LabelAxes :: ~PFO_LabelAxes ()
{
}

void PFO_LabelAxes::InitCommon()
{
    reportOK        = true;
    usePolyOffset   = true;

    AddInPort(labelArrayObjRef, typeid(DO_LabelArray));
    checkLabelArray = GetLastInPortCheckRef();

    AddInPort(tableObjRef, typeid(DO_TableData));
    checkTable = GetLastInPortCheckRef();

    labelText.SetStringLen(maxLabelLen);

    DoStatusChk();
}

PFO_LabelAxes& PFO_LabelAxes::operator= (const PFO_LabelAxes& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

bool PFO_LabelAxes::ContainsLinesOnly()
{
    return true;
}


void  PFO_LabelAxes::LocalCopy(const PFO_LabelAxes& a)
{
    labelSource         = a.labelSource;
    labelArrayObjRef    = a.labelArrayObjRef;
    tableObjRef         = a.tableObjRef;
    skipCol0            = a.skipCol0;

    isXAxis             = a.isXAxis;
    limTxtLen           = a.limTxtLen;
    maxTxtLen           = a.maxTxtLen;

    CopyString(axesLabel, a.axesLabel, maxLabelLen);
}


void  PFO_LabelAxes:: DoStatusChk()
{
    *checkLabelArray = (labelSource == lsLabelArray);
    *checkTable = (labelSource != lsLabelArray);
    // base class check
    PlotObjC::DoStatusChk();

    if (labelSource == lsLabelArray)
    {
        if (!CheckInputFO(labelArrayObjRef, "Label array source"))
            return;

        const DO_LabelArray& inDO = *(static_cast<DO_LabelArray*>(GetInPortData(labelArrayObjRef)));

        labelText = inDO.dataLabels;

        labelVal.Alloc(labelText.Size());
        for (int i = 0; i < labelText.Size(); i++)
            if (isXAxis)
                labelVal += inDO.dataCoords[i].cX;
            else
                labelVal += inDO.dataCoords[i].cY;

        SortLabels(inDO.dataLabels);
    }
    else
    {
        if (!CheckInputFO(tableObjRef, "Table source"))
            return;

        const DC_TableData& inDC = static_cast<DO_TableData*>(GetInPortData(tableObjRef))->GetTable();

        if ((labelSource == lsTableCol) && skipCol0 && (inDC.GetNCols() < 2))
        {
            SetObjErrMsg("Only 1 column - cannot skip");
            return;
        }

        if (labelSource == lsTableCol)
            labelText = inDC.columnDesc;
        else
            labelText = inDC.rowDesc;

        if (labelText.IsEmpty())
        {
            SetObjErrMsg("now row/column descriptions in input table");
            return;
        }

        labelVal.Alloc(labelText.Size());
        for (int i = 0; i < labelText.Size(); i++)
            labelVal += double(i + 1);

        if ((labelSource == lsTableCol) && skipCol0)
        {
            labelText.DeleteItem(0);
            labelVal.DeleteItem(0);
        }
    }

    if (StatusNotOK())
        return;

    if (limTxtLen)
        LimitTextLength();


    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);
    if (isXAxis && (!xLin))
        SetObjErrMsg("X axes must be linear");
    else if ((!isXAxis) && (!yLin))
        SetObjErrMsg("Y axes must be linear");

    ClipToLimits();
}

void PFO_LabelAxes:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

bool PFO_LabelAxes::ClipToLimits()
{
    int nlab = labelText.Size();
    labelInLimits.AllocAndFill(nlab, false);

    Limit3D currLimits = GetCurrentViewLimits();
    double axStart = currLimits.minLim.cY;
    double axEnd = currLimits.maxLim.cY;
    if (isXAxis)
    {
        axStart = currLimits.minLim.cX;
        axEnd = currLimits.maxLim.cX;
    }

    if (axEnd <= axStart)
        Swap(axStart, axEnd);

//  {
//      SetObjErrMsg("X/Y axis must be ascending");
//      return false;
//  }

    for (int i = 0; i < labelText.Size(); i++)
        labelInLimits[i] = ((labelVal[i] >= axStart) && (labelVal[i] <= axEnd));

    if (labelInLimits.GetSetCount() == 0)
    {
        SetObjErrMsg("No labels within axes limits");
        return false;
    }

    return true;
}

Limit3D PFO_LabelAxes::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (!doPlot))
        return currLimits;

    double minVal = labelVal[0];
    double maxVal = labelVal.LastIndex();
    if (labelVal.Size() > 1)
    {
        minVal -= (labelVal[1] - labelVal[0]) / 2.0;
        maxVal += (labelVal.LastIndex() - labelVal.LastIndex(1)) / 2.0;
    }

    Coord3D tempCoord;
    if (isXAxis)
    {
        tempCoord.cX = minVal;
        currLimits.AddToLimits(tempCoord);

        tempCoord.cX = maxVal;
        currLimits.AddToLimits(tempCoord);
    }
    else
    {
        tempCoord.cY = minVal;
        currLimits.AddToLimits(tempCoord);

        tempCoord.cY = maxVal;
        currLimits.AddToLimits(tempCoord);
    }

    return currLimits;
}

void PFO_LabelAxes::SortLabels(const SC_StringArray& unsortedText)
{
    int nval = labelVal.Size();
    SC_IntArray indexes;
    indexes.AllocAndIndex(nval);
    labelVal.Sort(true, indexes);

    labelText.AllocAndSetSize(nval);
    for (int i = 0; i < labelVal.Size(); i++)
        labelText.SetString(unsortedText[indexes[i]], i);
}

void PFO_LabelAxes::LimitTextLength()
{
    char label[maxLabelLen];
    char temp[maxLabelLen * 2];

    for (int i = 0; i < labelText.Size(); i++)
    {
        CopyString(label, labelText[i], maxLabelLen);
        FullTrim(label);
        int labelLen = StringLength(label);
        int indx = 0;
        int lastIndx = 0;
        int count = 0;
        for (int j = 0; j < labelLen; j++)
        {
            if (label[j] != ' ')
            {
                temp[indx++] = label[j];
                count++;
            }
            else
            {
                if (count > maxTxtLen)
                {
                    temp[indx++] = '\\';
                    temp[indx++] = 'n';
                    count = 0;
                }
                else
                {
                    lastIndx = indx;
                    temp[indx++] = label[j];
                    count++;
                }
            }

            if ((count > maxTxtLen) && (lastIndx > 0))
            {
                count = 0;
                for (int k = indx; k > lastIndx; k--)
                {
                    temp[k + 1] = temp[k];
                    count++;
                }
                temp[lastIndx] = '\\';
                temp[lastIndx + 1] = 'n';
                indx++;
                lastIndx = 0;
            }
        }//character loop

        temp[indx] = '\0';
        CopyString(labelText[i], temp, maxLabelLen);
    }
}

