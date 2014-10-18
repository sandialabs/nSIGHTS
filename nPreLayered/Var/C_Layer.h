///////////////////////////////////////////////////////////////////////////////////
//
// C_Layer.h
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


#ifndef C_LAYER_H
#define C_LAYER_H

#include <genClass/T_SC_Array.h>
#include <genClass/SC_StringArray.h>
#include <genClass/SC_SetupErr.h>

#include <genClass/U_Str.h>
#include <genApp/C_CfgFileObj.h>

#include <Var/C_Units.h>


class  IntervalBase {
public:
  enum        {intervalIDLen  = 40};
  char        intervalID[intervalIDLen];

  UnitReal      topElevation;
  UnitReal      topDepth;

  double        currThickness;

  int         nintervalNodes;

public:
  IntervalBase();
  IntervalBase(const IntervalBase& a);
  IntervalBase&   operator=(const IntervalBase& a);
  bool      CheckError(SC_SetupErr& glErr);
  void      SetID(const char* intID);

  void      GetUserDepthString(char* uStr, int strLen);
  void      GetUserThickString(char* uStr, int strLen);

  bool      IsSet() const {
    return IsNotNull(intervalID);
  }

private:
  void    LocalCopy(const IntervalBase& a);
};


class  GeologyLayer : public IntervalBase {
public:
  bool        layerHasSkin;
  bool        layerIsIsotropic;
  int         parIndex;       // index into layeredPar - each unique

public:
  GeologyLayer();
  GeologyLayer(const GeologyLayer& a);
  GeologyLayer&   operator=(const GeologyLayer& a);
  bool      CheckError(SC_SetupErr& glErr);

private:
  void    LocalCopy(const GeologyLayer& a);
};


class GeologyLayers: public T_SC_Array<GeologyLayer> {

public :
  //  constructors
  GeologyLayers():T_SC_Array<GeologyLayer>() {}
  GeologyLayers(int nAll):T_SC_Array<GeologyLayer>(nAll) {}

  //  type specific methods
  bool    GetIDs(SC_StringArray& geoLayerIDs) const;
  bool    HasSkin() const;


};



class WellBoreZone : public IntervalBase {
public:
  bool        zoneIsPacker;
  int         seqIndex;       // index into layeredSequences - each unique
  UnitReal      deltaVolume;      // vol change from calc
  UnitReal      tzComp;         // compressibility

public:
  WellBoreZone();
  WellBoreZone(const WellBoreZone& a);
  WellBoreZone&     operator=(const WellBoreZone& a);
  void        CheckError(SC_SetupErr& wbzErr);

  void        GetUserDeltaVolumeString(char* uStr, int strLen);
  void        GetUserTZCompString(char* uStr, int strLen);

private:
  void        LocalCopy(const WellBoreZone& a);

};

class WellBoreZones: public T_SC_Array<WellBoreZone> {

public :
  //  constructors
  WellBoreZones():T_SC_Array<WellBoreZone>() {}
  WellBoreZones(int nAll):T_SC_Array<WellBoreZone>(nAll) {}

  //  type specific methods
  bool    GetIDs(SC_StringArray& geoLayerIDs);
  bool    GetIndex(const char* inID, int& outIndex) const;

};


#endif
