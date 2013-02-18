///////////////////////////////////////////////////////////////////////////////////
//
// DFO_BasicResidual.cpp
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
//      basic residual operations (standardize, sort).
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osClass/U_Resid.h>

#include <osDFO/DFO_BasicResidual.h>

DFO_BasicResidual :: DFO_BasicResidual() : FuncObjC("BasicResidual")
{
    sortingSpec = brNoSort;
    standardize = false;
    xoutputIsIndex = true;
    InitCommon();
}

DFO_BasicResidual::DFO_BasicResidual(const DFO_BasicResidual& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_BasicResidual :: ~DFO_BasicResidual ()
{
}

void DFO_BasicResidual::InitCommon()
{
    AddInPort(residInputObjRef, typeid(DO_ResidData));

    outputXYDO.xyData = &outputXY;
    AddOutPort(outputXYDO);

    DoStatusChk();
}

DFO_BasicResidual& DFO_BasicResidual::operator= (const DFO_BasicResidual& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_BasicResidual::LocalCopy(const DFO_BasicResidual& a)
{
    residInputObjRef    = a.residInputObjRef;
    sortingSpec         = a.sortingSpec;
    standardize         = a.standardize;
    xoutputIsIndex      = a.xoutputIsIndex;
}


void  DFO_BasicResidual:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (CheckInputFO(residInputObjRef, "input data source"))
        inputXY = static_cast<DO_ResidData*>(GetInPortData(residInputObjRef))->xyData;
}


void DFO_BasicResidual:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    outputXY.CreateFrom(inputXY->xData, inputXY->yData);

    if (standardize)
        StandardizeResiduals(outputXY.yData);

    bool sort = sortingSpec != brNoSort;
    if (sort)
        outputXY.yData.Sort(sortingSpec == brSortUp);

    if (sort || xoutputIsIndex)
        for (int i = 0; i < outputXY.xData.Size(); i++)
            outputXY.xData[i] = double(i + 1);

    outputXY.SetID("Residual");
}


