///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYToXYArray.cpp
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
//      Adds successive XY data to an XY array
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/DO_XYData.h>

#include <objDFO/DFO_XYToXYArray.h>


DFO_XYToXYArray :: DFO_XYToXYArray()
    : FuncObjC("Add XY to XY Array")
{
    xyTableData.SetResizable(10);
    InitCommon();
}

DFO_XYToXYArray::DFO_XYToXYArray(const DFO_XYToXYArray& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYToXYArray :: ~DFO_XYToXYArray ()
{
}

void DFO_XYToXYArray::InitCommon()
{
    AddInPort(xyInputObjRef, typeid(DO_XYData));

    xyDataArrayDO.xyDataArray = &xyTableData;
    AddOutPort(xyDataArrayDO);

    DoStatusChk();
}

DFO_XYToXYArray& DFO_XYToXYArray::operator= (const DFO_XYToXYArray& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_XYToXYArray::LocalCopy(const DFO_XYToXYArray& a)
{
    xyInputObjRef = a.xyInputObjRef;
}



void  DFO_XYToXYArray:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    if (CheckInputFO(xyInputObjRef, "input data source"))
        inputXY = static_cast<DO_XYData*>(GetInPortData(xyInputObjRef))->xyData;
}


void DFO_XYToXYArray:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    //  don't recalc on full calc -- existing table is OK
    if (calcType == foc_Full)
        return;

    // apply clears 
    if (calcType == foc_Apply)
    {
        xyTableData.SetEmpty();
        return;
    }


    // port only adds 
    xyTableData += (*inputXY);

}


