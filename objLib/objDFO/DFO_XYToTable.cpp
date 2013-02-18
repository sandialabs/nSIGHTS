///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYToTable.cpp
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

#include <objDFO/DFO_XYToTable.h>

DFO_XYToTable :: DFO_XYToTable() :
    FuncObjC("Convert XY to Table")
{
    InitCommon();
}

DFO_XYToTable::DFO_XYToTable(const DFO_XYToTable& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYToTable :: ~DFO_XYToTable ()
{
}

void DFO_XYToTable::InitCommon()
{
    AddInPort(inputXYObjRef, typeid(DO_XYData));

    outputTableDO.tableData = &outputTableDC;
    AddOutPort(outputTableDO);
    DoStatusChk();
}

DFO_XYToTable& DFO_XYToTable::operator= (const DFO_XYToTable& a)
{
    if (&a != this)
    {
        FuncObjC::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYToTable::LocalCopy(const DFO_XYToTable& a)
{
    inputXYObjRef = a.inputXYObjRef;
}


void  DFO_XYToTable:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(inputXYObjRef, "input XY");
}

void DFO_XYToTable:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;
    outputTableDC.SetID("XYData");

    const DC_XYData& inputXY = static_cast<DO_XYData*>(GetInPortData(inputXYObjRef))->GetXY();

    outputTableDC.Alloc(2, inputXY.Size());
    outputTableDC.columnDesc += "X";
    outputTableDC.columnDesc += "Y";
    outputTableDC.dataTable += inputXY.xData;
    outputTableDC.dataTable += inputXY.yData;
    outputTableDC.SetRowDesc("XY"); 

}

