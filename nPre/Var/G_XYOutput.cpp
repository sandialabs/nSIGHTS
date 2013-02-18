///////////////////////////////////////////////////////////////////////////////////
//
// G_XYOutput.cpp
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

#include "StdAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/C_ConvFile.h>

#include <Var/G_Control.h>
#include <Var/G_DataCapture.h>
#include <Var/G_OutputFiles.h>

#include <osMain/G_Optimize.h>
#include <osMain/G_Vary.h>

#include <Var/G_XYOutput.h>

namespace nsOutputFiles  {


    // declarations
    IO_XYSimResults             xyForwardOutputFile;
    XYForwardOutputFO           xyForwardOutputFO;

    //  ********************* XYForwardOutputFO

    XYForwardOutputFO::XYForwardOutputFO() :
        FuncObjC("WriteXYOutput")
    {
        AddUseFlagInPort(xyDataArrayObjRef, typeid(DO_XYDataArray), xyForwardOutputDesc.writeFile);
        AddUseFlagInPort(seqTimesObjRef, typeid(DO_SequenceTimes), xyForwardOutputDesc.writeFile);
        AddUseFlagInPort(staticPressureObjRef, typeid(DO_Real), xyForwardOutputDesc.writeFile);
        doingFinalUpdate = false;
        DoStatusChk();
    }

    void XYForwardOutputFO::CalcNCase()
    {
        nCase    = 0;
        ncasePar = 0;
        if (control.IsNormal())
        {
            if (control.IsOptimization() && optimize.doOptPerturbations)
            {
                nCase = optimize.nPerturb;
            }
            else
            {
                nCase = vary.GetnVaryCombinations();
                ncasePar = orderedVary.Size();
            }
        }
        else
        {
            nCase    = SampVar::GetnTrials();
            ncasePar = SampVar::GetnActive();
        }

        if (control.IsOptimization())
        {
            nCase *= optFileOutput.GetNFits();
            ncasePar += OptVar::GetnActive() + 1;
        }

        if (ncasePar == 0)
            ncasePar = 1;
    }


    bool  XYForwardOutputFO::OutputSetup(bool& addOK)
    {
        addOK = true;
        if (!xyForwardOutputDesc.writeFile)
            return true;

        DoStatusChk();
        if (objStatus == os_Bad)
            return false;

        doingOutput = true;
        CopyString(runResultData.runID, xyForwardOutputDesc.runID, XYSimRunResults::runIDLen);

        CalcNCase();
        runResultData.caseValDesc.Alloc(ncasePar);
        runResultData.caseValShortDesc.Alloc(ncasePar);

        if (control.IsNormal())
        {
            if (orderedVary.IsEmpty() && (!control.IsOptimization()))
            {
                runResultData.caseValDesc += "Single run";
                runResultData.caseValShortDesc += "Single run";
            }
            else
            {
                for (int i = 0; i < orderedVary.Size(); i++)
                {
                    runResultData.caseValDesc += orderedVary[i]->GetLongID();
                    runResultData.caseValShortDesc += orderedVary[i]->GetShortID();
                }
            }
        }
        else
        {
            SampVarArray currSamp;
            for (int i = 0; i < currSamp.Size(); i++)
            {
                runResultData.caseValDesc += currSamp[i]->GetLongID();
                runResultData.caseValShortDesc += currSamp[i]->GetShortID();
            }
        }

        if (control.IsOptimization())
        {
            OptVarArray currOpt;
            for (int i = 0; i < currOpt.Size(); i++)
            {
                runResultData.caseValDesc += currOpt[i]->GetLongID();
                runResultData.caseValShortDesc += currOpt[i]->GetShortID();
            }
            runResultData.caseValDesc += "Fit value";
            runResultData.caseValShortDesc += "Fit value";
        }

        if (!runResultData.ChkAlloc(nCase))
        {
            doingOutput = false;
            return false;
        }

        runResultData.caseSeqData = *(sequenceTimesDC);
        if ((!xyForwardOutputDesc.addExisting) ||
            (xyDataArrayDC == NULL) ||
            (!ConvFileC::CFfileExists(xyForwardOutputDesc.fileName)))
            return true;

        addOK = xyForwardOutputFile.OKtoAdd(xyDataArrayDC->Size(), runResultData.caseValDesc.Size());

        return addOK;
    }

    void XYForwardOutputFO::SetFitData(const char* fitID, int fitIndex)
    {
        currFitID = fitID;
        currFitIndex = fitIndex;
    }

    void XYForwardOutputFO::SimulationComplete()
    {
        if (!doingOutput)
            return;

        if (!runResultData.IsEmpty())
            xyForwardOutputFile.AddSimRun(runResultData, xyForwardOutputDesc.addExisting);

        Cleanup();
    }



    void XYForwardOutputFO::Cleanup()
    {
        runResultData.DeAlloc();
        FileOutputControl::Cleanup();
    }

    void XYForwardOutputFO::DoStatusChk()
    {
        // default set
        if (xyDataArrayObjRef.objRef == 0)
            xyDataArrayObjRef = FuncObjRef(capturedDataFO);

        FuncObjC::DoStatusChk();
        sequenceTimesDC = 0;
        xyDataArrayDC   = 0;
        staticPressure  = 0.0;

        if (!xyForwardOutputDesc.writeFile)
            return;

        if (CheckInputFO(xyDataArrayObjRef, "XY data source"))
            xyDataArrayDC = static_cast<DO_XYDataArray*>(GetInPortData(xyDataArrayObjRef))->xyDataArray;

        if (CheckInputFO(seqTimesObjRef, "Sequence time data source"))
            sequenceTimesDC = static_cast<DO_SequenceTimes*>(GetInPortData(seqTimesObjRef))->seqTimes;

        if (CheckInputFO(staticPressureObjRef, "Static pressure"))
            staticPressure = static_cast<DO_Real*>(GetInPortData(staticPressureObjRef))->realVal;

    }

    void  XYForwardOutputFO::CalcOutput(FOcalcType  calcType)
    {
        if ((!doingOutput) || (!doingFinalUpdate))
            return;

        DoStatusChk();
        if (StatusNotOK())
            GenAppInternalError("XYForwardOutputFO::CalcOutput-StatusNotOK");

        // next case results
        runResultData.SetSize(runResultData.Size()+1);

        XYSimCaseResults& currCase = runResultData.LastIndex();
        currCase.caseValues.Alloc(ncasePar);
        currCase.caseID[0] = '\0';
        if (control.IsOptimization() && (optFileOutput.GetNFits() > 1))
        {
            CopyString(currCase.caseID, currFitID, XYSimCaseResults::caseIDLen);
            ConcatString(currCase.caseID, "/", XYSimCaseResults::caseIDLen);
            currCase.caseIndex = currCaseIndex * optFileOutput.GetNFits() + currFitIndex;
        }
        else
            currCase.caseIndex = currCaseIndex;

        // make description
        if (control.IsNormal())
        {
            if (control.IsOptimization() && optimize.doOptPerturbations)
            {
                ConcatString(currCase.caseID, "Pert#", XYSimCaseResults::caseIDLen);
                ConcatInt(currCaseIndex + 1, currCase.caseID, 2, XYSimCaseResults::caseIDLen);
            }
            else if (orderedVary.IsEmpty())
            {
                ConcatString(currCase.caseID, "only case", XYSimCaseResults::caseIDLen);
                if(!control.IsOptimization())
                    currCase.caseValues += 0.0;
            }
            else
            {
                vary.GetCaseID(currCase.caseID, XYSimCaseResults::caseIDLen);
                vary.GetCaseValues(currCase.caseValues);
            }
        }
        else
        {
            ConcatString(currCase.caseID, "Sample#", XYSimCaseResults::caseIDLen);
            ConcatInt(currCaseIndex + 1, currCase.caseID, XYSimCaseResults::caseIDLen);

            if (SampVar::GetTrialIndex() != currCaseIndex)
                GenAppInternalError("XYForwardOutputFO::CalcOutput-index");

            SampVarArray currSamp;
            for (int i = 0; i < currSamp.Size(); i++)
                currCase.caseValues += currSamp[i]->GetFinalSampleVal();
        }

        if (control.IsOptimization())
        {
            OptVarArray currOpt;
            for (int i = 0; i < currOpt.Size(); i++)
                currCase.caseValues += currOpt[i]->GetFinalParEstimate();
            currCase.caseValues += optimizeInput.GetFitValue();
        }


        currCase.staticPressure = staticPressure;
        currCase.caseData = (*xyDataArrayDC);
    }


};  // end namespace


