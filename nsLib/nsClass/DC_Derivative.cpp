///////////////////////////////////////////////////////////////////////////////////
//
// DC_Derivative.cpp
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
//      Derived from C_Derivative and adds derivative calculation.
//
///////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <genClass/C_Common.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objClass/DC_Curve.h>

#include <nsClass/DC_Derivative.h>

DC_Derivative::DC_Derivative() :
    derivativeIsCreated(false)
{
}

DC_Derivative::DC_Derivative(const DC_Derivative& a) :
    DerivativeDescription(a)
{
    LocalCopy(a);
}

DC_Derivative::DC_Derivative(const DerivativeDescription& a) :
    DerivativeDescription(a),
    derivativeIsCreated(false)
{
}

DC_Derivative::~DC_Derivative()
{
    // empty
}

DC_Derivative& DC_Derivative::operator= (const DC_Derivative& a)
{
    if (&a != this)
    {
        DerivativeDescription::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

DC_Derivative& DC_Derivative::operator= (const DerivativeDescription& a)
{
    if (&a != this)
        DerivativeDescription::operator=(a);
    return *this;
}

void  DC_Derivative::LocalCopy(const DC_Derivative& a)
{
    derivativeIsCreated   = a.derivativeIsCreated;
}


bool DC_Derivative::CreateDerivative(SC_SetupErr& errData)
{
    derivativeIsCreated = true;
    return derivativeIsCreated;
}

void DC_Derivative::Cleanup()
{
    actualT.DeAlloc();
    processedT.DeAlloc();
    processedY.DeAlloc();
    windowT.DeAlloc();
    windowWork.DeAlloc();
    savedT.DeAlloc();
}

bool DC_Derivative::PrepareInput(const DC_XYData&      inputXY,
                                 const DC_TimeProcess* const timeProcess)
{
    if (!derivativeIsCreated)
        GenAppInternalError("DC_Derivative::PrepareInput-1");
    if (useSuperTime && (timeProcess == 0))
        GenAppInternalError("DC_Derivative::PrepareInput-2");

    Cleanup();
    processErr.ClearErrors();

    DC_XYData inputCopy;
    if (!inputCopy.CreateFrom(inputXY.xData, inputXY.yData))
    {
        processErr.SetConstantError("all null/memory allocation ???");
        return false;
    }
    if (!DerivativeSetupOK(inputCopy.Size(), processErr))
        return false;

    actualT.Alloc(inputCopy.Size());
    processedT.Alloc(inputCopy.Size());
    processedY.Alloc(inputCopy.Size());
    windowT.Alloc(inputCopy.Size());

    bool logX = (derivativeType != dt_dY_dX);
    bool lnX = (derivativeType == dt_dY_dlnX);
    bool logY = (derivativeType == dt_dlog10Y_dlog10X);
    bool windowLog =  (derivativeCalcType == dctLogPctWindow) || (derivativeCalcType == dctLogValWindow);

    for (int i = 0; i < inputCopy.Size(); i++)
    {
        double inputT = inputCopy.xData[i];
        double inputY = inputCopy.yData[i];
        double procT = inputT;

        if (useSuperTime && (!timeProcess->GetProcessedTime(inputT, procT)))
            continue;

        if ((logX || windowLog) && (procT < logEps))
        {
//          std::cout << "X less than log eps "<< procT << "  " << logEps <<std::endl;
            continue;
        }

        if (logY)
        {
            inputY += offsetY;
            if (inputY < logEps)
            {
//              std::cout << "Y less than log eps "<< inputY << "  " << logEps <<std::endl;
                continue;
            }

            inputY = log10(inputY);
        }

        actualT += inputT;
        if (windowLog)
            windowT += log10(procT);
        else
            windowT += procT;

        if (logX)
            if (lnX)
                procT = log(procT);
            else
                procT = log10(procT);

        processedT += procT;
        processedY += inputY;
    }

    if (actualT.IsEmpty())
    {
        processErr.SetConstantError("no data passed processing");
//      std::cout << "input size "<< inputCopy.Size() << std::endl;
    }
    else
        windowWork.Alloc(actualT.Size());

    return !processErr.ErrorFound();
}

bool DC_Derivative::WindowCalc(int      ptIndx,
                               int      stIndx,
                               int      endIndx,
                               double&  dVal)
{
    switch (windowCalcType)
    {
        case Derivative:: wctRegress : {
            windowWork.xData.SetEmpty();
            windowWork.yData.SetEmpty();
            for (int i = stIndx; i <= endIndx; i++)
            {
                windowWork.xData += processedT[i];
                windowWork.yData += processedY[i];
            }
            DC_Curve crvSpec;
            crvSpec.curveType = Curve::ctPolynomial;
            crvSpec.polyOrder = 1;
            if (!crvSpec.CreateCurve(windowWork, processErr))
                return false;
            // second coeff is slope
            dVal = crvSpec.polyCoefficients[1];
            break;
        }
        case Derivative:: wctClark : {
            double t1 = processedT[ptIndx] - processedT[stIndx];
            double t2 = processedT[endIndx] - processedT[ptIndx];
            double X1 = processedY[ptIndx] - processedY[stIndx];
            double X2 = processedY[endIndx] - processedY[ptIndx];
            dVal = (X1 / t1 * t2 + X2 / t2 * t1) / (t1 + t2);
            break;
        }
        case Derivative:: wctSimmons : {
            int midIndx = ptIndx;
            if ((ptIndx == 0) || (ptIndx == (processedT.Size() - 1)))
                midIndx = (stIndx + endIndx) / 2;
            double t1  = processedT[midIndx] - processedT[stIndx];
            double t1_2 = t1 * t1;

            double t2  = processedT[endIndx] - processedT[midIndx];
            double t2_2 = t2 * t2;

            double t12 = processedT[endIndx] - processedT[stIndx];
            double t12_2 = t12 * t12;

            double X1 = processedY[stIndx];
            double X2 = processedY[midIndx];
            double X3 = processedY[endIndx];
            if (ptIndx == 0)
            {
                dVal = ((1.0 - t12_2 / t1_2) * X1 + (t12_2 / t1_2) * X2 - X3) /
                        (t12_2 / t1 - t12);
            }
            else if (ptIndx == (processedT.Size() - 1))
            {
                 dVal = ((1.0 - (t12_2/ t2_2)) * X3 + t12_2 / t2_2 * X2 - X1) /
                            (t12 - t12_2 / t2_2);
            }
            else
            {
                dVal = (t1_2 * X3 + (t2_2 - t1_2) * X2 - t2_2 * X1) /
                            (t1_2 * t2 + t2_2 * t1);
            }
            break;
        }
    }
    return true;
}


bool DC_Derivative::WindowOK(int  ptIndx,
                             int  stIndx,
                             int  endIndx)
{
    bool dataOK = true;
    switch (windowCalcType)
    {
        case Derivative:: wctRegress : {
            break;
        }
        case Derivative:: wctClark : {
            dataOK = (stIndx < ptIndx) && (endIndx > ptIndx);
            break;
        }
        case Derivative:: wctSimmons : {
            if ((ptIndx > 0) && (ptIndx < (processedT.Size() - 1)))
                dataOK = (stIndx < ptIndx) && (endIndx > ptIndx);
            break;
        }
    }
    return dataOK && ((endIndx - stIndx) > 2);
}

void DC_Derivative::LogLinWindowCalc(DC_XYData&      outputXY)
{
    double minT, maxT;
    processedT.CalcMinMax(minT, maxT);
    double delT = maxT - minT;

    double tSpan;
    if ((derivativeCalcType == dctLogValWindow) || (derivativeCalcType == dctLinValWindow))
    {
        tSpan = windowValSpan;
        if (tSpan > delT / 2.0)
        {
            processErr.SetConstantError("actual span > delta T / 2");
            return;
        }
    }
    else
    {
        tSpan = delT * windowPctSpan / 200.0;
        if (tSpan < logEps)
        {
            processErr.SetConstantError("calculated span < log epsilon");
            return;
        }
    }

    for (int i = 0; i < processedT.Size(); i++)
    {

        double tempT = windowT[i];

        int j;
        int stIndx = i;
        int endIndx = i;

        for (j = i - 1; j >= 0; j--)
        {
            delT = fabs(windowT[j] - tempT);
            if (delT > tSpan)
                break;

            stIndx = j;
        }

        for (j = i + 1; j < processedT.Size(); j++)
        {
            delT = fabs(windowT[j] - tempT);
            if (delT > tSpan)
                break;

            endIndx = j;
        }

        if (!WindowOK(i, stIndx, endIndx))
            continue;

        double dVal;
        if (!WindowCalc(i, stIndx, endIndx, dVal))
            return;

        savedT += processedT[i];
        outputXY.xData += actualT[i];
        outputXY.yData += dVal;
    }
}


void DC_Derivative::FixedWindowCalc(DC_XYData&      outputXY)
{
    for (int i = 0; i < processedT.Size(); i++)
    {
        int stIndx = 0;
        if (i > windowPtsSpan)
            stIndx = i - windowPtsSpan;
        int endIndx = i + windowPtsSpan;
        if (endIndx > (processedT.Size() - 1))
            endIndx = processedT.Size() - 1;

        if (!WindowOK(i, stIndx, endIndx))
            continue;

        double dVal;
        if (!WindowCalc(i, stIndx, endIndx, dVal))
            return;

        savedT += processedT[i];
        outputXY.xData += actualT[i];
        outputXY.yData += dVal;
    }
}

void DC_Derivative::TwoSlopeCalc(DC_XYData&      outputXY)
{
    for (int i = windowPtsSpan; i < (processedT.Size() - windowPtsSpan); i++)
    {
        double X1 = processedT[i - windowPtsSpan];
        double X2 = processedT[i];
        double X3 = processedT[i + windowPtsSpan];
        double dX1 = X2 - X1;
        double dX2 = X3 - X2;
        if ((fabs(dX1) < logEps) || (fabs(dX2) < logEps))
            continue;

        double Y1 = processedY[i - windowPtsSpan];
        double Y2 = processedY[i];
        double Y3 = processedY[i + windowPtsSpan];

        double dVal = ((Y3 - Y2) / dX2 + (Y2 - Y1) / dX1) / 2.0;

        savedT += X2;
        outputXY.xData += actualT[i];
        outputXY.yData += dVal;
    }
}

void DC_Derivative::SimpleCalc(DC_XYData&      outputXY)
{
    for (int i = 0; i < (processedT.Size() - 1); i++)
    {
        double X1 = processedT[i];
        double X2 = processedT[i + 1];
        double dX = X2 - X1;
        if (fabs(dX) < logEps)
            continue;

        double Y1 = processedY[i];
        double Y2 = processedY[i + 1];

        double dVal = (Y2 - Y1) / dX;

        savedT += (X1 + X2) / 2.0;
        outputXY.xData += (actualT[i] + actualT[i + 1]) / 2.0;
        outputXY.yData += dVal;
    }
}

bool DC_Derivative::DoCalc(DC_XYData&  outputXY)
{
    outputXY.Alloc(processedT.Size());
    savedT.Alloc(processedT.Size());
    switch (derivativeCalcType)
    {
        case Derivative::dctSimple : {
            SimpleCalc(outputXY);
            break;
        }
        case Derivative::dct2SlopeAverage: {
            TwoSlopeCalc(outputXY);
            break;
        }
        case Derivative::dctFixedWindow : {
            FixedWindowCalc(outputXY);
            break;
        }
        case Derivative::dctLinPctWindow :
        case Derivative::dctLogPctWindow :
        case Derivative::dctLinValWindow :
        case Derivative::dctLogValWindow :
        {
            LogLinWindowCalc(outputXY);
            break;
        }
    }

    if (outputXY.IsEmpty())
        processErr.SetConstantError("no derivatives calculated ??");

    if (processErr.NoErrors() && (timeMultType != Derivative::tmtNone))
    {
        for (int i = 0; i < outputXY.Size(); i++)
            if (timeMultType == Derivative::tmtTime)
                outputXY.yData[i] *= outputXY.xData[i];
            else
                outputXY.yData[i] *= savedT[i];
    }

    if (processErr.NoErrors() && absOutputY)
    {
        for (int i = 0; i < outputXY.Size(); i++)
            outputXY.yData[i] = fabs(outputXY.yData[i]);
    }

    return processErr.NoErrors();
}


bool DC_Derivative::GetDerivatives(const DC_XYData&      inputXY,
                                         DC_XYData&      outputXY,
                                         SC_SetupErr&    errData)
{
    if (PrepareInput(inputXY, 0))
        DoCalc(outputXY);
    errData = processErr;
    Cleanup();
    return !outputXY.IsEmpty();
}

bool DC_Derivative::GetDerivatives(const DC_XYData&      inputXY,
                                   const DC_TimeProcess& timeProcess,
                                         DC_XYData&      outputXY,
                                         SC_SetupErr&    errData)
{
    if (PrepareInput(inputXY, &timeProcess))
        DoCalc(outputXY);
    errData = processErr;
    Cleanup();
    return !outputXY.IsEmpty();
}


