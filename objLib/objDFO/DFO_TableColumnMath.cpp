///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableColumnMath.cpp
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

#include <objClass/DC_Curve.h>

#include <objDFO/DFO_TableColumnMath.h>

DFO_TableColumnMath :: DFO_TableColumnMath() :
    FuncObjC("Table Column Math")
{
    selectedYCol    = 1;
    logX            = false;
    logY            = false;
    mathOp          = moAdd;
    outputFullTable = true;
    SetToNull(columnID);

    InitCommon();
}

DFO_TableColumnMath::DFO_TableColumnMath(const DFO_TableColumnMath& a) :
    FuncObjC(a),
    DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableColumnMath :: ~DFO_TableColumnMath ()
{
}

void DFO_TableColumnMath::InitCommon()
{
    outputTableDC.SetID("MathResults");
    outputTableDO.tableData = &outputTableDC;

    DSC_TableColSel::AddPort(*this);
    AddOutPort(outputTableDO);

    DoStatusChk();
}

DFO_TableColumnMath& DFO_TableColumnMath::operator= (const DFO_TableColumnMath& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableColumnMath::LocalCopy(const DFO_TableColumnMath& a)
{
    selectedYCol    = a.selectedYCol;
    logX            = a.logX;
    logY            = a.logY;
    mathOp          = a.mathOp;
    outputFullTable = a.outputFullTable;
    CopyString(columnID, a.columnID, columnIDLen);
}


void  DFO_TableColumnMath:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
    if (StatusOK())
    {
        if (inputData->dataTable.Size() <= selectedYCol)
            SetObjErrMsg("Y column index out of range");
    }

    if (mathOp == moIntegrate)
        SetObjErrMsg("Integrate not implemented");
}

void DFO_TableColumnMath:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;

    const DC_TableData& inTableDC = *inputData;

    int nrows = inTableDC.GetNRows();
    int ncols = inTableDC.GetNCols() + 1;

    if (!outputFullTable) 
        ncols = 3;

    if (!outputTableDC.Alloc(ncols, nrows, inTableDC.columnDesc.GetStringLen()))
    {
        SetObjErrMsg("table allocation ??");
        return;
    }


    if (outputFullTable) 
    {
        for (int i = 0; i < ncols - 1; i++)
        {
            outputTableDC.columnDesc.SetString(inTableDC.columnDesc[i], i);
            outputTableDC.dataTable[i] = inTableDC.dataTable[i];
        }
    }
    else
    {
        outputTableDC.columnDesc.SetString(inTableDC.columnDesc[selectedCol], 0);
        outputTableDC.columnDesc.SetString(inTableDC.columnDesc[selectedYCol], 1);
        outputTableDC.dataTable[0] = inTableDC.dataTable[selectedCol];
        outputTableDC.dataTable[1] = inTableDC.dataTable[selectedYCol];
    }

    outputTableDC.rowDesc = inTableDC.rowDesc;

    if (IsNull(columnID))
    {
        const char* opLabels[] = {"X+Y", "X-Y", "X*Y", "X/Y", "Int(Y)"};
        outputTableDC.columnDesc += opLabels[int(mathOp)];
    }
    else
        outputTableDC.columnDesc += columnID;

    outputTableDC.dataTable.SetSize(ncols);

    const SC_DoubleArray& xIn = *selectedColData;
    const SC_DoubleArray& yIn = inputData->dataTable[selectedYCol];
    SC_DoubleArray& zOut = outputTableDC.dataTable.LastIndex();
    zOut.SetSize(nrows);

    for (int i = 0; i < nrows; i++)
    {
        double& zVal = zOut[i];
        zVal = nullReal;

        double xVal = xIn[i];
        if (logX && (!IsPositive(xVal)))
            continue;

        double yVal = yIn[i];
        if (logY && (!IsPositive(yVal)))
            continue;

        if (RealIsNull(xVal) || RealIsNull(yVal))
            continue;

        if (logX)
            xVal = Lgt(xVal);
        if (logY)
            yVal = Lgt(yVal);

        switch (mathOp) {
            case moAdd : {zVal = xVal + yVal; break;}
            case moSubtract : {zVal = xVal - yVal; break;}
            case moMultiply : {zVal = xVal * yVal; break;}
            case moDivide : {if (fabs(yVal) > stdEps)
                                    zVal = xVal / yVal; 
                                break;}

            case moIntegrate : {zVal = 1.0; break;}
        }

    }

    SetDefaultID(outputTableDC.columnDesc.LastIndex());
}

