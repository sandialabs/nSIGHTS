///////////////////////////////////////////////////////////////////////////////////
//
// G_Sample.cpp
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
//      support for sampler usage.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>


#ifndef ADCONSOLEAPP
#include <genPlotClass/C_PlotDef.h>
#endif

#include <genApp/AppConfigFile.h>

#include <osMain/osConfigFile.h>
#include <osMain/C_Sample.h>

#include <osMain/G_Sample.h>

namespace osSample  {

    SampleErrorListing      sampleErrorListing;

    SampleStaticSupport     sample;
    SampleResults           sampResultsFO;
    UncertInput             uncertInput;
    UncertResults           uncertResultsFO;

    //////////////////////////////////////
    UncertInput::UncertInput() :
        GlobalFunc("UncertInput")
    {
        for (int i = 0; i < maxMetricSel; i++)
        {
            AddUseFlagInPort(inputMetrics[i].metricObject, typeid(DO_Real), inputMetrics[i].useMetric);
        }
        DoStatusChk();
    }

    void  UncertInput::IsBeingDeleted(FuncObjC* delObj)
    {
        for (int i = 0; i < maxMetricSel; i++)
        {
            if (inputMetrics[i].metricObject.objRef == delObj)
            {
                inputMetrics[i].uncertOnMetric = false;
                inputMetrics[i].metricObject = FuncObjRef();
            }
        }
    }

    void UncertInput::DoStatusChk()
    {
        for (int i = 0; i < maxMetricSel; i++)
            inputMetrics[i].useMetric = (!SampleControlStatics::IsMonteCarlo() && inputMetrics[i].uncertOnMetric);

        int numMetrics = GetNMetrics();
        if (numMetrics < 1)
            SetObjErrMsg("no metrics selected ???");

        /*MetricSelection& currMetric = inputMetrics[currSelectedMetric];
        if (!currMetric.uncertOnMetric)
            SetObjErrMsg("current metric not selected ???");*/

        FuncObjC::DoStatusChk();

        for (int i = 0; i < maxMetricSel; i++)
        {
            if (!inputMetrics[i].useMetric)
                continue;
            if (!CheckInputFO(inputMetrics[i].metricObject, "Selected metric"))
            {
                if (!inputMetrics[i].metricObject.IsNull())
                {
                    char errMsg[255];
                    MakeString(errMsg, "SelMetricErr:", inputMetrics[i].metricObject.objRef->GetID(), ":", inputMetrics[i].metricObject.objRef->GetStatusMsg(), 255);
                    SetObjErrMsg(errMsg);
                }
            }
        }


        lastMetricVal.Alloc(numMetrics);
    }

    int UncertInput::GetNMetrics()
    {
        int nmetrics = 0;
        for (int i = 0; i < maxMetricSel; i++)
            if (inputMetrics[i].uncertOnMetric)
                nmetrics++;

        return nmetrics;
    }

    void UncertInput::CalcOutput(FOcalcType  calcType)
    {
        DoStatusChk();

        if (StatusOK())
        {
            calcWasPerformed = true;
            //lastMetricVal = static_cast<DO_Real*>(GetInPortData(inputMetrics[currSelectedMetric].metricObject))->realVal;
            lastMetricVal.SetEmpty();
            for (int i = 0; i < maxMetricSel; i++)
            {
                if (inputMetrics[i].uncertOnMetric)
                    lastMetricVal += static_cast<DO_Real*>(GetInPortData(inputMetrics[i].metricObject))->realVal;
            }
        }
    }
    //////////////////////////////////////
    SampleResults::SampleResults() :
        GlobalFunc("SampleOutput")
    {
        sampResultsDO.tableData = &sampResultsDC;
        AddOutPort(sampResultsDO);

        DoStatusChk();
    }


    void SampleResults::DoStatusChk()
    {
        FuncObjC::DoStatusChk();

        if (!SampleControlStatics::IsMonteCarlo())
        {
            sampResultsDC.ClearAll();
            sampResultsDC.SetID("no samples");
        }
        else if (!Sampler::SamplesCurrent())
        {
            SetObjErrMsg("Sample data unavailable or out of date");
            sampResultsDC.ClearAll();
            sampResultsDC.SetID("no samples");
        }
        else
            sampResultsDC.SetID("samples");

    }


    void SampleResults::CalcOutput(FOcalcType  calcType)
    {
        DoStatusChk();
        if (StatusNotOK())
            return;

        if (!SampleControlStatics::IsMonteCarlo())
            return;

        SampVarArray currSamp;
        if (!sampResultsDC.Alloc(currSamp.Size(), SampVar::numberOfTrials, 80))
        {
            SetObjErrMsg("Table allocation error");
            return;
        }

        for (int i = 0; i < currSamp.Size(); i++)
        {
            SampVar& currV = *(currSamp[i]);
            SC_DoubleArray& currD = sampResultsDC.dataTable[i];
            sampResultsDC.SetTitle(i, currV.GetShortID());
            for (int j = 0; j < SampVar::numberOfTrials; j++)
                currD[j] = currV.GetFinalValue(j);
            currD.SetSize(SampVar::numberOfTrials);
        }
        sampResultsDC.dataTable.SetSize(currSamp.Size());
    }

    ///////////////////////////////////

    const UncertaintyPropagationPackage* UncertResults::currUncertPkg = 0;

    UncertResults::UncertResults() :
        GlobalFunc("UncertaintyOutput")
    {
        uncertInputDO.tableData = &uncertInputDC;
        AddOutPort(uncertInputDO);

        uncertResultsDO.tableData = &uncertResultsDC;
        AddOutPort(uncertResultsDO);

        uncertComponentResultsDO.tableData = &uncertComponentResultsDC;
        AddOutPort(uncertComponentResultsDO);

        DoStatusChk();
    }


    void UncertResults::DoStatusChk()
    {
        FuncObjC::DoStatusChk();

        //input
        nVar = SampVar::GetnActive();
        if (SampleControlStatics::IsMonteCarlo() || (nVar == 0))
        {
            uncertInputDC.SetID("no variables");
            uncertInputDC.ClearData();
        }
        else
        {
            if (uncertInputDC.GetNRows() != nVar)
            {
                uncertInputDC.ResizableAlloc(2, nVar, 40);
            }
            uncertInputDC.SetID("variables");
            uncertInputDC.SetTitle(0, "Mean");
            uncertInputDC.SetTitle(1, "StdDev");
        }
        //output

        nMet = uncertInput.GetNMetrics();
        if (SampleControlStatics::IsMonteCarlo() || (nMet == 0))
        {
            uncertResultsDC.SetID("no results");
            uncertResultsDC.ClearData();

            if (!SampleControlStatics::IsMonteCarlo())
            {
                SetObjErrMsg("no metrics set for FOSM/PEM");
                return;
            }
        }
        else
        {
            if (uncertResultsDC.GetNRows() != nMet)
            {
                uncertResultsDC.ResizableAlloc(2, nMet, 40);
            }
            uncertResultsDC.SetID("Results");
            uncertResultsDC.SetTitle(0, "Mean");
            uncertResultsDC.SetTitle(1, "StdDev");
        }

        //component output
        if (SampleControlStatics::IsMonteCarlo() || (nVar == 0) || (nMet == 0))
        {
            uncertComponentResultsDC.SetID("no variables or metrics");
            uncertComponentResultsDC.ClearData();
        }
        else
        {
            int nrows = nVar * nMet;
            if (uncertComponentResultsDC.GetNRows() != nrows)
            {
                if (SampleControlStatics::IsFOSM())
                    uncertComponentResultsDC.ResizableAlloc(3, nrows, 40);
                else
                    uncertComponentResultsDC.ResizableAlloc(2, nrows, 40);
            }
            uncertComponentResultsDC.SetID("Component Results");
            uncertComponentResultsDC.SetTitle(0, "Mean Component");
            uncertComponentResultsDC.SetTitle(1, "Variance Component");
            if (SampleControlStatics::IsFOSM())
                uncertComponentResultsDC.SetTitle(2, "Derivative");
        }
    }


    void UncertResults::CalcOutput(FOcalcType  calcType)
    {
        DoStatusChk();
        if (StatusNotOK())
            return;

        if (SampleControlStatics::IsMonteCarlo())
            return;

        uncertInputDC.ClearData();
        if (currUncertPkg != 0)
        {
            if (nVar == currUncertPkg->uncertProp->stdDevVarValues.Size())  //check to make sure we've got data
            {
                uncertInputDC.SetID("variables");
                for (int i = 0; i < nVar; i++)
                {
                    uncertInputDC.dataTable[0] += currUncertPkg->uncertProp->meanVarValues[i];
                    uncertInputDC.dataTable[1] += currUncertPkg->uncertProp->stdDevVarValues[i];
                    uncertInputDC.rowDesc += currUncertPkg->uncertProp->uncertainVars[i]->GetShortID();
                }
            }
        }

        for (int i = 0; i < 2; i++) //check each column to see if empty - should be full if there is data
        {
            if (uncertInputDC.dataTable[i].IsEmpty())
            {
                uncertInputDC.SetID("no variables");
                break;
            }
        }

        uncertResultsDC.ClearData();
        uncertResultsDC.rowDesc.SetEmpty();
        if (currUncertPkg != 0)
        {
            if (nMet == currUncertPkg->uncertProp->meanMetricValues.Size())
            {
                for (int i = 0; i < nMet; i++)
                {
                    uncertResultsDC.dataTable[0] += currUncertPkg->uncertProp->meanMetricValues[i];
                    uncertResultsDC.dataTable[1] += currUncertPkg->uncertProp->stdDevMetricValues[i];
                    uncertResultsDC.rowDesc += uncertInput.inputMetrics[i].metricObject.objRef->GetID();
                }
            }
        }

        for (int i = 0; i < 2; i++)  //check each column to see if empty - should be full if there is data
        {
            if (uncertResultsDC.dataTable[i].IsEmpty())
            {
                uncertResultsDC.SetID("no results");
                break;
            }
        }

        //component results
        uncertComponentResultsDC.ClearData();
        uncertComponentResultsDC.rowDesc.SetEmpty();
        if (currUncertPkg != 0)
        {
            if ((nVar == currUncertPkg->uncertProp->stdDevComponentValues.Size())  //check to make sure we've got data
                && (nMet == currUncertPkg->uncertProp->meanComponentValues[0].AllocSize()))
            {
                for (int i = 0; i < nVar; i++)
                {
                    for (int j = 0; j < nMet; j++)
                    {
                        if (SampleControlStatics::IsFOSM())
                        {
                            uncertComponentResultsDC.dataTable[0] += nullReal;
                            uncertComponentResultsDC.dataTable[2] += currUncertPkg->uncertProp->derivValues[i][j];
                        }
                        else
                            uncertComponentResultsDC.dataTable[0] += currUncertPkg->uncertProp->meanComponentValues[i][j];
                        uncertComponentResultsDC.dataTable[1] += currUncertPkg->uncertProp->stdDevComponentValues[i][j];
                        char rowID[40];
                        MakeString(rowID, currUncertPkg->uncertProp->uncertainVars[i]->GetShortID(),
                            "-", uncertInput.inputMetrics[j].metricObject.objRef->GetID(), 40);
                        uncertComponentResultsDC.rowDesc += rowID;
                    }
                }
            }
        }

        for (int i = 0; i < 2; i++)  //check each column to see if empty - should be full if there is data
        {
            if (uncertComponentResultsDC.dataTable[i].IsEmpty())
            {
                uncertComponentResultsDC.SetID("no results");
                break;
            }
        }
    }
    /////////////////////////////

    SampleListing::SampleListing()
        :ListObjC("Sample")
    {
    }

    void SampleListing::CreateListing()
    {
        StdTitle("Uncertainty Propogation Setup");

        ClearEnumStrs();
        AddEnumStr("Monte Carlo", utMonteCarlo);
        AddEnumStr("FOSM", utFOSM);
        AddEnumStr("PEM", utPEM);

        AddStdEnum("Uncertainty", uncertType);
        AddBoolText("Unspecified correlations", forceCorrelationsToZero, "Force to 0.0", "Random");

        if (IsMonteCarlo())
        {
            AddBoolText("Sampling procedure", samplingIsLHS, "Latin Hyper-Cube", "Monte-Carlo");
            AddStdInt("Number of trials", numberOfTrials);
            AddStdInt("Random number seed", randSeed);

            AddYesNo("User-specified correlations ?", specifyCorrelations);
                }
        else if (IsFOSM())
        {
            AddBoolText("Derivative Span", calcDerivSpan, "Calculated", "User-Specified");
            if (calcDerivSpan)
                AddStdReal("Initial Derivative Span", derivSpan);
            else
                AddStdReal("Specified Derivative Span", derivSpan);
        }

        AddStdInt("# of sampled variables", SampVar::GetnActive());

        SC_SetupErr tempErr;
        SampVarArray currSamp;
        for (int i = 0; i < currSamp.Size(); i++)
            AddStatus(currSamp[i]->GetLongID(), currSamp[i]->SampleSetupOK(tempErr));
    }

    SampleErrorListing::SampleErrorListing()
        :ErrorListObjC("Sample")
    {
    }

    void SampleErrorListing::CreateListing()
    {
        StdTitle("Sampler Setup Errors");
        SC_SetupErr sampErr;

        SampVarArray currSamp;
        if (currSamp.Size() < 1)
        {
            sampErr.SetConstantError("at least 1 variable must be sampled");
            AddError(sampErr);
        }

        for (int i = 0; i < currSamp.Size(); i++)
            if (!currSamp[i]->SampleSetupOK(sampErr))
                AddError(sampErr);

        //for FOSM or PEM - check to make sure all vars belong to same corr group
        if (!IsMonteCarlo())
        {
            int uncertCorrGroup = currSamp[0]->GetCorrGroup();
            if (!forceCorrelationsToZero)
            {
                //case where no correlations are set
                if ((currSamp[0]->GetnCorrGroup(uncertCorrGroup) > 1) && (currSamp[0]->correlations.Size() == 0))
                    AddError("correlations not set or forced to zero");
            }
            for (int i = 1; i < currSamp.Size(); i++)
            {
                int currCorrGroup = currSamp[i]->GetCorrGroup();
                if (currCorrGroup != uncertCorrGroup)
                    AddError("FOSM/PEM: all uncertain vars must belong to same correlation group");
                if (!forceCorrelationsToZero)
                {
                    //case where individual correlations are not set
                    for (int j = 0; j < currSamp[i]->correlations.Size(); j++)
                    {
                        if (RealIsNull(currSamp[i]->correlations[j].correlationValue))
                            AddError("correlations not set or forced to zero");
                    }
                }
            }
        }
    }



    SampleStaticSupport::SampleStaticSupport()
    {
    }

    void SampleStaticSupport::SetToInitial()
    {
        uncertType                = utMonteCarlo;

        //Monte Carlo
        numberOfTrials            = 100;
        randSeed                  = 34969827;

        samplingIsLHS             = true;
        specifyCorrelations       = true;
        forceCorrelationsToZero   = false;

        //FOSM
        calcDerivSpan             = false;
        derivSpan                 = 1.0E-2;

        sampResultsFO.SetDefaultID("Sample");
        sampResultsFO.DoStatusChk();

        uncertResultsFO.SetDefaultID("Uncertainty");
        uncertResultsFO.DoStatusChk();
    }


    void SampleStaticSupport::SetForFlags()
    {
    }

    bool SampleStaticSupport::SampleOK()
    {
        bool retVal = false;
        if (sampleErrorListing.ErrorCheckOK())
        {
            retVal = true;
            sampResultsFO.DoStatusChk();
            uncertResultsFO.DoStatusChk();
        }
        return retVal;
    }

    bool SampleStaticSupport::Uncertain()
    {
        if (SampleOK())
        {
    #ifndef ADCONSOLEAPP
            PlotDefC::ExecuteAndReplot(uncertResultsFO);
    #endif
            return true;
        }
        else
        {
            GenAppErrorMsg("Uncertain", "Uncertain setup bad");
            return false;
        }
    }

    void SampleStaticSupport::CopyUncertainTables(DC_TableData&  outputVarInp,
                                                  DC_TableData&  outputRes,
                                                  DC_TableData&  outputComp)
    {
        uncertResultsFO.GetInput(outputVarInp);
        uncertResultsFO.GetResults(outputRes);
        uncertResultsFO.GetComponents(outputComp);
    }

    bool SampleStaticSupport::Sample()
    {
        bool retVal = false;
        if (SampleOK())
        {
            SC_SetupErr sampErr;
            retVal = Sampler::DoSampling(sampErr);

    #ifndef ADCONSOLEAPP
            PlotDefC::ExecuteAndReplot(sampResultsFO);
    #endif

            if (!retVal)
                GenAppErrorMsg("Sampler", sampErr.GetErrorText());
        }
        else
            GenAppErrorMsg("Sampler", "Sample setup bad");

        return retVal;
    }


    appFuncObjGlobals::AppFuncArray SampleStaticSupport::sampleObj("Sampler");

    void SampleStaticSupport::InitAppGlobals()
    {
        sampleObj.AddTo(&uncertInput);
        sampleObj.AddTo(&sampResultsFO);
        sampleObj.AddTo(&uncertResultsFO);
        appFuncObjGlobals::appFuncData += &sampleObj;
    }

    const char* SampleStaticSupport::groupHeader = "Sampling";
    const int  SampleStaticSupport::majorVersion = 3;  // added uncertainty stuff
    //const int  SampleStaticSupport::majorVersion = 2;  // added single run selector
    //const int  SampleStaticSupport::majorVersion = 1;  // added global obj
    //const int  SampleStaticSupport::majorVersion = 0;
    const int  SampleStaticSupport::minorVersion = 0;

    void SampleStaticSupport::WriteToFile()
    {
        WriteGroupHeader(groupHeader, majorVersion, minorVersion);

        // global objects
        WriteGlobalFunc(sampResultsFO);
        WriteGlobalFunc(uncertInput);       //added v3
        WriteGlobalFunc(uncertResultsFO);   //added v3

        //  static control variables basic
        WriteInt(int(uncertType));  //added v3
        WriteBool(samplingIsLHS);
        WriteBool(specifyCorrelations);
        WriteBool(forceCorrelationsToZero);
        WriteInt(numberOfTrials);
        WriteInt(randSeed);
        WriteInt(singleRunRealization);
        WriteBool(calcDerivSpan);   //added v3
        WriteDouble(derivSpan);     //added v3
        WriteLine();

        // selected metrics - added v3
        for (int i = 0; i < UncertInput::maxMetricSel; i++)
        {
            WriteBool(uncertInput.inputMetrics[i].uncertOnMetric);
            WriteFuncObjRef(uncertInput.inputMetrics[i].metricObject);
        }
        WriteLine();

    }

    void  SampleStaticSupport::PreReadSetup()
    {
        SampVarPreReadSetup();
    }

    bool SampleStaticSupport::ReadFromFile()
    {
        ReadGroupHeader(groupHeader, majorVersion);

        if (currentObjReadMajorVersion > 0)
        {
            // global objects
            ReadGlobalFunc(sampResultsFO);
        }

        if (currentObjReadMajorVersion > 2)
        {
            ReadGlobalFunc(uncertInput);
            ReadGlobalFunc(uncertResultsFO);
            uncertType = UncertType(ReadInt());
        }

        //  static control variables basic
        samplingIsLHS           = ReadBool();
        specifyCorrelations     = ReadBool();
        forceCorrelationsToZero = ReadBool();
        numberOfTrials          = ReadInt();
        randSeed                = ReadInt();
        if (currentObjReadMajorVersion > 1)
            singleRunRealization = ReadInt();

        if (currentObjReadMajorVersion > 2)
        {
            calcDerivSpan = ReadBool();
            derivSpan = ReadDouble();

            for (int i = 0; i < UncertInput::maxMetricSel; i++)
            {
                uncertInput.inputMetrics[i].uncertOnMetric = ReadBool();
                ReadFuncObjRef(uncertInput.inputMetrics[i].metricObject);
            }
        }

        SampVarPostReadFixup();


        return true;
    }


};                                      // end namespace


