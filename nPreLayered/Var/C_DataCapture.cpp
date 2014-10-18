///////////////////////////////////////////////////////////////////////////////////
//
// C_DataCapture.cpp
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

#include <objClass/DO_XYDataArray.h>

#include <nsClass/nSightConst.h>

#include <Var/G_Control.h>
#include <Var/G_Layer.h>

#include <Var/C_DataCapture.h>

static UnitIndex dummy;

SuperComponent::SuperComponent() : superRadUnits(dummy),
  optRadVal(isOpt, rID, dummy),
  sampRadVal(isSampled, 2, rID, dummy),
  varyRadVal(isVary, rID, dummy)
{
  GenAppInternalError("C_DataCapture::SuperComponent()");
}

SuperComponent::~SuperComponent()
{

}

SuperComponent::SuperComponent(UnitIndex& inUnits) :
  optRadVal(isOpt, rID, inUnits),
  sampRadVal(isSampled, 3, rID, inUnits),
  varyRadVal(isVary, rID, inUnits),
  superRadUnits(inUnits),
  superOp(soAddP),
  superType(stConstant),
  constantRad(1.0),
  isOpt(false),
  isSampled(false)
{
  rID[0] = '\0';
}


void SuperComponent::ChangeSuperType(SuperType newType)
{
  // fix RR bug from 21 Nov 05
  if ((superType == stOptimized) && (newType == stConstant) && OptimizationControlStatics::updateParameterEstimates)
    constantRad = optRadVal.optParEstimate;

  if ((!control.IsOptimization()) && (newType == stOptimized))
    newType = stConstant;
  if ((!control.IsSampleSim()) && (newType == stSampled))
    newType = stConstant;
  if ((control.IsSampleSim()) && (newType == stVary))
    newType = stConstant;

  superType = newType;
  isOpt = (newType == stOptimized);
  isSampled = (newType == stSampled);
  isVary = (newType == stVary);
}


void SuperComponent::ClearWasActive()
{
  optRadVal.wasActive = false;
  sampRadVal.wasActive = false;
  varyRadVal.wasActive = false;
}

bool SuperComponent::SetupOK(SC_SetupErr& errData)
{
  if ((superType == stOptimized) && control.IsOptimization())
    return optRadVal.OptimizeSetupOK(errData);

  if ((superType == stSampled) && control.IsSampleSim())
    return sampRadVal.SampleSetupOK(errData);

  if ((superType == stVary) && (!control.IsSampleSim()))
    return varyRadVal.VarySetupOK(errData);

  return true;
}

void SuperComponent::SetID(const char* inDesig,
                           int   index)
{
  char tempStr[20];
  IntToString(index + 1, tempStr, 20);
  MakeString(rID, inDesig, "[", tempStr, "]", componentIDLen);
}


SuperComponentArray::SuperComponentArray(UnitIndex& inUnits) :
  superRadUnits(inUnits),
  T_SC_AllocPtrArray<SuperComponent>()
{
}

void  SuperComponentArray::MakeNew(int index)
{
  tListData[index] = new SuperComponent(superRadUnits);
}

void SuperComponentArray::ClearWasActive()
{
  for (int i = 0; i < Size(); i++)
    GetRef(i).ClearWasActive();
}

void SuperComponentArray::SetConstant()
{
  for (int i = 0; i < Size(); i++)
    GetRef(i).ChangeSuperType(SuperComponent::stConstant);
}



DataCaptureSpec::DataCaptureSpec() :
  captureType(ctPressure),
  pressureCapType(pctTestZone),
  flowCapType(fctWell),
  productionCapType(prctWell),
  testZoneCapType(tzctTemperature),
  radiusData(uDist, 1.0),
  zvalueIsRatio(true),
  actualZ(uDist, 0.0),
  normalizedZ(1.0),
  superData(radiusData),
  outputUnits(uPress),
  makeDefaultDataDesig(true),
  obsXLocation(uDist, 100.0),
  obsYLocation(uDist, 100.0)
{
  SetToNull(dataDesig);
  CopyString(wellboreZoneID, "XXX", IntervalBase::intervalIDLen);
}


DataCaptureSpec::DataCaptureSpec(const DataCaptureSpec& a) :
  radiusData(uDist, 1.0),
  superData(radiusData),
  makeDefaultDataDesig(true)
{
  captureType     = a.captureType;
  pressureCapType   = a.pressureCapType;
  flowCapType     = a.flowCapType;
  productionCapType = a.productionCapType;
  testZoneCapType   = a.testZoneCapType;
  radiusData      = a.radiusData;
  outputUnits     = a.outputUnits;

  zvalueIsRatio   = a.zvalueIsRatio;
  actualZ       = a.actualZ;
  normalizedZ     = a.normalizedZ;
  obsXLocation    = a.obsXLocation;
  obsYLocation    = a.obsYLocation;

  CopyString(wellboreZoneID, a.wellboreZoneID, IntervalBase::intervalIDLen);
}



DataCaptureSpec::~DataCaptureSpec() {}


void DataCaptureSpec::ChangeCaptureType(CaptureType newType)
{
  if (newType == captureType)
    return;

  if ((captureType == ctPressure) &&  (pressureCapType == pctSuperposition))
    superData.SetConstant();

  captureType = newType;
  SetOutputUnits();
}

void DataCaptureSpec::ClearWasActive()
{
  superData.ClearWasActive();
}


void DataCaptureSpec::SetOutputUnits()
{
  UnitType exUnits;
  switch (captureType)
  {
  case ctPressure  : {
    exUnits = uPress;
    break;
  }
  case ctFlow  : {
    if (control.IsGas()&& gasFlowIsMass)
      exUnits = uMflow;
    else
      exUnits = uFlow;
    break;
  }
  case ctProduction  : {
    if (control.IsGas()&& gasFlowIsMass)
      exUnits = uMass;
    else
      exUnits = uVol;
    break;
  }

  case ctTestZone : {
    switch (testZoneCapType)
    {
    case tzctTemperature : {
      exUnits = uTemp;
      break;
    }
    case tzctCompressibility : {
      exUnits = uComp;
      break;
    }
    case tzctVolume : {
      exUnits = uVol;
      break;
    }
    }
    break;
  }
  case ctWaterTable : {
    exUnits = uDist;
    break;
  }
  }

  if (outputUnits.unitType != exUnits)
    outputUnits = UnitIndex(exUnits);
}

void DataCaptureSpec::SetForFlags()
{
  SetOutputUnits();
  if ((captureType == ctPressure) &&  (pressureCapType == pctSuperposition))
  {
    for (int i = 0; i < superData.Size(); i++)
    {
      SuperComponent& currSuper = *(superData[i]);
      if ((!control.IsOptimization()) && currSuper.isOpt)
        currSuper.ChangeSuperType(SuperComponent::stConstant);
      if ((!control.IsSampleSim()) && currSuper.isSampled)
        currSuper.ChangeSuperType(SuperComponent::stConstant);
      if ((control.IsSampleSim()) && currSuper.isVary)
        currSuper.ChangeSuperType(SuperComponent::stConstant);

      if (currSuper.superType == SuperComponent::stConstant)
      {
        if (control.IsOptimization() && currSuper.optRadVal.wasActive)
          currSuper.ChangeSuperType(SuperComponent::stOptimized);
        if (control.IsSampleSim() && currSuper.sampRadVal.wasActive)
          currSuper.ChangeSuperType(SuperComponent::stSampled);
        if ((!control.IsSampleSim()) && currSuper.varyRadVal.wasActive)
          currSuper.ChangeSuperType(SuperComponent::stVary);
      }
    }
  }
  else
  {
    // clear old opt/sample/vary
    superData.SetConstant();
  }

  if ((captureType == ctTestZone) &&  control.IsGas())
  {
    captureType  = ctPressure;
    pressureCapType = pctTestZone;
  }

  if ((captureType == ctWaterTable) &&  control.IsConfined())
  {
    captureType  = ctPressure;
    pressureCapType = pctTestZone;
  }

}

void DataCaptureSpec::SetUserDesig(const char* inStr)
{
  makeDefaultDataDesig = false;
  CopyString(dataDesig, inStr, dataDesigLen);
}

void DataCaptureSpec::SetIDs()
{
  for (int i = 0; i < superData.Size(); i++)
    if (superData.IsValid(i))
      superData[i]->SetID(dataDesig, i);
}


bool DataCaptureSpec::SetupOK(SC_SetupErr& errData)
{
  SetIDs();
  SC_SetupErr dcapErr;

  if ((captureType == ctPressure) &&  (pressureCapType == pctSuperposition))
  {
    if (control.Is2DRadial())
      dcapErr.SetConstantError("Cannot do 2D superposition");

    superData.Cleanup();
    if (superData.IsEmpty())
      dcapErr.SetConstantError("No superposition data found");

    for (int i = 0; i < superData.Size(); i++)
      if (!(superData[i]->SetupOK(dcapErr)))
        break;

    return errData.SetupOK(dataDesig, dcapErr);
  }

  if ((captureType == ctPressure) &&  (pressureCapType == pctRadius))
  {
    //TODO - check for radius within limits

    if (control.IsHorizontalAnisotropic())
    {
      GetObsAngle(); // calculates radius
      if (radiusData.GetMetricVal() < 1.0)
      {
        dcapErr.SetConstantError("obs well radius < 1 m, anisotropy ???");
        return errData.SetupOK(dataDesig, dcapErr);
      }
    }
  }


  if (control.IsLayered() && IsTestZoneBased())
  {
    if (!wellBoreZones.GetIndex(wellboreZoneID, wellboreZoneIndex))
    {
      dcapErr.SetConstantError("no well bore zone corresponding to ID");
      return errData.SetupOK(dataDesig, dcapErr);
    }

    if (wellBoreZones[wellboreZoneIndex].zoneIsPacker)
    {
      dcapErr.SetConstantError("well bore zone corresponding to ID is packer");
      return errData.SetupOK(dataDesig, dcapErr);
    }
  }

  return true;
}

double  DataCaptureSpec::GetObsAngle() const
{
  if ((captureType == ctPressure) &&  (pressureCapType == pctRadius))
  {
    if (control.IsHorizontalAnisotropic())
    {
      double dx = obsXLocation.GetMetricVal() - wellXLocation.GetMetricVal();
      double dy = obsYLocation.GetMetricVal() - wellYLocation.GetMetricVal();
      // restrict to +/- 90
      if (dx < 0.0)
        dy = -dy;

      double radius = sqrt(Sqr(dx) + Sqr(dy));
      radiusData.SetMetricVal(radius);
      if (radius < 0.01)
        return 0.0;

      double wellAngle = asin(dy/radius); // radians

      return wellAngle;
    }
  }
  return 0.0;
}

