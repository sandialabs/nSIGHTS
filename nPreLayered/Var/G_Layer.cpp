///////////////////////////////////////////////////////////////////////////////////
//
// G_Layer.cpp
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


#ifndef ADCONSOLEAPP
#include <App/nPreUtil.h>
#endif

#include <Var/G_PreConfigFile.h>

#include <Var/G_Control.h>
#include <Var/G_Layer.h>

namespace nsLayer  {

GeologyLayers geologyLayers;
WellBoreZones wellBoreZones;

bool      enterLayerDepths = true;
UnitReal      bottomLayerDepth(uDist, 0.0);
UnitReal    bottomLayerElevation(uDist, 0.0);
UnitIndex   wellboreZoneDeltaVolumeUnits(uVol);
UnitIndex   wellboreZoneTZCompUnits(uComp);



bool      useCommonHistory = true;

LayerStaticSupport    layerSpec;
LayerErrorListing   layerErrorListing;


LayerStaticSupport::LayerStaticSupport()
{
}

void LayerStaticSupport::SetDefaultGeo()
{
  geologyLayers[0].SetID("TopLayer");
  geologyLayers[0].topDepth = UnitReal(uDist, 0.0);
  geologyLayers[0].topElevation = UnitReal(uDist, 0.0);
  geologyLayers.SetSize(1);
  for (int i = 0; i < geologyLayers.AllocSize(); i++)
    geologyLayers[i].parIndex = i;
}

void LayerStaticSupport::SetDefaultWellBore()
{
  // start with a default Layer
  wellBoreZones[0].SetID("TopZone");
  wellBoreZones[0].topDepth = UnitReal(uDist, 0.0);
  wellBoreZones[0].topElevation = UnitReal(uDist, 0.0);
  wellBoreZones[0].deltaVolume = UnitReal(uVol, 0.0);
  wellBoreZones[0].tzComp = UnitReal(uComp, 1.0E-09);
  wellBoreZones.SetSize(1);
  for (int i = 0; i < wellBoreZones.AllocSize(); i++)
    wellBoreZones[i].seqIndex = i;
}

void LayerStaticSupport::SetToInitial()
{
  geologyLayers.DeAlloc();
  geologyLayers.Alloc(maxGeoLayers);
  SetDefaultGeo();

  wellBoreZones.DeAlloc();
  wellBoreZones.Alloc(maxWellboreZones);
  SetDefaultWellBore();

  enterLayerDepths = true;
  bottomLayerDepth = UnitReal(uDist, 10.0);
  bottomLayerElevation = UnitReal(uDist, -10.0);
}


void LayerStaticSupport::SetForFlags()
{
}

bool LayerStaticSupport::LayersOK()
{
  return layerErrorListing.ErrorCheckOK();
}


bool LayerStaticSupport::GetGeoLayerIDs(SC_StringArray& ids)
{
  ids.SetStringLen(IntervalBase::intervalIDLen);
  ids.Alloc(geologyLayers.Size());
  for (int i = 0; i < geologyLayers.Size(); i++)
    ids += geologyLayers[i].intervalID;
  return !ids.IsEmpty();
}

void LayerStaticSupport::DeleteGeoLayer(int index)
{
  int lastIndex = geologyLayers.AllocSize() - 1;
  int saveIndex = geologyLayers[index].parIndex;
  for (int i = index; i < lastIndex; i++)
    geologyLayers[i] = geologyLayers[i + 1];

  geologyLayers[lastIndex] = GeologyLayer();
  geologyLayers[lastIndex].parIndex = saveIndex;

  GeoLayerCleanup();
}


void LayerStaticSupport::InsertGeoLayer(int index)
{
  int lastIndex = geologyLayers.AllocSize() - 1;
  int saveIndex = geologyLayers[lastIndex].parIndex;
  for (int i = lastIndex; i > index ; i--)
    geologyLayers[i] = geologyLayers[i - 1];

  GeologyLayer insItem;
  insItem.SetID("New");
  insItem.parIndex = saveIndex;
  geologyLayers[index] = insItem;

  GeoLayerCleanup();
}

void LayerStaticSupport::GeoLayerCleanup()
{
  for (int i = 0; i < geologyLayers.AllocSize(); i++)
    if (geologyLayers[i].IsSet())
      geologyLayers.SetSize(i+1);

  for (int i = 0; i < geologyLayers.Size(); i++)
    if (!geologyLayers[i].IsSet())
      geologyLayers[i].SetID("???");

  SetThicknesses();
}

bool LayerStaticSupport::GetWellBoreZoneIDs(SC_StringArray& ids)
{
  ids.SetStringLen(IntervalBase::intervalIDLen);
  ids.Alloc(wellBoreZones.Size());
  for (int i = 0; i < wellBoreZones.Size(); i++)
    ids += wellBoreZones[i].intervalID;
  return !ids.IsEmpty();
}


void LayerStaticSupport::DeleteWellBoreZone(int index)
{
  int lastIndex = wellBoreZones.AllocSize() - 1;
  int saveIndex = wellBoreZones[index].seqIndex;
  for (int i = index; i < lastIndex; i++)
    wellBoreZones[i] = wellBoreZones[i + 1];

  wellBoreZones[lastIndex] = WellBoreZone();
  wellBoreZones[lastIndex].seqIndex = saveIndex;

  WellBoreZoneCleanup();
}


void LayerStaticSupport::InsertWellBoreZone(int index)
{
  int lastIndex = wellBoreZones.AllocSize() - 1;
  int saveIndex = wellBoreZones[lastIndex].seqIndex;
  for (int i = lastIndex; i > index ; i--)
    wellBoreZones[i] = wellBoreZones[i - 1];

  WellBoreZone insItem;
  insItem.SetID("New");
  insItem.seqIndex = saveIndex;
  wellBoreZones[index] = insItem;

  WellBoreZoneCleanup();
}

void LayerStaticSupport::WellBoreZoneCleanup()
{
  for (int i = 0; i < wellBoreZones.AllocSize(); i++)
    if (wellBoreZones[i].IsSet())
      wellBoreZones.SetSize(i+1);

  for (int i = 0; i < wellBoreZones.Size(); i++)
    if (!wellBoreZones[i].IsSet())
      wellBoreZones[i].SetID("???");

  SetThicknesses();
}


bool LayerStaticSupport::SetThicknesses()
{
  bool allOK = true;
  for (int i = 0; i < geologyLayers.Size(); i++)
  {
    IntervalBase& currLayer = geologyLayers[i];
    if (enterLayerDepths)
    {
      if (i < geologyLayers.UpperBound())
        currLayer.currThickness = geologyLayers[i + 1].topDepth.GetMetricVal();
      else
        currLayer.currThickness = bottomLayerDepth.GetMetricVal();
      currLayer.currThickness -= currLayer.topDepth.GetMetricVal();
    }
    else
    {
      currLayer.currThickness = currLayer.topElevation.GetMetricVal();
      if (i < geologyLayers.UpperBound())
        currLayer.currThickness -= geologyLayers[i + 1].topElevation.GetMetricVal();
      else
        currLayer.currThickness -= bottomLayerElevation.GetMetricVal();
    }
    if (currLayer.currThickness <= 1.0E-4)
      allOK = false;
  }

  for (int i = 0; i < wellBoreZones.Size(); i++)
  {
    IntervalBase& currLayer = wellBoreZones[i];
    if (enterLayerDepths)
    {
      if (i < wellBoreZones.UpperBound())
        currLayer.currThickness = wellBoreZones[i + 1].topDepth.GetMetricVal();
      else
        currLayer.currThickness = bottomLayerDepth.GetMetricVal();
      currLayer.currThickness -= currLayer.topDepth.GetMetricVal();
    }
    else
    {
      currLayer.currThickness = currLayer.topElevation.GetMetricVal();
      if (i < wellBoreZones.UpperBound())
        currLayer.currThickness -= wellBoreZones[i + 1].topElevation.GetMetricVal();
      else
        currLayer.currThickness -= bottomLayerElevation.GetMetricVal();
    }
    if (currLayer.currThickness <= 1.0E-4)
      allOK = false;
  }

  return allOK;
}


LayerListing::LayerListing()
  : OSListObj("nPreLayers")
{
}


void LayerListing::CreateListing()
{
  StdTitle("Multiple Layer Specification");


  AddBoolText("Layer depth specification", enterLayerDepths, "Depth BGS", "Elevation ASL");

  const char* titleStr;
  UnitReal currThick;
  if (enterLayerDepths)
  {
    titleStr = "Depth";
    AddUnitReal("Depth of system bottom", bottomLayerDepth);
    currThick = bottomLayerDepth;
  }
  else
  {
    titleStr = "Elevation";
    AddUnitReal("Elevation of system bottom", bottomLayerElevation);
    currThick = bottomLayerElevation;
  }

  char unitStr[40];
  currThick.MakeUnitString(unitStr, 40);

  LayerStaticSupport::GeoLayerCleanup();

  SubTitle("Geology Layers");

  SC_IntArray colWidth(7, 15);
  SC_BoolArray leftJust(7, false);
  colWidth[0] = 20;
  leftJust[0] = true;
  TableSetup(colWidth, leftJust, 1, 0);
  SetTableCol("ID", titleStr, "Thickness", "Skin?", "Conductivity", "# nodes");
  SetTableCol(1, unitStr);
  SetTableCol(2, unitStr);
  AddNextLine();
  AddNextLine();

  char tempStr[80];
  for (int i = 0; i < geologyLayers.Size(); i++)
  {
    GeologyLayer& currLayer = geologyLayers[i];
    SetTableCol(0, currLayer.intervalID);

    currLayer.GetUserDepthString(tempStr, 80);
    SetTableCol(1, tempStr);

    currLayer.GetUserThickString(tempStr, 80);
    SetTableCol(2, tempStr);

    SetTableBool(3, currLayer.layerHasSkin, "Skin", "No Skin");
    SetTableBool(4, currLayer.layerIsIsotropic, "Isotropic", "Anisotropic");
    SetTableInt(5, currLayer.nintervalNodes);
    AddNextLine();
  }

  SubTitle("Wellbore Zones");

  LayerStaticSupport::WellBoreZoneCleanup();

  colWidth.SetSize(7);
  leftJust.SetSize(7);
  TableSetup(colWidth, leftJust, 1, 0);
  SetTableCol("ID", titleStr, "Thickness", "Type", "Delta Volume", "TZ Comp", "# nodes");
  SetTableCol(1, unitStr);
  SetTableCol(2, unitStr);

  char deltaVolUnitStr[40];
  wellboreZoneDeltaVolumeUnits.MakeUnitString(deltaVolUnitStr, 40);
  SetTableCol(4, unitStr);

  char tzCompUnitStr[40];
  wellboreZoneTZCompUnits.MakeUnitString(tzCompUnitStr, 40);
  SetTableCol(5, unitStr);

  AddNextLine();
  AddNextLine();


  for (int i = 0; i < wellBoreZones.Size(); i++)
  {
    WellBoreZone& currZone = wellBoreZones[i];
    SetTableCol(0, currZone.intervalID);

    currZone.GetUserDepthString(tempStr, 80);
    SetTableCol(1, tempStr);

    currZone.GetUserThickString(tempStr, 80);
    SetTableCol(2, tempStr);

    SetTableBool(3, currZone.zoneIsPacker, "Packer", "Zone");

    if (currZone.zoneIsPacker)
    {
      SetTableCol(4, "n/a");
      SetTableCol(5, "n/a");
    }
    else
    {
      currZone.GetUserDeltaVolumeString(tempStr, 80);
      SetTableCol(4, tempStr);
      currZone.GetUserTZCompString(tempStr, 80);
      SetTableCol(5, tempStr);
    }
    SetTableInt(6, currZone.nintervalNodes);

    AddNextLine();
  }

}


LayerErrorListing::LayerErrorListing()
  :ErrorListObjC("Layer")
{
}

void LayerErrorListing::CreateListing()
{
  StdTitle("Layer Setup Errors");

  layerSpec.SetThicknesses();

  double totThick = 0.0;
  for (int i = 0; i < geologyLayers.Size(); i++)
  {
    const double& currThick = geologyLayers[i].currThickness;
    if (currThick < 1.0E-03)
      AddError("Geology layer < 1 mm thick");

    const char* currID = geologyLayers[i].intervalID;
    for (int j = i + 1; j < geologyLayers.Size(); j++)
      if (SameString(currID, geologyLayers[j].intervalID))
        AddError("Duplicate geology layer ID");

    totThick += currThick;
  }
  for (int i = 0; i < wellBoreZones.Size(); i++)
  {
    const double& currThick = wellBoreZones[i].currThickness;
    if (currThick < 1.0E-03)
      AddError("Well bore zone < 1 mm thick");

    const char* currID = wellBoreZones[i].intervalID;
    for (int j = i + 1; j < wellBoreZones.Size(); j++)
      if (SameString(currID, wellBoreZones[j].intervalID))
        AddError("Duplicate wellbore zone ID");

    totThick -= currThick;
  }

  if (totThick < 0.0)
    AddError("Well bore zone thicker than defined geology");

}

const char* LayerStaticSupport::groupHeader = "Layers";
const int  LayerStaticSupport::majorVersion = 0;
const int  LayerStaticSupport::minorVersion = 0;


void LayerStaticSupport::WriteGeoLayer(const GeologyLayer& outLayer)
{
  WriteText(outLayer.intervalID);
  WriteUnitReal(outLayer.topDepth);
  WriteUnitReal(outLayer.topElevation);
  WriteInt(outLayer.nintervalNodes);
  WriteLine();

  WriteBool(outLayer.layerHasSkin);
  WriteBool(outLayer.layerIsIsotropic);
  WriteInt(outLayer.parIndex);
  WriteLine();
}


void LayerStaticSupport::ReadGeoLayer(GeologyLayer& inLayer)
{
  char temp[IntervalBase::intervalIDLen];
  ReadText(temp, IntervalBase::intervalIDLen);
  inLayer.SetID(temp);
  ReadUnitReal(inLayer.topDepth);
  ReadUnitReal(inLayer.topElevation);
  inLayer.nintervalNodes = ReadInt();

  inLayer.layerHasSkin = ReadBool();
  inLayer.layerIsIsotropic = ReadBool();
  inLayer.parIndex = ReadInt();
}

void LayerStaticSupport::WriteWellBoreZone(const WellBoreZone& outWBZ)
{
  WriteText(outWBZ.intervalID);
  WriteUnitReal(outWBZ.topDepth);
  WriteUnitReal(outWBZ.topElevation);
  WriteInt(outWBZ.nintervalNodes);
  WriteLine();

  WriteBool(outWBZ.zoneIsPacker);
  WriteUnitReal(outWBZ.deltaVolume);
  WriteUnitReal(outWBZ.tzComp);
  WriteInt(outWBZ.seqIndex);
  WriteLine();
}


void LayerStaticSupport::ReadWellBoreZone(WellBoreZone& inWBZ)
{
  char temp[IntervalBase::intervalIDLen];
  ReadText(temp, IntervalBase::intervalIDLen);
  inWBZ.SetID(temp);
  ReadUnitReal(inWBZ.topDepth);
  ReadUnitReal(inWBZ.topElevation);
  inWBZ.nintervalNodes = ReadInt();

  inWBZ.zoneIsPacker = ReadBool();

  ReadUnitReal(inWBZ.deltaVolume);
  ReadUnitReal(inWBZ.tzComp);

  inWBZ.seqIndex = ReadInt();
}

void LayerStaticSupport::WriteToFile()
{
  WriteGroupHeader(groupHeader, majorVersion, minorVersion);

  // control
  WriteBool(enterLayerDepths);
  WriteUnitReal(bottomLayerDepth);
  WriteUnitReal(bottomLayerElevation);
  WriteBool(useCommonHistory);
  WriteUnitIndex(wellboreZoneDeltaVolumeUnits);
  WriteUnitIndex(wellboreZoneTZCompUnits);
  WriteLine();

  WriteInt(geologyLayers.Size());
  WriteInt(wellBoreZones.Size());
  WriteLine();

  SetRealConvFormat(SC_SciFormat(12));

  for (int i = 0; i < geologyLayers.Size(); i++)
    WriteGeoLayer(geologyLayers[i]);

  for (int i = 0; i < wellBoreZones.Size(); i++)
    WriteWellBoreZone(wellBoreZones[i]);

}


bool LayerStaticSupport::ReadFromFile()
{
  ReadGroupHeader(groupHeader, majorVersion);

  enterLayerDepths = ReadBool();
  ReadUnitReal(bottomLayerDepth);
  ReadUnitReal(bottomLayerElevation);
  useCommonHistory  = ReadBool();
  ReadUnitIndex(wellboreZoneDeltaVolumeUnits);
  ReadUnitIndex(wellboreZoneTZCompUnits);

  geologyLayers.SetSize(ReadInt());
  wellBoreZones.SetSize(ReadInt());

  for (int i = 0; i < geologyLayers.Size(); i++)
    ReadGeoLayer(geologyLayers[i]);

  for (int i = 0; i < wellBoreZones.Size(); i++)
    ReadWellBoreZone(wellBoreZones[i]);

  return true;
}


void LayerStaticSupport::GetLayerDiscretization(SC_DoubleArray& layerNodeZ,
    SC_IntArray&   layerWellBoreZoneIndex,
    SC_IntArray&   layerGeologyIndex)
{
  if (!layerSpec.SetThicknesses())
  {
    layerNodeZ.Alloc(2);
    layerNodeZ[0] = 0.0;
    layerNodeZ[1] = 1.0;

    layerWellBoreZoneIndex.AllocAndFill(1, 0);
    layerGeologyIndex.AllocAndFill(1, 0);
    return;
  }

  SC_DoubleArray geoLayerZ(GetNGeoLayer() + 1), geoLayerDZ(GetNGeoLayer());
  geoLayerZ[0] = bottomLayerElevation.GetMetricVal();
  for (int i = 0; i < GetNGeoLayer(); i++)
  {
    GeologyLayer& currLayer = geologyLayers[i];
    geoLayerZ[i + 1] = geoLayerZ[i] + currLayer.currThickness;
    geoLayerDZ[i] = currLayer.currThickness / double(currLayer.nintervalNodes);
  }
  double maxZ = geoLayerZ.LastIndex();

  SC_DoubleArray wbzLayerZ(GetNWellboreZone() + 1);
  wbzLayerZ[0] = geoLayerZ[0];
  for (int i = 0; i < GetNWellboreZone(); i++)
  {



  }



}

};                                      // end NS

