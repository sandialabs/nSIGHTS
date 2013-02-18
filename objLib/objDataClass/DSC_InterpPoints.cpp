///////////////////////////////////////////////////////////////////////////////////
//
// DSC_InterpPoints.cpp
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

#include <genClass/C_Common.h>
#include <genClass/U_Real.h>

#include <objDataClass/DSC_InterpPoints.h>


DSC_InterpPoints::DSC_InterpPoints()
{
    interpPointsOp   = ipoLinear;
    useDataLimits    = true;
    minUserLimit     = 0.0;
    maxUserLimit     = 1.0;
    logRelativestart = 0.01;
    numInterpPoints  = 250;
}

DSC_InterpPoints::DSC_InterpPoints(const DSC_InterpPoints& a)
{
    LocalCopy(a);
}


DSC_InterpPoints&   DSC_InterpPoints::operator= (const DSC_InterpPoints& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void DSC_InterpPoints::LocalCopy(const DSC_InterpPoints& a)
{
    interpPointsOp   = a.interpPointsOp;
    useDataLimits    = a.useDataLimits;
    minUserLimit     = a.minUserLimit;
    maxUserLimit     = a.maxUserLimit;
    logRelativestart = a.logRelativestart;
    numInterpPoints  = a.numInterpPoints;
}


void  DSC_InterpPoints::SetMinMax(const double& dataMin,
                                  const double& dataMax)
{
    currMin = dataMin;
    currMax = dataMax;
}

void  DSC_InterpPoints::SetMinMax(const DC_XYData& inXY)
{
    inXY.xData.CalcMinMax(currMin, currMax);
}


bool DSC_InterpPoints::InterpSetupOK(SC_SetupErr& interpErr)
{
    if (AtInputPoints())
        return true;

    if (!useDataLimits)
    {
        currMin = minUserLimit;
        currMax = maxUserLimit;
    }

    if (RealIsNull(currMin) || RealIsNull(currMax))
        interpErr.SetConstantError("min or max is null");
    else
    {
        switch (interpPointsOp)
        {
            case ipoAtInput : {
                return true;
            };
            case ipoLinear : {
                if (fabs(currMin - currMax) < stdEps)
                    interpErr.SetConstantError("interpolant limits must be different");
                break;
            };
            case ipoLogAbsolute : {
                if ((currMin < stdEps)||(currMax < stdEps))
                    interpErr.SetConstantError("max & min must be > 0.0");
                break;
            };
            case ipoLogRelative : {
                if (logRelativestart < stdEps)
                    interpErr.SetConstantError("first step must be  > 0.0");
                if ((currMax - currMin) < logRelativestart)
                    interpErr.SetConstantError("range must be greater than start");
                break;
            };
        }
    }

    return !interpErr.ErrorFound();
}

void  DSC_InterpPoints::GetInterpPoints(SC_DoubleArray& outPts)
{
    if (AtInputPoints())
        return;

    outPts.AllocAndSetSize(numInterpPoints);
    double ptsDiv = double(numInterpPoints - 1);

    switch (interpPointsOp)
    {
        case ipoAtInput : {
            return;
        };
        case ipoLinear : {
            double dX = (currMax - currMin) / ptsDiv;
            outPts[0] = currMin;
            for (int i = 1; i < numInterpPoints; i++)
                outPts[i] = outPts[i - 1] + dX;
            break;
        };
        case ipoLogAbsolute : {
            double lastLog = log10(currMin);
            double dX = (log10(currMax) - lastLog) / ptsDiv;
            outPts[0] = currMin;
            for (int i = 1; i < numInterpPoints; i++)
            {
                lastLog += dX;
                outPts[i] = InvLgt(lastLog);
            }
            break;
        };
        case ipoLogRelative : {
            double xRange = currMax - currMin;
            double lastLog = log10(logRelativestart);
            double dX = (log10(xRange) - lastLog) / ptsDiv;
            outPts[0] = currMin + logRelativestart;
            for (int i = 1; i < numInterpPoints; i++)
            {
                lastLog += dX;
                outPts[i] = currMin + InvLgt(lastLog);
            }
            break;
        };
    }

    // to avoid cumulative round off errors on last point
    outPts.LastIndex() = currMax;

};


void  DSC_InterpPoints::GetInterpPoints(DC_XYData& outXY)
{
    if (AtInputPoints())
        return;

    GetInterpPoints(outXY.xData);
    outXY.yData.AllocAndSetSize(numInterpPoints);
}


