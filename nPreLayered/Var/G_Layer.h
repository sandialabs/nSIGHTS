///////////////////////////////////////////////////////////////////////////////////
//
// G_Layer.h
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


#ifndef G_LAYER_H
#define G_LAYER_H

#include <genClass/SC_StringArray.h>

#include <genApp/C_CfgFileObj.h>

#include <osMain/C_OSListObj.h>

#include <genListClass/C_ErrorListObj.h>
#include <genListClass/C_ListFactory.h>


#include <Var/C_Layer.h>

#include <Var/C_Units.h>


namespace nsLayer  {

enum    {maxGeoLayers = 64};
enum    {maxWellboreZones = 32};

// globals
extern GeologyLayers  geologyLayers;
extern WellBoreZones  wellBoreZones;

// depth spec
extern bool     enterLayerDepths;
extern UnitReal     bottomLayerDepth;
extern UnitReal     bottomLayerElevation;
extern UnitIndex  wellboreZoneDeltaVolumeUnits;
extern UnitIndex  wellboreZoneTZCompUnits;

extern bool     useCommonHistory;

class LayerListing : public OSListObj {
public:
  LayerListing();
  ~LayerListing() {};

  virtual void  CreateListing();

private:
  void    ListGeologyLayer(const GeologyLayer& outLayer);
  void    ListWellBoreZones(const WellBoreZone& wboreZone);
};


class LayerErrorListing : public ErrorListObjC {
public:
  LayerErrorListing();
  ~LayerErrorListing() {};

  virtual void  CreateListing();
private:

};

extern LayerErrorListing  layerErrorListing;



class LayerStaticSupport : public ListFactory,
  public CfgFileObj

{
private:
  // object versioning support
  static const char*  groupHeader;
  static const int  majorVersion;
  static const int  minorVersion;

public:
  LayerStaticSupport();     // constructor initializes all parameters and vars
  ~LayerStaticSupport() {};

  static void     SetToInitial();     // for file New
  static void     SetForFlags();      // modifies settings
  static bool   SetThicknesses();

  static bool   LayersOK();

  // misc static routines for geology data
  static int    GetNGeoLayer()    {
    return geologyLayers.Size();
  }
  static bool   GetGeoLayerIDs(SC_StringArray& geoLayerIDs);
  static void   DeleteGeoLayer(int index);  // manages parameter indexes during deletion
  static void   InsertGeoLayer(int index);  // inserts at index, manages parameter indexes during insertion
  static void   GeoLayerCleanup();


  static int    GetNWellboreZone()  {
    return wellBoreZones.Size();
  }
  static bool   GetWellBoreZoneIDs(SC_StringArray& wellBoreZoneIDs);
  static void   DeleteWellBoreZone(int index);  // manages sequence indexes during deletion
  static void   InsertWellBoreZone(int index);  // inserts at index, manages parameter indexes during insertion
  static void   WellBoreZoneCleanup();

  static void   GetLayerDiscretization(SC_DoubleArray&  layerNodeZ,
                                       SC_IntArray&   layerWellBoreZoneIndex,
                                       SC_IntArray&   layerGeologyIndex);

  virtual void    WriteToFile();
  virtual bool    ReadFromFile();

  virtual ListObjC*   CreateListObject() {
    return new LayerListing();
  }
  virtual ErrorListObjC*  GetErrorListObject() {
    return &layerErrorListing;
  }

private:
  void    WriteGeoLayer(const GeologyLayer& outLayer);
  void    ReadGeoLayer(GeologyLayer& outLayer);

  void    WriteWellBoreZone(const WellBoreZone& outWBZ);
  void    ReadWellBoreZone(WellBoreZone& inWBZ);

  static void SetDefaultGeo();
  static void SetDefaultWellBore();

};


extern  LayerStaticSupport   layerSpec;     // the one and only

};

using namespace nsLayer;


#endif
