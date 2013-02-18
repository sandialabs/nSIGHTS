///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SingleFit.cpp
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
//      calculates fit metric for simulated and field data.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <math.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osDFO/DFO_SingleFit.h>


DFO_SingleFit :: DFO_SingleFit() : DFO_SingleFitBase("SingleFit")
{
    limitInput = false;
    limitMin   = 0.0;
    limitMax   = 1.0;

    InitCommon();
}

DFO_SingleFit::DFO_SingleFit(const DFO_SingleFit& a) : DFO_SingleFitBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_SingleFit :: ~DFO_SingleFit ()
{
}

void DFO_SingleFit::InitCommon()
{
    AddInPort(fieldDataObjRef, typeid(DO_XYData));

    limitedXY.SetID("Limited data");
    limitedXYDO.xyData = &limitedXY;
    AddOutPort(limitedXYDO);

    DoStatusChk();
}

DFO_SingleFit& DFO_SingleFit::operator= (const DFO_SingleFit& a)
{
    if (&a != this)
    {
        DFO_SingleFitBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SingleFit::LocalCopy(const DFO_SingleFit& a)
{
    fieldDataObjRef  = a.fieldDataObjRef;
    limitInput = a.limitInput;
    limitMin   = a.limitMin;
    limitMax   = a.limitMax;
}

void DFO_SingleFit::DoAllocCheck()
{
    if (CheckInputFO(fieldDataObjRef, "field data source"))
    {
        inputXY = static_cast<DO_XYData*>(GetInPortData(fieldDataObjRef))->xyData;

        if (limitInput)
        {
            fieldDataDC = &limitedXY;
        }
        else
        {
            fieldDataDC = inputXY;
        }

        //  allocation is OK as long as input is set
        fitResultsDC.nfitAlloc = inputXY->Size();
        //fitResultsDC.okToAlloc = (fitResultsDC.nfitAlloc > 2);
        fitResultsDC.okToAlloc = (fitResultsDC.nfitAlloc > 0);
    }
/*  else
    {
        if (fieldDataObjRef.objRef != 0)
        {
            char debugMsg[120];
            MakeString(debugMsg, "DfO_SingleFit ****", fieldDataObjRef.objRef->GetID(), "::", fieldDataObjRef.objRef->GetStatusMsg(), 120);
            SetObjErrMsg(debugMsg);
        }
    }
*/

}

void  DFO_SingleFit:: DoStatusChk()
{
    DFO_SingleFitBase::DoStatusChk();
    if (StatusNotOK())
        return;

    // basic checks
    if (inputXY->Size() < 1) //2)
        SetObjErrMsg("insufficient input field data points");
}


void DFO_SingleFit:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (limitInput)
    {
        int ninput = inputXY->Size();
        if (!limitedXY.Alloc(ninput))
        {
            SetObjErrMsg("limited XY allocation ??");
            return;
        }

        int nOK = 0;
        for (int i = 0; i < ninput; i++)
        {
            double xVal = inputXY->xData[i];
            if ((xVal >= limitMin) && (xVal <= limitMax))
            {
                limitedXY.xData[nOK] = xVal;
                limitedXY.yData[nOK++] = inputXY->yData[i];
            }
        }
        limitedXY.SetSize(nOK);

        if (nOK < 1) //2)
        {
            SetObjErrMsg("insufficient points after limiting");
            return;
        }
        SetFieldInfo();
    }

    DFO_SingleFitBase::CalcOutput(calcType);

}

