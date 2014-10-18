///////////////////////////////////////////////////////////////////////////////////
//
// C_Layer.cpp
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

#include <Var/G_Control.h>
#include <Var/G_Layer.h>

#include <nsClass/nSightConst.h>

#include <Var/C_Layer.h>

IntervalBase::IntervalBase() :
  topElevation(uDist, 0.0),
  topDepth(uDist, 0.0),
  nintervalNodes(10)
{
  SetToNull(intervalID);
}

IntervalBase::IntervalBase(const IntervalBase& a)
{
  LocalCopy(a);
}

IntervalBase&   IntervalBase::operator=(const IntervalBase& a)
{
  if (&a != this)
    LocalCopy(a);
  return *this;
}

void IntervalBase::LocalCopy(const IntervalBase& a)
{
  CopyString(intervalID, a.intervalID, intervalIDLen);

  topElevation  = a.topElevation;
  topDepth    = a.topDepth;
  nintervalNodes  = a.nintervalNodes;
}


void IntervalBase::SetID(const char* inID)
{
  CopyString(intervalID, inID, intervalIDLen);
}


void IntervalBase::GetUserDepthString(char* uStr, int strLen)
{
  if (enterLayerDepths)
  {
    topDepth.SetConvIndex(bottomLayerDepth.GetConvIndex());
    topDepth.GetUserString(uStr, strLen);
  }
  else
  {
    topElevation.SetConvIndex(bottomLayerElevation.GetConvIndex());
    topElevation.GetUserString(uStr, strLen);
  }
}

void IntervalBase::GetUserThickString(char* uStr, int strLen)
{
  UnitReal temp(uDist, currThickness);
  if (enterLayerDepths)
  {
    temp.SetConvIndex(bottomLayerDepth.GetConvIndex());
  }
  else
  {
    temp.SetConvIndex(bottomLayerElevation.GetConvIndex());
  }
  temp.GetUserString(uStr, strLen);
}


GeologyLayer::GeologyLayer()
{
  layerHasSkin = false;
  layerIsIsotropic = true;
  parIndex = -1;
}

GeologyLayer::GeologyLayer(const GeologyLayer& a)
  : IntervalBase(a)
{
  LocalCopy(a);
}

GeologyLayer&   GeologyLayer::operator=(const GeologyLayer& a)
{
  if (&a != this)
  {
    IntervalBase::operator=(a);
    LocalCopy(a);
  }
  return *this;
}

void GeologyLayer::LocalCopy(const GeologyLayer& a)
{
  layerHasSkin    = a.layerHasSkin;
  layerIsIsotropic  = a.layerIsIsotropic;
  parIndex      = a.parIndex;
}

bool GeologyLayer::CheckError(SC_SetupErr& glErr)
{
  return glErr.NoErrors();
}

bool GeologyLayers::GetIDs(SC_StringArray& ids) const
{
  ids.SetStringLen(IntervalBase::intervalIDLen);
  ids.Alloc(Size());
  for (int i = 0; i < Size(); i++)
    ids += (*this)[i].intervalID;
  return !ids.IsEmpty();
}

bool GeologyLayers::HasSkin() const
{
  for (int i = 0; i < Size(); i++)
    if ((*this)[i].layerHasSkin)
      return true;

  return false;
}

WellBoreZone::WellBoreZone()
  : deltaVolume(uVol, 0.0), tzComp(uComp, 1E-9)
{
  zoneIsPacker = false;
  seqIndex = -1;
}

WellBoreZone::WellBoreZone(const WellBoreZone& a)
  : IntervalBase(a)
{
  LocalCopy(a);
}

WellBoreZone&   WellBoreZone::operator=(const WellBoreZone& a)
{
  if (&a != this)
  {
    IntervalBase::operator=(a);
    LocalCopy(a);
  }
  return *this;
}

void WellBoreZone::LocalCopy(const WellBoreZone& a)
{
  zoneIsPacker  = a.zoneIsPacker;
  seqIndex    = a.seqIndex;
  deltaVolume   = a.deltaVolume;
  tzComp      = a.tzComp;
}

void WellBoreZone::GetUserDeltaVolumeString(char* uStr, int strLen)
{
  deltaVolume.SetConvIndex(wellboreZoneDeltaVolumeUnits.GetConvIndex());
  deltaVolume.GetUserString(uStr, strLen);
}

void WellBoreZone::GetUserTZCompString(char* uStr, int strLen)
{
  tzComp.SetConvIndex(wellboreZoneTZCompUnits.GetConvIndex());
  tzComp.GetUserString(uStr, strLen);
}


void WellBoreZone::CheckError(SC_SetupErr& wbzErr)
{
}

bool WellBoreZones::GetIDs(SC_StringArray& ids)
{
  ids.SetStringLen(IntervalBase::intervalIDLen);
  ids.Alloc(Size());
  for (int i = 0; i < Size(); i++)
    ids += (*this)[i].intervalID;
  return !ids.IsEmpty();
}


bool WellBoreZones::GetIndex(const char* inID, int& outIndex) const
{
  for (int i = 0; i < Size(); i++)
    if (SameString((*this)[i].intervalID, inID))
    {
      outIndex = i;
      return true;
    }

  outIndex = -1;
  return false;
}

