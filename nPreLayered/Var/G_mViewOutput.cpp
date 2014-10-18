///////////////////////////////////////////////////////////////////////////////////
//
// G_mViewOutput.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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

#include "StdAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_File.h>

#include <Var/C_Units.h>

#include <Var/G_Control.h>
#include <Var/G_DataCapture.h>
#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>

#include <osMain/G_Vary.h>
#include <Var/G_OutputFiles.h>

#include <Var/G_mViewOutput.h>

namespace nsOutputFiles  {


mViewForwardOutputFO   mViewOutputFO;          // data to output -- defaults to data capture XY Array output

//  ********************* mViewForwardOutputFO

mViewForwardOutputFO::mViewForwardOutputFO() :
  FuncObjC("WritemViewOutput")
{
  AddUseFlagInPort(staticPressureObjRef, typeid(DO_Real), mviewOutputDesc.writeFile);
  DoStatusChk();
}



bool  mViewForwardOutputFO::OutputSetup(bool& addOK)
{
  addOK = true;
  doingOutput = false;
  if (!(mviewOutputDesc.writeFile && control.IsNormal() && (!control.IsOptimization())))
    return true;

  DoStatusChk();
  if (StatusNotOK())
    return false;
  doingOutput = true;
  return true;
}


void mViewForwardOutputFO::CaseInit(const SC_DoubleArray& nodeRadius,
                                    const SC_DoubleArray& constantVertNodeZ)
{
  if (!doingOutput)
    return;
  /*
    doExtended = control.Is2DRadial();
    timeStepCount = 0;
    if (doExtended)
    {
      thicknessToPressureConversion = currThicknessToPressure;

      const UnitIndex distConv(uDist);
      AddNodeData(nodeRadius, extendedProfileData.nodeRadii, distConv);

      extendedProfileData.nconstantVert = constantVertNodeZ.Size();
      for (int i = 0; i < extendedProfileData.nconstantVert; i++)
        extendedProfileData.constantVertZPos[i] = distConv.MetricToUser(constantVertNodeZ[i]);
      extendedProfileData.constantVertZPos.SetSize(extendedProfileData.nconstantVert);
      extendedProfileData.nradial = extendedProfileData.nodeRadii.Size();

      extendedProfileData.SetEmpty();
    }
    else
    {
      AddNodeData(nodeRadius, profileData.yData, UnitIndex(uDist));
      profileData.xData.SetEmpty();
      profileData.gData.SetEmpty();
    }
  */
}



void mViewForwardOutputFO::SimulationComplete()
{
  if (!doingOutput)
    return;

//  if (!runResultData.IsEmpty())
//    profileOutputFile.AddSimRun(runResultData, profileOutputDesc.addExisting);

  Cleanup();
}


void mViewForwardOutputFO::Cleanup()
{
  FileOutputControl::Cleanup();
// runResultData.DeAlloc();
}

void mViewForwardOutputFO::DoStatusChk()
{
  // default set
  FuncObjC::DoStatusChk();

  if (!mviewOutputDesc.writeFile)
    return;

  if (CheckInputFO(staticPressureObjRef, "Static pressure"))
    double staticPressure = static_cast<DO_Real*>(GetInPortData(staticPressureObjRef))->realVal;
  /*
    char pathName[stdFileStrLen];
    ExtractFilePath(pathName, profileOutputFile.fileName, stdFileStrLen);
    if (!PathExists(pathName))
    {
      char pathErr[120];
      MakeString(pathErr, "Profile file path is bad:", pathName, 120);
          SetObjErrMsg(pathErr);
          return;
    }
  */
}

void  mViewForwardOutputFO::CalcOutput(FOcalcType  calcType)
{
  if (!doingOutput)
    return;

  DoStatusChk();
  if (!StatusOK())
    GenAppInternalError("mViewForwardOutputFO::CalcOutput");
  /*
    // next case results
    runResultData.SetSize(runResultData.Size()+1);

    ProfileSimCaseResults& currCase = runResultData.LastIndex();
    currCase.caseValues.Alloc(ncasePar);
    currCase.caseID[0] = '\0';

    // make description
    if (orderedVary.IsEmpty())
    {
      ConcatString(currCase.caseID, "only case", ProfileSimCaseResults::caseIDLen);
      currCase.caseValues += 0.0;
    }
    else
    {
      vary.GetCaseID(currCase.caseID, ProfileSimCaseResults::caseIDLen);
      vary.GetCaseValues(currCase.caseValues);
    }

    currCase.caseDataIsExtended = doExtended;
    if (doExtended)
      currCase.extendedCaseData = extendedProfileData;
    else
      currCase.caseData = profileData;
    currCase.staticPressure = staticPressure;
  */

}

};  // end namespace


