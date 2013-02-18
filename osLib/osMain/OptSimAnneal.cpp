///////////////////////////////////////////////////////////////////////////////////
//
// OptSimAnneal.cpp
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
//      Main optimizer/sampler code and global variables and other support classes used by optimizer and application.
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


#include "OptSimAnneal.h"

SimulatedAnnealingOptimizer::SimulatedAnnealingOptimizer()
{
    temperature = sa_T0;
    currSimplexSpan = 100.0;
}

SimulatedAnnealingOptimizer::~SimulatedAnnealingOptimizer()
{
}


bool SimulatedAnnealingOptimizer::Initialize(UpdateCB       upCB,
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

    annealRand.SetupRandom(saRandomSeed);

    return true;
}


void SimulatedAnnealingOptimizer::Optimize()
{
    //  initialize to avoid garbage in output
    temperature = sa_T0;
    currSimplexSpan = 100.0;
    optIterCount = 0;
    try
    {
        SimAnnealStartOpt();
        SimAnnealDoOpt();
    }
    catch (SimError& err)
    {
        OptErrorProcessing(err);
    }
}


int SimulatedAnnealingOptimizer::GetnExtraOutput() const
{
    return 2;
}
void SimulatedAnnealingOptimizer::GetExtraOutputID(SC_StringArray& extraIDs) const
{
    extraIDs.Alloc(2);
    extraIDs += "Temperature";
    extraIDs += "SimplexSpan";

}
void SimulatedAnnealingOptimizer::GetExtraOutput(SC_DoubleArray& extraData) const
{
    extraData.Alloc(2);
    extraData += temperature;
    extraData += currSimplexSpan;
}


double  SimulatedAnnealingOptimizer::SimAnnealEvalFunction(const SC_DoubleArray& normParEstimates)
{
    if (!NormParOK(normParEstimates))
        return currFitVal;

    // should simulate this
    if (!CalcFitValue(normParEstimates, currFitVal))
    {
        if (runCount == 1)
            throw SimError("failure on first sa calculation");
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


void SimulatedAnnealingOptimizer::SimAnnealCalcHiLo()
{
    loIndex = 0;
    hiIndex = 1;
    nextHiIndex = 0;

    ynhi = simplexFits[0] + ThermalFluct();
    ylo = ynhi;
    yhi = simplexFits[1] + ThermalFluct();

    if (ylo > yhi)
    {
        loIndex = 1;
        hiIndex = 0;
        nextHiIndex = 1;
        ynhi = yhi;
        yhi = ylo;
        ylo = ynhi;
    }

    for (int i = 2; i < nsimplexPoints; i++)
    {
        double yt = simplexFits[i] + ThermalFluct();
        if (yt < ylo)
        {
            loIndex = i;
            ylo = yt;
        }
        else if (yt > yhi)
        {
            ynhi = yhi;
            nextHiIndex = hiIndex;
            hiIndex = i;
            yhi = yt;
        }
        else if (yt > ynhi)
        {
            nextHiIndex = i;
            ynhi = yt;
        }
    }

    // calc span for output
    currSimplexSpan = 0.0;
    for (int j = 0; j < noptVars; j++)
    {
        double minEst = simplexParEst[0][j];
        double maxEst = minEst;
        for (int i = 1;  i < nsimplexPoints; i++)
        {
            double currEst = simplexParEst[i][j];
            if (currEst < minEst)
                minEst = currEst;
            else if (currEst > maxEst)
                maxEst = currEst;
        }
        double currSpan = maxEst - minEst;
        if (currSpan > currSimplexSpan)
            currSimplexSpan = currSpan;
    }
}


void  SimulatedAnnealingOptimizer::SimAnnealStartOpt()
{
    opMessage = "SimAnneal - initializing ...";

    currSimplexSpan = simplexSpan;

    simplexParEst[0] = startNormParEst;
    runPars[0] = startNormParEst;
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
/*    for (int i = 0;  i < nsimplexPoints; i++)
        simplexFits[i] = SimAnnealEvalFunction(simplexParEst[i]);
*/
    bool finalDerivCalc = false;
    bool run0FitDeteriorated = false;
    double dummy = 0.0;
//  runPars.SetSize(nsimplexPoints);
    OptimizerRunMultiple(runPars, false, 0, finalDerivCalc, false, dummy, run0FitDeteriorated);
    for (int i = 0;  i < nsimplexPoints; i++){
        simplexFits[i] = multRunFitChisq[i];
        if ((simplexFits[i]< bestFitVal) || (runCount == 1)){
            bestFitVal = simplexFits[i];
            bestNormParEst = simplexParEst[i];
            GetOptParEst(bestParEst);
        }
        if (simplexFits[i] > worstFitVal)
            worstFitVal = simplexFits[i];
    }

    SimAnnealCalcHiLo();
}

void SimulatedAnnealingOptimizer::SimAnnealReset()
{
/*  SC_DoubleArray& lowPar = simplexParEst[loIndex];
    for (int i = 0; i < nsimplexPoints; i++)
    {
        if (i != loIndex)
        {
            for (int k = 0; k < noptVars; k++)
                simplexParEst[i][k] = lowPar[k] +
                                        simplexShrinkageCoeff * (simplexParEst[i][k] - lowPar[k]);

            simplexFits[i] = SimAnnealEvalFunction(simplexParEst[i]);
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
                                        simplexShrinkageCoeff * (simplexParEst[i][k] - lowPar[k]);
            runPars[i] = simplexParEst[i];
        }
    }
    bool finalDerivCalc = false;
    bool run0FitDeteriorated = false;
    double dummy = 0.0;
    OptimizerRunMultiple(runPars, false, 0, finalDerivCalc, false, dummy, run0FitDeteriorated);
    for (int i = 0;  i < nsimplexPoints; i++){
        simplexFits[i] = multRunFitChisq[i];
    }
}

void SimulatedAnnealingOptimizer::SimAnnealDoOpt()
{
    opMessage = "SimAnneal - optimizing ...";

    while (true)
    {
        SimAnnealCalcHiLo();
        CalcTemperature();
        optIterCount++;

        // we have a best fit by this point
        okForOutput = true;

        if (ToleranceCheck(simplexFits[hiIndex], simplexFits[loIndex],
                            simplexParEst[hiIndex], simplexParEst[loIndex]))
            return;


        double ystar =  SimAnnealReflect();
        double ystarFlu = ystar - ThermalFluct();
        // between current lo and next hi?
        if ((ystarFlu > ylo) && (ystarFlu < ynhi))
        {
            yhi = ystarFlu;

            // replace Ph with P*
            simplexParEst[hiIndex] = simplexParStar;
            simplexFits[hiIndex] = ystar;
            opMessage = "SimAnneal - reflection in range ...";

            continue;
        }

        // better than old best ?
        if (ystarFlu <= ylo)
        {
            double ystarstar =  SimAnnealExpand();
            double ystarstarFlu = ystarstar - ThermalFluct();
            if (ystarstarFlu < ylo)
            {
                simplexParEst[hiIndex] = simplexParStarStar;
                simplexFits[hiIndex] = ystarstar;
                yhi = ystarstarFlu;
                opMessage = "SimAnneal - expansion passed ...";
            }
            else
            {
                simplexParEst[hiIndex] = simplexParStar;
                simplexFits[hiIndex] = ystar;
                yhi = ystarFlu;
                opMessage = "SimAnneal - reflection passed ...";
            }
            continue;
        }

        // original reflection was a failure
        if (ystarFlu < yhi)
        {
            // better than old worst
            simplexParEst[hiIndex] = simplexParStar;
            simplexFits[hiIndex] = ystar;
            yhi = ystarFlu;
        }

        // and contract
        double ystarstar =  SimAnnealContract();
        double ystarstarFlu = ystarstar - ThermalFluct();
        if (ystarstarFlu < yhi)
        {
            // contraction was an improvement
            simplexParEst[hiIndex] = simplexParStarStar;
            simplexFits[hiIndex] = ystarstar;
            yhi = ystarstarFlu;
            opMessage = "SimAnneal - contraction passed ...";
        }
        else
        {
            opMessage = "SimAnneal - reset all ...";
            SimAnnealReset();
        }

    }
}


double SimulatedAnnealingOptimizer::SimAnnealReflect()
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

    return SimAnnealEvalFunction(simplexParStar);
}

double SimulatedAnnealingOptimizer::SimAnnealExpand()
{
    // parSum is centroid
    for (int i =0; i < noptVars; i++)
        simplexParStarStar[i] = simplexExpansionCoeff  * simplexParStar[i]
                            + (1.0 - simplexExpansionCoeff) * simplexParSum[i];

    return SimAnnealEvalFunction(simplexParStarStar);
}


double SimulatedAnnealingOptimizer::SimAnnealContract()
{
    for (int i =0; i < noptVars; i++)
        simplexParStarStar[i] = simplexContractionCoeff  * simplexParEst[hiIndex][i]
                            + (1.0 - simplexContractionCoeff) * simplexParSum[i];

    return SimAnnealEvalFunction(simplexParStarStar);

}

void SimulatedAnnealingOptimizer::CalcTemperature()
{
    if ((coolingSchedule < csLuke1) != 0)
        return;

    double doneRatio = double(optIterCount) / double(sa_N);

    switch (coolingSchedule) {
/*      case csNR3 : {
            double newTemp =  saNR_beta * (simplexFits[loIndex] - bestFitVal);
            double minTemp = temperature * saNR_gamma;
            if (newTemp < minTemp)
                temperature = minTemp;
            else
                temperature = newTemp;
            break;
    }
    removed Sep05 at KV request */

        case csLuke1 : {
            temperature = sa_T0 * pow(saL_TN / sa_T0, doneRatio);
            break;
        }
        case csLuke2 : {
            double A = (sa_T0 - saL_TN) * double(sa_N + 1) / double(sa_N);
            double B = sa_T0 - A;
            temperature = A / double(optIterCount + 1) + B;
            break;
        }
/*      case csLuke6 : {
            temperature = 0.5 * (sa_T0 - saL_TN) *
                          (1.0 - tanh(10.0 * doneRatio - 5.0))
                          + saL_TN;
            break;
        }
    removed Sep05 at KV request */

        case csLuke7 : {
            temperature = (sa_T0 - saL_TN) / cosh(10.0 * doneRatio)
                          + saL_TN;

            break;
        }
        case csLuke8 : {
            double A = (1.0 / double(sa_N)) * log(sa_T0 /saL_TN);
            temperature = sa_T0 * exp(-A * double(optIterCount));
            break;
        }
        case csLuke9 : {
            double A = (1.0 / double(sa_N * sa_N)) * log(sa_T0 /saL_TN);
            temperature = sa_T0 * exp(-A * double(optIterCount * optIterCount));
            break;
        }
    }
}


double  SimulatedAnnealingOptimizer::ThermalFluct()
{
    double rVal = annealRand.Random();

    // make sure it is not 0.0
    while (rVal < 1.0E-05)
        rVal = annealRand.Random();

    return -temperature * log(rVal);
}

