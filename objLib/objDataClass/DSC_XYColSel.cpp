///////////////////////////////////////////////////////////////////////////////////
//
// DSC_XYColSel.cpp
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

#include <objDataClass/DSC_XYColSel.h>


DSC_XYColSel::DSC_XYColSel()
{
    operateOnX = false;
}

DSC_XYColSel::DSC_XYColSel(const DSC_XYColSel& a) :
    xyInputObjRef(a.xyInputObjRef),
    operateOnX(a.operateOnX)
{

}


DSC_XYColSel& DSC_XYColSel::operator= (const DSC_XYColSel& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void DSC_XYColSel::LocalCopy(const DSC_XYColSel& a)
{
    operateOnX    = a.operateOnX;
    xyInputObjRef = a.xyInputObjRef;
}

void  DSC_XYColSel::DoCheck(FuncObjC&  specFO)
{
    if (specFO.CheckInputFO(xyInputObjRef, "XY input data source"))
    {
        inputData = static_cast<DO_XYData*>(specFO.GetInPortData(xyInputObjRef))->xyData;
        if (operateOnX)
        {
            selectedColData = &inputData->xData;
        }
        else
        {
            selectedColData = &inputData->yData;
        }

    }
}

void DSC_XYColSel::AddPort(FuncObjC&   specFO)
{
    specFO.AddInPort(xyInputObjRef, typeid(DO_XYData));
}


