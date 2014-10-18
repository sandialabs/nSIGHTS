///////////////////////////////////////////////////////////////////////////////////
//
// G_Parameter.cpp
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
#include <genClass/U_Real.h>

#include <osMain/G_Vary.h>
#include <osClass/C_OptVar.h>

#include <Var/G_PreConfigFile.h>

#include <Var/G_Control.h>
#include <Var/G_Sequence.h>
#include <Var/G_Layer.h>

#include <Sim/G_SimulatorRadiiOutput.h>
#include <Sim/G_ParameterCurveOutput.h>

#include <Var/G_Parameter.h>


namespace nsParameterGlobals  {


ParameterErrorListing  parameterErrorListing;
PointsParameterErrorListing  pointsParameterErrorListing;

AllocatedParameterArray allParam;

ParameterListing::ParameterListing()
  : OSListObj("nPreParameters")
{
}

void ParameterListing::ListOneParameter(const Parameter& listPar)
{

  if (listPar.IsFixed())
  {
    AddUnitReal(listPar.fullName, listPar.parVal, listPar.GetOverrideFormat());
  }
  else
  {
    ClearEnumStrs();
    if (vary.IsSuite())
      AddEnumStr("Suite", Parameter::ptVary);
    else
      AddEnumStr("Range", Parameter::ptVary);

    AddEnumStr("Optimization", Parameter::ptOpt);
    AddEnumStr("Sampling", Parameter::ptSample);
    if (control.UseCurveFiles())
    {
      AddEnumStr("f(r) file", Parameter::ptFrFixed);
      AddEnumStr("f(P) file", Parameter::ptFPFixed);
      AddEnumStr("f(t) file", Parameter::ptFtFixed);
    }
    else
    {
      AddEnumStr("f(r) object", Parameter::ptFrFixed);
      AddEnumStr("f(P) object", Parameter::ptFPFixed);
      AddEnumStr("f(t) object", Parameter::ptFtFixed);
    }
    AddEnumStr("f(r) point", Parameter::ptFrPoint);
    AddEnumStr("f(P) point", Parameter::ptFPPoint);
    AddEnumStr("f(t) point", Parameter::ptFtPoint);
    AddStdEnum(listPar.fullName, listPar.parameterType);

    UnitIndex parUnits(listPar.parVal);

    switch (listPar.parameterType) {
    case Parameter::ptVary : {

      AddVaryVar(listPar.varyParVal, listPar.GetOverrideFormat());
      break;
    }
    case Parameter::ptOpt :  {
      AddOptVar(listPar.optParVal, listPar.GetOverrideFormat());
      break;
    }
    case Parameter::ptSample : {
      AddSampVar(listPar.sampParVal, listPar.GetOverrideFormat());
      break;
    }
    case Parameter::ptFrFixed : {
      break;
    }
    case Parameter::ptFPFixed : {
      break;
    }
    case Parameter::ptFtFixed : {
      break;
    }
    case Parameter::ptFrPoint : {
      break;
    }
    case Parameter::ptFPPoint : {
      break;
    }
    case Parameter::ptFtPoint : {
      break;
    }
    }
  }
}

void ParameterListing::ListParameterArray(const ParameterPtrArray& listArray)
{
  if (listArray.IsEmpty())
    return;
  SubTitle(listArray.parDesc);
  for (int i = 0; i < listArray.Size(); i++)
    ListOneParameter(listArray.GetRef(i));

}


void ParameterListing::CreateListing()
{
  StdTitle("Parameters");

  if (control.IsLayered())
  {
    ListParameterArray(layerCommon);
    ListParameterArray(fluidPar);
    ListParameterArray(numericPar);
    for (int i = 0; i < layerPar.Size(); i++)
      ListParameterArray(layerPar[i]);
  }
  else
  {
    ListParameterArray(formationPar);
    ListParameterArray(fracturePar);
    ListParameterArray(matrixPar);
    ListParameterArray(skinPar);
    ListParameterArray(fluidPar);
    ListParameterArray(gasPar);
    ListParameterArray(testZonePar);
    ListParameterArray(leakyPar);
    ListParameterArray(lowerLeakyPar);
    ListParameterArray(upperLeakyPar);
    ListParameterArray(numericPar);
  }
}


ParameterErrorListing::ParameterErrorListing()
  :ErrorListObjC("Parameter")
{
}

void ParameterErrorListing::CreateListing()
{
  StdTitle("Parameter Setup Errors");

  if (control.IsLayered())
  {
    CheckParameterArray(layerCommon);
    CheckParameterArray(fluidPar);
    CheckParameterArray(numericPar);
    for (int i = 0; i < layerPar.Size(); i++)
      CheckParameterArray(layerPar[i]);
  }
  else
  {
    CheckParameterArray(formationPar);
    CheckParameterArray(fracturePar);
    CheckParameterArray(matrixPar);
    CheckParameterArray(skinPar);
    CheckParameterArray(fluidPar);
    CheckParameterArray(gasPar);
    CheckParameterArray(testZonePar);
    CheckParameterArray(leakyPar);
    CheckParameterArray(lowerLeakyPar);
    CheckParameterArray(upperLeakyPar);
    CheckParameterArray(numericPar);
  }

  parameters.CheckForTimeAndNonlinear();
  if (parameters.NonLinearParameters() && parameters.TimeParameters())
  {
    AddError("Cannot combine f(t) and f(P) parameters");
  }
}

bool ParameterErrorListing::CheckParameterArray(const ParameterPtrArray& chkArray)
{
  SC_SetupErr parErr;
  for (int i = 0; i < chkArray.Size(); i++)
    if (!chkArray.GetRef(i).SetupOK(parErr))
      AddError(parErr);

  return true;
}

class FlowDParameter : public Parameter {
public:
  FlowDParameter();
  ~FlowDParameter();
  virtual bool  SetupOK(SC_SetupErr& errData);

protected:
  virtual bool  ParValOK(const double&      pVal,
                         const char         rangeID[],
                         SC_SetupErr& errData);

};

FlowDParameter::FlowDParameter()
  : Parameter("Flow dimension",
              "n",
              uNone,
              -1.0E+04,
              1.0E+04,
              true,
              false,
              true,
              false,
              SC_RealFormat(),
              2.0)
{
}

FlowDParameter::~FlowDParameter()
{
}
bool FlowDParameter::SetupOK(SC_SetupErr& errData)
{
  if (!Parameter::SetupOK(errData))
    return false;

  if (IsVary())
  {
    for (int i = 0; i < varyParVal.GetnVary(); i++)
    {
      if (!ParValOK(varyParVal.GetVaryVal(i), "vary flowD", errData))
        return false;
    }
  }

  return true;
}

bool FlowDParameter::ParValOK(const double&      pVal,
                              const char         rangeID[],
                              SC_SetupErr& errData)
{
  if (!Parameter::ParValOK(pVal, rangeID, errData))
    return false;

  if (fabs(pVal) < 1.0E-10)
  {
    errData.SetConstantError("ABS(flow dimension) must be > 1.0E-10");
    return false;
  }

  return true;
}



void ParameterSupport::InitAllParameters()
{
  allParam.AllocAndSetSize(maxParameters + maxGeoLayers * maxLayerParameters);

  allParam[pF_t]      = new Parameter("Formation thickness",          "b",        uDist,  1.0E-10,    5.0E+03, false, false, false, false);
  allParam[pF_n]      = new FlowDParameter();
  allParam[pF_Pf]     = new Parameter("Static formation pressure",    "P_fm",     uPress, 0.0,        1.0E+06, true,  false, false, false);
  allParam[pF_ro]     = new Parameter("External boundary radius",     "r_o",      uDist,  1.E-04,     1.0E+06, true,  false, false, false);
  allParam[pF_k ]     = new Parameter("Formation permeability",       "k_fm",     uPerm,  1.0E-30,    1.0E+00, true,  true,  true,  true, false);
  allParam[pF_HK]     = new Parameter("Formation conductivity",       "K_fm",     uK,     1.0E-30,    1.0E+02, true,  true,  true,  true, false);
  allParam[pF_Ss]     = new Parameter("Formation spec. storage",      "ss_fm",    uStor,  1.0E-30,    1.0E+03, true,  true,  true,  true, false);
  allParam[pF_C]      = new Parameter("Formation compressibility",    "C_fm",     uComp,  1.0E-30,    1.0E-01, true,  true,  true,  true, 1E-11, false);
  allParam[pF_Phi]  = new Parameter("Formation porosity",           "phi_fm",   uRatio, 0.0,        1.0E+00, true,  true,  true,  true, SC_RealFormat());
  allParam[pF_Kf]     = new Parameter("Formation klinkenburg factor", "b_kfm",    uPress, 0.0,        1.0E+06, true,  true,  true,  false, SC_SciFormat(5), 0.0);
  allParam[pF_Sy]     = new Parameter("Formation specific yield",   "Sy_fm",    uNone,  1.0E-15,    1.0E+03, true,  false, false, false);
  allParam[pF_kv ]    = new Parameter("Formation vertical permeability","kv_fm",  uPerm,  1.0E-30,    1.0E+00, true,  true,  true,  false, false);
  allParam[pF_HKv]    = new Parameter("Formation vertical conductivity","Kv_fm",  uK,     1.0E-30,    1.0E+02, true,  true,  true,  false, false);
  allParam[pF_KxyRatio] = new Parameter("Kx/Ky conductivity ratio",  "RatioXY_fm",  uNone,  1.0,   1000.0, true,  false, false, false, 1.0);
  allParam[pF_KxyAngle] = new Parameter("Kx/Ky ellipse orientation",   "ThetaXY_fm",  uNone,  -90.0,  90.0, true,  false, false, false, 0.0);


  allParam[pFr_k ]    = new Parameter("Fracture permeability",        "k_f",      uPerm,  1.0E-30,    1.0E+00, true,  true,  true, true, false);
  allParam[pFr_HK]    = new Parameter("Fracture conductivity",        "K_f",      uK,     1.0E-30,    1.0E+02, true,  true,  true, true, false);
  allParam[pFr_Ss]    = new Parameter("Fracture spec. storage",       "ss_f",     uStor,  1.0E-30,    1.0E+03, true,  true,  true, true, false);
  allParam[pFr_C]     = new Parameter("Fracture compressibility",     "C_f",      uComp,  1.0E-30,    1.0E-01, true,  true,  true, true, false);
  allParam[pFr_Phi] = new Parameter("Porosity within fracture",     "phi_f",  uRatio, 0.0,        1.0E+00, true,  true,  true, true, SC_RealFormat());

  allParam[pM_k ]     = new Parameter("Matrix permeability",          "k_m",      uPerm,  1.0E-30,    1.0E+00, true,  false, false, false, false);
  allParam[pM_HK]     = new Parameter("Matrix conductivity",          "K_m",      uK,     1.0E-30,    1.0E+02, true,  false, false, false, false);
  allParam[pM_Ss]     = new Parameter("Matrix spec. storage",         "ss_m",      uStor,  1.0E-30,    1.0E+03, true,  false, false, false, false);
  allParam[pM_C]      = new Parameter("Matrix compressibility",       "C_m",      uComp,  1.0E-30,    1.0E-01, true,  false, false, false, 1E-11, false);
  allParam[pM_Phi]  = new Parameter("Matrix porosity",              "phi_m",    uRatio, 0.0,        1.0E+00, true,  false, false, false, SC_RealFormat());
  allParam[pM_V]      = new Parameter("Matrix volume factor",         "V_m",      uRatio, 0.00001,    0.999999, true,  false, false, false, SC_RealFormat(6), 0.99);
  allParam[pM_Alpha]  = new Parameter("Geometry factor (Alpha)",      "alpha",    uGeom,  1.0E-10,    1.0E+05, true,  false, false, false, SC_RealFormat());
  allParam[pM_t]      = new Parameter("Slab matrix block thickness",  "m_t",      uDist,  1.0E-10,    1.0E+05, true,  false, false, false, SC_RealFormat());
  allParam[pM_d]      = new Parameter("Matrix sphere diameter",       "m_d",      uDist,  1.0E-10,    1.0E+05, true,  false, false, false, SC_RealFormat());

  allParam[pS_k ]     = new Parameter("Skin zone permeability",       "k_s",      uPerm,  1.0E-30,    1.0E+00, true,  true,  true, true, false);
  allParam[pS_HK]     = new Parameter("Skin zone conductivity",       "K_s",      uK,     1.0E-30,    1.0E+02, true,  true,  true, true, false);
  allParam[pS_Ss]     = new Parameter("Skin zone spec. storage",      "ss_s",      uStor,  1.0E-30,    1.0E+03, true,  true,  true, true, false);
  allParam[pS_C]      = new Parameter("Skin zone compressibility",    "C_s",      uComp,  1.0E-30,    1.0E-01, true,  true,  true,  true, false);
  allParam[pS_Phi]  = new Parameter("Skin zone porosity",           "phi_s",    uRatio, 0.0,        1.0E+00, true,  true,  true,  true, SC_RealFormat());
  allParam[pS_Kf]     = new Parameter("Skin zone klinkenburg factor", "b_ks",     uPress, 0.0,        1.0E+06, true,  true,  true, false, SC_SciFormat(5), 0.0);
  allParam[pS_t]      = new Parameter("Radial thickness of skin",     "t_s",      uDist,  1.0E-10,    5.0E+04, true,  false, false, false);
  allParam[pS_Sy]     = new Parameter("Skin zone specific yield",   "Sy_s",     uNone,  1.0E-15,    1.0E+03, true,  false, false, false);

  allParam[pFl_rho]   = new Parameter("Fluid density",                "rho",      uDens,  100.0,      2000.0,  false, true,  false, false, 1000.0);
  allParam[pFl_mu]    = new Parameter("Fluid viscosity",              "mu",       uVisc,  0.000050,   0.005,   false, true,  false, false);
  allParam[pFl_C]     = new Parameter("Fluid compressibility",        "C_w",      uComp,  1.0E-11,    1.0E-09, false, true,  false, false, 4E-10, false);
  allParam[pFl_TX]    = new Parameter("Fluid thermal exp. coeff.",    "C_T",      uTexp,  -1.0E-03,   5.0E-03, true, false, false, false, 0.0);

  allParam[pG_MW]     = new Parameter("Molecular weight",             "m_w",      uMass,  1.0,        1000.0,  false, false,  false, false);
  allParam[pG_Tref]   = new Parameter("Reference temperature",        "T",        uTemp,  5.0,        100.0,   false, false,  false, false, SC_DecFormat(4));
  allParam[pG_mu]     = new Parameter("Gas viscosity",                "mu_g",     uVisc,  1.0E-06,    1.0,     false, true,   false, false);
  allParam[pG_mu_m]   = new Parameter("Gas viscosity slope factor",   "b_mu",     uTime,  -1.0,       1.0,     true,  false,  false, false, 0.0);
  allParam[pG_Patm]   = new Parameter("Atmospheric pressure [abs]",   "P_a",      uPress, 10.0,       200.0,   false, false,  false, false, 100.0);

  allParam[pTZ_r_w]   = new Parameter("Well radius",          "r_w",      uDist,  0.0001,     1000.0,  false, false,  false, false);
  allParam[pTZ_r_s]   = new Parameter("Tubing string radius",         "r_ts",     uDist,  0.0001,     1000.0,  true,  false,  false, false);
  allParam[pTZ_dV]    = new Parameter("Volume change from normal",    "deltaV",   uVol,   -10000.0,   10000.0, false, false,  false, false, 0.0);
  allParam[pTZ_C]     = new Parameter("Test-zone compressibility",    "C_tz",     uComp,  1.0E-20,    0.10,    true,  true,   false, false, false);
  allParam[pTZ_pbo]   = new Parameter("Partial penetration bottom offset",  "PO_tz",   uDist,  0.01,       1000.0,  true,  false,  false, false, 0.0);
  allParam[pTZ_pl]    = new Parameter("Partial penetration screen length",  "PL_tz",   uDist,  0.01,       1000.0,  true,  false,  false, false, 1.0);

  allParam[pL_t]    = new Parameter("Leaky layer thickness",         "b_l",      uDist,  1.0E-10,    5.0E+03, false, false, false, false);
  allParam[pL_k ]   = new Parameter("Leaky permeability",            "k_l",      uPerm,  1.0E-30,    1.0E+00, true,  false, false, false, false);
  allParam[pL_HK]   = new Parameter("Leaky conductivity",            "K_l",      uK,     1.0E-30,    1.0E+02, true,  false, false, false, false);
  allParam[pL_Ss]     = new Parameter("Leaky spec. storage",           "ss_l",      uStor,  1.0E-30,    1.0E+03, true,  false, false, false, false);
  allParam[pL_C]    = new Parameter("Leaky compressibility",         "C_l",      uComp,  1.0E-30,    1.0E-01, true,  false, false, false, false);
  allParam[pL_Phi]  = new Parameter("Leaky porosity",                "phi_l", uRatio, 0.0,        1.0E+00, true,  false, false, false, SC_RealFormat());

  allParam[pLL_t]     = new Parameter("Lower leaky layer thickness",         "b_Ll",      uDist,  1.0E-10,    5.0E+03, false, false, false, false);
  allParam[pLL_k ]    = new Parameter("Lower leaky permeability",            "k_Ll",      uPerm,  1.0E-30,    1.0E+00, true,  false, false, false, false);
  allParam[pLL_HK]    = new Parameter("Lower leaky conductivity",            "K_Ll",      uK,     1.0E-30,    1.0E+02, true,  false, false, false, false);
  allParam[pLL_Ss]    = new Parameter("Lower leaky spec. storage",           "ss_Ll",      uStor,  1.0E-30,    1.0E+03, true,  false, false, false, false);
  allParam[pLL_C]     = new Parameter("Lower leaky compressibility",         "C_Ll",      uComp,  1.0E-30,    1.0E-01, true,  false, false, false, false);
  allParam[pLL_Phi] = new Parameter("Lower leaky porosity",                "phi_Ll",  uRatio, 0.0,        1.0E+00, true,  false, false, false, SC_RealFormat());

  allParam[pUL_t]     = new Parameter("Upper leaky layer thickness",         "b_Ul",      uDist,  1.0E-10,    5.0E+03, false, false, false, false);
  allParam[pUL_k ]    = new Parameter("Upper leaky permeability",            "k_Ul",      uPerm,  1.0E-30,    1.0E+00, true,  false, false, false, false);
  allParam[pUL_HK]    = new Parameter("Upper leaky conductivity",            "K_Ul",      uK,     1.0E-30,    1.0E+02, true,  false, false, false, false);
  allParam[pUL_Ss]    = new Parameter("Upper leaky spec. storage",           "ss_Ul",      uStor,  1.0E-30,    1.0E+03, true,  false, false, false, false);
  allParam[pUL_C]     = new Parameter("Upper leaky compressibility",         "C_Ul",      uComp,  1.0E-30,    1.0E-01, true,  false, false, false, false);
  allParam[pUL_Phi] = new Parameter("Upper leaky porosity",                "phi_Ul",  uRatio, 0.0,        1.0E+00, true,  false, false, false, SC_RealFormat());

  allParam[pN_n_r]    = new Parameter("# of radial nodes",                   "n_r",       uNone,  10.0,       1.0E+05, false, false, false, false, SC_DecFormat(0), 250.0);
  allParam[pN_n_m]    = new Parameter("# of matrix nodes",                   "n_m",       uNone,  1.0,        100.0,   false, false, false, false, SC_DecFormat(0), 1.0);
  allParam[pN_n_s]    = new Parameter("# of skin nodes",                     "n_s",       uNone,  5.0,        9000.0,  false, false, false, false, SC_DecFormat(0), 50.0);
  allParam[pN_n_l]    = new Parameter("# of leaky nodes",                    "n_l",       uNone,  2.0,        100.0,   false, false, false, false, SC_DecFormat(0), 5.0);
  allParam[pN_n_v2Dw] = new Parameter("# of vertical well nodes",            "n_vw",      uNone,  2.0,        100.0,   false, false, false, false, SC_DecFormat(0), 5.0);
  allParam[pN_n_v2Du] = new Parameter("# of vertical nodes above well",      "n_vu",      uNone,  1.0,        100.0,   false, false, false, false, SC_DecFormat(0), 5.0);
  allParam[pN_n_v2Dl] = new Parameter("# of vertical nodes below well",      "n_vb",      uNone,  1.0,        100.0,   false, false, false, false, SC_DecFormat(0), 5.0);
  allParam[pN_P_tol]  = new Parameter("Pressure solution tolerance",         "tol_P",     uPress, 1.0E-20,    1.0,     false, false, false, false, SC_SciFormat(5), 1.0E-10);
  allParam[pN_Qv_tol] = new Parameter("STP flow solution tolerance",         "tol_Qv",    uFlow,  1.0E-30,    1.0,     false, false, false, false, SC_SciFormat(5), 1.0E-15);
  allParam[pN_Qm_tol] = new Parameter("Mass flow solution tolerance",        "tol_Qm",    uMflow, 1.0E-30,    1.0,     false, false, false, false, SC_SciFormat(5), 1.0E-15);
  allParam[pN_kVMult] = new Parameter("Well conductance multiplier",         "kVMult",    uNone,  1.0,    1.0E+12, false, false, false, false, SC_SciFormat(2), 1.0E+06);

  allParam[pN_tol]   = new Parameter("Matrix solver tolerance",         "tol",      uNone,  1.0E-30,  1.0,   false, false, false, false, SC_SciFormat(2), 1.0E-06);
  allParam[pN_maxIter] = new Parameter("Matrix solver max iterations",        "maxIter",  uNone,  2.0,      1.0E+6,  false, false, false, false, SC_DecFormat(0), 10.0);

  for (int i = 0; i < maxGeoLayers; i++)
  {
    int nextPar = GetLayerOffset(i);

    allParam[nextPar + pLay_k]    = new Parameter("Layer permeability",       "k_l",     uPerm,  1.0E-30,    1.0E+00, true,  true,  true,  true, false);
    allParam[nextPar + pLay_kV]   = new Parameter("Layer vertical permeability","kv_l",  uPerm,  1.0E-30,    1.0E+00, true,  true,  true,  false, false);
    allParam[nextPar + pLay_HK]     = new Parameter("Layer conductivity",       "K_l",     uK,     1.0E-30,    1.0E+02, true,  true,  true,  true, false);
    allParam[nextPar + pLay_HKV]    = new Parameter("Layer vertical conductivity","Kv_l",  uK,     1.0E-30,    1.0E+02, true,  true,  true,  false, false);
    allParam[nextPar + pLay_Ss]     = new Parameter("Layer spec. storage",      "ss_l",    uStor,  1.0E-30,    1.0E+03, true,  true,  true,  true, false);
    allParam[nextPar + pLay_C]      = new Parameter("Layer compressibility",    "C_l",     uComp,  1.0E-30,    1.0E-01, true,  true,  true,  true, 1E-11, false);
    allParam[nextPar + pLay_Phi]  = new Parameter("Layer porosity",           "phi_l",   uRatio, 0.0,        1.0E+00, true,  true,  true,  true, SC_RealFormat());
    allParam[nextPar + pLay_Kf]     = new Parameter("Layer klinkenburg factor", "b_l",    uPress, 0.0,        1.0E+06, true,  true,  true,  false, SC_SciFormat(5), 0.0);

    allParam[nextPar + pLayS_k]   = new Parameter("Layer skin permeability",       "k_sl",     uPerm,  1.0E-30,    1.0E+00, true,  true,  true,  true, false);
    allParam[nextPar + pLayS_HK]    = new Parameter("Layer skin conductivity",       "K_sl",     uK,     1.0E-30,    1.0E+02, true,  true,  true,  true, false);
    allParam[nextPar + pLayS_Ss]    = new Parameter("Layer skin spec. storage",      "ss_sl",    uStor,  1.0E-30,    1.0E+03, true,  true,  true,  true, false);
    allParam[nextPar + pLayS_C]     = new Parameter("Layer skin compressibility",    "C_sl",     uComp,  1.0E-30,    1.0E-01, true,  true,  true,  true, 1E-11, false);
    allParam[nextPar + pLayS_Phi] = new Parameter("Layer skin porosity",           "phi_sl",   uRatio, 0.0,        1.0E+00, true,  true,  true,  true, SC_RealFormat());
    allParam[nextPar + pLayS_Kf]  = new Parameter("Layer skin klinkenburg factor", "b_sl",    uPress, 0.0,        1.0E+06, true,  true,  true,  false, SC_SciFormat(5), 0.0);

    allParam[nextPar + pLayS_Kf]  = new Parameter("Layer skin klinkenburg factor", "b_sl",    uPress, 0.0,        1.0E+06, true,  true,  true,  false, SC_SciFormat(5), 0.0);

    layerPar[i].Init();
  }

}

// constructor initializes everything
ParameterSupport::ParameterSupport()
{
  layerPar.AllocAndSetSize(maxGeoLayers);
  InitAllParameters();

  formationPar.Init();
  fracturePar.Init();
  matrixPar.Init();
  skinPar.Init();
  fluidPar.Init();
  gasPar.Init();
  testZonePar.Init();
  leakyPar.Init();
  lowerLeakyPar.Init();
  upperLeakyPar.Init();
  numericPar.Init();
}

ParameterSupport::~ParameterSupport()
{
  for (int i = 0; i < allParam.Size(); i++)
  {
    delete allParam[i];
  }
}

Parameter&  AllocatedParameterArray::GetRef(int indx)
{
  return *(*this)[indx];
}


ParameterPtrArray::ParameterPtrArray()
{
  SetResizable(10);
  SetToNull(parDesc);
}

ParameterPtrArray::ParameterPtrArray(const char* inDesc)
{
  SetResizable(10);
  CopyString(parDesc, inDesc, parDescLen);
}


void  ParameterPtrArray::Init()
{
  SetEmpty();
}


Parameter&  ParameterPtrArray::GetRef(int indx) const
{
  return *(*this)[indx];
}


void  ParameterPtrArray::AddActive(int parToAdd)
{
  Parameter* currPar = allParam[parToAdd];
  currPar->SetActive(true);
  (*this) += currPar;
}

void  ParameterPtrArray::StdGasParSetup(Parameters      pk,
                                        Parameters      pPhi,
                                        Parameters      pKf)
{
  AddActive(pk);          // permeability
  AddActive(pPhi);      // Porosity
  if (gasUseKlinkenburg)
    AddActive(pKf);     //klinkenburg
}

void ParameterPtrArray::StdFluidParSetup(Parameters      pk,
    Parameters      pHK,
    Parameters      pSs,
    Parameters      pC,
    Parameters      pPhi,
    Parameters      pSy)
{
  if (enterPermeabilityData)
    AddActive(pk);          // permeability
  else
    AddActive(pHK);         // Hydraulic conductivity

  if (enterSpecificStorageData)
    AddActive(pSs);         // Specific storage
  else
  {
    AddActive(pC);         // Specific storage
    AddActive(pPhi);     // Porosity
  }
  if (control.IsUnconfined())
    AddActive(pSy);         // Specific yield storage

}


ParameterPtrArray   formationPar("Formation");
ParameterPtrArray   fracturePar("Fracture");
ParameterPtrArray   matrixPar("Matrix");
ParameterPtrArray   skinPar("Skin");
ParameterPtrArray   fluidPar("Fluid");
ParameterPtrArray   gasPar("Gas");
ParameterPtrArray   testZonePar("Test Zone");
ParameterPtrArray   leakyPar("Leaky");
ParameterPtrArray   lowerLeakyPar("Lower Leaky");
ParameterPtrArray   upperLeakyPar("Upper Leaky");
ParameterPtrArray   numericPar("Numeric");
ParameterPtrArray   layerCommon("Layer Common");
ParameterPtrMatrix  layerPar;


ParameterSupport   parameters;

void  ParameterSupport::SetToInitial()
{
  for (int i = 0; i < allParam.Size(); i++)
  {
    delete allParam[i];
    allParam[i] = 0;
  }
  InitAllParameters();
  SetForFlags();
}


void ParameterSupport::SetSingleGasForFlags()
{
  formationPar.StdGasParSetup(pF_k, pF_Phi, pF_Kf);
  if (control.HasSkin())
    skinPar.StdGasParSetup(pS_k, pS_Phi, pS_Kf);

  testZonePar.AddActive(pTZ_dV);
  numericPar.AddActive(pN_P_tol);

  gasPar.AddActive(pG_Patm);
  if (gasFlowIsMass)
  {
    numericPar.AddActive(pN_Qm_tol);
    gasPar.AddActive(pG_MW);
  }
  else
    numericPar.AddActive(pN_Qv_tol);

  gasPar.AddActive(pG_Tref);
  gasPar.AddActive(pG_mu);
  if (gasUseSimpleViscositySlope)
    gasPar.AddActive(pG_mu_m);
}

void ParameterSupport::SetSingleFluidForFlags()
{
  if (control.IsSingle())
  {
    formationPar.StdFluidParSetup(pF_k, pF_HK, pF_Ss, pF_C, pF_Phi, pF_Sy);

    if (!(control.IsLeaky() || control.Is2DRadial()))
    {
      //  possible fP
      numericPar.AddActive(pN_P_tol);
      numericPar.AddActive(pN_Qv_tol);
    }

    if (control.Is2DRadial())
    {
      numericPar.AddActive(pN_n_v2Dw);   // # of well nodes

      if (control.IsPartialPenetration())
      {
        testZonePar.AddActive(pTZ_pl);   // well length

        if (control.IsTopPartialPenetration())
        {
          numericPar.AddActive(pN_n_v2Dl);  // nodes below
        }
        else if (control.IsMiddlePartialPenetration())
        {
          numericPar.AddActive(pN_n_v2Du);  // nodes above
          numericPar.AddActive(pN_n_v2Dl);  // nodes below
          testZonePar.AddActive(pTZ_pbo);   // offset from bottom
        }
        else
        {
          numericPar.AddActive(pN_n_v2Du);  // nodes above
        }
      }

      if (control.IsVerticalAnisotropic())
      {
        if (enterPermeabilityData)
          formationPar.AddActive(pF_kv);          // permeability
        else
          formationPar.AddActive(pF_HKv);         // Hydraulic conductivity
      }

      if (control.IsMultiplierWellModel())
        numericPar.AddActive(pN_kVMult);      // well multiplier

    }
  }
  else
  {
    fracturePar.StdFluidParSetup(pFr_k, pFr_HK, pFr_Ss, pFr_C, pFr_Phi, pF_Sy);
    matrixPar.AddActive(pM_V);           // volume factor
    if (enterDualAlpha)
      matrixPar.AddActive(pM_Alpha);
    else if (dualGeometryIsPrismatic)
      matrixPar.AddActive(pM_t);
    else
      matrixPar.AddActive(pM_d);
    matrixPar.StdFluidParSetup(pM_k, pM_HK, pM_Ss, pM_C, pM_Phi, pF_Sy);
    numericPar.AddActive(pN_n_m);
  }

  if (control.IsHorizontalAnisotropic())
  {
    formationPar.AddActive(pF_KxyRatio);          // ratio
    formationPar.AddActive(pF_KxyAngle);          // angle
  }

  if (control.HasSkin())
    skinPar.StdFluidParSetup(pS_k, pS_HK, pS_Ss, pS_C, pS_Phi, pS_Sy);

  if (sequence.ContainsSlugBC())
  {
    testZonePar.AddActive(pTZ_r_s);
  }

  if (sequence.ContainsPulseBC())
  {
    testZonePar.AddActive(pTZ_dV);
    testZonePar.AddActive(pTZ_C);
  }

  if (control.IsLeaky())
    numericPar.AddActive(pN_n_l);

  switch (leakageType)
  {
  case ltNone :
    break;
  case ltSingle : {
    leakyPar.AddActive(pL_t);
    leakyPar.StdFluidParSetup(pL_k, pL_HK, pL_Ss, pL_C, pL_Phi, pF_Sy);
    break;
  }
  case ltUpperLower : {
    lowerLeakyPar.AddActive(pLL_t);
    lowerLeakyPar.StdFluidParSetup(pLL_k, pLL_HK, pLL_Ss, pLL_C, pLL_Phi, pF_Sy);
    upperLeakyPar.AddActive(pUL_t);
    upperLeakyPar.StdFluidParSetup(pUL_k, pUL_HK, pUL_Ss, pUL_C, pUL_Phi, pF_Sy);
    break;
  }
  }

}

void ParameterSupport::SetFluidForFlags()
{
  //  fluid flags
  if (!enterSpecificStorageData)
    fluidPar.AddActive(pFl_C);

  if (!NSUnitsBase::headFlag)
  {
    fluidPar.AddActive(pFl_rho);
    allParam[pF_Pf]->fullName = "Static formation pressure";
  }
  else
    allParam[pF_Pf]->fullName = "Static formation head";

  if (enterPermeabilityData)
    fluidPar.AddActive(pFl_mu);
  fluidPar.AddActive(pFl_TX);

}

void ParameterSupport::SetSingleForFlags()
{
  //  always used
  formationPar.AddActive(pF_t);           // Thickness
  if (control.IsUnconfined())
    allParam[pF_t]->fullName = "Saturated thickness";
  else
    allParam[pF_t]->fullName = "Formation thickness";


  //if (control.Is1DRadial())   //adding flow dimension to 2d systems if less than 2
  formationPar.AddActive(pF_n);       // Flow dimension

  if (control.IsConfined())
    formationPar.AddActive(pF_Pf);      // Static pressure

  formationPar.AddActive(pF_ro);          // Boundary radius

  numericPar.AddActive(pN_n_r);

  if (control.HasSkin())
  {
    skinPar.AddActive(pS_t);
    numericPar.AddActive(pN_n_s);
  }

  testZonePar.AddActive(pTZ_r_w);

  if (control.IsGas())
  {
    SetSingleGasForFlags();
  }
  else
  {
    SetSingleFluidForFlags();
    SetFluidForFlags();
  }

}

void ParameterSupport::SetLayeredForFlags()
{
  layerCommon.AddActive(pTZ_r_w);
  layerCommon.AddActive(pF_ro);      // Boundary radius
  layerCommon.AddActive(pF_Pf);      // Static pressure
  numericPar.AddActive(pN_n_r);   // radial nodes

  layerPar.SetSize(geologyLayers.Size());
  bool skinFound = false;
  for (int i = 0; i < geologyLayers.Size(); i++)
  {
    int layerParOffset = GetLayerOffset(geologyLayers[i].parIndex);
    ParameterPtrArray& currLayerPar = layerPar[i];
    MakeString(currLayerPar.parDesc, geologyLayers[i].intervalID, " Parameters", ParameterPtrArray::parDescLen);
    currLayerPar.Init();

    const GeologyLayer& currLayer = geologyLayers[i];

    if (enterPermeabilityData)
    {
      currLayerPar.AddActive(layerParOffset + pLay_k);        // permeability
      if (!currLayer.layerIsIsotropic)
        currLayerPar.AddActive(layerParOffset + pLay_kV); // vert permeability
    }
    else
    {
      currLayerPar.AddActive(layerParOffset + pLay_HK);
      if (!currLayer.layerIsIsotropic)
        currLayerPar.AddActive(layerParOffset + pLay_HKV);
    }

    if (enterSpecificStorageData)
    {
      currLayerPar.AddActive(layerParOffset + pLay_Ss);         // Specific storage
    }
    else
    {
      currLayerPar.AddActive(layerParOffset + pLay_C);         // compressibility
      currLayerPar.AddActive(layerParOffset + pLay_Phi);     // Porosity
    }

    if (currLayer.layerHasSkin)
    {
      skinFound = true;
      if (enterPermeabilityData)
      {
        currLayerPar.AddActive(layerParOffset + pLayS_k);        // permeability
      }
      else
      {
        currLayerPar.AddActive(layerParOffset + pLayS_HK);
      }

      if (enterSpecificStorageData)
      {
        currLayerPar.AddActive(layerParOffset + pLayS_Ss);         // Specific storage
      }
      else
      {
        currLayerPar.AddActive(layerParOffset + pLayS_C);         // compressibility
        currLayerPar.AddActive(layerParOffset + pLayS_Phi);     // Porosity
      }
    }
  }

  if (skinFound)
  {
    numericPar.AddActive(pN_n_s);
    layerCommon.AddActive(pS_t);
  }
}

void ParameterSupport::SetForFlags()
{
  // clear
  for (int i = 0; i < allParam.Size(); i++)
    allParam[i]->SetActive(false);

  formationPar.SetEmpty();
  fracturePar.SetEmpty();
  matrixPar.SetEmpty();
  skinPar.SetEmpty();
  fluidPar.SetEmpty();
  gasPar.SetEmpty();
  testZonePar.SetEmpty();
  leakyPar.SetEmpty();
  lowerLeakyPar.SetEmpty();
  upperLeakyPar.SetEmpty();
  numericPar.SetEmpty();
  layerCommon.SetEmpty();
  layerPar.SetEmpty();

  if (control.IsLayered())
  {
    SetLayeredForFlags();
    SetFluidForFlags();
  }
  else
  {
    SetSingleForFlags();
  }
  //  all active param are set -- set inactive vary to fixed
  //  if sampling, set all vary to fixed
  //  if !sampling set all sampled to fixed
  for (int i = 0; i < allParam.Size(); i++)
  {
    Parameter& currPar = *(allParam[i]);
    // basic case -- inactive vary
    bool setFixed = (!currPar.IsActive()) && (currPar.IsVary());

    if (control.IsSampleSim())
      //  no vary on sampled
      setFixed = setFixed || currPar.IsVary();
    else
      //  no sampled on normal/range
      setFixed = setFixed || currPar.isSampled;

    // special case for opt to fixed
    if ((!control.IsOptimization()) && currPar.IsOpt())
    {
      setFixed = false;
      currPar.SetParameterType(Parameter::ptFixed, OptimizationControlStatics::updateParameterEstimates);
    }

    if (setFixed)
      currPar.SetParameterType(Parameter::ptFixed, false);
  }

  // now update to old opt/samp settings
  for (int i = 0; i < allParam.Size(); i++)
  {
    Parameter& currPar = allParam.GetRef(i);
    if  (!currPar.IsActive())
    {
      if (currPar.pPointsCurve)
        currPar.pPointsCurve->SetForFlags(false);
      if (currPar.rPointsCurve)
        currPar.rPointsCurve->SetForFlags(false);
      if (currPar.tPointsCurve)
        currPar.tPointsCurve->SetForFlags(false);
      continue;
    }

    if (currPar.IsFixed())
    {
      if (control.IsOptimization() && currPar.optParVal.wasActive)
        currPar.SetParameterType(Parameter::ptOpt, true);
      if (control.IsSampleSim() && currPar.sampParVal.wasActive)
        currPar.SetParameterType(Parameter::ptSample, true);
    }

    // and points
    if (currPar.pPointsCurve)
      currPar.pPointsCurve->SetForFlags(currPar.IsFPPoints());
    if (currPar.rPointsCurve)
      currPar.rPointsCurve->SetForFlags(currPar.IsFrPoints());
    if (currPar.tPointsCurve)
      currPar.tPointsCurve->SetForFlags(currPar.IsFtPoints());
  }

  if (!control.NonLinearOK())
  {
    // clear any nonlinear - set to basic
    for (int i = 0; i < allParam.Size(); i++)
    {
      Parameter& currPar = allParam.GetRef(i);
      if (currPar.IsActive() && currPar.IsNonLinear())
        currPar.SetParameterType(Parameter::ptFixed, true);
    }
  }
  if (!control.TimeVarOK())
  {
    // clear any nonlinear - set to basic
    for (int i = 0; i < allParam.Size(); i++)
    {
      Parameter& currPar = allParam.GetRef(i);
      if (currPar.IsActive() && currPar.IsTime())
        currPar.SetParameterType(Parameter::ptFixed, true);
    }
  }

  CheckForTimeAndNonlinear();
}


bool  ParameterSupport::nonlinearParFound = false;
bool  ParameterSupport::timeParFound = false;

void ParameterSupport::CheckForTimeAndNonlinear()
{
  nonlinearParFound = false;
  timeParFound = false;
  for (int i = 0; i < allParam.Size(); i++)
  {
    Parameter& currPar = allParam.GetRef(i);
    if (currPar.IsActive())
    {
      if (currPar.IsNonLinear())
        nonlinearParFound = true;
      if (currPar.IsTime())
        timeParFound = true;
    }
  }
}

bool ParameterSupport::ParametersOK()
{
  SetForFlags();
  return parameterErrorListing.ErrorCheckOK();
}


bool ParameterSupport::GetPointsParameters(ParameterPtrArray& pointParameters)
{
  // build array
  pointParameters.SetResizable(10);
  for (int i = 0; i < allParam.Size(); i++)
  {
    Parameter* currPar = allParam[i];
    if (currPar->IsActive() && currPar->IsPoints())
      pointParameters += currPar;
  }

  return !pointParameters.IsEmpty();
}

bool ParameterSupport::HasPointsParameters()
{
  ParameterPtrArray dummy;
  return GetPointsParameters(dummy);
}

void ParameterSupport::ResetPointsParameters()
{
  for (int i = 0; i < maxParameters; i++)
  {
    Parameter& currPar = allParam.GetRef(i);
    if (currPar.IsActive() && currPar.IsPoints())
      currPar.GetPointsCurve()->GetVals();
  }
}



// somewhat of a kluge to put it here ....
appFuncObjGlobals::AppFuncArray ParameterSupport::parameterObj("Parameters");

void ParameterSupport::InitAppGlobals()
{
  parameterObj.AddTo(&simulatorRadiiFO);
  parameterObj.AddTo(&parameterCurveOutputFO);

  appFuncObjGlobals::appFuncData += &parameterObj;
}

const char* ParameterSupport::groupHeader = "Parameters";
const int  ParameterSupport::majorVersion = 7;  // added global parameter curve FO
//const int  ParameterSupport::majorVersion = 6;  // added f(t) parameters
// const int  ParameterSupport::majorVersion = 5;  // changed FileCurve to FixedCure
//const int  ParameterSupport::majorVersion = 4;  // added pts curve wasActive
//const int  ParameterSupport::majorVersion = 3;  // added global obj
//const int  ParameterSupport::majorVersion = 2;  // removed vary par
//const int  ParameterSupport::majorVersion = 1;  // added points par
//const int  ParameterSupport::majorVersion = 0;
const int  ParameterSupport::minorVersion = 0;

void ParameterSupport::WriteParCurve(const ParameterCurveBase& inCurve)
{
  WriteBool(inCurve.xIsLog10);
  WriteBool(inCurve.yIsLog10);
  WriteUnitIndex(inCurve.xUnitIndex);
}

void ParameterSupport::ReadParCurve(ParameterCurveBase& outCurve)
{
  outCurve.xIsLog10 = ReadBool();
  outCurve.yIsLog10 = ReadBool();
  ReadUnitIndex(outCurve.xUnitIndex);
}
void ParameterSupport::WriteFixedCurve(const FixedCurve& inCurve)
{
  WriteParCurve(inCurve);
  WriteUnitIndex(inCurve.yUnitIndex);
  WriteLine();
  WriteFuncObjRef(inCurve.curveObjRef); // added v5
  WriteText(inCurve.curveID);
}

void ParameterSupport::ReadFixedCurve(FixedCurve& outCurve)
{
  ReadParCurve(outCurve);
  ReadUnitIndex(outCurve.yUnitIndex);
  if (currentObjReadMajorVersion > 4)
    ReadFuncObjRef(outCurve.curveObjRef); // added v5

  ReadText(outCurve.curveID, DC_CurveWithID::curveIDlen);
}

void ParameterSupport::WritePointsCurve(const PointsCurve& inCurve)
{
  WriteParCurve(inCurve);
  WriteLine();

  // opt control
  WriteDouble(inCurve.yoptMinLim);
  WriteDouble(inCurve.yoptMaxLim);
  WriteBool(inCurve.yoptIsLinear);
  WriteDouble(inCurve.yoptParSigma);

  WriteBool(inCurve.xoptIsLinear);
  WriteDouble(inCurve.xoptParSigma);
  WriteLine();

  // curve data
  const CurveDescription& currCrv = inCurve.pointsCurve;
  WriteInt(int(currCrv.curveType));
  WriteBool(currCrv.splineNaturalSlope);
  WriteDouble(currCrv.fixedSplineStartSlope);
  WriteDouble(currCrv.fixedSplineEndSlope);
  WriteDouble(currCrv.splineTension);
  WriteInt(currCrv.polyOrder);
  WriteLine();

  // opt points
  WriteInt(inCurve.pVals.Size());
  for ( int i = 0; i < inCurve.pVals.Size(); i++)
  {

    PointsCurveValue&  xVal = inCurve.pVals.GetRef(i).xVal;
    PointsCurveValue&  yVal = inCurve.pVals.GetRef(i).yVal;

    // y first
    WriteNullDouble(yVal.pVal);
    WriteInt(yVal.pType);
    bool varyDataOK = (yVal.pVaryData != 0);
    WriteBool(varyDataOK);
    if (varyDataOK)
      WriteVaryVar(*yVal.pVaryData);

    bool optDataOK = (yVal.pOptData != 0);
    WriteBool(optDataOK);
    if (optDataOK)
      WriteBool(yVal.pOptData->wasActive);  // added v4

    // x
    WriteNullDouble(xVal.pVal);
    WriteInt(xVal.pType);

    varyDataOK = (xVal.pVaryData != 0);
    WriteBool(varyDataOK);
    if (varyDataOK)
      WriteVaryVar(*xVal.pVaryData);

    optDataOK = (xVal.pOptData != 0);
    WriteBool(optDataOK);
    if (optDataOK)
    {
      WriteNullDouble(xVal.pOptData->optMinLim);
      WriteNullDouble(xVal.pOptData->optMaxLim);
      WriteBool(xVal.pOptData->wasActive);  // added v4
    }
    WriteLine();
  }

}


void ParameterSupport::ReadPointsCurve(PointsCurve& outCurve)
{
  ReadParCurve(outCurve);

  outCurve.yoptMinLim     = ReadDouble();
  outCurve.yoptMaxLim     = ReadDouble();
  outCurve.yoptIsLinear   = ReadBool();
  outCurve.yoptParSigma   = ReadDouble();

  outCurve.xoptIsLinear   = ReadBool();
  outCurve.xoptParSigma   = ReadDouble();

  CurveDescription& currCrv = outCurve.pointsCurve;
  currCrv.curveType               = Curve::CurveType(ReadInt());
  currCrv.splineNaturalSlope      = ReadBool();
  currCrv.fixedSplineStartSlope   = ReadDouble();
  currCrv.fixedSplineEndSlope     = ReadDouble();
  currCrv.splineTension           = ReadDouble();
  currCrv.polyOrder               = ReadInt();

  outCurve.pVals.AllocAndCreate(ReadInt());
  for (int i = 0; i < outCurve.pVals.Size(); i++)
  {
    PointsCurveValue&  xVal = outCurve.pVals.GetRef(i).xVal;
    PointsCurveValue&  yVal = outCurve.pVals.GetRef(i).yVal;

    yVal.pVal = ReadNullDouble();
    if (currentObjReadMajorVersion < 2)
    {
      if (ReadBool())
      {
        yVal.pType = PointsCurveValue::pcvOpt;
        yVal.InitOpt();  // sets to true and allocates
      }
      else
        yVal.pType = PointsCurveValue::pcvFixed;

    }
    else
    {
      yVal.pType = PointsCurveValue::PointsCurveValueType(ReadInt());
      if (ReadBool())
      {
        yVal.InitVary();
        ReadVaryVar(*yVal.pVaryData);
      }

      if (ReadBool())
      {
        yVal.InitOpt();
        if (currentObjReadMajorVersion > 3)
          yVal.pOptData->wasActive = ReadBool();  // added v4
      }
    }


    xVal.pVal = ReadNullDouble();
    if (currentObjReadMajorVersion < 2)
    {
      if (ReadBool())
        xVal.pType = PointsCurveValue::pcvOpt;
      else
        xVal.pType = PointsCurveValue::pcvFixed;

      bool optDataOK  = ReadBool();
      if (xVal.IsOpt() || optDataOK)
      {
        xVal.InitOpt();
        if (optDataOK)
        {
          xVal.pOptData->optMinLim = ReadNullDouble();
          xVal.pOptData->optMaxLim = ReadNullDouble();
        }
      }
    }
    else
    {
      xVal.pType = PointsCurveValue::PointsCurveValueType(ReadInt());

      if (ReadBool())
      {
        xVal.InitVary();
        ReadVaryVar(*xVal.pVaryData);
      }

      if (ReadBool())
      {
        xVal.InitOpt();
        xVal.pOptData->optMinLim = ReadNullDouble();
        xVal.pOptData->optMaxLim = ReadNullDouble();
        if (currentObjReadMajorVersion > 3)
          xVal.pOptData->wasActive = ReadBool();  // added v4
      }
    }
  }

  // reset all par with error check
  SC_SetupErr dummy;
  outCurve.SetupOK(dummy);
}

// implementation of virtuals
void ParameterSupport::WriteToFile()
{
  WriteGroupHeader(groupHeader, majorVersion, minorVersion);
  WriteGlobalFunc(simulatorRadiiFO);
  WriteGlobalFunc(parameterCurveOutputFO); // added v7

  SetRealConvFormat(SC_SciFormat(12));

  // get edited count
  int nedited = 0;
  for (int i = 0; i < allParam.Size(); i++)
    if (allParam[i]->WasEdited())
      nedited++;

  WriteInt(nedited);
  WriteLine();

  for (int i = 0; i < allParam.Size(); i++)
  {
    Parameter& currPar = allParam.GetRef(i);
    if (!currPar.WasEdited())
      continue;

    // index
    WriteInt(i);
    WriteLine();

    // par name for debugging only
    WriteText(currPar.fullName);
    WriteInt(currPar.parameterType);
    WriteUnitReal(currPar.parVal);
    WriteLine();


    WriteOptVar(currPar.optParVal);
    WriteSampVar(currPar.sampParVal);
    WriteVaryVar(currPar.varyParVal);   // v2

    // added v1
    WriteBool(currPar.pPointsCurve != 0);
    WriteBool(currPar.rPointsCurve != 0);
    WriteBool(currPar.tPointsCurve != 0);  // v6

    if (currPar.pPointsCurve != 0)
      WritePointsCurve(*currPar.pPointsCurve);
    if (currPar.rPointsCurve != 0)
      WritePointsCurve(*currPar.rPointsCurve);
    if (currPar.tPointsCurve != 0)          // v6
      WritePointsCurve(*currPar.tPointsCurve);  // v6

    WriteFixedCurve(currPar.pFixedCurve);
    WriteFixedCurve(currPar.rFixedCurve);
    WriteFixedCurve(currPar.tFixedCurve);     // v6

  }
}

bool ParameterSupport::ReadFromFile()
{
  ReadGroupHeader(groupHeader, majorVersion);

  if (currentObjReadMajorVersion > 2)
  {
    ReadGlobalFunc(simulatorRadiiFO);
    if (currentObjReadMajorVersion > 6)
      ReadGlobalFunc(parameterCurveOutputFO);
  }
  int nedited = ReadInt();

  for (int i = 0; i < nedited; i++)
  {
    Parameter& currPar = allParam.GetRef(ReadInt());

    // edited setting persists
    currPar.SetEdited();

    // par name for debugging only
    char tempStr[80];
    ReadText(tempStr, 80);

    currPar.parameterType = Parameter::ParameterType(ReadInt());
    ReadUnitReal(currPar.parVal);

    if (currentObjReadMajorVersion < 2)
    {
      VaryVar& currVary = currPar.varyParVal;
      ReadDoubleArray(currVary.suiteValues, true);
      currVary.rangeMinLim = ReadNullDouble();
      currVary.rangeMaxLim = ReadNullDouble();
      currVary.rangeIsLinear = ReadBool();
      currVary.rangeNStep = ReadInt();
    }


    ReadOptVar(currPar.optParVal);
    ReadSampVar(currPar.sampParVal);
    if (currentObjReadMajorVersion > 1)
      ReadVaryVar(currPar.varyParVal);

    if (currentObjReadMajorVersion > 0)
    {
      bool readpP = ReadBool();
      bool readrP = ReadBool();
      bool readtP = false;
      if (currentObjReadMajorVersion > 5)
        readtP = ReadBool();
      if (readpP)
      {
        currPar.pPointsCurve = new PointsCurve("P", currPar.shortName, uPress, currPar.parVal, currPar.defaultIsLinear);
        ReadPointsCurve(*currPar.pPointsCurve);
      }
      if (readrP)
      {
        currPar.rPointsCurve = new PointsCurve("R", currPar.shortName, uDist, currPar.parVal, currPar.defaultIsLinear);
        ReadPointsCurve(*currPar.rPointsCurve);
      }
      if (readtP)
      {
        currPar.tPointsCurve = new PointsCurve("T", currPar.shortName, uTime, currPar.parVal, currPar.defaultIsLinear);
        ReadPointsCurve(*currPar.tPointsCurve);
      }
    }

    ReadFixedCurve(currPar.pFixedCurve);
    ReadFixedCurve(currPar.rFixedCurve);
    if (currentObjReadMajorVersion > 5)
      ReadFixedCurve(currPar.tFixedCurve);
  }


  if (currentObjReadMajorVersion < 2)
  {
    int nskip = ReadInt();
    for (int j = 0; j < nskip; j++)
      ReadInt();
  }

  CheckForInputFailure("Reading Parameters");
  return true;
}



PointsParameterSupport   pointParameters;

PointsParameterListing::PointsParameterListing()
  : OSListObj("nPrePointsParameters")
{
}
void PointsParameterListing::CreateListing()
{
  StdTitle("f(x) Points Parameters");

  ParameterPtrArray pointParameters;
  parameters.GetPointsParameters(pointParameters);
  if (pointParameters.IsEmpty())
    AddText("No points parameters specified", 0);

  char tempStr[80];

  for (int i = 0; i < pointParameters.Size(); i++)
  {
    Parameter& currPar = *(pointParameters[i]);

    SubTitle(currPar.fullName);

    PointsCurve* currPoints = currPar.GetPointsCurve();
    const char* xStr;
    const char* pType;
    switch (currPar.GetCurveType()) {
    case Parameter::cptRadial: {
      xStr = " Radius";
      pType = "f(r)";
      break;
    }
    case Parameter::cptPressure: {
      xStr = " Pressure";
      pType = "f(P)";
      break;
    }
    case Parameter::cptTime: {
      xStr = " Time";
      pType = "f(t)";
      break;
    }
    }
    AddStdText("Points type", pType);

    bool xoptFound = false;
    bool yoptFound = false;

    UnitReal tmpX(currPoints->pVals.xUnits);
    UnitReal tmpY(currPoints->pVals.yUnits);

    for (int j = 0; j < currPoints->pVals.Size(); j++)
    {
      PointsCurveValue&  xVal = currPoints->pVals[j]->xVal;

      MakeString(tempStr, xStr, " #", 80);
      ConcatInt(j + 1, tempStr, 80);
      switch (xVal.pType) {
      case PointsCurveValue::pcvFixed : {
        tmpX.SetUserVal(xVal.pVal);
        AddUnitReal(tempStr, tmpX);
        break;
      }
      case PointsCurveValue::pcvOpt : {
        xoptFound = true;
        AddStdText(tempStr, "Optimized");
        OptVarUnits& currOpt = *(xVal.pOptData);
        tmpX.SetUserVal(currOpt.optMinLim);
        AddUnitReal("   Minimum", tmpX);
        tmpX.SetUserVal(currOpt.optParEstimate);
        AddUnitReal("   Estimat", tmpX);
        tmpX.SetUserVal(currOpt.optMaxLim);
        AddUnitReal("   Maximum", tmpX);
        break;
      }
      case PointsCurveValue::pcvVary: {
        AddStdText(tempStr, "Range/Suite");
        AddVaryVar(*(xVal.pVaryData), NULL);
        break;
      }
      }

      PointsCurveValue&  yVal = currPoints->pVals[j]->yVal;

      CopyString(tempStr, " Y value#", 80);
      ConcatInt(j + 1, tempStr, 80);
      switch (yVal.pType) {
      case PointsCurveValue::pcvFixed : {
        tmpY.SetUserVal(yVal.pVal);
        AddUnitReal(tempStr, tmpY);
        break;
      }
      case PointsCurveValue::pcvOpt : {
        yoptFound = true;
        AddStdText(tempStr, "Optimized");
        break;
      }
      case PointsCurveValue::pcvVary: {
        AddStdText(tempStr, "Range/Suite");
        AddVaryVar(*(yVal.pVaryData), NULL);
        break;
      }
      }
    }

    if (xoptFound)
    {
      AddConditionalBlank();
      AddBoolText("X opt range type", currPoints->xoptIsLinear, "Linear", "Log");
      AddStdReal( "X opt sigma", currPoints->xoptParSigma, SC_SciFormat(5));
    }

    if (yoptFound)
    {
      AddConditionalBlank();
      tmpY.SetUserVal(currPoints->yoptMinLim);
      AddUnitReal("Y opt minimum value", tmpY);
      tmpY.SetUserVal(currPoints->yoptMaxLim);
      AddUnitReal("Y opt maximum value", tmpY);
      AddBoolText("Y opt range type", currPoints->yoptIsLinear, "Linear", "Log");
      AddStdReal( "Y opt sigma", currPoints->yoptParSigma, SC_SciFormat(5));
    }

    CurveDescription& currCurve =  currPoints->pointsCurve;

    AddConditionalBlank();
    ClearEnumStrs();
    AddEnumStr("Cubic Spline", Curve::ctCubicSpline);
    AddEnumStr("Polynomial", Curve::ctPolynomial);
    AddEnumStr("Linear", Curve::ctLinear);
    AddEnumStr("Step Mid", Curve::ctStepMid);
    AddEnumStr("Step Full", Curve::ctStepFull);
    AddStdEnum("Parameter curve type" , currCurve.curveType);

    if (currCurve.curveType == Curve::ctCubicSpline)
    {
      AddBoolText("Spline end-slopes", currCurve.splineNaturalSlope, "Natural", "Set");
      if (!currCurve.splineNaturalSlope)
      {
        AddStdReal( "Initial slope ", currCurve.fixedSplineStartSlope, SC_SciFormat(5));
        AddStdReal( "Final slope ", currCurve.fixedSplineEndSlope, SC_SciFormat(5));
      }
      AddStdReal( "Spline tension", currCurve.splineTension, SC_DecFormat(3));
    }
    else if (currCurve.curveType == Curve::ctPolynomial)
    {
      AddStdInt( "Polynomial order", currCurve.polyOrder);
    }

  }
}

PointsParameterErrorListing::PointsParameterErrorListing()
  :ErrorListObjC("PointsParameter")
{

}

void PointsParameterErrorListing::CreateListing()
{
  StdTitle("f(P)/f(r)/f(t) Points Parameters Errors");

  ParameterPtrArray pointParameters;
  parameters.GetPointsParameters(pointParameters);
  SC_SetupErr parErr;
  for (int i = 0; i < pointParameters.Size(); i++)
    if (!pointParameters.GetRef(i).SetupOK(parErr))
      AddError(parErr);
}

ParameterCurveFO    parameterCurveFO;

ParameterCurveFO::ParameterCurveFO() :
  FuncObjC("ParameterCurveUpdate")
{
}

void ParameterCurveFO::IsBeingDeleted(FuncObjC* delObj)
{
  for (int i = 0; i < maxParameters; i++)
    if (allParam[i] != 0)
    {
      Parameter& currPar = *(allParam[i]);
      if (currPar.pFixedCurve.curveObjRef == delObj)
        currPar.pFixedCurve.curveObjRef = FuncObjRef();
      if (currPar.rFixedCurve.curveObjRef == delObj)
        currPar.rFixedCurve.curveObjRef = FuncObjRef();
      if (currPar.tFixedCurve.curveObjRef == delObj)
        currPar.tFixedCurve.curveObjRef = FuncObjRef();
    }
}

bool ParameterCurveFO::UsesAsInput(const FuncObjC* inputObj) const
{
  for (int i = 0; i < maxParameters; i++)
    if (allParam[i] != 0)
    {
      Parameter& currPar = *(allParam[i]);
      if ((currPar.pFixedCurve.curveObjRef.objRef == inputObj) ||
          (currPar.rFixedCurve.curveObjRef.objRef == inputObj) ||
          (currPar.tFixedCurve.curveObjRef.objRef == inputObj))
        return true;
    }
  return false;
}



void ParameterCurveFO::UpdateFuncObjRefs()
{
  for (int i = 0; i < maxParameters; i++)
    if (allParam[i] != 0)
    {
      Parameter& currPar = allParam.GetRef(i);
      PerformUpdate(currPar.pFixedCurve.curveObjRef);
      PerformUpdate(currPar.rFixedCurve.curveObjRef);
      PerformUpdate(currPar.tFixedCurve.curveObjRef);
    }
}


};  // end namespace


