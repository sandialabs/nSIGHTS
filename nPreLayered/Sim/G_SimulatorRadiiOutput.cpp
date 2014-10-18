///////////////////////////////////////////////////////////////////////////////////
//
// G_SimulatorRadiiOutput.cpp
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
#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>

#include <Var/C_Units.h>

#include "G_SimulatorRadiiOutput.h"


SimulatorRadiiOutput simulatorRadiiFO;

SimulatorRadiiOutput::SimulatorRadiiOutput() :
  GlobalFunc("SimRunOutput"),
  dualLambdaDO("Dual porosity lambda", "lambda"),
  dualOmegaDO("Dual porosity omega", "omega"),
  skinFactorDO("Skin factor", "s"),
  transmissivityDO("Transmissivity", "T"),
  storativityDO("Storativity", "S"),
  diffusivityDO("Diffusivity", "D"),
  isolatedWellboreStorageDO("Isolated wellbore storage", "C_i"),
  openholeWellboreStorageDO("Open hole wellbore storage", "C_o"),
  dualFractureTDO("Fracture transmissivity", "T_f")
{
  simulatorRadiiDO.tableData = &simulatorRadiiDC;
  AddOutPort(simulatorRadiiDO);

  AddOutPort(skinFactorDO);
  AddOutPort(transmissivityDO);
  AddOutPort(storativityDO);
  AddOutPort(diffusivityDO);
  AddOutPort(isolatedWellboreStorageDO);
  AddOutPort(openholeWellboreStorageDO);
  AddOutPort(dualLambdaDO);
  AddOutPort(dualOmegaDO);
  AddOutPort(dualFractureTDO);

  // log parameters
  dualLambdaDO.isLinear = false;
  dualOmegaDO.isLinear = false;
  transmissivityDO.isLinear = false;
  storativityDO.isLinear = false;
  diffusivityDO.isLinear = false;
  isolatedWellboreStorageDO.isLinear = false;
  openholeWellboreStorageDO.isLinear = false;
  dualFractureTDO.isLinear = false;

  // initial allocation so something shows up
  simulatorRadiiDC.SetID("f(r) output");
  simulatorRadiiDC.Alloc(5, 50, 40);
  simulatorRadiiDC.SetTitle(0, "Radius");
  simulatorRadiiDC.SetTitle(1, "FlowArea");
  simulatorRadiiDC.SetTitle(2, "NodeVol");
  simulatorRadiiDC.SetTitle(3, "CondArea");
  simulatorRadiiDC.SetTitle(4, "StoreArea");

  simulatorRadiiDC.dataTable.MatrixSetSize(5, 50);
  simulatorRadiiDC.dataTable.MatrixFill(0.0);

  DoStatusChk();
}

void SimulatorRadiiOutput::ResetCalcPars()
{
  dualLambdaDO.CalcInit();
  dualOmegaDO.CalcInit();
  skinFactorDO.CalcInit();
  transmissivityDO.CalcInit();
  diffusivityDO.CalcInit();
  storativityDO.CalcInit();
  isolatedWellboreStorageDO.CalcInit();
  openholeWellboreStorageDO.CalcInit();
  dualFractureTDO.CalcInit();

  UnitIndex defaultUnits(uTran);
  char unitStr[40];
  defaultUnits.MakeUnitString(unitStr, 40);

  transmissivityDO.SetCalcUnits(unitStr);
  diffusivityDO.SetCalcUnits(unitStr);
  dualFractureTDO.SetCalcUnits(unitStr);

  defaultUnits.unitType = uWbor;
  defaultUnits.MakeUnitString(unitStr, 40);
  isolatedWellboreStorageDO.SetCalcUnits(unitStr);
  openholeWellboreStorageDO.SetCalcUnits(unitStr);
}

void SimulatorRadiiOutput::DoStatusChk()
{
  FuncObjC::DoStatusChk();
}


void SimulatorRadiiOutput::CalcOutput(FOcalcType  calcType)
{
  DoStatusChk();
}

