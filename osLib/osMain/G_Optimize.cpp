///////////////////////////////////////////////////////////////////////////////////
//
// G_Optimize.cpp
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
//      support for optimizer usage.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <iostream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <genApp/AppConfigFile.h>

#include <osClass/DO_FitResults.h>
#include <osClass/DO_CalcParReal.h>

#include <osDFO/DFO_FitBase.h>

#include <osMain/G_Optimize.h>

namespace osOptimize  {


OptimizeStaticSupport           optimize;         // the one and only
OptimizeInput                   optimizeInput;
OptimizeRuntimeOutput           optCompleteOutput;  // for samples or perturbation runs
OptimizeRuntimeOutput           optProgressOutput;  // during each optimization
OptimizeFileOutput              optFileOutput;
OptimizeErrorListing            optimizeErrorListing;


OptimizeInput::OptimizeInput() :
    FuncObjC("OptInput")
{
    for (int i = 0; i < maxOptFitSel; i++)
        AddUseFlagInPort(inputFits[i].fitObject, typeid(DO_FitResults), inputFits[i].optimizeOnFit);
    for (int i = 0; i < maxOptCalcVar; i++)
        AddUseFlagInPort(inputCalcVars[i].calcVarObject, typeid(DO_CalcParReal), inputCalcVars[i].includeCalcVar);

    calcVarDesc.Alloc(maxOptCalcVar);
    calcVarValue.Alloc(maxOptCalcVar);

    DoStatusChk();
}

void  OptimizeInput::IsBeingDeleted(FuncObjC* delObj)
{
    for (int i = 0; i < maxOptFitSel; i++)
    {
        if (inputFits[i].fitObject.objRef == delObj)
        {
            inputFits[i].optimizeOnFit = false;
            inputFits[i].fitObject= FuncObjRef();
        }
    }
    for (int i = 0; i < maxOptCalcVar; i++)
    {
        if (inputCalcVars[i].calcVarObject.objRef == delObj)
        {
            inputCalcVars[i].includeCalcVar = false;
            inputCalcVars[i].calcVarObject = FuncObjRef();
        }
    }
}


void OptimizeInput::DoStatusChk()
{
    calcVarOK = true;

    FitSelection& currFit = inputFits[currSelectedFit];
    if (!currFit.optimizeOnFit)
        SetObjErrMsg("current fit not selected ???");

    FuncObjC::DoStatusChk();

    if (!CheckInputFO(currFit.fitObject, "Selected fit"))
    {
        if (!currFit.fitObject.IsNull())
        {
            char errMsg[255];
            MakeString(errMsg, "SelFitErr:", currFit.fitObject.objRef->GetID(), ":", currFit.fitObject.objRef->GetStatusMsg(), 255);
            SetObjErrMsg(errMsg);
        }
    }

    for (int i = 0; i < maxOptCalcVar; i++)
    {
        if (inputCalcVars[i].includeCalcVar)
            calcVarOK = calcVarOK && CheckInputFO(inputCalcVars[i].calcVarObject, "Calculated variable");
    }
}

int OptimizeInput::GetMaxNFitComponents()
{
    int retVal = 0;
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
        if (optimizeInput.inputFits[i].optimizeOnFit)
        {
            DFO_FitBase* currFit = static_cast<DFO_FitBase*>(optimizeInput.inputFits[i].fitObject.objRef);
            if (currFit)
            {
                int ncurrFit = currFit->GetFitResults().Size();
                retVal = IMax(retVal, ncurrFit);
            }
        }
    return retVal;
}


int OptimizeInput::GetNFits()
{
    int nfits = 0;
    for (int i = 0; i < maxOptFitSel; i++)
        if (inputFits[i].optimizeOnFit)
            nfits++;

    return nfits;
}

int OptimizeInput::GetnCalc()
{
    int ncalc = 0;
    for (int i = 0; i < maxOptCalcVar; i++)
        if (inputCalcVars[i].includeCalcVar)
            ncalc++;

    return ncalc;
}

void OptimizeInput::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();

    if (calcVarOK)
    {
        int ncalcVar = 0;
        for (int i = 0; i < maxOptCalcVar; i++)
        {
            if (inputCalcVars[i].includeCalcVar)
            {
                DO_CalcParReal& currVar = *(static_cast<DO_CalcParReal*>(GetInPortData(inputCalcVars[i].calcVarObject)));
                calcVarDesc[ncalcVar].SetVars(currVar.dataTypeLabel, currVar.shortID, currVar.unitText, currVar.isLinear);
                calcVarValue[ncalcVar] = currVar.realVal;
                ncalcVar++;
            }
        }
        calcVarDesc.SetSize(ncalcVar);
        calcVarValue.SetSize(ncalcVar);
    }
    else
    {
        calcVarDesc.SetEmpty();
        calcVarValue.SetEmpty();
    }

    if (StatusOK())
    {
        calcWasPerformed = true;
        lastFitVal = static_cast<DO_FitResults*>(GetInPortData(inputFits[currSelectedFit].fitObject))->realVal;
    }
}


FitResultsPtrArray& OptimizeInput::GetCurrentFitResults()
{
    return static_cast<DO_FitResults*>(GetInPortData(inputFits[currSelectedFit].fitObject))->fitResults;
}

int OptimizeInput::GetNCurrentFits()
{
    return static_cast<DO_FitResults*>(GetInPortData(inputFits[currSelectedFit].fitObject))->fitResults.Size();
}

const OptimizerPackage* OptimizeRuntimeOutput::currOptPkg = 0;

OptimizeRuntimeOutput::OptimizeRuntimeOutput() :
    GlobalFunc("OptOutput")
{
    optResultsDO.tableData = &optResultsDC;
    AddOutPort(optResultsDO);
    AddOutPort(lastFitDO);
    lastFitDO.SetTypeLabel("Last fit");
    lastFitDO.InitLabelAndValue(0.0);

    DoStatusChk();
}


int OptimizeRuntimeOutput::GetNTableCols()
{
    int ntableCols = OptVar::GetnActive()
                    + optimizeInput.calcVarDesc.Size() + 2;

    addSecondOptData = false;
    if (currOptPkg != 0)
    {
        ntableCols += currOptPkg->firstStageOpt->GetnBaseOutput();
        ntableCols += currOptPkg->firstStageOpt->GetnExtraOutput();
        if ((!singleStageOpt) && (firstStageOptType != secondStageOptType))
        {
            ntableCols += currOptPkg->secondStageOpt->GetnExtraOutput();
            addSecondOptData = true;
        }
    }
    return ntableCols;
}

void OptimizeRuntimeOutput::DoStatusChk()
{
    FuncObjC::DoStatusChk();

    int ntableCols = GetNTableCols();
    if (optResultsDC.GetNCols() != ntableCols)
    {
        optResultsDC.ResizableAlloc(ntableCols, 10, 40);
    }
    // reset titles
    int colNum = 0;
    optResultsDC.SetTitle(colNum++, "Index");
    optResultsDC.SetTitle(colNum++, "FitValue");

    if (currOptPkg != 0)
    {
        SC_StringArray optDescs;
        currOptPkg->firstStageOpt->GetBaseOutputID(optDescs);
        for (int i = 0; i < optDescs.Size(); i++)
            optResultsDC.SetTitle(colNum++, optDescs[i]);
        currOptPkg->firstStageOpt->GetExtraOutputID(optDescs);
        for (int i = 0; i < optDescs.Size(); i++)
            optResultsDC.SetTitle(colNum++, optDescs[i]);
        if (addSecondOptData)
        {
            currOptPkg->secondStageOpt->GetExtraOutputID(optDescs);
            for (int i = 0; i < optDescs.Size(); i++)
                optResultsDC.SetTitle(colNum++, optDescs[i]);
        }
    }

    optResultsDC.SetID("Results");

    OptVarArray currOptVar;
    for (int i = 0; i < currOptVar.Size(); i++)
        optResultsDC.SetTitle(colNum++, currOptVar[i]->GetShortID());

    for (int i = 0; i < optimizeInput.calcVarDesc.Size(); i++)
        optResultsDC.SetTitle(colNum++, optimizeInput.calcVarDesc[i].caseVarShortDesc);

}

void OptimizeRuntimeOutput::Clear()
{
    optResultsDC.ClearAll();
    DoStatusChk();
}


void OptimizeRuntimeOutput::Setup()
{
    DoStatusChk();
    optResultsDC.ClearData();
    PreCalcSetup();
}

void OptimizeRuntimeOutput::CalcOutput(FOcalcType  calcType)
{
//  bug check for full recalc
    if (optResultsDC.dataTable.IsEmpty())
    {
        optResultsDC.SetID("No results");
        return;
    }

//  test for initial run
    {
        int ntableCols = GetNTableCols();
        if (optResultsDC.GetNCols() != ntableCols)
            DoStatusChk();
    }

    lastFitDO.InitLabelAndValue(optimizeInput.GetFitValue());
    int colNum = 0;
    optResultsDC.dataTable[colNum++] += optResultsDC.dataTable[0].Size();
    optResultsDC.dataTable[colNum++] += optimizeInput.GetFitValue();

    if (currOptPkg != 0)
    {
        SC_DoubleArray optData;
        currOptPkg->firstStageOpt->GetBaseOutput(optData);
        for (int i = 0; i < optData.Size(); i++)
            optResultsDC.dataTable[colNum++] += optData[i];

        if (addSecondOptData)
        {
            currOptPkg->firstStageOpt->GetExtraOutput(optData);
            for (int i = 0; i < optData.Size(); i++)
                optResultsDC.dataTable[colNum++] += optData[i];

            currOptPkg->secondStageOpt->GetExtraOutput(optData);
            for (int i = 0; i < optData.Size(); i++)
                optResultsDC.dataTable[colNum++] += optData[i];
        }
        else
        {
            currOptPkg->lastOptRun->GetExtraOutput(optData);
            for (int i = 0; i < optData.Size(); i++)
                optResultsDC.dataTable[colNum++] += optData[i];
        }

    }

    OptVarArray currOptVar;
    for (int i = 0; i < currOptVar.Size(); i++)
        optResultsDC.dataTable[colNum++] += currOptVar[i]->GetFinalParEstimate();

    for (int i = 0; i < optimizeInput.calcVarDesc.Size(); i++)
        optResultsDC.dataTable[colNum++] += optimizeInput.calcVarValue[i];

}

// file output

int OptimizeFileOutput::GetNFits() const
{
    return optimizeInput.GetNFits();
}

void OptimizeFileOutput::SetupRunHeader(const char*                    runID,
                                        const OptSimResultsHeader&     inRunDesc,
                                        const CaseVarDescriptionArray& inCaseDesc,
                                        const Optimizer&               opt)
{
    runDesc  = inRunDesc;
    runHeader.caseDescriptions = inCaseDesc;

    // first the fits
    int nfits = GetNFits();

    // allocate basic ouput space
    nextCase.caseResults.AllocAndSetSize(nfits);
    runHeader.fitDescriptions.AllocAndSetSize(nfits);

    nfits = 0;
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
        if (optimizeInput.inputFits[i].optimizeOnFit)
        {
            // all objects descend from this
            DFO_FitBase* currFO = static_cast<DFO_FitBase*>(optimizeInput.inputFits[i].fitObject.objRef);

            // all the fit components for the object
            FitResultsPtrArray&  fitComp = currFO->GetFitResults();
            int nComp = fitComp.Size();

            // allocate basic ouput space
            nextCase.caseResults[nfits].fitComponents.AllocAndSetSize(nComp);

            //  grab description to fill
            OptFitDescription& currFit = runHeader.fitDescriptions[nfits];
            nfits++;

            // fill in basic info
            CopyString(currFit.fitID, currFO->GetID(), OptFitDescription::fitIDLen);  // ID
            currFO->GetFitScaling(currFit.fitScaling);  // scaling

            //  allocate for comp fill
            currFit.fitComponentDescription.AllocAndSetSize(nComp);

            for (int j = 0; j < nComp; j++)
            {
                OptFitComponentDescription& currCompDesc = currFit.fitComponentDescription[j];
                DC_FitResults& currCompDC = (*fitComp[j]);

                CopyString(currCompDesc.fitID, currCompDC.fitID, OptFitComponentDescription::fitIDLen);
                currCompDesc.nfitPoints     = currCompDC.nfitPoints;
                currCompDesc.fitChisqSigma  = currCompDC.sigma;
                currCompDesc.fitWasL1       = currCompDC.fitWasL1;
                currCompDesc.fitWasNormed   = currCompDC.fitWasNormed;

                if (runDesc.containsResiduals || runDesc.containsJacobian)
                    currCompDesc.fitResidJacobX = currCompDC.fitResiduals.xData;
            }
        }

    // now the vars
    runHeader.varDescriptions.AllocAndSetSize(opt.noptVars);
    for (int i = 0; i < opt.noptVars; i++)
    {
        OptVarDescription& currVar = runHeader.varDescriptions[i];
        OptVar& currOptVar = *(opt.varsToOptimize[i]);

        CopyString(currVar.varDesc, currOptVar.GetLongID(), OptVarDescription::varDescLen);
        CopyString(currVar.shortVarDesc, currOptVar.GetShortID(), OptVarDescription::shortVarDescLen);
        currOptVar.MakeUnitString(currVar.varUnits, OptVarDescription::varUnitsLen);

        double multVal      = currOptVar.GetFinalMultiplier();
        currVar.userMinLim  =  currOptVar.optMinLim * multVal;
        currVar.userMaxLim  =  currOptVar.optMaxLim * multVal;

        currVar.parSigma     = currOptVar.optParSigma;
        currVar.parIsLinear  = currOptVar.optIsLinear;
    }

    // calculated vars
    runHeader.calcDescriptions = optimizeInput.calcVarDesc;;

    //  complete run header info
    CopyString(runHeader.runID, runID, OptSimRunResults::runIDLen);
    if (singleStageOpt)
    {
        CopyString(runHeader.optDescription, optTypeNames.GetID(firstStageOptType), runHeader.optDescLen);
    }
    else
    {
        MakeString(runHeader.optDescription,
                        optTypeNames.GetID(firstStageOptType), "/",
                        optTypeNames.GetID(secondStageOptType),
                        runHeader.optDescLen);
    }
    runHeader.fullCovarianceCalc = use2ndOrderCovar;

    runHeader.nFits = nfits;
    runHeader.ncaseDesc = inCaseDesc.Size();

}

void OptimizeFileOutput::SetupCaseHeader(const char*           caseDesc,
                                         const SC_DoubleArray& inCaseVals,
                                              int             inCaseIndex)

{
    CopyString(nextCase.caseDesc, caseDesc,  OptCaseResults::caseDescLen);
    nextCase.caseParVals = inCaseVals;
    nextCase.caseIndex = inCaseIndex;
    nextFit = 0;
}

// adds results from last fit to nextCase
void OptimizeFileOutput::AddNextFit(bool fitAborted,
                                                const Optimizer& opt)
{
    OptSingleFit& currFit = nextCase.caseResults[nextFit++];
    DFO_FitBase& currFO = *(static_cast<DFO_FitBase*>(optimizeInput.inputFits[optimizeInput.currSelectedFit].fitObject.objRef));

    currFit.fitCompletedOK = !fitAborted;

    // from optimizer
    currFit.nsimsRequired  = opt.runCount;
    currFit.niteration     = opt.optIterCount;
    currFit.fitValue       = optimizeInput.GetFitValue();
    currFit.derivSpan      = opt.optDerivSpan;

    currFit.startNormParData = opt.startNormParEst;
    currFit.finalNormParData = opt.bestNormParEst;
    currFit.finalParData.AllocAndSetSize(opt.noptVars);
    currFit.startParData.AllocAndSetSize(opt.noptVars);
    currFit.fitDelta = 0.0;
    for (int i = 0; i < opt.noptVars; i++)
    {
        OptVar& currOptVar = *(opt.varsToOptimize[i]);
        double optMult = currOptVar.GetFinalMultiplier();
        currFit.finalParData[i] = opt.bestParEst[i] * optMult;
        currFit.startParData[i] = currOptVar.DeNormalize(opt.startNormParEst[i]) * optMult;
        currFit.fitDelta += fabs(opt.startNormParEst[i] - opt.bestNormParEst[i]);
    }

    currFit.calcParData = optimizeInput.calcVarValue;

    // set data in composite fits
    // all the fit components for the object
    FitResultsPtrArray&  fitComp = currFO.GetFitResults();
    int nComp = fitComp.Size();
    int ntotal = 0;
    currFit.sumSquaredError = 0.0;
    for (int i = 0; i < nComp; i++)
    {
        OptFitComponent& currComp = currFit.fitComponents[i];
        DC_FitResults& currCompDC = opt.multRunFitResults[i][0];  //(*fitComp[i]);

        currFit.sumSquaredError += currCompDC.sse;

        currComp.fitSumSquaredErrors  = currCompDC.sse;
        currComp.fitEstimatedVariance = currCompDC.estVar;
        currComp.nfitPoints           = currCompDC.nfitPoints;
        ntotal += currComp.nfitPoints;

        if (runDesc.containsCovariance)
        {
            opt.OptAllocMatrix(currComp.fitCovariance);
            opt.OptCopyMatrix(currComp.fitCovariance, opt.multFitHessianMatrix[i]);
        }

        if (runDesc.containsResiduals)
            currComp.fitResiduals     = currCompDC.fitResiduals.yData;

        if (runDesc.containsJacobian)
        {
            currComp.fitJacobian.AllocAndSetSize(opt.noptVars);
            SC_DoubleArray& resY = currCompDC.fitResults;
            for (int j = 0; j < opt.noptVars; j++)
            {
                currComp.fitJacobian[j].AllocAndSetSize(currCompDC.nfitPoints);
                SC_DoubleArray& currJac = currComp.fitJacobian[j];
                SC_DoubleArray& resDY = opt.multRunFitResults[i][j+1].fitDerivatives;
                for (int k  = 0; k < currCompDC.nfitPoints; k++)
                    currJac[k] = resDY[k] - resY[k];
            }
        }
    }

    // TODO -- check & clarify
    currFit.estimatedVariance = currFit.sumSquaredError / double (ntotal - nComp * opt.noptVars);
    currFit.meanSquaredError = currFit.sumSquaredError / double (ntotal - opt.noptVars);

    if (runDesc.containsCovariance)
    {
        opt.OptAllocMatrix(currFit.actualCovariance);
        opt.OptCopyMatrix(currFit.actualCovariance, opt.optActualCovar);
        opt.OptAllocMatrix(currFit.estimatedCovariance);
        opt.OptCopyMatrix(currFit.estimatedCovariance, opt.optEstimatedCovar);
    }

}

void OptimizeFileOutput::Cleanup()
{
    runHeader.Cleanup();
    nextCase.Cleanup();
}



OptimizeListing::OptimizeListing()
    :ListObjC("Optimizer")
{
}

void OptimizeListing::CreateListing()
{
    StdTitle("Optimization Setup");

    ClearEnumStrs();
    AddEnumStr("Simplex", otSimplex);
    AddEnumStr("Levenberg-Marquart", otLevMar);
    AddEnumStr("Simulated Annealing", otSimAnneal);
    AddEnumStr("SCEM",  otSCEM);

    if (singleStageOpt)
        AddStdEnum("Algorithm", firstStageOptType);
    else
    {
        AddStdEnum("First stage algorithm", firstStageOptType);
        AddStdEnum("Second stage algorithm", secondStageOptType);
    }


    AddYesNo("Calculate confidence limits ?", calcFinalCovariance);

    if (calcFinalCovariance)
    {
        AddBoolText("Covariance matrix calculations", use2ndOrderCovar, "2nd Order", "1st Order");
        AddYesNo("Fixed derivative span ?", useFixedDerivSpan);
        if (useFixedDerivSpan)
        {
            AddStdReal("Specified derivative span", fixedDerivSpan, SC_SciFormat(3));
        }
    }

    const char* tolStr = "Fit tolerance";
    const char* notUsedStr = "not used";
    if (useFitTolerance)
        AddStdReal(tolStr, fitTolerance, SC_SciFormat(4));
    else
        AddStdText(tolStr, notUsedStr);

    tolStr = "Parameter tolerance";
    if (useParameterTolerance)
        AddStdReal(tolStr, parameterTolerance, SC_SciFormat(4));
    else
        AddStdText(tolStr, notUsedStr);

/*  tolStr = "Error tolerance";
    if (useErrorTolerance)
        AddStdReal(tolStr, errorTolerance, SC_SciFormat(4));
    else
        AddStdText(tolStr, notUsedStr); */

    // TODO : add the rest

    AddStdInt("# of optimized variables", OptVar::GetnActive());

    OptVarArray currOptVar;
    SC_SetupErr tempErr;
    for (int i = 0; i < currOptVar.Size(); i++)
        AddStatus(currOptVar[i]->GetLongID(), currOptVar[i]->OptimizeSetupOK(tempErr));

    SubTitle("Fits to Optimize");
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        FitSelection& currFit = optimizeInput.inputFits[i];
        if (currFit.optimizeOnFit)
            if (currFit.fitObject.objRef == 0)
                AddStatus("No fits available/specified", false);
            else
                AddStatus(currFit.fitObject.objRef->GetID(), true);
    }

    SubTitle("Calculated Parameters Included");
    int ncalcPar = 0;
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        CalcVarSelection& currVar = optimizeInput.inputCalcVars[i];
        if (currVar.includeCalcVar)
        {
            if (currVar.calcVarObject.objRef == 0)
                AddStatus("No calc var value available/specified", false);
            else
                AddStatus(currVar.calcVarObject.objRef->GetID(), true);
            ncalcPar++;
        }
    }

    AddStdInt("# of calculated variables included", ncalcPar);

}

OptimizeErrorListing::OptimizeErrorListing()
    :ErrorListObjC("Optimizer")
{
}

void OptimizeErrorListing::CreateListing()
{
    StdTitle("Optimization Setup Errors");
    SC_SetupErr optErr;

    int nopt = OptVar::GetnActive();
    if (nopt < 2)
    {
        optErr.SetConstantError("at least 2 variables must be optimized");
        AddError(optErr);
    }

    if (!(useFitTolerance || useParameterTolerance))
    {
        optErr.SetConstantError("at least one termination criteria must be selected");
        AddError(optErr);
    }

    if (UseSimAnneal())
    {
        if (((coolingSchedule == csLuke8) || (coolingSchedule == csLuke9)) &&
            (saL_TN < stdEps))
        {
            optErr.SetConstantError("TN must be greater than 0 for SA Luke#8 or Luke#9");
            AddError(optErr);
        }
        if ((coolingSchedule == csLuke1) && ((saL_TN < stdEps) || (sa_T0 < stdEps)))
        {
            optErr.SetConstantError("TN and T0 must be greater than 0 for SA Luke#1");
            AddError(optErr);
        }
    }

    if (UseSCEM())
    {
        if (scem_m <= nopt)
        {
            optErr.SetConstantError("SCEM m must be greater than # of opt parameters");
            AddError(optErr);
        }
        if (scem_q > scem_m)
        {
            optErr.SetConstantError("SCEM q must be less than SCEM m");
            AddError(optErr);
        }
        if (scem_q < 2)
        {
            optErr.SetConstantError("SCEM q must be greater than 2");
            AddError(optErr);
        }
    }


    int nOpt = 0;
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        FitSelection& currFit =  optimizeInput.inputFits[i];
        if (currFit.optimizeOnFit)
        {
            if (currFit.fitObject.IsNull())
            {
                optErr.SetInternalError("no fit specified");
                AddError(optErr);
            }
            else
            {
                nOpt++;
                FitResultsPtrArray& fitList = static_cast<DO_FitResults*>(optimizeInput.GetInPortData(currFit.fitObject))->fitResults;
                if ( fitList.IsEmpty())
                {
                    optErr.SetInternalError(currFit.fitObject.objRef->GetID(), "no fits in input ??");
                    AddError(optErr);
                }
                for (int j = 0; j < fitList.Size(); j++)
                    if (!fitList[j]->okToAlloc)
                    {
                        optErr.SetInternalError(fitList[j]->fitID, "fit setup is bad - field input must be OK");
                        AddError(optErr);
                    }
            }
        }
    }

    if (nOpt == 0)
    {
        optErr.SetConstantError("no fits selected/specified");
        AddError(optErr);
    }

}

OptimizeStaticSupport::OptimizeStaticSupport()
{
//  SetToInitial();
}

void OptimizeStaticSupport::SetToInitial()
{
    singleStageOpt              = true;
    firstStageOptType           = otSimplex;
    secondStageOptType          = otLevMar;

    calcFinalCovariance         = true;

    use2ndOrderCovar     = false;
    useFixedDerivSpan    = false;

    derivativeTolerance           = 1.0E-05;
    fixedDerivSpan                = 1.0E-06;
    initialDerivSpan              = 1.0E-3;
    transitionToleranceMultiplier = 100.0;

    useFitTolerance         = true;
    useParameterTolerance   = false;
    combineToleranceWithOR  = true;
    parameterTolerance      = 1.0E-04;
    fitTolerance            = 1.0E-05;

//  removed Sep 05 at KV request
//  useErrorTolerance       = false;
//  errorTolerance          = 1.0E-04;

    simplexSpan          = 0.1;

    simplexReflectionCoeff          = 1.0;
    simplexContractionCoeff         = 0.5;
    simplexExpansionCoeff           = 3.5; // KV spec 7 Jul 05
    simplexShrinkageCoeff           = 0.5; // rev sep 04

    //L-M
    lmInitialDerivSpan         = 1.0E-08;
    lmInitialLambda            = 0.001;
    lmLambdaFactor             = 2.0;
    lmMinLambda                = 1.0E-08;
    lmRelChangeTolerance       = 1.0E-05;
    useLastLMForDerivativeCalc = true;
    lmLambda                   = 100.0;
    normalizeParMINPACK        = true;
    lmAdjustDerivSpan          = false;

    // sim anneal
    coolingSchedule      = csNR1;
    saRandomSeed         = 35471;
    sa_T0           = 100.0;// t0 for all
    sa_N            = 100;  // iteration budget


//  NR3 support removed Sep 05 by KV request
//  saNR_beta       = 0.9;  // beta for NR3
//  saNR_gamma      = 0.2;  // gamma for NR3

    saL_TN          = 0.0;      // tN for all Luke



    // SCEM
    scem_p = 2;             // # of complexes
    scem_m = 4;             // # of points in each complex
    scem_q = 3;             // # of points in each CCE subcomplex
    scem_alpha = 1;         // # of iterations in gen offspring -- KV spec 7 Jul 05
    scem_beta  = 2;         // # of iterations in CCE
    scem_seed  =  2976221;  // random seed

    doOptPerturbations       = false;
    perturbFromStart         = true;
    perturbMultiFitSameStart = false;
    nPerturb                = 50;
    perturbSpan             = 0.20;
    perturbRandomSeed       = 13597;

    startMultipleFromOriginalEst = true;
    updateParameterEstimates     = false;

    maxOptRun            = 10000;

    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        optimizeInput.inputFits[i] = FitSelection();
    }
    optimizeInput.inputFits[0].optimizeOnFit = true;

    optCompleteOutput.SetDefaultID("Perturb/Sample");
    optProgressOutput.SetDefaultID("OptProgress");

    optCompleteOutput.Clear();
    optProgressOutput.Clear();
}


void OptimizeStaticSupport::SetForFlags()
{
    UpdateStatus();
}

bool OptimizeStaticSupport::OptimizeOK()
{
    DFO_FitBase::useFitsForOptimization = true;

    bool retVal = false;
    if (optimizeErrorListing.ErrorCheckOK())
    {
        retVal = true;
        optCompleteOutput.DoStatusChk();
        optProgressOutput.DoStatusChk();

        // alloc all fits (may be duplicated)
        for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
            if (optimizeInput.inputFits[i].optimizeOnFit)
            {
                FitResultsPtrArray& fitList = static_cast<DO_FitResults*>(optimizeInput.GetInPortData(optimizeInput.inputFits[i].fitObject))->fitResults;
                for (int j = 0; j < fitList.Size(); j++)
                    retVal = retVal && fitList[j]->PreRunAlloc(true);
            }
    }
    return retVal;
}

bool OptimizeStaticSupport::FitCalcOK()
{
    DFO_FitBase::useFitsForOptimization = false;

    int nFit = 0;
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        FitSelection& currFit =  optimizeInput.inputFits[i];
        if (currFit.optimizeOnFit)
        {
            if (currFit.fitObject.IsNull())
                return false;

            nFit++;
            FitResultsPtrArray& fitList = static_cast<DO_FitResults*>(optimizeInput.GetInPortData(currFit.fitObject))->fitResults;
            if ( fitList.IsEmpty())
                return false;

            for (int j = 0; j < fitList.Size(); j++)
                if (!fitList[j]->PreRunAlloc(false))
                    return false;
        }
    }

    if (nFit == 0)
        return false;

    optCalculationRun = RequiresChisq();

    return true;
}


void OptimizeStaticSupport::UpdateStatus()
{
    optCompleteOutput.DoStatusChk();
    optProgressOutput.DoStatusChk();
}

appFuncObjGlobals::AppFuncArray OptimizeStaticSupport::optimizerObj("Optimizer");

void OptimizeStaticSupport::InitAppGlobals()
{
    optimizerObj.AddTo(&optimizeInput);
    optimizerObj.AddTo(&optCompleteOutput);
    optimizerObj.AddTo(&optProgressOutput);

    appFuncObjGlobals::appFuncData += &optimizerObj;
}


const char* OptimizeStaticSupport::groupHeader = "Optimize";
const int  OptimizeStaticSupport::majorVersion = 16;        // Switched out all minpack
//const int  OptimizeStaticSupport::majorVersion = 14;      // removed SA sched NR3 and Luke6
//const int  OptimizeStaticSupport::majorVersion = 13;      // added SCEM
//const int  OptimizeStaticSupport::majorVersion = 12;      // added perturb same start for multi
//const int  OptimizeStaticSupport::majorVersion = 11;      // added NR shrinkage
//const int  OptimizeStaticSupport::majorVersion = 10;      // sim anneal B
//const int  OptimizeStaticSupport::majorVersion = 9;       // sim anneal
//const int  OptimizeStaticSupport::majorVersion = 8;       // restructered opt type
//const int  OptimizeStaticSupport::majorVersion = 7;       // added simplex is NR
//const int  OptimizeStaticSupport::majorVersion = 6;   // added parameters for Kay - 11 Aug 04
//const int  OptimizeStaticSupport::majorVersion = 5;   // added combined type
//const int  OptimizeStaticSupport::majorVersion = 4;   // added calc vars
//const int  OptimizeStaticSupport::majorVersion = 3;   // revised system object approach
//const int  OptimizeStaticSupport::majorVersion = 2;   // added perturb random seed
//const int  OptimizeStaticSupport::majorVersion = 1;   // added more LM control
//const int  OptimizeStaticSupport::majorVersion = 0;
const int  OptimizeStaticSupport::minorVersion = 0;

void OptimizeStaticSupport::WriteToFile()
{
    WriteGroupHeader(groupHeader, majorVersion, minorVersion);

    // global objects
    WriteGlobalFunc(optimizeInput);
    WriteGlobalFunc(optCompleteOutput);
    WriteGlobalFunc(optProgressOutput);

    //  static control variables basic -- changed v8 - 16 Aug 04
    WriteBool(singleStageOpt);          // added v8
    WriteInt(int(firstStageOptType));   //
    WriteInt(int(secondStageOptType));  // added v8

    WriteBool(calcFinalCovariance);
    WriteBool(use2ndOrderCovar);
    WriteBool(useFixedDerivSpan);
    WriteLine();

    SetRealConvFormat(SC_SciFormat(8));
            //  general control
//    WriteDouble(parameterTolerance); moved below v6
    WriteDouble(derivativeTolerance);
    WriteDouble(initialDerivSpan);  // added v10
    WriteDouble(fixedDerivSpan);
    WriteDouble(transitionToleranceMultiplier); // added v5
    WriteLine();

    // added v6
    WriteBool(useFitTolerance);
    WriteDouble(fitTolerance);
    WriteBool(useParameterTolerance);
    WriteDouble(parameterTolerance);
//  WriteBool(useErrorTolerance);   - dropped v14
//  WriteDouble(errorTolerance);    - dropped v14
    WriteBool(combineToleranceWithOR);
    WriteLine();

            // multiple optimization control
    WriteBool(startMultipleFromOriginalEst);
    WriteBool(updateParameterEstimates);
    WriteLine();

    WriteDouble(simplexSpan);
//    WriteDouble(simplexDerivSpan); // added v6 - wasn't written before ??
//  removed v10

    // all added v6
    WriteDouble(simplexReflectionCoeff);
    WriteDouble(simplexContractionCoeff);
    WriteDouble(simplexExpansionCoeff);
    WriteDouble(simplexShrinkageCoeff);   // added v11
//  WriteBool(restartSimplexAtFirstSolution); removed v8
    WriteLine();

            //  Lev-Mar control
    WriteDouble(lmInitialDerivSpan);
    WriteDouble(lmInitialLambda);       // added v1
    WriteDouble(lmLambdaFactor);
    WriteDouble(lmMinLambda);
    WriteDouble(lmRelChangeTolerance);
    WriteBool(useLastLMForDerivativeCalc); // v10
    WriteDouble(lmLambda);              // added v15
    WriteBool(normalizeParMINPACK);     // added v15
    WriteBool(lmAdjustDerivSpan);       // added v15
    WriteInt(maxOptRun);
    WriteLine();

    // simulated annealing - added v9
    WriteInt(int(coolingSchedule));  // mod v14
    WriteInt(saRandomSeed);
    WriteInt(sa_N);

    WriteDouble(sa_T0);

//  NR3 support removed Sep 05 by KV request
//    WriteDouble(saNR_beta);
//    WriteDouble(saNR_gamma);

    WriteDouble(saL_TN);
    WriteLine();

    // SCEM added v13
    WriteInt(scem_p);
    WriteInt(scem_m);
    WriteInt(scem_q);
    WriteInt(scem_alpha);
    WriteInt(scem_beta);
    WriteInt(scem_seed);


            // Perturbation
    WriteBool(doOptPerturbations);  // added V1
    WriteBool(perturbFromStart);
    WriteBool(perturbMultiFitSameStart); // added v12
    WriteInt(nPerturb);
    WriteDouble(perturbSpan);
    WriteInt(perturbRandomSeed);   // added v2
    WriteLine();

    // selected fits
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        WriteBool(optimizeInput.inputFits[i].optimizeOnFit);
        WriteFuncObjRef(optimizeInput.inputFits[i].fitObject);
    }

    // calculated vars -- added V4
    for (int i = 0; i < OptimizeInput::maxOptCalcVar; i++)
    {
        WriteBool(optimizeInput.inputCalcVars[i].includeCalcVar);
        WriteFuncObjRef(optimizeInput.inputCalcVars[i].calcVarObject);
    }

}

OptimizationControlStatics::OptimizationType OptimizeStaticSupport::ReadOptType()
{
    OptimizationType optType;
    if (currentObjReadMajorVersion > 14)  //added MINPACK LM v15
    {
        optType = OptimizationType(ReadInt());
    }
    else
    {
        //MINPACK LM placed in middle of enum (third or index 2)
        int temp = ReadInt();
        if (temp > 1)
            temp++;
        optType = OptimizationType(temp);
    }
    return optType;
}

bool OptimizeStaticSupport::ReadFromFile()
{
    ReadGroupHeader(groupHeader, majorVersion);

    if (currentObjReadMajorVersion > 2)
    {
        // global objects
        ReadGlobalFunc(optimizeInput);
        ReadGlobalFunc(optCompleteOutput);
        ReadGlobalFunc(optProgressOutput);
    }

    //  static control variables basic -- changed v8 - 16 Aug 04
    if (currentObjReadMajorVersion > 7)
    {
        singleStageOpt = ReadBool();
        firstStageOptType = ReadOptType();
        secondStageOptType = ReadOptType();
    }
    else
    {
        firstStageOptType = ReadOptType();
        singleStageOpt = (firstStageOptType != otSimAnneal); // was otCombined
        if (!singleStageOpt)
        {
            // old combined
            firstStageOptType = otSimplex;
            secondStageOptType = otLevMar;
        }
    }

    calcFinalCovariance        = ReadBool();
    use2ndOrderCovar    = ReadBool();
    useFixedDerivSpan   = ReadBool();

            //  general control
    if (currentObjReadMajorVersion < 6)
        fitTolerance  = ReadDouble();  // was parameterTolerance - misnamed
    derivativeTolerance = ReadDouble();
    if (currentObjReadMajorVersion > 9)
        initialDerivSpan = ReadDouble();
    fixedDerivSpan      = ReadDouble();
    if (currentObjReadMajorVersion > 4)
    {
        transitionToleranceMultiplier = ReadDouble();
        if (currentObjReadMajorVersion < 6)
            //  was absolute fitTolerance - changed to multiplier in v6
            transitionToleranceMultiplier = transitionToleranceMultiplier / fitTolerance;
    }

    // new tolerance control added v6 - Aug 04
    if (currentObjReadMajorVersion > 5)
    {
        useFitTolerance = ReadBool();
        fitTolerance = ReadDouble();
        useParameterTolerance = ReadBool();
        parameterTolerance = ReadDouble();

        // dropped v14
        if (currentObjReadMajorVersion < 14)
        {
            double useErrorTolerance = ReadBool();
            double errorTolerance = ReadDouble();
        }

        combineToleranceWithOR = ReadBool();
    }
            // multiple optimization control
    startMultipleFromOriginalEst = ReadBool();
    updateParameterEstimates     = ReadBool();


    //  ***************  simplex control

    if (currentObjReadMajorVersion > 6 && currentObjReadMajorVersion < 16)
        ReadBool();

    simplexSpan = ReadDouble();
    // new simplex control added v6 - Aug 04
    if (currentObjReadMajorVersion > 5)
    {
        if (currentObjReadMajorVersion < 10)
            double simplexDerivSpan = ReadDouble(); // added v6 - removed v10

        // all added v6
        simplexReflectionCoeff = ReadDouble();
        simplexContractionCoeff = ReadDouble();
        simplexExpansionCoeff = ReadDouble();

        if (currentObjReadMajorVersion > 10)
            simplexShrinkageCoeff = ReadDouble();   // added v11

        //  removed v8
        if (currentObjReadMajorVersion < 8)
            bool restartSimplexAtFirstSolution = ReadBool();
    }

            //  Lev-Mar control
    lmInitialDerivSpan     = ReadDouble();
    if (currentObjReadMajorVersion > 0)
        lmInitialLambda = ReadDouble();

    lmLambdaFactor  = ReadDouble();
    lmMinLambda     = ReadDouble();
    lmRelChangeTolerance = ReadDouble();

    if (currentObjReadMajorVersion > 9)
        useLastLMForDerivativeCalc = ReadBool(); // v10

    if (currentObjReadMajorVersion > 14)
    {
        lmLambda = ReadDouble();            // v15
        normalizeParMINPACK = ReadBool();   // v15
        lmAdjustDerivSpan = ReadBool();     //v15
    }

    maxOptRun       = ReadInt();

    // simulated annealing - added v9
    if (currentObjReadMajorVersion > 8)
    {
        if (currentObjReadMajorVersion < 14)
        {
            enum OldCoolingSchedule     {ocsNR1, ocsNR2,
                                         ocsNR3, //removed Sep05 by KV request
                                         ocsLuke1, ocsLuke2,
                                         ocsLuke6, //removed Sep05 by KV request
                                         ocsLuke7, ocsLuke8, ocsLuke9};
            OldCoolingSchedule oldSchedule = OldCoolingSchedule(ReadInt());
            switch (oldSchedule) {
                case ocsNR1 : {coolingSchedule = csNR1; break;}
                case ocsNR2 : {coolingSchedule = csNR2; break;}
                case ocsNR3 : {coolingSchedule = csNR1; break;} // replacement
                case ocsLuke1 : {coolingSchedule = csLuke1; break;}
                case ocsLuke2 : {coolingSchedule = csLuke2; break;}
                case ocsLuke6 : {coolingSchedule = csLuke1; break;} // replacement
                case ocsLuke7 : {coolingSchedule = csLuke7; break;}
                case ocsLuke8 : {coolingSchedule = csLuke8; break;}
                case ocsLuke9 : {coolingSchedule = csLuke9; break;}
            }
        }
        else
            coolingSchedule = CoolingSchedule(ReadInt());

        saRandomSeed = ReadInt();
        sa_N = ReadInt();
        if (currentObjReadMajorVersion < 16)
		{
            ReadInt();
		}

        sa_T0 = ReadDouble();
        if (currentObjReadMajorVersion < 16)
		{
            ReadDouble();
            ReadDouble();
		}

        if (currentObjReadMajorVersion < 14)
        {
            //  NR3 support removed Sep 05 by KV request
            double saNR_beta = ReadDouble();
            double saNR_gamma = ReadDouble();
        }
        saL_TN = ReadDouble();
    }

    // SCEM - added v13
    if (currentObjReadMajorVersion > 12)
    {
        scem_p = ReadInt();
        scem_m = ReadInt();
        scem_q = ReadInt();
        scem_alpha = ReadInt();
        scem_beta = ReadInt();
        scem_seed = ReadInt();
    }

    // minpack ripped out...these NL2SOL variables are no longer relevent
    if (currentObjReadMajorVersion == 15)
    {
        ReadDouble();
        ReadDouble();
        ReadDouble();
        ReadBool();
    }
            // Perturbation
    if (currentObjReadMajorVersion > 0)
        doOptPerturbations = ReadBool();  // added V1

    perturbFromStart    = ReadBool();
    if (currentObjReadMajorVersion > 11)
        perturbMultiFitSameStart = ReadBool(); // added v12

    nPerturb            = ReadInt();
    perturbSpan         = ReadDouble();
    if (currentObjReadMajorVersion > 1)
        perturbRandomSeed = ReadInt();   // added v2


    // selected fits
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
        optimizeInput.inputFits[i].optimizeOnFit = ReadBool();
        ReadFuncObjRef(optimizeInput.inputFits[i].fitObject);
    }

    if (currentObjReadMajorVersion > 3)
    {
        for (int i = 0; i < OptimizeInput::maxOptCalcVar; i++)
        {
            optimizeInput.inputCalcVars[i].includeCalcVar = ReadBool();
            ReadFuncObjRef(optimizeInput.inputCalcVars[i].calcVarObject);
        }
    }

    return true;
}


};                                      // end namespace

