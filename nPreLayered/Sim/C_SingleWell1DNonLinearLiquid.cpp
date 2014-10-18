///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell1DNonLinearLiquid.cpp
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

#include "C_SingleWell1DNonLinearLiquid.h"


SingleWell1DNonLinearLiquid::SingleWell1DNonLinearLiquid()
{
}
SingleWell1DNonLinearLiquid::~SingleWell1DNonLinearLiquid()
{
}


void SingleWell1DNonLinearLiquid::SetRunParameters()
{
  SingleWell1DNonLinear::SetRunParameters();

  ScaleGeomTerms(kpaToPa, kpaToPa);

  CalcFluidFormationFactors();

  nrTolQ = wPar[pN_Qv_tol].GetData();

  // set D & S terms
  SetDSTerms();
}

double SingleWell1DNonLinearLiquid::NLGetDTerm(    int    nodeNum,
    const double&  pN,
    const double&  pN1)
{
  if (dTermIsLinear[nodeNum])
    return dLinearTerm[nodeNum];

  double pAvg = (pN + pN1) / 2.0;

  const ParameterGroups& currRock = (*rockMap[nodeNum]);

  double dTerm;
  if (enterPermeabilityData)
    dTerm = NLGetRockPar(currRock.kIndx, nodeNum, pAvg) /
            NLGetFluidPar(pFl_mu, pAvg);
  else
    dTerm = NLGetRockPar(currRock.hkIndx, nodeNum, pAvg) /
            (gravConst * NLGetFluidPar(pFl_rho, pAvg));

  if (horizontalAnisotropy)
    dTerm *= sqrt(1.0 / wPar[pF_KxyRatio].GetData());

  return dTerm * dGeomTerm[nodeNum];
}


double SingleWell1DNonLinearLiquid::NLGetSTerm(   int     nodeNum,
    const double& pN)
{
  if (sTermIsLinear[nodeNum])
    return sLinearTerm[nodeNum];

  double skTerm = 0.0;
  if (control.HasSkin() && (nodeNum == fracStartNode))
  {
    if (enterSpecificStorageData)
      skTerm = NLGetRockPar(skinParGroup.ssIndx, nodeNum, pN)
               / (gravConst * NLGetFluidPar(pFl_rho, pN));
    else
      skTerm = NLGetRockPar(skinParGroup.cIndx, nodeNum, pN)
               +  NLGetRockPar(skinParGroup.phiIndx, nodeNum, pN) *
               NLGetFluidPar(pFl_C, pN);
    skTerm *= sLastSkinGeom;
  }

  const ParameterGroups& currRock = (*rockMap[nodeNum]);
  double sTerm;
  if (enterSpecificStorageData)
    sTerm = NLGetRockPar(currRock.ssIndx, nodeNum, pN)
            / (gravConst * NLGetFluidPar(pFl_rho, pN));
  else
    sTerm = NLGetRockPar(currRock.cIndx, nodeNum, pN)
            +  NLGetRockPar(currRock.phiIndx, nodeNum, pN) *
            NLGetFluidPar(pFl_C, pN);


  return skTerm + sTerm * sGeomTerm[nodeNum];
}

double SingleWell1DNonLinearLiquid::NLCalcDTerm(    int     nodeNum,
    const double& pN,
    const double& pN1)
{
  return NLGetDTerm(nodeNum, pN, pN1) * (pN - pN1);
}


double  SingleWell1DNonLinearLiquid::AnalyticPartialD(int nodeNum,
    const double&  pN)
{
  if (!dTermIsLinear[nodeNum])
    GenAppInternalError("SingleWell1DNonLinearLiquid::AnalyticPartialD");

  return dLinearTerm[nodeNum];
}

double SingleWell1DNonLinearLiquid::GetFrCondVal(int nodeNum)
{
  const ParameterGroups& currRock = (*rockMap[nodeNum]);

  double dTerm;
  if (enterPermeabilityData)
    dTerm = NLGetRockPar(currRock.kIndx, nodeNum, staticPressure)
            * (gravConst * NLGetFluidPar(pFl_rho, staticPressure))
            /  NLGetFluidPar(pFl_mu, staticPressure);
  else
    dTerm = NLGetRockPar(currRock.hkIndx, nodeNum, staticPressure);
  return dTerm;
}


double SingleWell1DNonLinearLiquid::NLGetWellPressure()
{
  return currSeqTZ.tzPressure;
}


double SingleWell1DNonLinearLiquid::NLGetExternalPressure()
{
  return staticPressure;
}


double SingleWell1DNonLinearLiquid::GetFrStoreVal(int nodeNum)
{
  const ParameterGroups& currRock = (*rockMap[nodeNum]);

  double sTerm;
  if (enterSpecificStorageData)
    sTerm = NLGetRockPar(currRock.ssIndx, nodeNum, staticPressure);
  else
    sTerm = gravConst * NLGetFluidPar(pFl_rho, staticPressure) *
            (NLGetRockPar(currRock.cIndx, nodeNum, staticPressure)
             +  NLGetRockPar(currRock.phiIndx, nodeNum, staticPressure) *
             NLGetFluidPar(pFl_C, staticPressure));
  return sTerm;
}


void SingleWell1DNonLinearLiquid::MatSolve(bool& setupRequired)
{
  NonlinearMatSolve();

  if ((nodePressure[0] < 0.0) && (!allowNegativePressure))
    throw SimError("well sucked dry - reduce pumping rate", SimError::seSemiFatal);

  currSeqTZ.tzPressure = nodePressure[0];
  currSeqTZ.formFlow = NLCalcDTerm(0, nodePressure[0], nodePressure[1])
                       + NLCalcFormSTerm(nodePressure[0])
                       - NLCalcFormSTerm(lastSeqTZ.tzPressure);
}

