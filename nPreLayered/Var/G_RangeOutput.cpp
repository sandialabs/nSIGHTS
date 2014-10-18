///////////////////////////////////////////////////////////////////////////////////
//
// G_RangeOutput.cpp
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
#include <iostream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>
#include <genClass/C_ConvFile.h>

#include <Var/G_Control.h>

#include <osMain/G_Optimize.h>
#include <osMain/G_Vary.h>
#include <osMain/G_OptRange.h>

#include <Var/G_OutputFiles.h>

#include <Var/G_RangeOutput.h>

namespace nsOutputFiles  {


IO_RangeSimResults  rangeOutputFile;        // includes file name
RangeOutputControl  rangeOutputControl;


RangeOutputControl::RangeOutputControl()
{
}

bool RangeOutputControl::OutputSetup(bool& addOK)
{
  doingOutput = false;
  addOK = true;

  if (!rangeOutputDesc.writeFile)
    return true;

  doingOutput = true;
  if ((!rangeOutputDesc.addExisting) || (!ConvFileC::CFfileExists(rangeOutputDesc.fileName)))
    return true;

  addOK = rangeOutputFile.OKtoAdd(optRangeOutputFO.doingCubes);

  return addOK;
}


void RangeOutputControl::SimulationComplete()
{
  if (!doingOutput)
    return;

  RangeSimRunResults runResultData;

  CopyString(runResultData.runID, rangeOutputDesc.runID, RangeSimRunResults::runIDLen);
  runResultData.AllocAndSetSize(optRangeOutputFO.gridOutputDC.Size());

  runResultData.runIsCube = optRangeOutputFO.doingCubes;

  if (runResultData.runIsCube)
  {
    runResultData.xData = optRangeOutputFO.cubeOutputDC[0].xData;
    runResultData.yData = optRangeOutputFO.cubeOutputDC[0].yData;
    runResultData.zData = optRangeOutputFO.cubeOutputDC[0].zData;

    for (int i = 0; i < runResultData.Size(); i++)
    {
      RangeSimOutput& currSim = runResultData[i];
      DC_CubeData& currCube = optRangeOutputFO.cubeOutputDC[i];
      CopyString(currSim.outputID, currCube.cubeID, RangeSimOutput::outputIDLen);
      int nCube = currCube.Size();
      currSim.gridCubeData.AllocAndSetSize(nCube);
      for (int j = 0; j < nCube; j++)
        currSim.gridCubeData[j] = currCube[j];
    }
  }
  else
  {
    runResultData.xData = optRangeOutputFO.gridOutputDC[0].xData;
    runResultData.yData = optRangeOutputFO.gridOutputDC[0].yData;

    for (int i = 0; i < runResultData.Size(); i++)
    {
      RangeSimOutput& currSim = runResultData[i];
      DC_GridData& currGrid = optRangeOutputFO.gridOutputDC[i];
      CopyString(currSim.outputID, currGrid.gridID, RangeSimOutput::outputIDLen);
      int ngrid = currGrid.Size();
      currSim.gridCubeData.AllocAndSetSize(ngrid);
      for (int j = 0; j < ngrid; j++)
        currSim.gridCubeData[j] = currGrid[j];
    }
  }

  if (!runResultData.IsEmpty())
    rangeOutputFile.AddSimRun(runResultData, rangeOutputDesc.addExisting);

  Cleanup();
}


};                                      // end NS

