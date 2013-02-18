///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYToCDF.cpp
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

#include <genClass/DO_XYData.h>

#include <genClass/U_Str.h>

#include <objDFO/DFO_XYToCDF.h>


DFO_XYToCDF :: DFO_XYToCDF()
    : FuncObjC("Validate XY as CDF")
{
    xdataIsLog = false;
    InitCommon();
}

DFO_XYToCDF::DFO_XYToCDF(const DFO_XYToCDF& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYToCDF :: ~DFO_XYToCDF ()
{
}

void DFO_XYToCDF::InitCommon()
{
    AddInPort(xyInputObjRef, typeid(DO_XYData));

    outputCDFDC.SetID("CDF");
    outputCDFDO.SetCDF(outputCDFDC);
    AddOutPort(outputCDFDO);

    DoStatusChk();
}

DFO_XYToCDF& DFO_XYToCDF::operator= (const DFO_XYToCDF& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_XYToCDF::LocalCopy(const DFO_XYToCDF& a)
{
    xyInputObjRef = a.xyInputObjRef;
    xdataIsLog = a.xdataIsLog;
}



void  DFO_XYToCDF:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    if (CheckInputFO(xyInputObjRef, "input data source"))
        inputXYDC = static_cast<DO_XYData*>(GetInPortData(xyInputObjRef))->xyData;
}


void DFO_XYToCDF:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;
    
    outputCDFDC.DC_XYData::operator =(*inputXYDC);

    if (xdataIsLog)
        outputCDFDC.SetLog();
    else
        outputCDFDC.SetLinear();


    SC_SetupErr cdfErr;
    if (!outputCDFDC.SetupOK(cdfErr))
    {
        SetObjErrMsg(cdfErr.GetErrorText());
        outputCDFDC.DeAlloc();
    }
    else
    {
        char newID[80];
        MakeString(newID, "CDF(",inputXYDC->GetID(), ")", 80);
        outputCDFDC.SetID(newID);

        MakeString(newID, "CDF(",xyInputObjRef.objRef->GetID(), ")", 80);
        SetID(newID);
    }

}


