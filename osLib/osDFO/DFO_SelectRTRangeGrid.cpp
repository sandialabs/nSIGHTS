///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectRTRangeGrid.cpp
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
//      For calculating fits, and for reading, selecting, and operating on optimizer results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/SC_DoubleArray.h>

#include <osClass/DO_GridArray.h>

#include <osMain/G_Range.h>

#include <osDFO/DFO_SelectRTRangeGrid.h>

RangeOutputFO*  DFO_SelectRTRangeGrid::defaultFO = 0;

DFO_SelectRTRangeGrid :: DFO_SelectRTRangeGrid() :
    DFO_SelectBase("SelectRTGrid", baseMS)
{
    InitCommon();
}

DFO_SelectRTRangeGrid::DFO_SelectRTRangeGrid(const DFO_SelectRTRangeGrid& a) :
    DFO_SelectBase(a, baseMS)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectRTRangeGrid::InitCommon()
{
    InitMSPorts();

    AddOutPort(gridDataDO);
    AddInPort(arrayInputObjRef, typeid(DO_GridArray));
    arrayInputObjRef = FuncObjRef(*defaultFO);
    DoStatusChk();
}

void DFO_SelectRTRangeGrid::SetDefaultRangeFO(RangeOutputFO& defFO)
{
    defaultFO = &defFO;
}

DFO_SelectRTRangeGrid& DFO_SelectRTRangeGrid::operator= (const DFO_SelectRTRangeGrid& a)
{
    if (&a != this)
    {
        DFO_SelectBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectRTRangeGrid::LocalCopy(const DFO_SelectRTRangeGrid& a)
{
    arrayInputObjRef   = a.arrayInputObjRef;
}

void  DFO_SelectRTRangeGrid:: DoStatusChk()
{
    gridDataDO.gridData = 0;
    DFO_SelectBase::DoStatusChk();
    indexMS.SetTypeLabel("Selected Grid");

    if (!CheckInputFO(arrayInputObjRef, "Data source"))
        return;

    if (defaultFO->doingCubes)
    {
        SetObjErrMsg("not doing range grids: doing cubes");
        return;
    }

    arrayInputDO = static_cast<DO_GridArray*>(GetInPortData(arrayInputObjRef));

    if (IndexRangeOK(arrayInputDO->gridArray->Size()))
    {
        int index = indexMS.GetCurrentIndex();
        gridDataDO.gridData = &((*arrayInputDO->gridArray)[index]);
        indexMS.indexDO.SetValueLabel(gridDataDO.gridData->gridID);
    }

}

void DFO_SelectRTRangeGrid:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
        SetDefaultID(gridDataDO.gridData->gridID);
}

