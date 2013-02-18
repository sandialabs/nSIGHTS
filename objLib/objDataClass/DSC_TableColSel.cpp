///////////////////////////////////////////////////////////////////////////////////
//
// DSC_TableColSel.cpp
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

#include <genClass/DO_TableData.h>

#include <objDataClass/DSC_TableColSel.h>


DSC_TableColSel::DSC_TableColSel()
{
    selectedCol = 0;
}

DSC_TableColSel::DSC_TableColSel(const DSC_TableColSel& a) :
    tableInputObjRef(a.tableInputObjRef),
    selectedCol(a.selectedCol)
{

}


DSC_TableColSel& DSC_TableColSel::operator= (const DSC_TableColSel& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void DSC_TableColSel::LocalCopy(const DSC_TableColSel& a)
{
    selectedCol      = a.selectedCol;
    tableInputObjRef = a.tableInputObjRef;
}

void  DSC_TableColSel::DoCheck(FuncObjC&  specFO)
{
    if (specFO.CheckInputFO(tableInputObjRef, "table input data source"))
    {
        inputData = static_cast<DO_TableData*>(specFO.GetInPortData(tableInputObjRef))->tableData;
        if (inputData->dataTable.Size() <= selectedCol)
            specFO.SetObjErrMsg("column index out of range");
        else
            selectedColData = &inputData->dataTable[selectedCol];
    }
}

void DSC_TableColSel::AddPort(FuncObjC&   specFO)
{
    specFO.AddInPort(tableInputObjRef, typeid(DO_TableData));
}


