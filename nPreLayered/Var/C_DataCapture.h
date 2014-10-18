///////////////////////////////////////////////////////////////////////////////////
//
// C_DataCapture.h
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


#ifndef C_DATACAPTURE_H
#define C_DATACAPTURE_H

#include <Var/C_Layer.h>

#include <genClass/C_FuncObj.h>
#include <genClass/DO_XYData.h>
#include <genClass/T_SC_AllocPtrArray.h>

#include <genClass/SC_SetupErr.h>
#include <osClass/C_OptVarUnits.h>
#include <osClass/C_SampVarUnits.h>
#include <osClass/C_VaryVarUnits.h>


// these classes support data extraction as implemented in G_DataCapture

// superposition record
class SuperComponent {
public:
  enum          SuperOp { soAddP,   // add pressure at point
                          soSubP,   // subtract pressure at point
                          soAddDP,  // add delta pressure at point
                          soSubDP
                        }; // subtract delta pressure at point

  enum          SuperType { stConstant,  // radius is constant
                            stOptimized, // radius is optimized
                            stSampled,   // radius is sampled
                            stVary
                          };   // varied

  SuperOp         superOp;
  SuperType         superType;

  UnitIndex&        superRadUnits;     // units for record

  double          constantRad;

  bool                  isOpt;              //  isActive flag used by OptVar class
  OptVarUnits           optRadVal;          //  data for simple optimized

  bool                  isSampled;          //  isActive flag used by SampVar class
  SampVarUnits          sampRadVal;         //  data for sampled

  bool                  isVary;       //  isActive flag used by VaryVar class
  VaryVarUnits          varyRadVal;         //  data for vary

private:
  enum                  {componentIDLen = 32};
  char                  rID[componentIDLen];   //  for OptVar/SampVar ctor

public:
  SuperComponent();  // default constructor for T_SC_AllocPtrArray -- NOT USED
  SuperComponent(UnitIndex& inUnits);
  ~SuperComponent();

  void        ChangeSuperType(SuperType newType);
  void        ClearWasActive();

  bool                SetupOK(SC_SetupErr& errData);      // returns true if basic setup is OK

  void        SetID(const char* inDesig,
                    int   index);

  const char*     GetID() const {
    return rID;
  }

};


class SuperComponentArray: public T_SC_AllocPtrArray<SuperComponent> {
private:
  UnitIndex&      superRadUnits;     // kluge for constructability

public :
  SuperComponentArray(UnitIndex&  inUnits);

  void  ClearWasActive();
  void  SetConstant();

  //  overrides to deal with constructor issues
  virtual void  MakeNew(int index);
};



class DataCaptureSpec {
public:
  enum          CaptureType {ctPressure, ctFlow, ctProduction, ctTestZone, ctWaterTable};
  enum          PressureCapType {pctTestZone, pctRadius, pctSuperposition};
  enum          FlowCapType {fctWell, fctFormation, fctTestZone, fctWellboreStorage};
  enum          ProductionCapType {prctWell, prctFormation, prctTestZone};
  enum          TestZoneCapType {tzctTemperature, tzctCompressibility, tzctVolume};

  CaptureType       captureType;
  PressureCapType     pressureCapType;
  FlowCapType       flowCapType;
  ProductionCapType   productionCapType;
  TestZoneCapType     testZoneCapType;

  mutable UnitReal    radiusData;  // for obs well & water table (calcd for aniso)
  // unconfined/layered partial penetration support
  bool          zvalueIsRatio; // normalized or actual
  UnitReal        actualZ;
  double          normalizedZ;

  // layered test zone selection
  char          wellboreZoneID[IntervalBase::intervalIDLen];
  int           wellboreZoneIndex;  // set in check routines

  // horizontal anisotropy support
  UnitReal        obsXLocation;
  UnitReal        obsYLocation;

  SuperComponentArray   superData;

  UnitIndex       outputUnits;

  enum          {dataDesigLen = 20};
  char          dataDesig[dataDesigLen];
  bool          makeDefaultDataDesig;

public:
  DataCaptureSpec();
  DataCaptureSpec(const DataCaptureSpec& a);
  ~DataCaptureSpec();

  void          ChangeCaptureType(CaptureType newType);
  void          ClearWasActive();

  void          SetOutputUnits();
  void          SetForFlags();
  void          SetIDs();
  void          SetUserDesig(const char* inStr);
  bool          IsSuperposition() const {
    return
      (captureType == ctPressure) &&
      (pressureCapType == pctSuperposition);
  }

  // support for autosetup routines
  bool          IsWell()  const {
    return
      ((captureType == ctPressure) &&
       ((pressureCapType == pctTestZone) ||
        (pressureCapType == pctRadius))) ||
      ((captureType == ctFlow) &&
       (flowCapType == fctWell));
  }
  bool          IsPressure() const {
    return captureType == ctPressure;
  }
  bool          IsFlow() const {
    return captureType == ctFlow;
  }
  bool          IsWaterTable() const {
    return captureType == ctWaterTable;
  }
  bool          UsesRadius() const {
    return (IsPressure() && (pressureCapType == pctRadius)) || IsWaterTable();
  }
  bool          UsesRadiusUnits() const {
    return (IsPressure() && (pressureCapType != pctTestZone)) || IsWaterTable();
  }

  bool          IsTestZoneBased() const {
    return (
             ((captureType == ctPressure) && (pressureCapType == pctTestZone)) ||
             (captureType == ctFlow) ||
             (captureType == ctProduction) ||
             (captureType == ctTestZone));
  }

  bool                  SetupOK(SC_SetupErr& errData);      // returns true if basic setup is OK

  double          GetObsAngle() const;
  // 0.0 unless horizontal anisotropy
  // then angle to obs well in radians

};


#endif
