///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectXY.cpp
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
#include <genClass/SC_DoubleArray.h>

#include <objClass/DO_XYDataArray.h>

#include <objDFO/DFO_SelectXY.h>


DFO_SelectXY :: DFO_SelectXY() :
    DFO_SelectBase("SelectXY", baseMS)
{
    InitCommon();
}

DFO_SelectXY::DFO_SelectXY(const DFO_SelectXY& a) :
    DFO_SelectBase(a, baseMS)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectXY::InitCommon()
{
    InitMSPorts();

    xyDataDO.xyData = &xyData;
    AddOutPort(xyDataDO);
    AddInPort(xyDataArrayObjRef, typeid(DO_XYDataArray));

    DoStatusChk();
}

DFO_SelectXY& DFO_SelectXY::operator= (const DFO_SelectXY& a)
{
    if (&a != this)
    {
        DFO_SelectBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectXY::LocalCopy(const DFO_SelectXY& a)
{
    xyDataArrayObjRef   = a.xyDataArrayObjRef;
}

void  DFO_SelectXY:: DoStatusChk()
{
    DFO_SelectBase::DoStatusChk();
    indexMS.SetTypeLabel("Selected XY");

    if (!CheckInputFO(xyDataArrayObjRef, "Data source"))
        return;

    dataDO = static_cast<DO_XYDataArray*>(GetInPortData(xyDataArrayObjRef));

    if (IndexRangeOK(dataDO->xyDataArray->Size()))
    {
        indexMS.indexDO.SetValueLabel((*dataDO->xyDataArray)[indexMS.GetCurrentIndex()].dataID);
    }

}

void DFO_SelectXY:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int index = indexMS.GetCurrentIndex();

    DC_XYData& currXY = (*dataDO->xyDataArray)[index];

    if (! xyData.CreateFrom(currXY.xData, currXY.yData))
    {
        SetObjErrMsg("all input null or memory allocation ??");
        return;
    }

    CopyString(xyData.dataID, currXY.dataID, DC_XYData::dataIDLen);
    SetDefaultID(xyData.dataID);
}

