///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SingleFitBase.cpp
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

#include <osDFO/DFO_SingleFitBase.h>



DFO_SingleFitBase :: DFO_SingleFitBase(const char* inID) : DFO_FitBase(inID)
{
    fitCalcType  = fctSumSquares;
    fitSigma     = 1.0;
    fitIsNormed  = false;
    minCoverage  = 0.8;
    coveragePenalty = 0.0;

    removeFieldDuplicates = false;  // added v2
    removeSimDuplicates = false;  // added v2
    duplicateEps    = 1.0E-07;

    InitCommon();
}

DFO_SingleFitBase::DFO_SingleFitBase(const DFO_SingleFitBase& a) : DFO_FitBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_SingleFitBase :: ~DFO_SingleFitBase ()
{
}

void DFO_SingleFitBase::InitCommon()
{
    AddInPort(simResultsObjRef, typeid(DO_XYData));
    residualsDO.xyData = &fitResultsDC.fitResiduals;

    AddOutPort(residualsDO);
    fitResultsDO.fitResults += &fitResultsDC;
}

DFO_SingleFitBase& DFO_SingleFitBase::operator= (const DFO_SingleFitBase& a)
{
    if (&a != this)
    {
        DFO_FitBase::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SingleFitBase::LocalCopy(const DFO_SingleFitBase& a)
{
    simResultsObjRef = a.simResultsObjRef;
    fitCalcType      = a.fitCalcType;
    fitSigma         = a.fitSigma;
    fitIsNormed      = a.fitIsNormed;
    minCoverage      = a.minCoverage;
    coveragePenalty  = a.coveragePenalty;

    removeFieldDuplicates = a.removeFieldDuplicates;  // added v2
    removeSimDuplicates   = a.removeSimDuplicates;  // added v2
    duplicateEps          = a.duplicateEps;
}

void  DFO_SingleFitBase:: DoStatusChk()
{
    fieldDataOK = false;
    simDataOK = false;


    FuncObjC::DoStatusChk();

    fieldDataDC = 0;
    simResultsDC = 0;
    fitResultsDO.Reset("Single fit");
    fitResultsDC.okToAlloc = false;

    CopyString(fitResultsDC.fitID, GetID(), DC_FitResults::fitIDLen);
    fitResultsDC.sigma = fitSigma;

    if (RequiresChisq())
    {
        fitResultsDC.fitWasL1 = false;
        fitResultsDC.fitWasNormed = false;
    }
    else
    {
        fitResultsDC.fitWasL1 = (fitCalcType == DFO_SingleFitBase::fctSumAbsDelta);
        fitResultsDC.fitWasNormed = fitIsNormed;
    }
    DoAllocCheck();
    if (StatusNotOK())
        return;

    if (fieldDataDC->Size() > 0)
        SetFieldInfo();

    if (CheckInputFO(simResultsObjRef, "sim results source"))
        simResultsDC = static_cast<DO_XYData*>(GetInPortData(simResultsObjRef))->xyData;

    if (StatusNotOK())
        return;

    SetSimInfo();

    // basic checks
    if (nsimPts < 1) //2)
        SetObjErrMsg("insufficient simulation data points");
}


void DFO_SingleFitBase::SetFieldInfo()
{

    fieldDataOK = true;
    //  assume ascending - don't really care
    const SC_DoubleArray& fieldX = fieldDataDC->xData;
    fieldMinX = fieldX[0];
    fieldMaxX = fieldX.LastIndex();
    nfieldPts = fieldX.Size();
    nfieldPre = 0;
    nfieldPost = 0;
}

void DFO_SingleFitBase::SetSimInfo()
{
    simDataOK = true;
    const SC_DoubleArray& simX = simResultsDC->xData;
    simMinX = simX[0];
    simMaxX = simX.LastIndex();
    nsimPts = simX.Size();
    nsimPre = 0;
    nsimPost = 0;
}

void DFO_SingleFitBase::CheckOverlaps()
{
    // duplicate processing done here
    if (removeFieldDuplicates)
    {
        reducedFieldDC = *fieldDataDC;
        reducedFieldDC.RemoveXDuplicates(duplicateEps);
        fieldDataDC = &reducedFieldDC;
        nfieldPts = reducedFieldDC.Size();
    }
    if (removeSimDuplicates)
    {
        reducedSimDC = *simResultsDC;
        reducedSimDC.RemoveXDuplicates(duplicateEps);
        simResultsDC = &reducedSimDC;
        nsimPts = reducedSimDC.Size();
    }

    const SC_DoubleArray& fieldX = fieldDataDC->xData;
    const SC_DoubleArray& simX = simResultsDC->xData;

    double lastX;
    for (int i = 0; i < fieldX.Size(); i++)
    {
        double currX = fieldX[i];
        if (RealIsNull(currX))
        {
            SetObjErrMsg("field input contains nulls");
            continue;
        }

        if (currX < simMinX)
            nfieldPre++;
        if (currX > simMaxX)
            nfieldPost++;

        // check min max on off chance
        if (currX < fieldMinX)
            fieldMinX = currX;
        if (currX > fieldMaxX)
            fieldMaxX = currX;

        if ((i > 0) && (lastX > currX))
            SetObjErrMsg("field X non ascending");
        lastX = currX;
    }

    for (int i = 0; i < simX.Size(); i++)
    {
        double currX = simX[i];
        if (RealIsNull(currX))
        {
            SetObjErrMsg("sim input contains nulls");
            continue;
        }
        if (currX < fieldMinX)
            nsimPre++;
        if (currX  > fieldMaxX)
            nsimPost++;
        if ((i > 0) && (lastX > currX))
            SetObjErrMsg("sim X non ascending");
        lastX = currX;
    }

    double coveragePct = 1.0 - double(nfieldPre + nfieldPost) / double(nfieldPts);
    if (coveragePct < minCoverage)
        SetObjErrMsg("insufficient field X coverage");
    else
        fitMultiplier = 1.0 + coveragePenalty / coveragePct;

}

void DFO_SingleFitBase::UpdateOverlapStatus()
{
    fitMultiplier = 1.0;
    if (StatusOK())
    {
        SetFieldInfo();
        SetSimInfo();
        CheckOverlaps();
    }
}
void DFO_SingleFitBase:: CalcOutput(FOcalcType  calcType)
{
    bool isSinglePt = true;
    if ((nfieldPts > 1) && (nsimPts > 1))
        isSinglePt = false;

    fitMultiplier = 1.0;

    // brute force check - don't bother if only single point
    if (!isSinglePt)
        CheckOverlaps();
    if (StatusNotOK())
        return;

    // get refs for ease of use
    const SC_DoubleArray& fieldX = fieldDataDC->xData;
    const SC_DoubleArray& fieldY = fieldDataDC->yData;

    const SC_DoubleArray& simX = simResultsDC->xData;
    const SC_DoubleArray& simY = simResultsDC->yData;

    // check that allocations are OK
    int nfieldX = fieldX.Size();
    bool allocOK = fitResultsDC.fitResults.AllocSize() >= nfieldX;

        allocOK = allocOK && (fitResultsDC.fitDerivatives.AllocSize() > 0);
        if (allocOK)
            allocOK = allocOK && (fitResultsDC.fitDerivatives.AllocSize() >= nfieldX);

    if ((!allocOK) && (!fitResultsDC.PreRunAlloc(useFitsForOptimization)))
    {
        SetObjErrMsg("fit results alloc bad - no opt vars set ??");
        return;
    }

    if ((fitResultsDC.fitResiduals.yData.AllocSize() < nfieldX) ||
        (fitResultsDC.fitResiduals.xData.AllocSize() < nfieldX))
    {
        SetObjErrMsg("fit residuals alloc bad");
        return;
    }


        //  only initialize in ! deriv
    fitResultsDC.fitValue = 0.0;
    fitResultsDC.sse = 0.0;

    // should be smaller
    double nextX = fieldX[0] - 1.0E+06;
    double nextY, prevY, prevX;
    int nextIndex = 0;
    int maxIndex = simX.Size() - 1;

    double fitVal = 0.0;
    int nextPt = 0;

    bool doMultiplier = false;
    if (!isSinglePt)
    {
        for (int i = 0; i < nfieldX; i++)
        {
            double xVal = fieldX[i];
            if (xVal < simMinX)
            {
                doMultiplier = true;
                continue;
            }

            double simYatFieldX;
            if (xVal > simMaxX)
            {
                // check for almost coincident points
                if (fabs(xVal - simMaxX) > simMaxX / 1.0E+06)
                {
                    doMultiplier = true;
                    continue;
                }

                simYatFieldX = simY.LastIndex();
            }
            else
            {
                if (nextX < xVal)
                {
                    while (nextX < xVal)
                    {
                        nextIndex++;
                        if (nextIndex > maxIndex)
                        {
                            SetObjErrMsg("field X out of range ???");
                            return;
                        }
                        nextX = simX[nextIndex];
                    }
                    prevX = simX[nextIndex - 1];
                    nextY = simY[nextIndex];
                    prevY = simY[nextIndex - 1];
                }

                if (RealsAreSame(nextX, prevX))
                    // pathological case
                    simYatFieldX = nextY;
                else
                    // linear interp
                    simYatFieldX =  prevY + (xVal - prevX) * (nextY - prevY) / (nextX - prevX);
            }
            double ptResid = fieldY[i] - simYatFieldX;

            fitResultsDC.fitResults[nextPt] = simYatFieldX;
            fitResultsDC.fitResiduals.xData[nextPt] = xVal;
            fitResultsDC.fitResiduals.yData[nextPt] = ptResid;
            fitResultsDC.sse += Sqr(ptResid);  // calc SSE even if not covar run

            if (optCalculationRun)
            {
                    fitResultsDC.fitDerivatives[nextPt] = simYatFieldX;
                    fitResultsDC.fitResidualDerivatives[nextPt] = ptResid;
            }
            else
            {
                // only for non LM calcs is there a choice
                if (OptimizationControlStatics::RequiresChisq() || (fitCalcType  == fctSumSquares))
                    fitVal += Sqr(ptResid);
                else
                    fitVal += fabs(ptResid);
            }
            nextPt++;
        }

    }
    else //single pt
    {
        double xVal = fieldX[0];
        double simYatFieldX = simY[0];
        double ptResid = fieldY[0] - simYatFieldX;

        fitResultsDC.fitResults[nextPt] = simYatFieldX;
        fitResultsDC.fitResiduals.xData[nextPt] = xVal;
        fitResultsDC.fitResiduals.yData[nextPt] = ptResid;
        fitResultsDC.sse += Sqr(ptResid);  // calc SSE even if not covar run

        if (optCalculationRun)
        {
            fitResultsDC.fitDerivatives[nextPt] = simYatFieldX;
            fitResultsDC.fitResidualDerivatives[nextPt] = ptResid;
        }
        else
        {
            // only for non LM calcs is there a choice
            if (OptimizationControlStatics::RequiresChisq() || (fitCalcType  == fctSumSquares))
                fitVal += Sqr(ptResid);
            else
                fitVal += fabs(ptResid);
        }
        nextPt++;
    }

    // set sizes
    fitResultsDC.nfitPoints = nextPt;
    fitResultsDC.fitResiduals.xData.SetSize(nextPt);
    fitResultsDC.fitResiduals.yData.SetSize(nextPt);
    fitResultsDC.fitResults.SetSize(nextPt);

    if (optCalculationRun)
    {
        fitResultsDC.fitValue = fitResultsDC.sse / Sqr(fitResultsDC.sigma);
    }
    else
    {
        if (OptimizationControlStatics::RequiresChisq())
        {
            fitVal = fitResultsDC.sse / Sqr(fitResultsDC.sigma);
        }
        else
            if (fitIsNormed)
                fitVal /= double(nfieldX);

        fitResultsDC.fitValue = fitVal;
        if (doMultiplier)
            fitResultsDC.fitValue *= fitMultiplier;
    }
    fitResultsDO.InitLabelAndValue(fitResultsDC.fitValue);
}

