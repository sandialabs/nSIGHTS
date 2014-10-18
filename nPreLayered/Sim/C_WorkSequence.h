///////////////////////////////////////////////////////////////////////////////////
//
// C_WorkSequence.h
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



#ifndef C_WORKSEQUENCE_H
#define C_WORKSEQUENCE_H

#include <objClass/DC_Curve.h>

#include <Var/C_TestZoneCurve.h>
#include <Var/C_Sequence.h>
#include <Sim/C_WorkCommon.h>
#include <Sim/C_WorkParameter.h>
#include <Sim/C_DynamicTS.h>

class DynamicTimeSteps;

class WorkingSequence;

typedef T_SC_Array<WorkingSequence> WorkingSequenceArray;

class WorkingBC {

  friend class WorkingSequence;

private:
  bool      useCurve;
  double        fixedVal;   // non-curve val

  DC_Curve*   curveToUse;

  // input time conversions
  bool      doTimeConversion;   // timeIsSequenceBased
  UnitIndex   curveTimeUnits;     // conversion for metric time to curve time units
  double      offsetVal;      // offsetTime -- added to converted sequence time
  bool      doLogTime;      // if offset time -- log taken after convert & add

  //  output data conversions
  UnitIndex     curveYUnits;
  bool      curveYisLog10;

public:
  WorkingBC();
  void      Clear();
  double      GetCurveY(const SeqTimeVals& nextTimeVal);  // returns null if error
  void      CurveSetup(TestZoneCurve::TZCurveType curvType,
                       int              currSeq);
};


class WorkingSequence {

  friend bool WorkingSequenceSetup(DynamicTimeSteps&    currSim,
                                   WorkingSequenceArray&  wSeq,
                                   WorkingParameterArray& currPar,
                                   SC_SetupErr&         errData);


public:
  // standard info
  int               seqIndx;
  Sequence::SequenceType        seqType;
  double              seqLength;        // duration in seconds
  const char*           seqID;        // for debugging
  FlowHistBase::WellboreStorage   wellboreStorage;  // set as specd by flow/hist
  // to wsIsolated for Pulse
  // to wsOpen for Slug
  // time step data
  TimeStep::TimeStepType      timeStepType;
  double              linDeltaT;
  double              logTStart;
  double              logTMult;
  double              startTime;    // start time in user units

  //  dynamic TS support
  double              minTS;      // dynamic min in secs
  double              maxTS;      // dynamic max in secs
  double              lastP;      // init to p at t = 0
  double              lastQ;

  //  these are set in simcore at seq start for dynamic P
  DynamicTimeSteps*       currSim;  // needed to update pressure limits
  double              maxPChange;
  double              minPChange;
  bool              dynUnderMin;  // last change was less than min, adjust TS
  int               nochangeCount;
  bool              lastQOK;

  // hist / flow data
  double              fixedOffset;    // in metric units, is curve offset if
  // CurveSample
  // slug/pulse data
  PulseSlugBase::InitialPressureType  initialPressureType;
  double                  initialPressure;  // metric val
  double                tsSavePressure;  // maintained for duration of pulse
  bool                isThermal;

private:
  WorkingBC         tzPressBC;
  WorkingBC         tzFlowBC;
  WorkingBC         tzCompFTBC;
  WorkingBC         tzTempBC;
  WorkingBC         tzVolBC;
  double          volOffset;

  WorkingParameter*   tzCompFP;
  WorkingParameter*   tzDensFP;

  static const double timeEpsFactor;


public:
  WorkingSequence();
  ~WorkingSequence();

  // to satify compiler
  WorkingSequence&  operator= (const WorkingSequence& a) {
    return *this;
  }

  bool        SetData(int         sIndx,
                      DynamicTimeSteps& cSim,
                      const double&   fixedTZVol,
                      const double&     fixedTZComp,
                      const double&   fixedTZTemp,
                      SC_SetupErr&    errData);

  void        StartSequence(    SeqTimeVals&    initTime,
                                const SeqBoundVals& lastSeqEndBC,  // at end of prev seq - used for TS/TZ rel init pressure
                                SeqBoundVals& initBC);

  bool        SetNextTimeStep(SeqTimeVals&  timeVal);     // returns true if DT changed

  void        DynamicSetup();

  bool        DynamicRestart(     SeqTimeVals&  timeVal,  // true if dynamic TS failure
                                  const SeqBoundVals& currBC);


  void        UpdateTimes();

  bool        IsComplete(const SeqTimeVals&  timeVal);

  void        GetTZvals(const SeqTimeVals&    timeVal,
                        SeqBoundVals& nextBC);

  bool        IsDynamicTS() {
    return (timeStepType == TimeStep::tstDynamicP) ||
           (timeStepType == TimeStep::tstDynamicQ);
  }

private:

  void        SetDynamicPressureLimits();
  void        CheckDynamicPressureLimits();

};

typedef T_SC_Array<WorkingSequence> WorkingSequenceArray;

extern bool WorkingSequenceSetup(DynamicTimeSteps&    currSim,
                                 WorkingSequenceArray&  wSeq,
                                 WorkingParameterArray& currPar,
                                 SC_SetupErr&         errData);




#endif
