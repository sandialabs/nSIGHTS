///////////////////////////////////////////////////////////////////////////////////
//
// OptSimplex.cpp
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
//      Main optimizer/sampler code and global variables and other support classes
//  used by optimizer and application.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>

#include <osClass/C_SimErr.h>
#include <osMain/G_Optimize.h>

#include <genApp/DebugUtil.h>

#include <genApp/MPIAppSupport.h>

#include "OptSimplex.h"


SimplexOptimizer::SimplexOptimizer()
{
    currSimplexSpan = 1000.0;
}
SimplexOptimizer::~SimplexOptimizer()
{
}


bool SimplexOptimizer::Initialize(UpdateCB       upCB,
                           RunnableC&  simulator)
{
    if (!Optimizer::Initialize(upCB, simulator))
        return false;
    try
    {

        simplexParSum.AllocAndSetSize(noptVars);
        simplexParTry.AllocAndSetSize(noptVars);
        simplexParStar.AllocAndSetSize(noptVars);
        simplexParStarStar.AllocAndSetSize(noptVars);

        nsimplexPoints = noptVars + 1;
        simplexFits.AllocAndSetSize(nsimplexPoints);
        simplexParEst.MatrixAlloc(nsimplexPoints, noptVars);
    }
    catch (SC_Array::AllocationError)
    {
        return false;
    }

    return true;
}


void SimplexOptimizer::Optimize()
{
    currSimplexSpan = 1000.0;

    //  initialize to avoid garbage in output
    optIterCount = 0;
    try
    {
        SimplexStartOpt();
        SimplexDoOpt();
    }
    catch (SimError& err)
    {
        OptErrorProcessing(err);
    }
}

int SimplexOptimizer::GetnExtraOutput() const
{
    return 1;
}
void SimplexOptimizer::GetExtraOutputID(SC_StringArray& extraIDs) const
{
    extraIDs.Alloc(1);
    extraIDs += "SimplexSpan";
}
void SimplexOptimizer::GetExtraOutput(SC_DoubleArray& extraData) const
{
    extraData.Alloc(1);
    extraData += currSimplexSpan;
}


double  SimplexOptimizer::SimplexEvalFunction(const SC_DoubleArray& normParEstimates)
{

    if (!NormParOK(normParEstimates))
        return currFitVal;

    // should simulate this
    if (!CalcFitValue(normParEstimates, currFitVal))
    {
        if (runCount == 1)
            throw SimError("failure on first simplex calculation");
        currFitVal = worstFitVal * 1.1;
    }
    else
        optProgressOutput.CalcAndUpdate();

    updateCB(*this);

    if ((currFitVal < bestFitVal) || (runCount == 1))
    {
        bestFitVal = currFitVal;
        bestNormParEst = normParEstimates;
        GetOptParEst(bestParEst);
    }

    if (currFitVal > worstFitVal)
        worstFitVal = currFitVal;

    return currFitVal;
}


void SimplexOptimizer::SimplexCalcHiLo()
{
    hiIndex = 0;
    nextHiIndex = 1;
    if (simplexFits[0] < simplexFits[1])
    {
        hiIndex = 1;
        nextHiIndex = 0;
    }
    loIndex = 0;

    for (int i = 0; i < nsimplexPoints; i++)
    {
        double currFit = simplexFits[i];
        if (currFit < simplexFits[loIndex])
        {
            loIndex = i;
        }
        else if (currFit > simplexFits[hiIndex])
        {
            nextHiIndex = hiIndex;
            hiIndex = i;
        }
 //       else if ((i != nextHiIndex) && (currFit > simplexFits[nextHiIndex]))
        else if ((i != hiIndex) && (currFit > simplexFits[nextHiIndex]))
            nextHiIndex = i;
    }

    // calc span for output
    currSimplexSpan = 0.0;
    for (int j = 0; j < noptVars; j++)
    {
        double minEst = simplexParEst[0][j];
        double maxEst = minEst;
        for (int i = 1;  i < nsimplexPoints; i++)
        {
            DMinMax(simplexParEst[i][j], minEst, maxEst);
        }
        double currSpan = maxEst - minEst;
        if (currSpan > currSimplexSpan)
            currSimplexSpan = currSpan;
    }
}


void  SimplexOptimizer::SimplexStartOpt()
{
    opMessage = "Simplex - initializing ...";
    currSimplexSpan = simplexSpan;

    runPars.SetSize(nsimplexPoints);
    simplexParEst[0] = startNormParEst;
    runPars[0] =  startNormParEst;
    for (int i = 1;  i < nsimplexPoints; i++)
    {
        simplexParEst[i] = simplexParEst[0];
        double currPar = simplexParEst[0][i - 1] + simplexSpan;
        if (currPar > 1.0)
            currPar = simplexParEst[0][i - 1] - simplexSpan;

        simplexParEst[i][i - 1] = currPar;
        runPars[i] = simplexParEst[i];
    }

    hiIndex     = 0;
    nextHiIndex = 0;
    loIndex     = 0;

    worstFitVal    = 0.0;
    bestFitVal     = 1.0E+08;
    //replace
/*    for (int i = 0;  i < nsimplexPoints; i++)
        simplexFits[i] = SimplexEvalFunction(simplexParEst[i]);
*/
    bool finalDerivCalc=false;
    bool run0FitDeteriorated = false;
    double dummy = 0.0;
    OptimizerRunMultiple(runPars, false, 0, finalDerivCalc, false, dummy, run0FitDeteriorated);

    for (int i = 0;  i < nsimplexPoints; i++)
    {
        simplexFits[i] = multRunFitChisq[i];


        if ((simplexFits[i]< bestFitVal) || (runCount == 1))
        {
            bestFitVal = simplexFits[i];
            bestNormParEst = simplexParEst[i];
            GetOptParEst(bestParEst);
        }
        if (simplexFits[i] > worstFitVal)
            worstFitVal = simplexFits[i];
    }

    SimplexCalcHiLo();

}


void SimplexOptimizer::SimplexReset()
{
    // replace
/*  SC_DoubleArray& lowPar = simplexParEst[loIndex];
    for (int i = 0; i < nsimplexPoints; i++)
    {
        if (i != loIndex)
        {
            for (int k = 0; k < noptVars; k++)
                simplexParEst[i][k] = lowPar[k] +
                                        currShrinkage * (simplexParEst[i][k] - lowPar[k]);
            simplexFits[i] = SimplexEvalFunction(simplexParEst[i]);
        }
    }
*/  //MX
    SC_DoubleArray& lowPar = simplexParEst[loIndex];
    runPars[loIndex] = simplexParEst[loIndex];
    for (int i = 0; i < nsimplexPoints; i++)
    {
        if (i != loIndex)
        {
            for (int k = 0; k < noptVars; k++)
                simplexParEst[i][k] = lowPar[k] +
                                        currShrinkage * (simplexParEst[i][k] - lowPar[k]);
            runPars[i] = simplexParEst[i];
        }
    }
    bool finalDerivCalc=false;
    bool run0FitDeteriorated = false;
    double dummy = 0.0;
    OptimizerRunMultiple(runPars, false, 0, finalDerivCalc, false, dummy, run0FitDeteriorated);
    for (int i = 0;  i < nsimplexPoints; i++)
    {
        simplexFits[i] = multRunFitChisq[i];
    }

}

void SimplexOptimizer::SimplexDoOpt()
{
    opMessage = "Simplex - optimizing ...";

    SimplexCalcPsum();

    currShrinkage = simplexShrinkageCoeff;

    while (true)
    {
        optIterCount++;
        SimplexCalcHiLo();


        okForOutput = true;

        if (ToleranceCheck(simplexFits[hiIndex], simplexFits[loIndex],
                            simplexParEst[hiIndex], simplexParEst[loIndex]))
            return;

        DoNelderMeadSimplex();
    }
}

void SimplexOptimizer::SimplexCalcPsum()
{
    for (int i =0; i < noptVars; i++)
    {
        simplexParSum[i] = 0.0;
        for (int k =0; k < nsimplexPoints; k++)
            simplexParSum[i] += simplexParEst[k][i];
    }
}

// ******************* Nelder/Mead approach


void SimplexOptimizer::DoNelderMeadSimplex()
{
    // Nelder/Mead
    double ystar =  SimplexReflect();
    // between current lo and next hi?
    if ((ystar > simplexFits[loIndex]) && (ystar < simplexFits[nextHiIndex]))
    {
        // replace Ph with P*
        simplexParEst[hiIndex] = simplexParStar;
        simplexFits[hiIndex] = ystar;
        opMessage = "Simplex - reflection in range ...";

        return;
    }

    // better than old best ?
    if (ystar <= simplexFits[loIndex])
    {
        double ystarstar =  SimplexExpand();
        if (ystarstar < simplexFits[loIndex])
        {
            simplexParEst[hiIndex] = simplexParStarStar;
            simplexFits[hiIndex] = ystarstar;
            opMessage = "Simplex - expansion passed ...";

        }
        else
        {
            simplexParEst[hiIndex] = simplexParStar;
            simplexFits[hiIndex] = ystar;
            opMessage = "Simplex - reflection passed ...";
        }
        return;
    }

    // original reflection was a failure
    if (ystar < simplexFits[hiIndex])
    {
        // better than old worst
        simplexParEst[hiIndex] = simplexParStar;
        simplexFits[hiIndex] = ystar;
    }

    // and contract
    double ystarstar =  SimplexContract();
    if (ystarstar < simplexFits[hiIndex])
    {
        // contraction was an improvement
        simplexParEst[hiIndex] = simplexParStarStar;
        simplexFits[hiIndex] = ystarstar;
        opMessage = "Simplex - contraction passed ...";
    }
    else
    {
        opMessage = "Simplex - shrinking - reset all ...";
        SimplexReset();
    }
}

double SimplexOptimizer::SimplexReflect()
{
    // calc centroid Pbar for all != hi
    for (int i =0; i < noptVars; i++)
    {
        simplexParSum[i] = 0.0;
        for (int k =0; k < nsimplexPoints; k++)
            if (k != hiIndex)
                simplexParSum[i] += simplexParEst[k][i];

        simplexParSum[i] /= double(nsimplexPoints - 1);
    }

    // new P*
    for (int i =0; i < noptVars; i++)
        simplexParStar[i] = (1.0 + simplexReflectionCoeff)  * simplexParSum[i]
                - simplexReflectionCoeff * simplexParEst[hiIndex][i];

    return SimplexEvalFunction(simplexParStar);
}

double SimplexOptimizer::SimplexExpand()
{
    // parSum is centroid
    for (int i =0; i < noptVars; i++)
        simplexParStarStar[i] = simplexExpansionCoeff  * simplexParStar[i]
                            + (1.0 - simplexExpansionCoeff) * simplexParSum[i];

    return SimplexEvalFunction(simplexParStarStar);
}


double SimplexOptimizer::SimplexContract()
{
    for (int i =0; i < noptVars; i++)
        simplexParStarStar[i] = simplexContractionCoeff  * simplexParEst[hiIndex][i]
                            + (1.0 - simplexContractionCoeff) * simplexParSum[i];

    return SimplexEvalFunction(simplexParStarStar);

}

