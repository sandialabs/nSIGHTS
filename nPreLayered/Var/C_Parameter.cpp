///////////////////////////////////////////////////////////////////////////////////
//
// C_Parameter.cpp
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
#include <objClass/DO_Curve.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <nsClass/nSightConst.h>

#include <Var/G_Control.h>
#include <Var/G_CurveFiles.h>

#include <Var/C_Parameter.h>


ParameterCurve::ParameterCurve(UnitType   xUnitType,
                               UnitIndex& yUnitIndexInit)
  : yUnitIndex(yUnitIndexInit)
{
  xUnitIndex = xUnitType;
  xIsLog10 = false;
  yIsLog10 = false;
}

ParameterCurve& ParameterCurve::operator= (const ParameterCurve& a)
{
  if (&a != this)
  {
    xIsLog10 = a.xIsLog10;
    yIsLog10 = a.yIsLog10;
  }
  return *this;
}
double ParameterCurve::GetMetricCurveY(const double&   metricXVal,
                                       DC_Curve& curve)
{
  // sanity checks
  if (!curve.IsCreated())
    GenAppInternalError("ParameterCurve::GetMetricPointCurveY #1");
  if (RealIsNull(metricXVal))
    GenAppInternalError("ParameterCurve::GetMetricPointCurveY #2");

  //  convert metric x to user x
  double userX = xUnitIndex.MetricToUser(metricXVal);

  //  take log if required
  if (xIsLog10)
  {
    if (userX < stdEps)
      GenAppInternalError("ParameterCurve::GetMetricPointCurveY #3");
    userX = log10(userX);
  }

  // get curve value
  double userY = curve.GetCurveY(userX);

  // if log then transform
  if (yIsLog10)
  {
    if ((userY < -99.0) || (userY > 99.0))
      GenAppInternalError("ParameterCurve::GetMetricPointCurveY #4");
    userY = pow(10.0, userY);
  }

  //  convert units && return
  return yUnitIndex.UserToMetric(userY);
}


PointsCurveValue::PointsCurveValue(UnitIndex&  inUnitIndex, bool defIsLinear)
  : unitIndex(inUnitIndex), defaultIsLinear(defIsLinear)
{
  pType    = pcvFixed;
  pVal         = 0.0;
  pOptData     = 0;
  pVaryData    = 0;
  pID[0]       = '\0';
  pOptIsActive = false;
  pVaryIsActive = false;
}

PointsCurveValue::~PointsCurveValue()
{
  if (pOptData)
    delete pOptData;
  if (pVaryData)
    delete pVaryData;
}

PointsCurveValue& PointsCurveValue::operator= (const PointsCurveValue& a)
{
  if (&a != this)
  {
    pVal     = a.pVal;
    pType  = a.pType;
    if (a.pOptData)
    {
      if (!pOptData)
        pOptData = new OptVarUnits(pOptIsActive, pID, unitIndex, defaultIsLinear);
      (*static_cast<OptVar*>(pOptData)).operator= (*a.pOptData);
    }
    if (a.pVaryData)
    {
      if (!pVaryData)
        pVaryData = new VaryVarUnits(pVaryIsActive, pID, unitIndex, defaultIsLinear);
      (*static_cast<VaryVar*>(pVaryData)).operator= (*a.pVaryData);
    }
  }
  return *this;
}

void PointsCurveValue::SetActive(bool parIsActive)
{
  pOptIsActive = parIsActive && IsOpt() && control.IsOptimization();
  pVaryIsActive = parIsActive && IsVary() && (!control.IsSampleSim());
}

void PointsCurveValue::ClearWasActive()
{
  if (pOptData)
    pOptData->wasActive = false;
  if (pVaryData)
    pVaryData->wasActive = false;
}


void PointsCurveValue::SetForFlags(bool parIsActive)
{
  if (IsOpt() && (!control.IsOptimization()))
    pType = pcvFixed;

  if (IsVary() && control.IsSampleSim())
    pType = pcvFixed;

  if (IsFixed() && control.IsOptimization() && pOptData && pOptData->wasActive)
    pType = pcvOpt;

  SetActive(parIsActive);
}


void PointsCurveValue::InitOpt()
{
  if (!pOptData)
    pOptData = new OptVarUnits(pOptIsActive, pID, unitIndex, defaultIsLinear);
}

void PointsCurveValue::InitVary()
{
  if (!pVaryData)
    pVaryData = new VaryVarUnits(pVaryIsActive, pID, unitIndex, defaultIsLinear);
}

void PointsCurveValue::SetID(const char baseName[],
                             int  index)
{
  char tempStr[20];
  IntToString(index + 1, tempStr, 2, 20);
  MakeString(pID, baseName, "[", tempStr, "]", pointsIDLen);
}

void PointsCurveValue::GetVals()
{
  if (IsOpt() && control.IsOptimization())
    pVal = pOptData->GetParEstimate();
  else if (IsVary() && (!control.IsSampleSim()))
    pVal = pVaryData->GetVaryVal();
}

void PointsCurveValue::SetVals()
{
  if (IsOpt() && control.IsOptimization())
    pOptData->SetParEstimate(pVal);
}


bool PointsCurveValue::SetupOK(SC_SetupErr& errData,
                               bool     pIsLog)
{

  if (IsOpt())
    return pOptData->OptimizeSetupOK(errData);
  else if (IsVary())
    return pVaryData->VarySetupOK(errData);

  SC_SetupErr parErr;
  if (RealIsNull(pVal))
    parErr.SetConstantError("data value not set");
  else if (pIsLog && (pVal <= 1.0E-40))
    parErr.SetConstantError("log curve is -ve");

  return errData.SetupOK(pID, parErr);
}


static UnitIndex dummy;
PointsCurveXY::PointsCurveXY() :
  xVal(dummy, true), yVal(dummy, true)
{
  GenAppInternalError("PointsCurveXY::PointsCurveXY()");
}


PointsCurveXY::PointsCurveXY(UnitIndex&  inX, UnitIndex&  inY, bool defYisLin)
  : xVal(inX, true), yVal(inY, defYisLin)
{
}
PointsCurveXY& PointsCurveXY::operator= (const PointsCurveXY& a)
{
  if (&a != this)
  {
    xVal = a.xVal;
    yVal = a.yVal;
  }
  return *this;
}

PointsCurveXYArray::PointsCurveXYArray(UnitIndex&  inX, UnitIndex&  inY, bool initDefYisLinear) :
  xUnits(inX), yUnits(inY), defYIsLinear(initDefYisLinear),
  T_SC_AllocPtrArray<PointsCurveXY>()
{
}

void  PointsCurveXYArray::MakeNew(int index)
{
  tListData[index] = new PointsCurveXY(xUnits, yUnits, defYIsLinear);
}


PointsCurve::PointsCurve(const char* xVarIDInit,
                         const char* yVarIDInit,
                         UnitType   xUnitType,
                         UnitIndex& yUnitIndexInit,
                         bool   parIsLinear)
  : ParameterCurve(xUnitType, yUnitIndexInit),
    pVals(xUnitIndex, yUnitIndexInit, parIsLinear),
    xVarID(xVarIDInit), yVarID(yVarIDInit)
{
  xIsLog10 = false;
  yIsLog10 = false;

  yoptMinLim   = 0.0;
  yoptMaxLim   = 1.0;
  yoptIsLinear = parIsLinear;
  if (!parIsLinear)
    yoptMinLim   = 1.0E-05;

  yoptParSigma = 1.0;

  xoptIsLinear = true;
  xoptParSigma = 1.0;
}

/*
PointsCurve& PointsCurve::operator= (const PointsCurve& a)
{
    if (&a != this)
    {
        ParameterCurve::operator=(a);
        for ( int i = 0; i < maxPointArray; i++)
        {
            xVal[i] = a.xVal[i];
            yVal[i] = a.yVal[i];
        }
        pointsCurve = a.pointsCurve;

        yoptMinLim   = a.yoptMinLim  ;
        yoptMaxLim   = a.yoptMaxLim  ;
        yoptIsLinear = a.yoptIsLinear;
        yoptParSigma = a.yoptParSigma;

        xoptIsLinear = a.xoptIsLinear;
        xoptParSigma = a.xoptParSigma;
    }
    return *this;
}
*/

//  standard for parameters
void PointsCurve::GetVals()  // gets current opt val(s) for each xy
{
  pVals.Cleanup();
  for (int i = 0; i < pVals.Size(); i++)
  {
    pVals.GetRef(i).xVal.GetVals();
    pVals.GetRef(i).yVal.GetVals();
  }
}

void PointsCurve::SetVals()
{
  pVals.Cleanup();
  for (int i = 0; i < pVals.Size(); i++)
  {
    pVals.GetRef(i).xVal.SetVals();
    pVals.GetRef(i).yVal.SetVals();
  }
}

void PointsCurve::SetActive(bool parIsActive)
{
  pVals.Cleanup();
  for (int i = 0; i < pVals.Size(); i++)
  {
    pVals.GetRef(i).xVal.SetActive(parIsActive);
    pVals.GetRef(i).yVal.SetActive(parIsActive);
  }
}

void PointsCurve::ClearWasActive()
{
  pVals.Cleanup();
  for (int i = 0; i < pVals.Size(); i++)
  {
    pVals.GetRef(i).xVal.ClearWasActive();
    pVals.GetRef(i).yVal.ClearWasActive();
  }
}


void PointsCurve::SetForFlags(bool parIsActive)
{
  pVals.Cleanup();
  for (int i = 0; i < pVals.Size(); i++)
  {
    pVals.GetRef(i).xVal.SetForFlags(parIsActive);
    pVals.GetRef(i).yVal.SetForFlags(parIsActive);
  }
}


bool PointsCurve::SetupOK(SC_SetupErr& errData)
{
  pVals.Cleanup();

  SetVals();

  SC_SetupErr parErr;
  if (pVals.IsEmpty())
    parErr.SetConstantError("no curve points");

  // check each point
  double lastX;
  double lastMaxX;
  char varID[40];

  for (int i = 0; i < pVals.Size(); i++)
  {
    PointsCurveValue&  xVal = pVals.GetRef(i).xVal;
    PointsCurveValue&  yVal = pVals.GetRef(i).yVal;

    // set IDs for individual error checking use

    MakeString(varID, yVarID, ".", xVarID, 40);
    xVal.SetID(varID, i);
    MakeString(varID, yVarID, ".V", 40);
    yVal.SetID(varID, i);

    // set and check
    if (control.IsOptimization())
    {
      if (xVal.IsOpt())
      {
        OptVarUnits& currOpt = *xVal.pOptData;
        currOpt.optParSigma = xoptParSigma;
        currOpt.optIsLinear = xoptIsLinear;
        if (!currOpt.OptimizeSetupOK(parErr))
          break;
      }

      if (yVal.IsOpt())
      {
        OptVarUnits& currOpt = *yVal.pOptData;

        currOpt.optMinLim   = yoptMinLim;
        currOpt.optMaxLim   = yoptMaxLim;

        if (yIsLog10)
        {
          if ((currOpt.optMinLim <= 0.0)  || (currOpt.optMaxLim <= 0.0))
          {
            parErr.SetConstantError("Y log10 limits are -ve");
            break;
          }
        }

        currOpt.optParSigma = yoptParSigma;
        currOpt.optIsLinear = yoptIsLinear;

        if (!currOpt.OptimizeSetupOK(parErr))
          break;
      }
    }


    // do basic check
    if (!(xVal.SetupOK(parErr, xIsLog10) && yVal.SetupOK(parErr, yIsLog10)))
      break;

    // check for ascending X
    if (i > 0)
    {
      //  basic check
      if (xVal.pVal <= lastX)
      {
        parErr.SetConstantError("X values must ascend");
        break;
      }

      // optimized X ranges must not overlap
      if (control.IsOptimization() && xVal.IsOpt())
      {
        if (xVal.pOptData->optMinLim <= lastMaxX)
        {
          parErr.SetConstantError("X optimization ranges overlap");
          break;
        }
        lastMaxX = xVal.pOptData->optMaxLim;
      }

      if (!control.IsSampleSim() && xVal.IsVary())
      {
        double minX, maxX;
        xVal.pVaryData->GetMinMax(minX, maxX);
        if (minX <= lastMaxX)
        {
          parErr.SetConstantError("X range/suite ranges overlap");
          break;
        }
        lastMaxX = maxX;
      }
    }
    else
    {
      if (control.IsOptimization() && xVal.IsOpt())
        lastMaxX = xVal.pOptData->optMaxLim;
      else
        lastMaxX = xVal.pVal;
    }

    lastX = xVal.pVal;

    if (xVal.IsFixed())
      lastMaxX = lastX;
  }

  if (parErr.NoErrors())
    pointsCurve.CurveSetupOK(pVals.Size(), parErr);

  return errData.SetupOK(yVarID, parErr);
}


bool PointsCurve::CurveOK(SC_SetupErr& errData)
{
  //  assumes basic setup OK
  GetVals();

  // copy xy data into curve input
  DC_XYData curveInput;
  curveInput.Alloc(pVals.Size());

  for (int i = 0; i < pVals.Size(); i++)
  {
    double xVal = xUnitIndex.MetricToUser(pVals.GetRef(i).xVal.pVal);
    double yVal = yUnitIndex.MetricToUser(pVals.GetRef(i).yVal.pVal);
    if (xIsLog10)
      xVal = log10(xVal);
    if (yIsLog10)
      yVal = log10(yVal);

    curveInput.xData += xVal;
    curveInput.yData += yVal;
  }

  SC_SetupErr crvErr;
  pointsCurve.CreateCurve(curveInput, crvErr);

  return errData.SetupOK(yVarID, crvErr);
}

//  curve extraction
double PointsCurve::GetMetricPointCurveY(const double& metricXVal)
{
  return GetMetricCurveY(metricXVal, pointsCurve);
}

FixedCurve::FixedCurve(       CurveFile& curveFileInit,
                              UnitType   xUnitType,
                              UnitType   yUnitType,
                              const  char*      parIDInit)
  : ParameterCurve(xUnitType, yUnitIndex),
    yUnitIndex(yUnitType), curveFile(curveFileInit), parID(parIDInit)
{
  SetToNull(curveID);
  fileIndex = -1;
  curveToUse  = 0;
}

FixedCurve& FixedCurve::operator= (const FixedCurve& a)
{
  if (&a != this)
  {
    ParameterCurve::operator=(a);
    fileIndex = a.fileIndex;
    curveObjRef = a.curveObjRef;
  }
  return *this;
}

bool  FixedCurve::SetupOK(SC_SetupErr& errData)
{
  fileIndex = -1;
  curveToUse = 0;
  SC_SetupErr crvErr;

  if (control.UseCurveFiles())
  {
    if (IsNull(curveID))
    {
      crvErr.SetConstantError("curve ID not set");
    }
    if (curveFile.SetupOK(crvErr))
    {
      DC_CurveArray*  currCrv = curveFile.GetCurveArray();
      if (currCrv == 0)
        crvErr.SetConstantError("curve file not loaded");
      else if (!currCrv->GetIDIndex(curveID, fileIndex))
        crvErr.SetConstantError("curve ID not found in curve");
      else
        curveToUse = &(*currCrv)[fileIndex];
    }
  }
  else
  {
    DataObjC* curveDO = 0;
    if (!curveObjRef.CheckAndGetData(typeid(DO_Curve), curveDO))
    {
      if (curveObjRef.IsNull())
        crvErr.SetConstantError("curve object not selected/not set");

      if (curveDO == 0)
        crvErr.SetConstantError("curve data not in object ??");
      else
        crvErr.SetInternalError("Par Curve:", curveDO->objErrMsg);
    }
    else
    {
      curveToUse = static_cast<DO_Curve*>(curveDO)->curve;
    }
  }

  return errData.SetupOK(parID, crvErr);
}


double FixedCurve::GetMetricFileCurveY(const double& metricXVal)
{
  // sanity checks
  if (curveToUse == 0)
    GenAppInternalError("FileCurve::GetMetricPointCurveY #1");
  return GetMetricCurveY(metricXVal, *curveToUse);
}



ParSampVar::ParSampVar(bool& isActiveInit,
                       const char* varShortIDInit, const char* varLongIDInit,
                       UnitIndex& unitIndexInit,
                       const double& minL, const double& maxL, bool defIsLinear) :
  SampVarUnits(isActiveInit, 0, varShortIDInit, varLongIDInit,  unitIndexInit, defIsLinear),
  minLim(minL), maxLim(maxL)
{
}

bool ParSampVar::CheckLimits(int sampleNum) const
{
  double dVal = GetCurrentValue(sampleNum);
  return (dVal >= minLim) && (dVal <= maxLim);
}

Parameter::Parameter(const char*       initFullName,
                     const char*       initShortName,
                     UnitType    initUnitType,
                     const double&      initMinLimit,
                     const double&      initMaxLimit,
                     bool        initOptIsOK,
                     bool        initFpIsOK,
                     bool        initFrIsOK,
                     bool        initFtIsOK,
                     bool    initDefIsLinear) :

  fullName(initFullName), shortName(initShortName),
  useUnitFormat(true),
  parVal(initUnitType),
  minLimit(initMinLimit), maxLimit(initMaxLimit),
  optIsOK(initOptIsOK),
  fpIsOK(initFpIsOK),
  frIsOK(initFrIsOK),
  ftIsOK(initFtIsOK),
  defaultIsLinear(initDefIsLinear),

  pFixedCurve(fPParameterCurveFile, uPress, initUnitType, initShortName),
  rFixedCurve(frParameterCurveFile, uDist, initUnitType, initShortName),
  tFixedCurve(ftParameterCurveFile, uTime, initUnitType, initShortName),
  varyParVal(isVary, initShortName, initFullName, parVal, initDefIsLinear),
  optParVal(isOpt, initShortName, initFullName, parVal, initDefIsLinear),
  sampParVal(isSampled, initShortName, initFullName, parVal, minLimit, maxLimit, initDefIsLinear)

{
  InitCommon();
}

Parameter::Parameter(const char*       initFullName,
                     const char*       initShortName,
                     UnitType    initUnitType,
                     const double&      initMinLimit,
                     const double&      initMaxLimit,
                     bool        initOptIsOK,
                     bool        initFpIsOK,
                     bool        initFrIsOK,
                     bool        initFtIsOK,
                     const  double&     initVal,
                     bool    initDefIsLinear) :

  fullName(initFullName), shortName(initShortName),
  useUnitFormat(true),
  parVal(initUnitType, initVal),
  minLimit(initMinLimit), maxLimit(initMaxLimit),
  optIsOK(initOptIsOK),
  fpIsOK(initFpIsOK),
  frIsOK(initFrIsOK),
  ftIsOK(initFtIsOK),
  defaultIsLinear(initDefIsLinear),

  pFixedCurve(fPParameterCurveFile, uPress, initUnitType, initShortName),
  rFixedCurve(frParameterCurveFile, uDist, initUnitType, initShortName),
  tFixedCurve(ftParameterCurveFile, uTime, initUnitType, initShortName),
  varyParVal(isVary, initShortName, initFullName, parVal, initDefIsLinear),
  optParVal(isOpt, initShortName, initFullName, parVal, initDefIsLinear),
  sampParVal(isSampled, initShortName, initFullName, parVal, minLimit, maxLimit, initDefIsLinear)

{
  InitCommon();
}


Parameter::Parameter(const char*       initFullName,
                     const char*       initShortName,
                     UnitType    initUnitType,
                     const double&      initMinLimit,
                     const double&      initMaxLimit,
                     bool        initOptIsOK,
                     bool        initFpIsOK,
                     bool        initFrIsOK,
                     bool        initFtIsOK,
                     const SC_RealFormat& initFormat,
                     bool    initDefIsLinear) :

  fullName(initFullName), shortName(initShortName),
  useUnitFormat(false),
  parVal(initUnitType),
  minLimit(initMinLimit), maxLimit(initMaxLimit),
  optIsOK(initOptIsOK),
  fpIsOK(initFpIsOK),
  frIsOK(initFrIsOK),
  ftIsOK(initFtIsOK),
  format(initFormat),
  defaultIsLinear(initDefIsLinear),

  pFixedCurve(fPParameterCurveFile, uPress, initUnitType, initShortName),
  rFixedCurve(frParameterCurveFile, uDist, initUnitType, initShortName),
  tFixedCurve(ftParameterCurveFile, uTime, initUnitType, initShortName),
  varyParVal(isVary, initShortName, initFullName, parVal, initDefIsLinear),
  optParVal(isOpt, initShortName, initFullName, parVal, initDefIsLinear),
  sampParVal(isSampled, initShortName, initFullName, parVal, minLimit, maxLimit, initDefIsLinear)

{
  InitCommon();
}

Parameter::Parameter(const char*       initFullName,
                     const char*       initShortName,
                     UnitType    initUnitType,
                     const double&      initMinLimit,
                     const double&      initMaxLimit,
                     bool        initOptIsOK,
                     bool        initFpIsOK,
                     bool        initFrIsOK,
                     bool        initFtIsOK,
                     const SC_RealFormat& initFormat,
                     const double&      initVal,
                     bool    initDefIsLinear) :

  fullName(initFullName), shortName(initShortName),
  useUnitFormat(false),
  parVal(initUnitType, initVal),
  minLimit(initMinLimit), maxLimit(initMaxLimit),
  optIsOK(initOptIsOK),
  fpIsOK(initFpIsOK),
  frIsOK(initFrIsOK),
  ftIsOK(initFtIsOK),
  format(initFormat),
  defaultIsLinear(initDefIsLinear),

  pFixedCurve(fPParameterCurveFile, uPress, initUnitType, initShortName),
  rFixedCurve(frParameterCurveFile, uDist, initUnitType, initShortName),
  tFixedCurve(ftParameterCurveFile, uTime, initUnitType, initShortName),
  varyParVal(isVary, initShortName, initFullName, parVal, initDefIsLinear),
  optParVal(isOpt, initShortName, initFullName, parVal, initDefIsLinear),
  sampParVal(isSampled, initShortName, initFullName, parVal, minLimit, maxLimit, initDefIsLinear)

{
  InitCommon();
}

Parameter::~Parameter()
{
  delete pPointsCurve;
  delete rPointsCurve;
  delete tPointsCurve;
}


void Parameter::InitCommon()
{
  wasEdited = false;
  parameterType = ptFixed;

  isOpt       = false;
  isSampled   = false;
  isVary    = false;

  pPointsCurve = 0;
  rPointsCurve = 0;
  tPointsCurve = 0;
}


void  Parameter::SetParameterType(ParameterType  newParameterType,
                                  bool           updateFromOldType)
{
  if (newParameterType == parameterType)
    return;

  // sanity checks
  if ((newParameterType == ptOpt) && (!optIsOK))
    GenAppInternalError("Parameter::SetParameterType #1");

  if (((newParameterType == ptFrFixed) || (newParameterType == ptFrPoint)) && (!frIsOK))
    GenAppInternalError("Parameter::SetParameterType #2");

  if (((newParameterType == ptFPFixed) || (newParameterType == ptFPPoint)) && (!fpIsOK))
    GenAppInternalError("Parameter::SetParameterType #3");

  if (((newParameterType == ptFtFixed) || (newParameterType == ptFtPoint)) && (!ftIsOK))
    GenAppInternalError("Parameter::SetParameterType #4");

  // update cases
  if (updateFromOldType)
  {
    if ((newParameterType == ptFixed) && (parameterType == ptOpt))
      parVal.SetMetricVal(optParVal.GetParEstimate());
    if ((newParameterType == ptOpt) && (parameterType == ptFixed))
      optParVal.SetParEstimate(parVal.GetMetricVal());

    // maybe more ...
  }

  //  allocate if required
  if ((newParameterType == ptFrPoint) && (!rPointsCurve))
    rPointsCurve = new PointsCurve("R", shortName, uDist, parVal, defaultIsLinear);

  if ((newParameterType == ptFPPoint) && (!pPointsCurve))
    pPointsCurve = new PointsCurve("P", shortName, uPress, parVal, defaultIsLinear);

  if ((newParameterType == ptFtPoint) && (!tPointsCurve))
    tPointsCurve = new PointsCurve("T", shortName, uTime, parVal, defaultIsLinear);

  // change opt status on points curve
  if (parameterType == ptFPPoint)
    pPointsCurve->SetActive(false);

  if (parameterType == ptFrPoint)
    rPointsCurve->SetActive(false);

  if (parameterType == ptFtPoint)
    tPointsCurve->SetActive(false);

  // change & update
  parameterType = newParameterType;

  //  restore old opt settings
  if (parameterType == ptFPPoint)
    pPointsCurve->SetActive(true);
  if (parameterType == ptFrPoint)
    rPointsCurve->SetActive(true);
  if (parameterType == ptFtPoint)
    tPointsCurve->SetActive(true);

  isVary      = (parameterType == ptVary);
  isOpt       = (parameterType == ptOpt);
  isSampled   = (parameterType == ptSample);
}


void Parameter::ClearWasActive()
{
  optParVal.wasActive = false;
  sampParVal.wasActive = false;
  varyParVal.wasActive = false;
  if (pPointsCurve)
    pPointsCurve->ClearWasActive();
  if (rPointsCurve)
    rPointsCurve->ClearWasActive();
  if (tPointsCurve)
    tPointsCurve->ClearWasActive();
}


void Parameter::SetActive(bool isActive)
{
  parIsActive = isActive;
  isOpt       = isActive && (parameterType == ptOpt);
  isSampled   = isActive && (parameterType == ptSample);
  isVary    = isActive && (parameterType == ptVary);

  if (pPointsCurve)
    pPointsCurve->SetActive((parameterType == ptFPPoint) && isActive);
  if (rPointsCurve)
    rPointsCurve->SetActive((parameterType == ptFrPoint) && isActive);
  if (tPointsCurve)
    tPointsCurve->SetActive((parameterType == ptFtPoint) && isActive);
}


const SC_RealFormat* Parameter::GetOverrideFormat() const
{
  if (useUnitFormat)
    return NULL;
  return &format;
}


void Parameter::ParValToString(const double&  metricVal,
                               char     parString[],
                               int      parStrLen)
{
  if (useUnitFormat)
    parVal.UnitIndex::GetUserString(metricVal, parString, parStrLen);
  else
    parVal.UnitIndex::GetUserString(metricVal, parString, parStrLen, format);
}

void  Parameter::ParValToString(const double&  metricVal, // in dialogs were units can be chaged
                                int   unitIndex,
                                char     parString[],
                                int      parStrLen)
{
  // kluge
  int currIndex = parVal.GetActualConvIndex();
  parVal.SetConvIndex(unitIndex);
  ParValToString(metricVal, parString, parStrLen);
  parVal.SetConvIndex(currIndex);

}

#ifdef _WIN32

void Parameter::ParValToString(const double&  metricVal,
                               CString& parString)
{
  if (useUnitFormat)
  {
    parVal.UnitIndex::GetUserString(metricVal, parString);
  }
  else if (RealIsNull(metricVal))
  {
    parString = nullStr;
  }
  else
  {
    format.RealToString(parVal.MetricToUser(metricVal), parString);
  }
}

void Parameter::ParValToString(const double&  metricVal,
                               int    unitIndex,
                               CString& parString)
{
  int currIndex = parVal.GetActualConvIndex();
  parVal.SetConvIndex(unitIndex);
  ParValToString(metricVal, parString);
  parVal.SetConvIndex(currIndex);
}

#endif


bool Parameter::ParValOK(const double&      pVal,
                         const char         rangeID[],
                         SC_SetupErr& errData)
{
  SC_SetupErr rangeErr;
  char rangeErrMsg[80];

  if (RealIsNull(pVal))
    rangeErr.SetConstantError("value not set");
  else if ((pVal  < minLimit) || (pVal > maxLimit))
  {
    char minStr[80];
    ParValToString(minLimit, minStr, 80);
    char maxStr[80];
    ParValToString(maxLimit, maxStr, 80);

    MakeString(rangeErrMsg, "must be > ", minStr, " and < ", maxStr, 80);

    rangeErr.SetConstantError(rangeErrMsg);
  }
  return errData.SetupOK(rangeID, rangeErr);
}


bool Parameter::SetupOK(SC_SetupErr& errData)
{
  SC_SetupErr parErr;

  switch (parameterType)
  {
  case ptFixed : {
    ParValOK(parVal.GetMetricVal(), "fixed", parErr);
    break;
  }
  case ptVary : {
    if (varyParVal.VarySetupOK(parErr))
    {
      double minP, maxP;
      varyParVal.GetMinMax(minP, maxP);
      bool dummy = ParValOK(minP, "range/suite min", parErr) &&
                   ParValOK(maxP, "range/suite max", parErr);
    }
    break;
  }

  case ptOpt : {
    if (optParVal.OptimizeSetupOK(parErr))
    {
      bool dummy = ParValOK(optParVal.optMinLim, "opt min", parErr) &&
                   ParValOK(optParVal.optMaxLim, "opt max", parErr);
    }
    break;
  }
  case ptSample : {
    if (sampParVal.SampleSetupOK(parErr))
    {
      if (sampParVal.UsesMean())
        ParValOK(sampParVal.distMean, "sample mean", parErr);
      if (sampParVal.UsesMinMax())
        bool dummy = ParValOK(sampParVal.distMin, "sample min", parErr) &&
                     ParValOK(sampParVal.distMax, "sample max", parErr);
    }
    break;
  }
  case ptFrFixed : {
    rFixedCurve.SetupOK(parErr);
    break;
  }
  case ptFPFixed : {
    pFixedCurve.SetupOK(parErr);
    break;
  }
  case ptFtFixed : {
    tFixedCurve.SetupOK(parErr);
    break;
  }
  case ptFrPoint : {
    if (rPointsCurve->SetupOK(parErr))
    {
      for (int i = 0; i < rPointsCurve->pVals.Size(); i++)
      {
        double pVal = rPointsCurve->pVals.GetRef(i).yVal.pVal;

        // lazy, not checking opt limits
        if (!ParValOK(pVal, "point Y", parErr))
          break;
      }

      //  check that r is not -ve
      if ((!rPointsCurve->xIsLog10) && (rPointsCurve->pVals.GetRef(0).xVal.pVal < 0.0))
        parErr.SetConstantError("f(r) Points X must be > 0.0");
    }
    break;
  }

  case ptFPPoint : {
    if (pPointsCurve->SetupOK(parErr))
    {
      for (int i = 0; i < pPointsCurve->pVals.Size(); i++)
        // lazy, not checking opt limits
        if (!ParValOK(pPointsCurve->pVals.GetRef(i).yVal.pVal, "point Y", parErr))
          break;

      //  check that P is not -ve
      if (pPointsCurve->pVals.GetRef(0).xVal.pVal < 0.0)
        parErr.SetConstantError("f(P) points X must be > 0.0");
    }
    break;
  }
  case ptFtPoint : {
    if (tPointsCurve->SetupOK(parErr))
    {
      for (int i = 0; i < tPointsCurve->pVals.Size(); i++)
        // lazy, not checking opt limits
        if (!ParValOK(tPointsCurve->pVals.GetRef(i).yVal.pVal, "point Y", parErr))
          break;

      //  check that t is not -ve
      if (tPointsCurve->pVals.GetRef(0).xVal.pVal < 0.0)
        parErr.SetConstantError("f(t) points X must be > 0.0");
    }
    break;
  }

  default : {
    break;
  }
  }

  return errData.SetupOK(fullName, parErr);
}


int Parameter::GetIntMaxPar() const
{
  if (!parIsActive)
    return 0;

  double tempVal;
  if (parameterType == ptFixed)
  {
    tempVal = parVal.GetMetricVal();
  }
  else if (parameterType == ptVary)
  {
    double minP;
    varyParVal.GetMinMax(minP, tempVal);
  }
  else
    GenAppInternalError("Parameter::GetIntMaxPar()-type");

  if ((tempVal < 0.0) || (tempVal > 1.0E+06))
    GenAppInternalError("Parameter::GetIntMaxPar()-range");

  return int(tempVal);
}

double Parameter::GetParValue() const
{
  // sanity checks
  if (IsCurve())
    GenAppInternalError("Parameter::GetParValue() #1");
  if (IsRadial())
    GenAppInternalError("Parameter::GetParValue() #2");
  if (IsTime())
    GenAppInternalError("Parameter::GetParValue() #3");


  switch (parameterType)
  {
  case ptFixed : {
    return parVal.GetMetricVal();
  }
  case ptVary : {
    if (varyParVal.varyIndex < 0)
      return nullReal;
    return varyParVal.GetVaryVal();
  }
  case ptOpt : {
    return optParVal.GetParEstimate();
  }
  case ptSample : {
    int currIndex = sampParVal.GetTrialIndex();
    if (currIndex < 0)
      return nullReal;
    return sampParVal.GetCurrentValue(currIndex);
  }
  }

  GenAppInternalError("Parameter::GetParValue() #3");

  // to satisfy the compiler
  return nullReal;
}


double  Parameter::GetVaryParValue() const
{
  // sanity checks
  if (!IsVary())
    GenAppInternalError("Parameter::GetVaryParValue() #1");

  return varyParVal.GetVaryVal();
}

bool Parameter::GetRadialParValue(const SC_DoubleArray& rVal,
                                  SC_DoubleArray& parVals,
                                  SC_SetupErr& errData)
{
  // sanity
  if (!IsRadial())
    GenAppInternalError("Parameter::GetRadialParValue");

  parVals.AllocAndSetSize(rVal.Size());
  if (IsPoints())
  {
    if (rPointsCurve == 0)
      GenAppInternalError("Parameter::GetRadialParValue_1");
    if (!rPointsCurve->CurveOK(errData))
      return false;

    for (int i = 0; i < rVal.Size(); i++)
      parVals[i] = rPointsCurve->GetMetricPointCurveY(rVal[i]);
  }
  else
  {
    for (int i = 0; i < rVal.Size(); i++)
      parVals[i] = rFixedCurve.GetMetricFileCurveY(rVal[i]);
  }
  return true;
}


bool Parameter::GetPressureCurve(WorkingParameterCurve& fpCurve,
                                 SC_SetupErr& errData)
{
  // sanity
  if (!IsNonLinear())
    GenAppInternalError("Parameter::GetPressureCurve");

  if (IsPoints())
  {
    if (pPointsCurve == 0)
      GenAppInternalError("Parameter::GetPressureCurve_1");
    if (!pPointsCurve->CurveOK(errData))
      return false;

    // all ParameterCurve data
    fpCurve = (*pPointsCurve);
    //and curve itself
    fpCurve.parCurve = pPointsCurve->pointsCurve;
  }
  else
  {
    // all ParameterCurve data
    fpCurve = pFixedCurve;
    //and curve itself
    fpCurve.parCurve = *pFixedCurve.curveToUse;
  }

  return true;
}

bool Parameter::GetTimeCurve(WorkingParameterCurve& ftCurve,
                             SC_SetupErr& errData)
{
  // sanity
  if (!IsTime())
    GenAppInternalError("Parameter::GetTimeCurve");

  if (IsPoints())
  {
    if (tPointsCurve == 0)
      GenAppInternalError("Parameter::GetTimeCurve_1");
    if (!tPointsCurve->CurveOK(errData))
      return false;

    // all ParameterCurve data
    ftCurve = (*tPointsCurve);
    //and curve itself
    ftCurve.parCurve = tPointsCurve->pointsCurve;
  }
  else
  {
    // all ParameterCurve data
    ftCurve = tFixedCurve;
    //and curve itself
    ftCurve.parCurve = *tFixedCurve.curveToUse;
  }

  return true;
}

Parameter::CurveParameterType Parameter::GetCurveType()
{
  if (IsRadial())
    return cptRadial;

  if (IsTime())
    return cptTime;

  if (IsNonLinear())
    return cptPressure;

  GenAppInternalError("Parameter::GetCurveType");
  return cptRadial;
}

PointsCurve* Parameter::GetPointsCurve()
{
  if (IsFPPoints())
    return pPointsCurve;

  if (IsFrPoints())
    return rPointsCurve;

  if (IsFtPoints())
    return tPointsCurve;

  GenAppInternalError("Parameter::GetPointsCurve");
  return 0;
}


WorkingParameterCurve::WorkingParameterCurve()
{
}

WorkingParameterCurve&  WorkingParameterCurve::operator= (const ParameterCurve& a)
{
  xIsLog10    = a.xIsLog10;
  yIsLog10    = a.yIsLog10;
  xUnitIndex    = a.xUnitIndex;
  yUnitIndex    = a.yUnitIndex;
  return *this;
}

double WorkingParameterCurve::GetMetricCurveY(const double&   metricXVal)
{
  // sanity checks
  if (!parCurve.IsCreated())
    GenAppInternalError("WorkingParameterCurve::GetMetricCurveY_1");
  if (RealIsNull(metricXVal))
    GenAppInternalError("WorkingParameterCurve::GetMetricCurveY_2");

  //  convert metric x to user x
  double userX = xUnitIndex.MetricToUser(metricXVal);

  //  take log if required
  if (xIsLog10)
  {
    if (userX < stdEps)
      userX = -6.0; // kluge for time
    else
      userX = log10(userX);
  }

  // get curve value
  double userY = parCurve.GetCurveY(userX);

  // if log then transform
  if (yIsLog10)
  {
    if ((userY < -99.0) || (userY > 99.0))
      GenAppInternalError("WorkingParameterCurve::GetMetricPointCurveY #4");
    userY = pow(10.0, userY);
  }

  //  convert units && return
  return yUnitIndex.UserToMetric(userY);
}



