///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableToXY.cpp
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
//      Converts two columns in a table to an XY (time series) data type
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <genClass/U_Str.h>

#include <objDFO/DFO_TableToXY.h>

DFO_TableToXY :: DFO_TableToXY() :
    FuncObjC("Convert Table to XY")
{
    outputDesc[0] = '\0';
    selectedYCol = 0;
    InitCommon();
}

DFO_TableToXY::DFO_TableToXY(const DFO_TableToXY& a) :
    FuncObjC(a),
    DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableToXY :: ~DFO_TableToXY ()
{
}

void DFO_TableToXY::InitCommon()
{
    outputXYDO.xyData = &outputXY;
    DSC_TableColSel::AddPort(*this);
    AddOutPort(outputXYDO);
    DoStatusChk();
}

DFO_TableToXY& DFO_TableToXY::operator= (const DFO_TableToXY& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableToXY::LocalCopy(const DFO_TableToXY& a)
{
    CopyString(outputDesc, a.outputDesc, outputDescLen);
    selectedYCol   = a.selectedYCol;
}


void  DFO_TableToXY:: DoStatusChk()
{
    outputXY.SetID("XColYCol");
    FuncObjC::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
    if (StatusOK())
    {
        if (inputData->dataTable.Size() <= selectedYCol)
            SetObjErrMsg("Y column index out of range");
    }
}

void DFO_TableToXY:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;

    outputXY.CreateFrom(*selectedColData, inputData->dataTable[selectedYCol]);
    outputXY.CleanUp();
    if (outputXY.IsEmpty())
    {
        SetObjErrMsg("Results are all null");
        outputXY.DeAlloc();
    }

    SetDefaultID(inputData->columnDesc[selectedYCol]);

    if (StringLength(outputDesc))
        outputXY.SetID(outputDesc);
    else
        outputXY.SetID(GetID());

}

