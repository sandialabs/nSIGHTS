///////////////////////////////////////////////////////////////////////////////////
//
// C_WorkParameter.cpp
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
#include <math.h>

#include <genClass/U_Msg.h>

#include <nsClass/nSightConst.h>

#include <Var/C_Units.h>
#include <Var/G_Control.h>
#include <Var/G_Parameter.h>

#include "G_ParameterCurveOutput.h"

#include "C_WorkParameter.h"

namespace nsWorkingParameter  {


bool WorkingParameter::SetData(Parameter&     inData,
                               SC_SetupErr&   errData)
{
  if (!inData.IsActive())
  {
    parameterType = wptNotSet;
    return true;
  }

  if (inData.IsConstant())
  {
    parameterType = wptFixed;
    fixedVal = inData.GetParValue();
  }
  else if (inData.IsRadial())
  {
    parameterType = wptRadius;
    if (!inData.GetRadialParValue(*nodeRadii, nodeVals, errData))
      return false;
  }
  else if (inData.IsTime())
  {
    parameterType = wptFt;
    if (!inData.GetTimeCurve(fpCurve, errData))
      return false;
  }
  else if (inData.IsNonLinear())
  {
    parameterType = wptFP;
    if (!inData.GetPressureCurve(fpCurve, errData))
      return false;
  }
  return true;
}

int WorkingParameter::GetIntData()
{
  if (parameterType == wptNotSet)
    return 0;

  return int(GetData());
}

double WorkingParameter::GetData()
{
  if (!IsFixed())
    GenAppInternalError("WorkingParameter::GetData()");
  return fixedVal;
}

double WorkingParameter::GetData(int i)
{
  if (IsFixed())
    return fixedVal;
  if (!IsRadius())
    GenAppInternalError("WorkingParameter::GetData(int i)");
  return nodeVals[i];
}

double WorkingParameter::GetData(const double& metricInputP)
{
  if (IsFixed())
    return fixedVal;
  if (!(IsFP() || IsFt()))
    GenAppInternalError("WorkingParameter::GetData(double)");
  return fpCurve.GetMetricCurveY(metricInputP);
}

double WorkingParameter::GetData(int i,
                                 const double& metricInputP)
{
  switch (parameterType)
  {
  case  wptFixed   : {
    return fixedVal;
  }
  case  wptRadius    : {
    return nodeVals[i];
  }
  case  wptFP :
  case wptFt : {
    return fpCurve.GetMetricCurveY(metricInputP);
  }
  default        :
    ;
  }
  GenAppInternalError("WorkingParameter::GetData(int, double)");
  return 0.0;
}


bool WorkingParameterSetup(WorkingParameterArray& wPar,
                           SC_DoubleArray&      nodeRadii,
                           SC_SetupErr&       errData)
{
  wPar.AllocAndSetSize(maxParameters);
  // clear prev settings
  for (int i = 0; i < maxParameters; i++)
  {
    wPar[i].parameterType = WorkingParameter::wptNotSet;
    wPar[i].nodeRadii = &nodeRadii;
  }

  SC_IntArray reqPar(10);
  reqPar.SetEmpty();
  reqPar += pN_n_r;               // # of radial
  reqPar += pF_ro;          // outside radius
  reqPar += pTZ_r_w;          // well radius
  if (control.HasSkin())
  {
    reqPar += pS_t;       // skin thickness
    reqPar += pN_n_s;     // # of skin nodes
  }

  for (int i = 0; i < reqPar.Size(); i++)
    if (!wPar[reqPar[i]].SetData(*allParam[reqPar[i]], errData))
      return false;

  // if flow dimension not active (2D or leakage) set default to 2.0
  if (!allParam[pF_n]->IsActive())
  {
    wPar[pF_n].parameterType = WorkingParameter::wptFixed;
    wPar[pF_n].fixedVal = 2.0;
  }


  int   nNodes  =  int(wPar[pN_n_r].GetData());
  double  ro    =  wPar[pF_ro].GetData();
  double  rw    =  wPar[pTZ_r_w].GetData();
  int   nskinNodes = 0;
  double  skinThick = 0.0;

  if (control.HasSkin())
  {
    nskinNodes  = int(wPar[pN_n_s].GetData());
    skinThick =  wPar[pS_t].GetData();
  }

  // sanity checks
  if ((ro - skinThick - rw ) < 0.1)
  {
    errData.SetConstantError("formation radii  < 0.1 m ??");
    return false;
  }

  if ((nNodes - nskinNodes ) < 5)
  {
    errData.SetConstantError("too many skin nodes ??");
    return false;
  }

  nodeRadii.AllocAndSetSize(nNodes);
  nodeRadii[0] = rw;
  int nextIndex = 1;

  if (control.HasSkin())
  {
    double deltaA = exp(log((rw + skinThick) / rw) / double(nskinNodes - 1));
    for (int i = 1; i < nskinNodes; i++)
      nodeRadii[i] = nodeRadii[i - 1] * deltaA;
    nextIndex = nskinNodes;

    // adjustment for radii
    nskinNodes--;
  }

  double deltaA = exp(log(ro / (rw + skinThick)) / double(nNodes - nskinNodes - 1));
  for (int i = nextIndex; i < nNodes; i++)
    nodeRadii[i] = nodeRadii[i - 1] * deltaA;

  // radii setup done -- now the rest of the parameters
  int ncurves = 0;
  for (int i = 0; i < maxParameters; i++)
  {
    // skip already set, or not required
    WorkingParameter& currPar = wPar[i];

    if (currPar.IsFixed() || (!allParam[i]->IsActive()))
      continue;

    if (!currPar.SetData(*allParam[i], errData))
      return false;

    if (currPar.IsFP() || currPar.IsFt())
      ncurves++;
  }

  // check for 0 n, and n > 2 for 2dimensions
  for (int i = 0; i < nNodes; i++)
  {
    double nValue = fabs(wPar[pF_n].GetData(i));
    if (nValue < 1.0E-10)
    {
      errData.SetConstantError("flow dimension < abs(1.0E-10) found");
      return false;
    }
    if (control.Is2DRadial())
    {
      if (nValue > 2.0)
      {
        errData.SetConstantError("flow dimension cannot be > 2 for 2D problems");
        return false;
      }

    }
  }


  // 2D partial penetration checks
  if (control.Is2DRadial() && control.IsPartialPenetration())
  {
    double formThick = wPar[pF_t].GetData();
    int nwellNodes = wPar[pN_n_v2Dw].GetIntData();

    const double minThick = 0.001;

    double wellDist = wPar[pTZ_pl].GetData();

    if (control.IsBottomPartialPenetration())
    {
      double aboveDist = formThick - wellDist;
      if (aboveDist < minThick)
      {
        errData.SetConstantError("unscreened top interval < 0.001");
        return false;
      }
    }
    else if (control.IsMiddlePartialPenetration())
    {
      double belowDist = wPar[pTZ_pbo].GetData();
      double aboveDist = formThick - wellDist - belowDist;
      if (aboveDist < minThick)
      {
        errData.SetConstantError("unscreened top interval < 0.001");
        return false;
      }
    }
    else if (control.IsTopPartialPenetration())
    {
      double belowDist = formThick - wellDist;
      if (belowDist < minThick)
      {
        errData.SetConstantError("unscreened bottom interval < 0.001");
        return false;
      }
    }
  }

  // add special case for density
  if (NSUnitsBase::headFlag)
  {
    wPar[pFl_rho].parameterType = WorkingParameter::wptFixed;
    wPar[pFl_rho].fixedVal = NSUnitsBase::headDensity.GetMetricVal();
  }

  // and for gas viscosity slope factor
  if (control.IsGas() && gasUseSimpleViscositySlope)
  {
    // verify fixed
    if (!wPar[pG_mu_m].IsFixed())
      GenAppInternalError("WorkingParameter::WorkingParameterSetup");
    wPar[pG_mu_m].fixedVal *= 1000.0;  // slope is given in Pa.s / Pa units
    // actual pressure is kPa
  }

  // add curves
  parameterCurveOutputFO.parameterCurvesDC.AllocAndSetSize(ncurves);
  ncurves = 0;
  for (int i = 0; i < maxParameters; i++)
  {
    // skip already set, or not required
    WorkingParameter& currPar = wPar[i];
    if (currPar.IsFP() || currPar.IsFt())
    {
      DC_CurveWithID& nextCurve = parameterCurveOutputFO.parameterCurvesDC[ncurves++];
      nextCurve = currPar.fpCurve.parCurve;
      nextCurve.SetID(allParam[i]->shortName);
    }
  }
  if (ncurves > 0)
    parameterCurveOutputFO.CalcAndUpdate();


  // all done OK
  return true;
}


void WorkingParameterArray::InitLinearFluidVars(const double& staticPressure)
{
  if (!control.IsLiquid())
    return;

  fluidDensity = (*this)[pFl_rho].GetData(staticPressure);

  if (!enterSpecificStorageData)
    fluidComp    = (*this)[pFl_C].GetData(staticPressure);
  // for perm conversions
  if (enterPermeabilityData)
    fluidPermTerm = fluidDensity * gravConst / (*this)[pFl_mu].GetData(staticPressure);

}


double WorkingParameterArray::CalcStoreTerm(const ParameterGroups& parGroup) const
{
  if (!control.IsLiquid())
    GenAppInternalError("WorkingParameterArray::CalcStoreTerm");

  if (enterSpecificStorageData)
    return (*this)[parGroup.ssIndx].GetData(0, 0.0);

  return gravConst * fluidDensity *
         ((*this)[parGroup.cIndx].GetData(0, 0.0)  +
          (*this)[parGroup.phiIndx].GetData(0, 0.0)  * fluidComp);

}
double WorkingParameterArray::CalcKTerm(const ParameterGroups& parGroup) const
{
  if (!control.IsLiquid())
    GenAppInternalError("WorkingParameterArray::CalcKTerm");

  if (enterPermeabilityData)
    return (*this)[parGroup.kIndx].GetData(0, 0.0) * fluidPermTerm;

  return  (*this)[parGroup.hkIndx].GetData(0, 0.0);

}


};                                      // end namespace
