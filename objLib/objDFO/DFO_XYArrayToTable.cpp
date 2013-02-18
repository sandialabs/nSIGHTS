///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYArrayToTable.cpp
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
#include <genClass/DO_XYData.h>
#include <objClass/DO_XYDataArray.h>
#include <objClass/DC_Curve.h>

#include <objDFO/DFO_XYArrayToTable.h>

DFO_XYArrayToTable :: DFO_XYArrayToTable() :
    FuncObjC("Convert XY Array to Table")
{
    interpolateToCommonX = false;
    InitCommon();
}

DFO_XYArrayToTable::DFO_XYArrayToTable(const DFO_XYArrayToTable& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYArrayToTable :: ~DFO_XYArrayToTable ()
{
}

void DFO_XYArrayToTable::InitCommon()
{
    AddInPort(inputXYArrayObjRef, typeid(DO_XYDataArray));
    AddUseFlagInPort(interpolateToCommonXObjRef, typeid(DO_XYData), interpolateToCommonX);

    outputTableDO.tableData = &outputTableDC;
    AddOutPort(outputTableDO);
    DoStatusChk();
}

DFO_XYArrayToTable& DFO_XYArrayToTable::operator= (const DFO_XYArrayToTable& a)
{
    if (&a != this)
    {
        FuncObjC::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYArrayToTable::LocalCopy(const DFO_XYArrayToTable& a)
{
    inputXYArrayObjRef = a.inputXYArrayObjRef;
    interpolateToCommonXObjRef = a.interpolateToCommonXObjRef;
    interpolateToCommonX = a.interpolateToCommonX;
}


void  DFO_XYArrayToTable:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(inputXYArrayObjRef, "input XY");
    if (interpolateToCommonX)
        CheckInputFO(interpolateToCommonXObjRef, "interpolate input XY");
}

void DFO_XYArrayToTable:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;
    outputTableDC.SetID("XYDataArray");

    const DC_XYDataArray& inputXYArray = *static_cast<DO_XYDataArray*>(GetInPortData(inputXYArrayObjRef))->xyDataArray;
    int ncols = inputXYArray.Size();
    int nrows = inputXYArray[0].Size();
    const SC_DoubleArray*   inputX;
    if (!interpolateToCommonX)
    {
        for (int i = 1; i < ncols; i++)
            if (inputXYArray[i].Size() != nrows)
            {
                SetObjErrMsg("All XY arrays must have the same # of rows");
                return;
            }
        inputX = &inputXYArray[0].xData;
    }
    else
    {
        inputX = &static_cast<DO_XYData*>(GetInPortData(interpolateToCommonXObjRef))->GetXY().xData;
        nrows = inputX->Size(); 
    }

    outputTableDC.Alloc(ncols + 1, nrows);
    outputTableDC.columnDesc += "X";
    outputTableDC.dataTable += *inputX;
    SC_DoubleArray outputY;
    for (int i = 0; i < ncols; i++)
    {
        if (interpolateToCommonX)
        {
            DC_Curve curveDC;
            SC_SetupErr curvErr;    
            if (!curveDC.CreateCurve(inputXYArray[i], curvErr))
            {
                SetObjErrMsg("Error creating curve");
                return;
            }
            curveDC.GetCurveY(*inputX, outputY);
            outputTableDC.dataTable += outputY;
        }
        else
        {
            outputTableDC.dataTable += inputXYArray[i].yData;
        }
        outputTableDC.columnDesc += inputXYArray[i].GetID();
    }
    outputTableDC.SetRowDesc("X");
}

