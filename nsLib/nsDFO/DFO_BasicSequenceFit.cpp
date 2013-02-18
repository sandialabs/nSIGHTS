///////////////////////////////////////////////////////////////////////////////////
//
// DFO_BasicSequenceFit.cpp
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
//      Calculates fit metric for one or more sequences.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objClass/DC_Curve.h>
#include <nsClass/DO_SequenceTimes.h>

#include <nsDFO/DFO_BasicSequenceFit.h>

DFO_BasicSequenceFit :: DFO_BasicSequenceFit() : DFO_SingleFitBase("SequenceFit")
{
    InitCommon();
}

DFO_BasicSequenceFit::DFO_BasicSequenceFit(const DFO_BasicSequenceFit& a) : DFO_SingleFitBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_BasicSequenceFit :: ~DFO_BasicSequenceFit ()
{
}

void DFO_BasicSequenceFit::InitCommon()
{
    AddInPort(xyInputObjRef, typeid(DO_XYData));
    timeLimits.AddPort(*this);

    outputXYDO.xyData = &outputXY;
    AddOutPort(outputXYDO);

    DoStatusChk();
}

DFO_BasicSequenceFit& DFO_BasicSequenceFit::operator= (const DFO_BasicSequenceFit& a)
{
    if (&a != this)
    {
        DFO_SingleFitBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_BasicSequenceFit::LocalCopy(const DFO_BasicSequenceFit& a)
{
    xyInputObjRef = a.xyInputObjRef;
    timeLimits    = a.timeLimits;
    interpPoints  = a.interpPoints;
}

void DFO_BasicSequenceFit::DoAllocCheck()
{
    if (CheckInputFO(xyInputObjRef, "field data source"))
    {
        inputXY = static_cast<DO_XYData*>(GetInPortData(xyInputObjRef))->xyData;

        maxPts = inputXY->Size();
        if (!interpPoints.AtInputPoints())
            maxPts = interpPoints.numInterpPoints;

        fieldDataDC = &outputXY;

        //  allocation is OK as long as input is set
        fitResultsDC.nfitAlloc = maxPts;
        fitResultsDC.okToAlloc = (maxPts > 2);
    }
}

void  DFO_BasicSequenceFit:: DoStatusChk()
{
    timeLimits.DoPreCheck();

    DFO_SingleFitBase::DoStatusChk();
    if (StatusNotOK())
        return;

    timeLimits.DoCheck(*this);

    if (timeLimits.AllData()  && (!interpPoints.AtInputPoints()))
        inputXY->xData.CalcMinMax(timeLimits.timeMin, timeLimits.timeMax);

    if (!interpPoints.AtInputPoints())
    {
        if (interpPoints.useDataLimits)
            interpPoints.SetMinMax(timeLimits.timeMin, timeLimits.timeMax);

        SC_SetupErr interpErr;
        if (!interpPoints.InterpSetupOK(interpErr))
            SetObjErrMsg(interpErr.GetErrorText());
    }
}


void DFO_BasicSequenceFit:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    // working copy
    DC_XYData tempXY;

    if (!(outputXY.Alloc(maxPts) && tempXY.Alloc(inputXY->Size())))
    {
        SetObjErrMsg("allocation ??");
        return;
    }

    tempXY = *inputXY;
    timeLimits.ExtractLimits(tempXY);

    if (tempXY.Size() < 2)
    {
        SetObjErrMsg("no data in range");
        return;
    }

    if (interpPoints.AtInputPoints())
    {
        outputXY = tempXY;
    }
    else
    {
        DC_Curve tempCrv;
        tempCrv.curveType = Curve::ctLinear;

        SC_SetupErr crvErr;
        if (!tempCrv.CreateCurve(tempXY, crvErr))
        {
            SetObjErrMsg(crvErr.GetErrorText());
            return;
        }

        interpPoints.GetInterpPoints(outputXY);
        tempCrv.GetCurveY(outputXY);
    }

    // final checks before fit calc
    if (fieldDataDC->Size() < 2)
        SetObjErrMsg("insufficient field data data points");
    else if (fieldDataDC->xData[0] < simResultsDC->xData[0])
        SetObjErrMsg("field data start before simulation");
    else if (fieldDataDC->xData.LastIndex() > simResultsDC->xData.LastIndex())
        SetObjErrMsg("field data end after simulation");
    else
        SetFieldInfo();

    if (StatusOK())
        DFO_SingleFitBase::CalcOutput(calcType);

}

