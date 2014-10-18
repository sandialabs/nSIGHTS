///////////////////////////////////////////////////////////////////////////////////
//
// C_TestZoneCurve.cpp
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
#include <objClass/DO_Curve.h>

#include <Var/G_Control.h>
#include <Var/G_Layer.h>
#include <Var/G_CurveFiles.h>
#include <Var/G_Sequence.h>

#include <Var/C_TestZoneCurve.h>


TestZoneCurve::TestZoneCurve() :
  tzType(tzPressure),
  timeIsSequenceBased(false),
  curveYUnits(uPress),
  curveYisLog10(false),
  curveTimeUnits(uTime),
  curveTimeisLog10(false),
  wellboreZone(-1),
  curveIndex(-1),
  startSeq(-1),
  endSeq(-1)
{
  SetToNull(curveID);
  SetToNull(wellboreZoneID);
  SetToNull(startSeqID);
  SetToNull(endSeqID);
}


void TestZoneCurve::ChangeCurveType(TZCurveType newType)
{
  // check for unit change
  bool oldIsVol = (tzType == tzDeltaVolume) || (tzType == tzAbsVolume);
  bool newIsVol = (newType == tzDeltaVolume) || (newType == tzAbsVolume);
  if ((newType == tzType) || (oldIsVol && newIsVol))
  {
    tzType = newType;
    return;
  }

  tzType = newType;
  switch (tzType) {
  case tzPressure : {
    curveYUnits = UnitIndex(uPress);
    break;
  }
  case tzFlowRate : {
    if (gasFlowIsMass && control.IsGas())
      curveYUnits = UnitIndex(uMflow);
    else
      curveYUnits = UnitIndex(uFlow);
    break;
  }
  case tzTemperature : {
    curveYUnits = UnitIndex(uTemp);
    break;
  }
  case tzCompressibility : {
    curveYUnits = UnitIndex(uComp);
    break;
  }
  case tzDeltaVolume :
  case tzAbsVolume :  {
    curveYUnits = UnitIndex(uVol);
    break;
  }
  }
}


void TestZoneCurve::SetForFlags()
{
  if (control.IsGas())
  {
    // gas is isothermal and perfectly compressible and TZ volume changes are not incorporated
    if ((tzType == tzTemperature) || (tzType == tzCompressibility) ||
        (tzType == tzDeltaVolume) || (tzType == tzAbsVolume))
      tzType = tzFlagReset;

    if (tzType == tzFlowRate)
    {
      if (gasFlowIsMass && (curveYUnits.unitType == uFlow))
        curveYUnits = UnitIndex(uMflow);
      if ((!gasFlowIsMass) && (curveYUnits.unitType == uMflow))
        curveYUnits = UnitIndex(uFlow);
    }
  }
  else
  {
    if ((tzType == tzFlowRate) && (curveYUnits.unitType == uMflow))
      curveYUnits = UnitIndex(uFlow);

    if ((!testZoneCompressibilityVaries) && (tzType == tzCompressibility))
      tzType = tzFlagReset;

    if ((!testZoneTemperatureVaries) && (tzType == tzTemperature))
      tzType = tzFlagReset;

    if ((!testZoneVolumeVaries) && ((tzType == tzDeltaVolume) || (tzType == tzAbsVolume)))
      tzType = tzFlagReset;
  }

}



bool TestZoneCurve::SetupOK(SC_SetupErr& errData)
{
  curveIndex  = -1;

  wellboreZone = -1;
  startSeq  = -1;
  endSeq    = -1;
  curveToUse  = 0;

  SC_SetupErr crvErr;

  if (control.UseCurveFiles())
  {
    if (IsNull(curveID))
    {
      crvErr.SetConstantError("curve ID not set");
      return errData.SetupOK("CRV???", crvErr);
    }

    if (nsCurveFileStatics::testZoneBCCurveFile.SetupOK(crvErr))
    {
      DC_CurveArray*  currCrv = nsCurveFileStatics::testZoneBCCurveFile.GetCurveArray();
      if (currCrv == 0)
      {
        crvErr.SetConstantError("curve file not loaded");
      }
      else if (!currCrv->GetIDIndex(curveID, curveIndex))
      {
        crvErr.SetConstantError("curve ID not found in curve");
      }
      else
      {
        curveToUse = &(*currCrv)[curveIndex];
      }
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
        crvErr.SetInternalError("TZ Curve:", curveDO->objErrMsg);
    }
    else
    {
      curveToUse = static_cast<DO_Curve*>(curveDO)->curve;
    }
  }


  SequenceArray* tzSequences = 0;
  if (control.IsLayered())
  {
    if (SameString(wellboreZoneID, "PRE"))
    {
      tzSequences = &layerHistorySequenceData;
      wellboreZone = -1;

    }
    else if (!wellBoreZones.GetIndex(wellboreZoneID, wellboreZone))
    {
      crvErr.SetConstantError("well bore zone not set/not valid");
    }
    else
      tzSequences = &layerSequenceData[wellBoreZones[wellboreZone].seqIndex];
  }
  else
    tzSequences = &singleSequenceData;

  if (tzSequences != 0)
  {
    if (!tzSequences->GetSequenceIndex(startSeqID, startSeq))
    {
      crvErr.SetConstantError("start sequence not set/not valid");
    }
    else if (!tzSequences->GetSequenceIndex(endSeqID, endSeq))
    {
      crvErr.SetConstantError("end sequence not set/not valid");
    }
    else if (endSeq < startSeq)
    {
      crvErr.SetConstantError("end sequence before start");
    }
  }
  return errData.SetupOK(curveID, crvErr);
}

