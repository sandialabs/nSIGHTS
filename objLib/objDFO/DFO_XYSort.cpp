///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYSort.cpp
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

#include <objDFO/DFO_XYSort.h>

DFO_XYSort :: DFO_XYSort() :
    FuncObjC("Sort XY")
{
    sortAscending = true;
    InitCommon();
}

DFO_XYSort::DFO_XYSort(const DFO_XYSort& a) :
    FuncObjC(a),
    DSC_XYColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYSort :: ~DFO_XYSort ()
{
}

void DFO_XYSort::InitCommon()
{
    DSC_XYColSel::AddPort(*this);
    sortedXYDO.xyData = &sortedXYDC;

    AddOutPort(sortedXYDO);

    DoStatusChk();
}

DFO_XYSort& DFO_XYSort::operator= (const DFO_XYSort& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        DSC_XYColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYSort::LocalCopy(const DFO_XYSort& a)
{
    sortAscending = a.sortAscending;
}


void  DFO_XYSort:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    DSC_XYColSel::DoCheck(*this);
    if (StatusNotOK())
    {
        sortedXYDO.xyData = 0;
        sortedXYDC.SetEmpty();
    }

}

void DFO_XYSort:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    sortedXYDC = *inputData;
    sortedXYDO.xyData = &sortedXYDC;

    SC_IntArray sortIndexes;
    int nRows = sortedXYDC.xData.GetnDataOK();
    sortIndexes.AllocAndIndex(nRows);

    if (operateOnX)
    {
        sortedXYDC.xData.Sort(sortAscending, sortIndexes);
        const SC_DoubleArray& dataIn = inputData->yData;
        for (int j = 0; j < nRows; j++)
            sortedXYDC.yData[j] = dataIn[sortIndexes[j]];
    }
    else
    {
        sortedXYDC.yData.Sort(sortAscending, sortIndexes);
        const SC_DoubleArray& dataIn = inputData->xData;
        for (int j = 0; j < nRows; j++)
            sortedXYDC.xData[j] = dataIn[sortIndexes[j]];
    }

    /*const SC_StringArray& inRowDesc = inputData->rowDesc;
    for (int j = 0; j < sortedTableDC.GetNRows(); j++)
    {
        int inputRow = sortIndexes[j];
        if (inputRow < inRowDesc.Size())
            sortedTableDC.rowDesc.SetString(inRowDesc[inputRow], j);
        else
        {
            char tempStr[80];
            ConcatInt(inputRow, "I", tempStr, 4, 80);
            sortedTableDC.rowDesc.SetString(tempStr, j);
        }
    }*/


}

