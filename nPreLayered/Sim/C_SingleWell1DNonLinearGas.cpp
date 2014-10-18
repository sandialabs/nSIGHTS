///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell1DNonLinearGas.cpp
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
#include <sstream>

#include <genClass/U_Real.h>

#include <nsClass/nSightConst.h>
#include <osClass/C_SimErr.h>

#include <Var/G_Control.h>
#include <Var/G_Parameter.h>
#include <Var/E_Parameter.h>

#include "G_SimulatorRadiiOutput.h"

#include "C_SingleWell1DNonLinearGas.h"


const double SingleWell1DNonLinearGas::tKelvin      = 273.2;    // deg C + = deg K
const double SingleWell1DNonLinearGas::univGasConstant = 8.314312; // joules / (moles . K)


SingleWell1DNonLinearGas::SingleWell1DNonLinearGas()
{
}
SingleWell1DNonLinearGas::~SingleWell1DNonLinearGas()
{
}


void SingleWell1DNonLinearGas::DoAllocateAll()
{
  SingleWell1DNonLinear::DoAllocateAll();

  int maxradialNodes = allParam[pN_n_r]->GetIntMaxPar();

  gasOutputPressures.Alloc(maxradialNodes);
}


void SingleWell1DNonLinearGas::SetRuntimeSize()
{
  SingleWell1DNonLinear::SetRuntimeSize();
  gasOutputPressures.SetSize(nradialNodes);
}


void SingleWell1DNonLinearGas::SetRunParameters()
{

  SingleWell1DNonLinear::SetRunParameters();

  double referenceTemp = wPar[pG_Tref].GetData();
  if (gasFlowIsMass)
  {
    double gasMolWeight = wPar[pG_MW].GetData();
    gasConstant = gasMolWeight / (univGasConstant * (referenceTemp + tKelvin));
  }
  else
  {
    gasConstant = (gasStdTemperature.GetMetricVal() + tKelvin) /
                  ((referenceTemp + tKelvin) * gasStdPressure.GetMetricVal() * kpaToPa);
  }
  ScaleGeomTerms(kpaToPa * kpaToPa * gasConstant, kpaToPa * gasConstant);

  // gas skin factor calc
  if (control.HasSkin())
    CalcSkinFactor(skinParGroup.kIndx, formParGroup.kIndx);

  if (gasFlowIsMass)
    nrTolQ = wPar[pN_Qm_tol].GetData();
  else
    nrTolQ = wPar[pN_Qv_tol].GetData();

  // set D & S terms
  SetDSTerms();

  //  pressure initialization
  // this must be set before geom values are set
  atmosPressure  = wPar[pG_Patm].GetData();

  nodePressure.FillToSize(staticPressure + atmosPressure);
}

double SingleWell1DNonLinearGas::NLGetDTerm(     int    nodeNum,
    const double&  pN,
    const double&  pN1)
{
  if (dTermIsLinear[nodeNum])
    return dLinearTerm[nodeNum];

  double pAvg = (pN + pN1) / 2.0;

  const ParameterGroups& currRock = (*rockMap[nodeNum]);

  double kTerm = NLGetRockPar(currRock.kIndx, nodeNum, pAvg);
  if (gasUseKlinkenburg)
  {
    lastWasNonLinear = true;
    kTerm *= (1.0 + wPar[currRock.kfIndx].GetData() / pAvg);
  }

  double vTerm = NLGetFluidPar(pG_mu, pAvg);
  if (gasUseSimpleViscositySlope)
  {
    lastWasNonLinear = true;
    vTerm +=  wPar[pG_mu_m].GetData() * pAvg;
  }

  double dTerm = 0.5 * kTerm / vTerm;
  return dTerm * dGeomTerm[nodeNum];
}


double SingleWell1DNonLinearGas::NLGetSTerm(    int     nodeNum,
    const double& pN)
{
  if (sTermIsLinear[nodeNum])
    return sLinearTerm[nodeNum];

  double skTerm = 0.0;
  if (control.HasSkin() && (nodeNum == fracStartNode))
  {
    skTerm = NLGetRockPar(skinParGroup.phiIndx, nodeNum, pN);
    skTerm *= sLastSkinGeom;
  }

  const ParameterGroups& currRock = (*rockMap[nodeNum]);
  double sTerm = NLGetRockPar(currRock.phiIndx, nodeNum, pN);

  return skTerm + sTerm * sGeomTerm[nodeNum];
}

double SingleWell1DNonLinearGas::NLCalcDTerm(   int     nodeNum,
    const double& pN,
    const double& pN1)
{
  return NLGetDTerm(nodeNum, pN, pN1) * (Sqr(pN) - Sqr(pN1));
}


double  SingleWell1DNonLinearGas::AnalyticPartialD(int nodeNum,
    const double&  pN)
{
  if (!dTermIsLinear[nodeNum])
    GenAppInternalError("SingleWell1DNonLinearGas::AnalyticPartialD");

  return dLinearTerm[nodeNum] * pN * 2.0;
}

double SingleWell1DNonLinearGas::GetFrCondVal(int nodeNum)
{
  const ParameterGroups& currRock = (*rockMap[nodeNum]);

  double pAvg = staticPressure + atmosPressure;

  double kTerm = NLGetRockPar(currRock.kIndx, nodeNum, pAvg);
  if (gasUseKlinkenburg)
    kTerm *= (1.0 + wPar[currRock.kfIndx].GetData() / pAvg);

  double vTerm = NLGetFluidPar(pG_mu, pAvg);
  if (gasUseSimpleViscositySlope)
    vTerm +=  wPar[pG_mu_m].GetData() * pAvg;

  return 0.5 * kTerm / vTerm;
}


double SingleWell1DNonLinearGas::NLGetWellPressure()
{
  return currSeqTZ.tzPressure + atmosPressure;
}

double SingleWell1DNonLinearGas::GetLastWellPressure()
{
  return lastSeqTZ.tzPressure + atmosPressure;
}

double SingleWell1DNonLinearGas::NLGetExternalPressure()
{
  return staticPressure + atmosPressure;
}


double SingleWell1DNonLinearGas::GetFrStoreVal(int nodeNum)
{
  const ParameterGroups& currRock = (*rockMap[nodeNum]);
  return NLGetRockPar(currRock.phiIndx, nodeNum, staticPressure + atmosPressure);
}

double  SingleWell1DNonLinearGas::GetWstoreCompressibility() // overriden for gas
{
  return gasConstant;
}


void SingleWell1DNonLinearGas::MatSolve(bool& setupRequired)
{
  NonlinearMatSolve();

  if (nodePressure[0] < 0.0)
    throw SimError("-ve pressure in gas well - reduce pumping rate", SimError::seSemiFatal);

  currSeqTZ.tzPressure = nodePressure[0] - atmosPressure;
  currSeqTZ.formFlow = NLCalcDTerm(0, nodePressure[0], nodePressure[1])
                       + NLCalcFormSTerm(nodePressure[0])
                       - NLCalcFormSTerm(lastSeqTZ.tzPressure + atmosPressure);
}


void SingleWell1DNonLinearGas::UpdateTSOutput()
{
  for (int j = 0; j < nradialNodes; j++)
    gasOutputPressures[j] = nodePressure[j] - atmosPressure;

  wCap.ApplySpec(currSeqTime.testTime, gasOutputPressures, currSeqTZ);
}

