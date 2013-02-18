///////////////////////////////////////////////////////////////////////////////////
//
// UncertFOSM.cpp
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
#include <osMain/G_Sample.h>


#include "UncertFOSM.h"


FOSM::FOSM()
{
}
FOSM::~FOSM()
{
}

bool FOSM::Initialize(RunnableC&  simulator)
{
    if (!UncertaintyPropagation::Initialize(simulator))
        return false;
    try
    {
       //allocate arrays
        newVarVals.Alloc(nUncertVars);
        derivMetricValues.Alloc(numM);
        derivative.MatrixAlloc(numM, nUncertVars);
        variance.Alloc(numM);
    }
    catch (SC_Array::AllocationError)
    {
        return false;
    }

    return true;
}


void  FOSM::CalcUncert()
{
    try
    {
        //note that mean and std dev values are in log space if the distribution is log

        //calculate mean
        //convert mean variable values out of log space where required
        newVarVals = meanVarValues;
        for (int j = 0; j < nUncertVars; j++)
        {
            if (isLogVar[j])
                newVarVals[j] = exp(newVarVals[j]);
        }
        CalcFitValue(newVarVals, meanMetricValues);


        //calculate std deviation
        //first calculate derivatives
        for (int i = 0; i < nUncertVars; i++)
        {
            newVarVals = meanVarValues;
            double actualDerivSpan = derivSpan * varRange[i]; //TODO add calculated deriv span
            newVarVals[i] += actualDerivSpan;
            //convert variable values out of log space where required
            for (int j = 0; j < nUncertVars; j++)
            {
                if (isLogVar[j])
                    newVarVals[j] = exp(newVarVals[j]);
            }

            CalcFitValue(newVarVals, derivMetricValues);
            double diff;
            if (isLogVar[i])
                diff = meanVarValues[i] - log(newVarVals[i]);
            else
                diff = meanVarValues[i] - newVarVals[i];
            for (int j = 0; j < numM; j++)
            {
                derivative[j][i] = (meanMetricValues[j] - derivMetricValues[j]) / diff;
                derivValues[i][j] = derivative[j][i];
            }
        }
        //calc std dev
        //double variance = 0.0;
        variance.FillToAlloc(0.0);
        stdDevComponentValues.MatrixSetSize(nUncertVars, numM);
        stdDevComponentValues.MatrixFill(0.0);
        for (int i = 0; i < nUncertVars; i++)
        {
            for (int j = 0; j < nUncertVars; j++)
            {
                double corrVal = uncertainVars[i]->GetUncertainCorrelation(uncertainVars[j]);
                double multiplier = corrVal * stdDevVarValues[i] * stdDevVarValues[j];
                for (int k = 0; k < numM; k++)
                {
                    double component = derivative[k][i] * derivative[k][j] * multiplier;
                    variance[k] += component;
                    stdDevComponentValues[i][k] += component;
                }
            }
        }
        for (int i = 0; i < numM; i++)
            stdDevMetricValues[i] = sqrt(variance[i]);
    }
    catch (SimError& err)
    {
        throw err; //TODO check
    }
    uncertResultsFO.CalcAndUpdate();
}

