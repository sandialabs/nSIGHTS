///////////////////////////////////////////////////////////////////////////////////
//
// OptSCEM.cpp
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


#include "OptSCEM.h"

SCEMOptimizer::SCEMOptimizer()
{
}
SCEMOptimizer::~SCEMOptimizer()
{
}


bool SCEMOptimizer::Initialize(UpdateCB       upCB,
                           RunnableC&  simulator)
{
    if (!Optimizer::Initialize(upCB, simulator))
        return false;
    try
    {
        nsamplePoints = scem_m * scem_p;
        samplePoints.MatrixAllocAndSetSize(nsamplePoints, noptVars);
        sampleF.AllocAndSetSize(nsamplePoints);
        complexes.CubeAllocAndSetSize(scem_p, scem_m, noptVars);
        subComplex.MatrixAllocAndSetSize(scem_q, noptVars);
        subComplexF.AllocAndSetSize(scem_q);
        complexF.MatrixAllocAndSetSize(scem_p, scem_m);
        scemRand.SetupRandom(scem_seed);
        centroid.AllocAndSetSize(noptVars);
    }
    catch (SC_Array::AllocationError)
    {
        return false;
    }

    return true;
}


void SCEMOptimizer::Optimize()
{
    //  initialize to avoid garbage in output
    optIterCount = 0;
    try
    {
        SCEMStartOpt();
        SCEMDoOpt();
    }
    catch (SimError& err)
    {
        OptErrorProcessing(err);
    }
}

int SCEMOptimizer::GetnExtraOutput() const
{
    return 3;
}
void SCEMOptimizer::GetExtraOutputID(SC_StringArray& extraIDs) const
{
    extraIDs.Alloc(3);
    extraIDs += "nReflect";
    extraIDs += "nContract";
    extraIDs += "nMutate";
}
void SCEMOptimizer::GetExtraOutput(SC_DoubleArray& extraData) const
{
    extraData.Alloc(3);
    extraData += double(nReflect);
    extraData += double(nContract);
    extraData += double(nMutate);
}


double  SCEMOptimizer::EvalFunction(const SC_DoubleArray& normParEstimates)
{
    if (!NormParOK(normParEstimates))
        return currFitVal;

    // should simulate this
    bool calcOK = false;
    try
    {
        calcOK = CalcFitValue(normParEstimates, currFitVal);
        if (!calcOK)
        {
            if (runCount == 1)
                throw SimError("failure on first SCEM calculation");
            currFitVal = worstFitVal * 1.1;
        }
        nFail = 0;
    }
    catch (SimError& err)
    {
        // trap bad fits
        if (err.IsFatal() && (!optimizeInput.GetCalcFlag()))
        {
            if (runCount == 1)
                throw SimError("fit calc failure on first SCEM calculation");
            nFail++;
            if (nFail > 10)
                throw err;
            currFitVal = worstFitVal * 1.1;
        }
        else
            throw err;
    }

    if (calcOK)
        optProgressOutput.CalcAndUpdate();

    updateCB(*this);

    if ((currFitVal < bestFitVal)  || (runCount == 1))
    {
        bestFitVal = currFitVal;
        bestNormParEst = normParEstimates;
        GetOptParEst(bestParEst);
    }

    if (currFitVal > worstFitVal)
        worstFitVal = currFitVal;

    return currFitVal;
}


void  SCEMOptimizer::SCEMStartOpt()
{
    opMessage = "SCEM - initializing ...";

    worstFitVal    = 0.0;
    bestFitVal     = 1.0E+08;

    nReflect = 0;
    nContract = 0;
    nMutate = 0;
    nFail = 0;

    // deviation from algorithm - use estimate as one point in complexes
    // from Duan code
    samplePoints[0] = startNormParEst;
    runPars.ReAlloc(nsamplePoints);
    runPars.SetSize(nsamplePoints);
    runPars[0] = startNormParEst;
//MX    sampleF[0] = EvalFunction(startNormParEst);

    for (int i = 1; i < nsamplePoints; i++)
    {
        for (int j = 0; j < noptVars; j++)
            samplePoints[i][j] = scemRand.Random();
//MX        sampleF[i] = EvalFunction(samplePoints[i]);
        runPars[i] = samplePoints[i];
    }

    bool finalDerivCalc = false;
    bool run0FitDeteriorated = false;
    double dummy = bestFitVal;  //0.0;
    OptimizerRunMultiple(runPars, true, 0, finalDerivCalc, false, dummy, run0FitDeteriorated);
    for (int i = 0;  i < nsamplePoints; i++){
        sampleF[i] = multRunFitChisq[i];
        if ((sampleF[i]< bestFitVal) || (runCount == 1)){
            bestFitVal = sampleF[i];
            bestNormParEst = runPars[i];
            GetOptParEst(bestParEst);
        }
        if (sampleF[i] > worstFitVal)
            worstFitVal = sampleF[i];
    }

        // sorted results
    SC_IntArray rank;
    rank.AllocAndIndex(nsamplePoints);
    sampleF.Sort(true, rank);
    SC_DoubleMatrix temp(samplePoints);
    for (int i = 0; i < nsamplePoints; i++)
        samplePoints[i] = temp[rank[i]];

}

void SCEMOptimizer::SCEMDoOpt()
{
    opMessage = "SCEM - optimizing ...";

    while (true)
    {
        optIterCount++;

        // into complexes
        int indx = 0;
        for (int i = 0; i < scem_m; i++)
            for (int j = 0; j < scem_p; j++)
            {
                complexes[j][i] = samplePoints[indx];
                complexF[j][i] = sampleF[indx++];
            }

        for (int i = 0; i < scem_p; i++)
            Evolve(complexes[i], complexF[i]);

        indx = 0;
        for (int i = 0; i < scem_m; i++)
            for (int j = 0; j < scem_p; j++)
            {
                samplePoints[indx] = complexes[j][i];
                sampleF[indx++] = complexF[j][i];
            }

        // we have a best fit by this point
        okForOutput = true;

        // sorted results
        SC_IntArray rank;
        rank.AllocAndIndex(nsamplePoints);
        sampleF.Sort(true, rank);
        SC_DoubleMatrix temp(samplePoints);
        for (int i = 0; i < nsamplePoints; i++)
            samplePoints[i] = temp[rank[i]];


        if (ToleranceCheck(sampleF[0], sampleF[scem_m - 1],
                samplePoints[0] , samplePoints[scem_m - 1]))
            return;
    }
}


void SCEMOptimizer::Evolve(SC_DoubleMatrix&  complex,
                           SC_DoubleArray&   complexFitVals)
{
    SC_IntArray lcs;
    SC_DoubleArray r(noptVars);
    SC_IntArray rank;

    for (int kk = 0; kk < scem_beta; kk++)
    {
        if (scem_q == scem_m)
        {
            subComplex = complex;
            subComplexF = complexFitVals;
        }
        else
        {
            lcs.Alloc(scem_q);

            double npg = double(scem_m);
            while (!lcs.IsFull())
            {
                double rval = scemRand.Random();
                int indx = int(npg + 0.5 - sqrt(Sqr(npg + 0.5) - npg * (npg + 1.0) * rval));
                lcs.AddIfNotFound(indx);
            }

            lcs.Sort(true);
            for (int i = 0; i < scem_q; i++)
            {
                subComplex[i] = complex[lcs[i]];
                subComplexF[i] = complexFitVals[lcs[i]];
            }
        }

        for (int k = 0; k < scem_alpha; k++)
        {
            // calc centroid
            for (int j = 0; j < noptVars; j++)
            {
                centroid[j] = 0.0;
                for (int i = 0; i < scem_q - 1; i++)
                    centroid[j] += subComplex[i][j];
                centroid[j] /=  double(scem_q - 1);
            }

            double fr = SCEMReflect(r);

            if (fr < subComplexF.LastIndex())
            {
                subComplexF.LastIndex() = fr;
                subComplex.LastIndex() = r;
            }
            else
            {
                double fc = SCEMContract(r);
                if (fc > subComplexF.LastIndex())
                    fc = SCEMMutate(r);

                subComplexF.LastIndex() = fc;
                subComplex.LastIndex() = r;
            }

            rank.AllocAndIndex(scem_q);
            subComplexF.Sort(true, rank);
            SC_DoubleMatrix temp(subComplex);
            for (int i = 0; i < scem_q; i++)
                subComplex[i] = temp[rank[i]];
        }

        if (scem_q == scem_m)
        {
            complex = subComplex;
            complexFitVals = subComplexF;
        }
        else
        {
            for (int i = 0; i < scem_q; i++)
            {
                complex[lcs[i]] = subComplex[i];
                complexFitVals[lcs[i]] = subComplexF[i];
            }
        }
    }
}


double SCEMOptimizer::SCEMReflect(SC_DoubleArray&  r)
{
    nReflect++;

    for (int i = 0; i < noptVars; i++)
    {
        double nextP = 2.0 * centroid[i] - subComplex.LastIndex()[i];
        if ((nextP < 0.0) || (nextP > 1.0))
            return (SCEMMutate(r));

        r[i] = nextP;
    }
    return EvalFunction(r);
}

double SCEMOptimizer::SCEMMutate(SC_DoubleArray&  r)
{
    nMutate++;

    for (int i = 0; i < noptVars; i++)
        r[i] = scemRand.Random();

    return EvalFunction(r);
}

double SCEMOptimizer::SCEMContract(SC_DoubleArray&  r)
{
    nContract++;

    for (int i = 0; i < noptVars; i++)
    {
        r[i] = (centroid[i] + subComplex.LastIndex()[i]) / 2.0;
    }
    return EvalFunction(r);
}


