///////////////////////////////////////////////////////////////////////////////////
//
// DFO_InterpTableColumn.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objDFO/DFO_InterpTableColumn.h>

DFO_InterpTableColumn :: DFO_InterpTableColumn() :
    FuncObjC("Interpolate Table"), interpXValMS("Interp X")
{
    selectedYCol = 0;
    logInterpX = false;
    logInterpY = false;

    InitCommon();
}

DFO_InterpTableColumn::DFO_InterpTableColumn(const DFO_InterpTableColumn& a) :
    FuncObjC(a),
    interpXValMS(a.interpXValMS),
    DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_InterpTableColumn :: ~DFO_InterpTableColumn ()
{
}

void DFO_InterpTableColumn::InitCommon()
{
    interpXValMS.AddPort(*this);
    DSC_TableColSel::AddPort(*this);
    AddOutPort(outputDO);
    DoStatusChk();
}

DFO_InterpTableColumn& DFO_InterpTableColumn::operator= (const DFO_InterpTableColumn& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        DSC_TableColSel::operator=(a);
        interpXValMS = a.interpXValMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_InterpTableColumn::LocalCopy(const DFO_InterpTableColumn& a)
{
    selectedYCol   = a.selectedYCol;
    logInterpX = a.logInterpX;
    logInterpY = a.logInterpY;
}


void  DFO_InterpTableColumn:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
    interpXValMS.DoCheck("Extract XY Source", *this);

    if (StatusOK())
    {
        if (inputData->dataTable.Size() <= selectedYCol)
        {
            SetObjErrMsg("Y column index out of range");
            return;
        }

        interpXVal = interpXValMS.GetCurrentValue();
        SetValue(interpXVal, logInterpX);
        if (RealIsNull(interpXVal))
        {
            SetObjErrMsg("Interp X is not +ve for log X");
        }

        selectedYColData = &(inputData->dataTable[selectedYCol]);
        if (selectedColData->Size() != selectedYColData->Size())
        {
            SetObjErrMsg("columns are different sizes ??");
        }
    }
}

void  DFO_InterpTableColumn::SetValue(double& currVal,
                                       bool    logVal)
{
    if (logVal)
    {
        if (IsPositive(currVal))
        {
            currVal = log10(currVal);
        }
        else
            currVal = nullReal;
    }
}


void DFO_InterpTableColumn:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;

    double nextX, nextY;
    double currX = (*selectedColData)[0];
    SetValue(currX, logInterpX);

    double currY = (*selectedYColData)[0];
    SetValue(currY, logInterpY);

    for (int i = 0; i < selectedColData->UpperBound(); i++)
    {
        nextX = (*selectedColData)[i + 1];
        SetValue(nextX, logInterpX);
        nextY = (*selectedYColData)[i + 1];
        SetValue(nextY, logInterpY);

        if (! (RealIsNull(currX) || RealIsNull(currY) || RealIsNull(nextX)|| RealIsNull(nextY)))
        {
            if ((currX <= interpXVal) && (nextX >= interpXVal))
            {
                double tVal = (interpXVal - currX) / (nextX - currX);
                double interpVal = currY + tVal * (nextY - currY);
                if (logInterpY)
                    interpVal = InvLgt(interpVal);

                char tempStr[80];
                MakeString(tempStr, "I(", inputData->columnDesc[selectedYCol], ")", 80);
                SetDefaultID(tempStr);
                outputDO.Reset(tempStr);
                outputDO.InitLabelAndValue(interpVal);
                return;
            }
        }
        currX = nextX;
        currY = nextY;
    }

    SetObjErrMsg("Interpolant not found/or results null");
    outputDO.Reset("Interp table");
}

