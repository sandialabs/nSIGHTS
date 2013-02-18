///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableToReal.cpp
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
//      Extracts a single value from a selected table column based upon a criteria
//  (min, max, first, last, specific row)
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <genClass/U_Str.h>

#include <objDFO/DFO_TableToReal.h>

DFO_TableToReal :: DFO_TableToReal() :
    FuncObjC("Extract Real from Table")
{
    extractOp = eonRows;
    selectedIndex = 0;
    InitCommon();
}

DFO_TableToReal::DFO_TableToReal(const DFO_TableToReal& a) :
    FuncObjC(a),
    DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableToReal :: ~DFO_TableToReal ()
{
}

void DFO_TableToReal::InitCommon()
{
    DSC_TableColSel::AddPort(*this);
    AddOutPort(extractOutputDO);
    DoStatusChk();
}



DFO_TableToReal& DFO_TableToReal::operator= (const DFO_TableToReal& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableToReal::LocalCopy(const DFO_TableToReal& a)
{
    extractOp       = a.extractOp;
    selectedIndex   = a.selectedIndex;
}


void  DFO_TableToReal:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    extractOutputDO.Reset("TableToReal");
    DSC_TableColSel::DoCheck(*this);
    if (StatusOK())
    {
        if ((extractOp == eoSelectedIndex) && inputData->dataTable[0].Size() <= selectedIndex)
            SetObjErrMsg("row index out of range");
        if (inputData->dataTable[0].IsEmpty())
            SetObjErrMsg("no data in input table");
    }
}

void DFO_TableToReal:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;

    const SC_DoubleArray& colData = *selectedColData;

    double min, max;
    if ((extractOp ==  eoMin) || (extractOp ==  eoMax))
        colData.CalcMinMax(min, max);

    double outVal;
    char* extraStr = " ";
    switch ( extractOp)
    {
        case  eoMin : {
            outVal = min;
            extraStr = " minimum";
            break;
        }
        case  eoMax : {
            outVal = max;
            extraStr = " maximum";
            break;
        }
        case  eonRows : {
            outVal = double(colData.Size());
            extraStr = " count";
            break;
        }
        case  eoLastIndex : {
            outVal = colData.LastIndex();
            break;
        }
        case  eoSelectedIndex : {
            outVal = colData[selectedIndex];
            break;
        }
    }

    char descStr[40];
    if (inputData->columnDesc.Size() > selectedCol)
        CopyString(descStr, inputData->columnDesc[selectedCol], 40);
    else
        CopyString(descStr, "Result" , 40);
    ConcatString(descStr, extraStr, 40);

    extractOutputDO.Reset(descStr);
    extractOutputDO.realVal = outVal;
    extractOutputDO.SetValueLabel(outVal);

}

