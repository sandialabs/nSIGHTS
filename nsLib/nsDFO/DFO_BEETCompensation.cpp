///////////////////////////////////////////////////////////////////////////////////
//
// DFO_BEETCompensation.cpp
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
//      Corrects pressure data based on a response function, barometric pressure
//  data and earth tide data.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Matrix.h>

#include <objClass/DC_Curve.h>

#include <nsDFO/DFO_BEETCompensation.h>

DFO_BEETCompensation :: DFO_BEETCompensation() :
    FuncObjC("BP/ET Compensation")
{
    InitCommon();
}

DFO_BEETCompensation::DFO_BEETCompensation(const DFO_BEETCompensation& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_BEETCompensation :: ~DFO_BEETCompensation ()
{
}

void DFO_BEETCompensation::InitCommon()
{
    AddInPort(responseFuncInputObjRef, typeid(DO_XYResponseFunction));
    AddInPort(pressureInputObjRef, typeid(DO_XYData));
    AddInPort(barometricInputObjRef, typeid(DO_XYData));
    AddInPort(earthTideInputObjRef, typeid(DO_XYData));

    AddOutPort(outputPressureDO);
    outputPressureDO.xyData = &outputPressureDC;

    DoStatusChk();
}

DFO_BEETCompensation& DFO_BEETCompensation::operator= (const DFO_BEETCompensation& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_BEETCompensation::LocalCopy(const DFO_BEETCompensation& a)
{
    responseFuncInputObjRef = a.responseFuncInputObjRef;
    pressureInputObjRef = a.pressureInputObjRef;
    barometricInputObjRef = a.barometricInputObjRef;
    earthTideInputObjRef = a.earthTideInputObjRef;
}

void  DFO_BEETCompensation:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    inputResponseFunctionDC = 0;
    if (CheckInputFO(responseFuncInputObjRef, "response function input source"))
    {
        DO_XYResponseFunction* tempDO = static_cast<DO_XYResponseFunction*>(GetInPortData(responseFuncInputObjRef));
        if (!tempDO->DataOK())
        {
            SetObjErrMsg("response function data not okay");
            return;
        }
        inputResponseFunctionDC = tempDO->GetResponseFunction();
    }

    inputPressureDC = 0;
    if (CheckInputFO(pressureInputObjRef, "test pressure input source"))
        inputPressureDC = static_cast<DO_XYData*>(GetInPortData(pressureInputObjRef))->xyData;

    if (StatusNotOK())
        return;

    double tStart = inputPressureDC->xData[0];
    double tEnd = inputPressureDC->xData.LastIndex();

    inputBarometricDC = 0;
    useBE = inputResponseFunctionDC->useBE;
    if (useBE)
    {
        if (!CheckInputFO(barometricInputObjRef, "barometric pressure input source"))
            return;

        inputBarometricDC = static_cast<DO_XYData*>(GetInPortData(barometricInputObjRef))->xyData;

        if (tStart < inputBarometricDC->xData[0])
            SetObjErrMsg("test pressure start before barometric pressure start");
        if (tEnd > inputBarometricDC->xData.LastIndex())
            SetObjErrMsg("test pressure end after barometric pressure end");
    }

    inputEarthTideDC = 0;
    useET = inputResponseFunctionDC->useET;
    if (useET)
    {
        if (!CheckInputFO(earthTideInputObjRef, "earth tide signal input source"))
            return;

        inputEarthTideDC = static_cast<DO_XYData*>(GetInPortData(earthTideInputObjRef))->xyData;
        if (tStart < inputEarthTideDC->xData[0])
            SetObjErrMsg("test pressure start before earth tide signal start");
        if (tEnd > inputEarthTideDC->xData.LastIndex())
            SetObjErrMsg("test pressure end after earth tide signal end");
    }
    convertType = inputResponseFunctionDC->convertType;

    // change to BETCO local vars for comparison clarity
    m = inputResponseFunctionDC->memory;                // m is "memory"
    timeSpacing = inputResponseFunctionDC->timeStep;

    double ntime  = (tEnd - tStart) / timeSpacing;
    if ((ntime < 10.0) || (ntime > 1.0E5))
        SetObjErrMsg("less than 10 or more than 100000 pressure points");
    else
        n = int(ntime) - 1;

}


void DFO_BEETCompensation:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int offset = 0;

    SC_DoubleArray corBP(n - m);
    if (useBE)
    {
        CalcCorrectionVector(inputBarometricDC, offset, corBP);
        offset = m + 1;
    }
    else
        corBP.FillToSize(0.0);

    SC_DoubleArray corET(n - m);
    if (useET)
    {
        CalcCorrectionVector(inputEarthTideDC, offset, corET);
    }
    else
        corET.FillToSize(0.0);


    //need time with even spacing to get curve for correction vector
    //initial time is time 0 of pressure data - for proper memory lag during correction
    SC_DoubleArray time(n - m);
    time[0] = inputPressureDC->xData[0];
    SC_DoubleArray cor(n - m);
    cor[0] = corBP[0] + corET[0];
    for (int i = 1; i < (n - m); i++)
    {
        time[i] = time[i - 1] + timeSpacing;
        cor[i] = corBP[i] + corET[i];
    }

    DC_Curve corCurve;
    SC_SetupErr curveErr;
    if (!corCurve.CreateCurve(time, cor, curveErr))
    {
        SetObjErrMsg("error converting correction to curve");
        return;
    }

    DC_Curve inputPressureCurve;
    if (!inputPressureCurve.CreateCurve(*inputPressureDC, curveErr))
    {
        SetObjErrMsg("error converting correction to curve");
        return;
    }

    outputPressureDC.DeAlloc();
    if (!outputPressureDC.CreateFrom(inputPressureDC->xData, inputPressureDC->yData))
    {
        SetObjErrMsg("memory allocation ??");
        return;
    }
    SC_DoubleArray& corTime = outputPressureDC.xData;
    SC_DoubleArray& corWL = outputPressureDC.yData;

    // apply correction
    double maxTimeDelay = m * timeSpacing;
    double preLagTime = maxTimeDelay + corTime[0];
    for(int i = 0; i < corTime.Size(); i++)
    {
        double currTime = corTime[i];
        if (currTime <= preLagTime) // set pre-lag
            corWL[i]= corWL[0];
        else
            corWL[i] -= corCurve.GetCurveY(currTime - maxTimeDelay - timeSpacing);
    }
    if(convertType == 1)
    {
        for(int i = 0; i < corTime.Size(); i++)
        {
            corWL[i] = corWL[i] - inputBarometricDC->yData[i];
        }
    }
    else if(convertType == 2)
    {
        for(int i = 0; i < corTime.Size(); i++)
        {
            corWL[i] = corWL[i] + inputBarometricDC->yData[i];
        }
    }
    outputPressureDC.SetID(GetID());
}

void DFO_BEETCompensation::CalcCorrectionVector(const DC_XYData* inputData,
                                                const int        offset,
                                                SC_DoubleArray&  cor)
{
    // based on/extracted from BETCO code from Nate Toll
    // modified to use GenLib arrays and changed to all 0 based

    SC_SetupErr curveErr;
    DC_Curve dataCurve;
    if (!dataCurve.CreateCurve(*inputData, curveErr))
    {
        SetObjErrMsg("error converting data to curve");
        return;
    }

    SC_DoubleArray delta(n);

    // sets array values to differenced data (set up for Eqn 23 & 24)
    double time = inputPressureDC->xData[0];
    for(int i = 0; i < n; i++)
    {
        delta[i] = dataCurve.GetCurveY(time + timeSpacing) - dataCurve.GetCurveY(time);
        time += timeSpacing;
    }

    //set up X matrix and u vector
    SC_DoubleArray partialU(m + 1);
    SC_DoubleMatrix x;
    x.FixedMatrixAllocAndSetSize(n - m, m + 1);

    //populate vector (Eqn 21 and 22)
    for(int i = 0; i < (m + 1); i++)
    {
        partialU[i]= inputResponseFunctionDC->u[i + offset];
    }

    //populate matrix (Eqn 23 and 24)
    for(int i = 0; i < (n-m); i++)
    {
        for(int k = 0; k < (m+1); k++)
        {
            x[i][k]= delta[m + i - k];
        }
    }

    // calculate correction vector (cumulative sum of ux in Eqn 27 and 28)
    double sum = 0.0;
    for (int k = 0; k < (m + 1); k++)
    {
        sum += x[0][k] * partialU[k];
    }
    cor[0] = sum;

    for (int i = 1; i < (n - m); i++)
    {
        sum = 0.0;
        for (int k = 0; k < (m + 1); k++)
        {
            sum += x[i][k] * partialU[k];
        }
        cor[i] = cor[i - 1] + sum;
    }

}

