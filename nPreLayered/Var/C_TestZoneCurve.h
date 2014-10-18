///////////////////////////////////////////////////////////////////////////////////
//
// C_TestZoneCurve.h
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



#ifndef C_TESTZONECURVE_H
#define C_TESTZONECURVE_H

#include <genClass/SC_SetupErr.h>
#include <objClass/DC_CurveArray.h>
#include <Var/C_Units.h>
#include <Var/C_Layer.h>
#include <Var/C_Sequence.h>

class TestZoneCurve {
public:
  enum TZCurveType     {  tzPressure,     // specd pressure for History seq
                          tzFlowRate,     // specd flow for Flow seq
                          tzTemperature,    //  TZ temp for non-isothermal pulse
                          tzCompressibility,  //  TZ comp for all isolated flow and pulse
                          tzDeltaVolume,      //  TZ delta volume for all isolated flow and pulse
                          tzAbsVolume,    //  TZ abs volume for all isolated flow and pulse
                          tzFlagReset     //  was reset -- is invalid
                       };

  char            curveID[DC_CurveWithID::curveIDlen];
  FuncObjRef        curveObjRef; // for curve object support
  TZCurveType       tzType;
  char          wellboreZoneID[IntervalBase::intervalIDLen];
  char          startSeqID[Sequence::seqIDLen];
  char          endSeqID[Sequence::seqIDLen];

  bool          timeIsSequenceBased;  // default false curve time is test time and units
  UnitIndex       curveYUnits;
  bool          curveYisLog10;

  UnitIndex       curveTimeUnits;   // only if zero based
  bool          curveTimeisLog10;  // only if zero based

  //  following are set up by call to SetupOK
  int           curveIndex;
  int           wellboreZone;
  int           startSeq;
  int           endSeq;
  DC_Curve*       curveToUse;            // set by SetupOK()

public:
  TestZoneCurve();

  // checks/changes time step to be OK for new type
  void          ChangeCurveType(TZCurveType newType);

  //
  void          SetForFlags();

  bool                  SetupOK(SC_SetupErr& errData);      // returns true if basic setup is OK

};



#endif
