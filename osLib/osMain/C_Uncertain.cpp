///////////////////////////////////////////////////////////////////////////////////
//
// C_Uncertain.cpp
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
#include <genClass/U_Matrix.h>
#include <iostream>
#include <osClass/C_SimErr.h>

#include <osClass/C_SampVarUnits.h>
#include <osMain/G_Sample.h>

#include "UncertFOSM.h"
#include "UncertPEM.h"

#include "C_Uncertain.h"

UncertaintyPropagation::UncertaintyPropagation() :
    updateCB(0),
    currSimulator(0)
{
}
UncertaintyPropagation::~UncertaintyPropagation()
{
}

void UncertaintyPropagation::CalcUncert()
{
    GenAppInternalError("UncertaintyPropagation::CalcUncert");
}

bool UncertaintyPropagation::Initialize(RunnableC&  simulator)
{
    currSimulator = &simulator;

    //  get current vars
    SampVar::GetActiveArray(uncertainVars);
    nUncertVars = uncertainVars.Size();

    try
    {
        //allocate arrays
        meanVarValues.AllocAndSetSize(nUncertVars);
        stdDevVarValues.Alloc(nUncertVars);
        isLogVar.Alloc(nUncertVars);
        varRange.Alloc(nUncertVars);

        numM = uncertInput.GetNMetrics();
        meanMetricValues.Alloc(numM);
        stdDevMetricValues.Alloc(numM);

        meanComponentValues.MatrixAlloc(nUncertVars, numM);
        stdDevComponentValues.MatrixAlloc(nUncertVars, numM);
        derivValues.MatrixAlloc(nUncertVars, numM);

        //get mean and stdev values
        for (int i = 0; i < nUncertVars; i++)
        {
            SampVarUnits* unitSamp = dynamic_cast<SampVarUnits*>(uncertainVars[i]);
            if (unitSamp == 0)
                GenAppInternalError("UncertaintyPropagation::Initialize");
            isLogVar[i] = unitSamp->IsLog(unitSamp->svDistribution);

            //note that log mean and log std dev are only log if distr is log
            meanVarValues[i] = unitSamp->GetLogMean();
            stdDevVarValues[i] = unitSamp->GetLogStdDev();

            if (isLogVar[i])
                varRange[i] = log(unitSamp->GetMaxVarValue()) - log(unitSamp->GetMinVarValue());
            else
                varRange[i] = unitSamp->GetMaxVarValue() - unitSamp->GetMinVarValue();
        }
        stdDevVarValues.SetSize(nUncertVars);
    }
    catch (SC_Array::AllocationError)
    {
        return false;
    }

    return true;
}

bool UncertaintyPropagation::CalcFitValue(const SC_DoubleArray& varVals,
                                          SC_DoubleArray&       metricVals)
                                        //      double&         nextMetric)
{
    // set parameters
    for (int i = 0; i < nUncertVars; i++)
        uncertainVars[i]->SetUncertVal(varVals[i]);

    // fatal setup error
    SC_SetupErr errData;
    if (!currSimulator->SetupForRun(errData))
        throw SimError(errData.GetErrorText());

    uncertInput.ClearCalcFlag();
    try
    {
        currSimulator->Execute();
        currSimulator->UpdateOutput();
    }
    catch (SimError& err)
    {
        if (err.errorLevel == SimError::seSemiFatal)
        {
            lastSemiFatal = err;
            return false;
        }

        // re throw
        throw err;
    }

    if (!uncertInput.GetCalcFlag())
        throw SimError("metric calculation error. See List->Messages");

    metricVals = uncertInput.GetMetricValue();

    return true;
}

////////////////////////////////////////////////////

UncertaintyPropagationPackage::UncertaintyPropagationPackage()
{
    uncertProp   = 0;
}

UncertaintyPropagationPackage::~UncertaintyPropagationPackage()
{
    ClearUncertPropTechnique();
}

void UncertaintyPropagationPackage::SetUncertPropTechnique()
{
    ClearUncertPropTechnique();

    uncertProp = GetUncertTechnique(uncertType);

    UncertResults::currUncertPkg = this;
}

void UncertaintyPropagationPackage::ClearUncertPropTechnique()
{
    //std::cout << uncertProp << "Clear uncert prop" << std::endl;
    delete uncertProp;
    uncertProp = 0;

    UncertResults::currUncertPkg = 0;
}

bool UncertaintyPropagationPackage::Initialize(RunnableC&       simulator)
{
    if (!uncertProp->Initialize(simulator))
    {
        GenAppErrorMsg("UncertInit", "Uncertainty propagation technique initialization failure");
        return false;
    }
    uncertProp->meanMetricValues.SetEmpty();
    uncertProp->stdDevMetricValues.SetEmpty();
    uncertProp->meanComponentValues.SetEmpty();
    uncertProp->stdDevComponentValues.SetEmpty();
    uncertProp->derivValues.SetEmpty();
    return true;
}


void UncertaintyPropagationPackage::MultiStageUncertRun()//int metricIndex)
{
    lastErr = SimError();
    //uncertInput.currSelectedMetric = metricIndex;
    uncertProp->CalcUncert();
}


UncertaintyPropagation* UncertaintyPropagationPackage::GetUncertTechnique(UncertType  uncertType)
{
    switch (uncertType) {
        case utFOSM          : return new FOSM();
        case utPEM           : return new PEM();
        default              : {
            GenAppInternalError("UncertaintyPropagationPackage::GetUncertTechnique");
            return 0;
        }

    }
    return 0;
}


