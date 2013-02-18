///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ViewTable.cpp
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

#include <objDFO/DFO_ViewTable.h>

DFO_ViewTable :: DFO_ViewTable() :
    FuncObjC("View Table")
{
    InitCommon();
}

DFO_ViewTable::DFO_ViewTable(const DFO_ViewTable& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ViewTable::InitCommon()
{
    AddInPort(inputTableObjRef, typeid(DO_TableData));
    AddOutPort(tableDataDO);
    DoStatusChk();
}

DFO_ViewTable& DFO_ViewTable::operator= (const DFO_ViewTable& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ViewTable::LocalCopy(const DFO_ViewTable& a)
{
    inputTableObjRef = a.inputTableObjRef;
    tableFormat      = a.tableFormat;
}

void  DFO_ViewTable:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (CheckInputFO(inputTableObjRef, "table input data source"))
        tableDataDO = *(static_cast<DO_TableData*>(GetInPortData(inputTableObjRef)));


}

void DFO_ViewTable:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        tableDataDO = DO_TableData();

}

