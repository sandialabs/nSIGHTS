///////////////////////////////////////////////////////////////////////////////////
//
// PC_Axes.cpp
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
//      basic axes definition and derived class for Lin/Log axes.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/U_Real.h>

#include <genPlotClass/PC_Axes.h>

const double PC_Axes :: rAxesEps = 1.0E-30;


PC_Axes :: PC_Axes()
{
    axesAreLinear   = true;
    autoScaleAxes   = true;
    axisMajorIncs   = aitTic;;
    autoMajorIncs   = true;
    axisMinorIncs   = aitTic;
    autoMinorIncs   = true;
    startAtFirst    = false;
}

PC_Axes :: PC_Axes(const PC_Axes& a)
{
    LocalCopy(a);
}


PC_Axes& PC_Axes::operator= (const PC_Axes& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}

void  PC_Axes::FullCopy(const PC_Axes& a)
{
        LocalCopy(a);
}

void  PC_Axes::LocalCopy(const PC_Axes& a)
{
    axesAreLinear   = a.axesAreLinear;
    autoScaleAxes   = a.autoScaleAxes;
    axisMajorIncs   = a.axisMajorIncs;
    autoMajorIncs   = a.autoMajorIncs;
    axisMinorIncs   = a.axisMinorIncs;
    autoMinorIncs   = a.autoMinorIncs;
}




bool PC_Axes::StartIsMajor(const double&  stVal,
                           const double&  majIncVal)
{
    if ( axesAreLinear)
    {
        double nrem =  fmod(fabs(stVal), majIncVal);
        if ((nrem <= (fabs(stVal) / 1.0E+06)) || (nrem < rAxesEps))
            return true;
    }
    else
    {
        // is stVal a log cycle ?
        double logSt = log10(stVal);
        if ( fabs(floor(logSt) - logSt) < 1.0E-03)
            return true;

        if ( fabs(ceil(logSt) - logSt) < 1.0E-03)
            return true;
    }
    return false;
}

void PC_Axes::MakeDifferent(double& axStart,
                            double& axEnd,
                            bool    logAxes)
{
    if ( fabs(axStart - axEnd) > rAxesEps)
        return;

    if ( logAxes)
        if ( axStart < rAxesEps)
        {
            axStart = 0.1;
            axEnd = 1.0;
        }
        else
        {
            axStart = axStart / 10.0;
            axEnd = axEnd * 10.0;
        }
    else
        if (fabs(axStart) < rAxesEps)
        {
            axStart = -1.0;
            axEnd = 1.0;
        }
        else
        {
            axStart = axStart * 0.90;
            axEnd = axEnd * 1.10;
        }
}

//  axes auto adjust
//  this code is a direct translate from GPlib pascal routines

bool PC_Axes::InRange(const double&   axStVal,
                    const double&   axEndVal,
                    bool            reversed,
                    const double&   tVal)
{
    if (reversed)
        return (tVal <= (axStVal + rAxesEps)) && (tVal >= (axEndVal - rAxesEps));
    else
        return (tVal >= (axStVal - rAxesEps)) && (tVal <= (axEndVal + rAxesEps));
}


void PC_Axes::LinFirstInc(const double&   axStVal,
                          const double&   axEndVal,
                          const double&   axIncVal,
                          double&         firstInc)
{
    firstInc = axStVal;
    if (startAtFirst && (!autoMajorIncs))
        return;

    double incVal = fabs(axIncVal);
    double temp  = axStVal / incVal;
    if (temp > 10000.0)
        return;

    bool reversed = (axStVal > axEndVal);
    temp = floor(temp) * incVal;

    if ((axStVal < 0.0) && (!reversed))
          temp = temp - incVal;
    if ((axStVal >= 0.0) && reversed)
          temp = temp + incVal;

    if (InRange(axStVal, axEndVal, reversed, temp))
        firstInc = temp;
    else
        firstInc = temp + axIncVal;
}


void PC_Axes::SetIncVal(const double& setVal,
                        bool          reversed,
                        double&       axIncVal)
{
    if (reversed)
        axIncVal = - setVal;
    else
        axIncVal = setVal;
}

bool PC_Axes::AutoLinIncs(const double&   axStVal,
                          const double&   axEndVal,
                          double&         firstInc,
                          double&         axIncVal)
{
    const int nIncs = 4;
    const double incVals[] = {1.0, 2.0, 2.5, 5.0};

    firstInc = axStVal;
    double delVal = fabs(axStVal - axEndVal);
    bool reversed = (axStVal > axEndVal);

    if ( delVal < rAxesEps)
    {
        SetIncVal(1.0, reversed, axIncVal);
        return false;
    }

    SetIncVal(delVal / 5.0, reversed, axIncVal);

    double logInt   = floor(log10(delVal)) - 4.0;
    double multVal  = pow(10.0, logInt);

    bool    okFound = false;
    bool    resOK[nIncs];
    double  resVals[nIncs];
    do
    {
        multVal = multVal * 10.0;
        for (int j = 0; j < nIncs; j++)
        {
            resOK[j]   = false;
            resVals[j] = delVal / (incVals[j] * multVal);

            if ((resVals[j] < 9.0) && (resVals[j] >= 3.0))
            {
                resOK[j] = true;
                okFound = true;
            }
        }
    } while ((!okFound) && (multVal < 1.0E+06));

    if (!okFound)
        return false;

    int bestInc = 0;
    int incWrds[nIncs];
    for (int j = 0; j < nIncs; j++)
    {
        if (resOK[j])
            incWrds[j] = int(floor(resVals[j]));
        else
            incWrds[j] = 0;
        if ((incWrds[j] == 4) || (incWrds[j] == 5))
            bestInc = j;
    }

    if (bestInc == 0)
        for (int j = 0; j < nIncs; j++)
            if (incWrds[j] > 0)
                bestInc = j;

    double incVal = incVals[bestInc] * multVal;
    SetIncVal(incVal, reversed, axIncVal);

    LinFirstInc(axStVal, axEndVal, axIncVal, firstInc);
    return true;
}

void PC_Axes::LinAdjust(const double& firstInc,
                        const double& incVal,
                              double& minVal,
                             double& maxVal)
{
    if (fabs(minVal - firstInc) > rAxesEps)
        minVal = firstInc - incVal;
    double temp = minVal;
    for ( int j = 0; j < 10; j++)
    {
        temp += incVal;
        if (((maxVal - temp) < rAxesEps) || (temp > maxVal))
        {
            maxVal = temp;
            break;
        }
    }
}


void PC_Axes::SetLinearMajorIncs(const double& axMin,
                                 const double& axMax,
                                 const double& incVal,
                                       SC_DoubleArray& incVals)
{
    double currInc;
    LinFirstInc(axMin, axMax, incVal, currInc);
    incVals.Alloc(maxIncAlloc);
    bool reversed = axMax < axMin;
    bool incsComplete;
    do
    {
        incVals += currInc;
        if (incVals.IsFull())
            return;

        currInc += incVal;

        // avoid small near zero in middle of axes
        if (fabs(currInc) < rAxesEps)
            currInc = 0.0;
        else
        {
            if (fabs(incVal) / fabs(currInc) > 1.0E05)
                currInc = 0.0;
        }

        if (reversed)
            incsComplete = (currInc < axMax);
        else
            incsComplete = (currInc > axMax);

    } while (!incsComplete);
}


void PC_Axes::SetLinearMinorIncs(const double& majorIncVal,
                                       int     nMinorPerMajor,
                                       SC_DoubleArray& incVals)
{
    double incVal = majorIncVal / double(nMinorPerMajor + 1);
    incVals.Alloc(nMinorPerMajor);
    incVals += incVal;
    for (int i = 1; i < nMinorPerMajor; i++)
        incVals += (incVals[i - 1] + incVal);
}


void PC_Axes::AutoLogAdjust(double& minVal,
                            double& maxVal)
{
    double minLgt = log10(minVal);

    double temp = pow(10.0, floor(minLgt));
    double incVal = 1.0;
    while ((temp * incVal) <= minVal)
        incVal += 1.0;
    if (incVal > 1.0)
        incVal -= 1.0;

    minVal = temp * incVal;

    double maxLgt = log10(maxVal);
    temp = pow(10.0, floor(maxLgt));
    incVal = 1.0;
    while ((temp * incVal) < maxVal)
          incVal += 1.0;
    maxVal = temp * incVal;
}

void PC_Axes::AutoLogIncs(const double&   axStVal,
                          const double&   axEndVal,
                          double&         firstInc,
                          double&         axIncVal)
{
    bool reversed = (axStVal > axEndVal);

    double minLgt = log10(axStVal);
    int minLog = int(floor(minLgt));
    if (reversed)
    {
       axIncVal = 0.1;
       minLog++;
    }
    else
        axIncVal = 10.0;

    firstInc = pow(10.0, double(minLog));
}


LinLogAxis1D::LinLogAxis1D()
{
    axisLinMin = 0.0;
    axisLinMax = 100.0;
    axisLogMin = 0.01;
    axisLogMax = 100.0;

    majorIncVal = 25.0;
    nminorPerMajor = 5;

    incrFont.fontRotation = PSC_Font::fr_Horiz;
}

LinLogAxis1D :: LinLogAxis1D(const LinLogAxis1D& a) : PC_Axes(a)
{
    LocalCopy(a);
}

LinLogAxis1D& LinLogAxis1D::operator= (const LinLogAxis1D& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}

void  LinLogAxis1D::FullCopy(const LinLogAxis1D& a)
{
    PC_Axes::FullCopy(a);
    LocalCopy(a);
}

void  LinLogAxis1D::LocalCopy(const LinLogAxis1D& a)
{
    axisLinMin      = a.axisLinMin    ;
    axisLinMax      = a.axisLinMax    ;
    axisLogMin      = a.axisLogMin    ;
    axisLogMax      = a.axisLogMax    ;

    majorIncVal     = a.majorIncVal   ;
    nminorPerMajor  = a.nminorPerMajor;

    axisIncFormat       = a.axisIncFormat ;
    cursorReportFormat  = a.cursorReportFormat;
}


void LinLogAxis1D::AdjustAxisLimits()
{
    if (! autoScaleAxes)
        return;

    if ( axesAreLinear)
    {
        MakeDifferent(axisLinMin, axisLinMax, false);
        double firstInc, incVal;
        if (AutoLinIncs(axisLinMin, axisLinMax, firstInc, incVal))
        {
            LinFirstInc(axisLinMin, axisLinMax, incVal, firstInc);
            LinAdjust(firstInc, incVal, axisLinMin, axisLinMax);
        }
    }
    else
    {
        MakeDifferent(axisLogMin, axisLogMax, true);
        AutoLogAdjust(axisLogMin, axisLogMax);
    }
}

bool LinLogAxis1D:: SetMajorIncs()
{
    if (!autoMajorIncs )
        return false;

    double firstInc;
    bool doRound;
    if (axesAreLinear)
    {
        AutoLinIncs(axisLinMin,  axisLinMax, firstInc, majorIncVal);
        doRound = (fabs(axisLinMin - firstInc) > rAxesEps);
    }
    else
    {
        AutoLogIncs(axisLogMin,  axisLogMax, firstInc, majorIncVal);
        doRound = (fabs(axisLogMin - firstInc) > rAxesEps);
    }

    return doRound;
}

void LinLogAxis1D:: SetMinorIncs()
{
    if (autoMinorIncs && axesAreLinear)
    {
        double dummySt, incVal;
        if (AutoLinIncs(0.0, majorIncVal, dummySt, incVal))
            nminorPerMajor = int(fabs(majorIncVal/incVal)) - 1;
    }
}

void LinLogAxis1D::SetAxisLimits(const double& minVal,
                                 const double& maxVal)
{
    if (axesAreLinear)
    {
        axisLinMin = minVal;
        axisLinMax = maxVal;
    }
    else
    {
        if (maxVal > rAxesEps)
            axisLogMax = maxVal;
        else
            axisLogMax = 100.0;

        if (minVal > rAxesEps)
            axisLogMin = minVal;
        else
            axisLogMin = axisLogMax / 1000.0;
    }
}

void LinLogAxis1D::GetAxisLimits(double& minVal,
                                 double& maxVal)
{
    if (axesAreLinear)
    {
        minVal = axisLinMin;
        maxVal = axisLinMax;
    }
    else
    {
        minVal = axisLogMin;
        maxVal = axisLogMax;
    }
}

void LinLogAxis1D::GetAxesMajorIncs(SC_DoubleArray& incVals)
{
    if (axesAreLinear)
        SetLinearMajorIncs(axisLinMin, axisLinMax, majorIncVal, incVals);
    else
    {
        double currInc, axIncVal;
        AutoLogIncs(axisLogMin, axisLogMax, currInc, axIncVal);
        incVals.Alloc(maxIncAlloc);
        bool reversed = axisLogMin > axisLogMax;
        bool incsComplete;
        do
        {
            incVals += currInc;
            if (incVals.IsFull())
                return;
            currInc *= axIncVal;

            if (reversed)
                incsComplete = (currInc < axisLogMax);
            else
                incsComplete = (currInc > axisLogMax);

        } while (!incsComplete);
    }
}

void LinLogAxis1D::GetAxesMinorIncs(SC_DoubleArray& incVals)
{
    if (axesAreLinear)
        SetLinearMinorIncs(majorIncVal, nminorPerMajor, incVals);
    else
    {
        incVals.Alloc(8);
        for (int i = 2; i < 10; i++)
            incVals += double(i);
    }
}

bool  LinLogAxis1D::InAxisLimits(const double& inVal)
{
    if (RealIsNull(inVal))
        return false;

    if (axesAreLinear)
        return Limit::WithinOneLimit(axisLinMin, axisLinMax, inVal);
    else
        return Limit::WithinOneLimit(axisLogMin, axisLogMax, inVal);
}

double LinLogAxis1D::AxisSpanValue(const double& userVal)
{
    if (axesAreLinear)
        return (userVal - axisLinMin) / (axisLinMax - axisLinMin);
    else
        return (log10(userVal) - log10(axisLogMin)) / (log10(axisLogMax) - log10(axisLogMin));
}


double  LinLogAxis1D::AxisUserValue(const double& spanVal)
{
    if (axesAreLinear)
        return axisLinMin + spanVal * (axisLinMax - axisLinMin);
    else
        return pow(10.0, log10(axisLogMin) + spanVal * (log10(axisLogMax) - log10(axisLogMin)));
}

