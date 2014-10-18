///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell.h
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


#ifndef C_SINGLEWELL_H
#define C_SINGLEWELL_H

#include <genClass/C_GlobalFunc.h>

#include <osClass/C_Runnable.h>

#include <Var/E_Parameter.h>

#include <Sim/C_WorkCommon.h>
#include <Sim/C_WorkSequence.h>
#include <Sim/C_WorkParameter.h>
#include <Sim/C_DynamicTS.h>


class SingleWellSimulator : public RunnableC, public DynamicTimeSteps  {

  friend class WorkingSequence;  // for dynamic TS stuff


  typedef T_SC_Array<const ParameterGroups*> RockMapArray;


public:

  //  unit conversions
  static const double kpaToPa;
  static const double cPToPas;

protected:
  RockMapArray    rockMap;

  //-------------   case scalars
  int         nradialNodes, nskinNodes, fracStartNode;

  // parameters
  double        staticPressure;

  // boundary
  double        externalBoundaryTerm;

  // Geometry
  SC_DoubleArray    nodeVolume;   // volume associated with node
  SC_DoubleArray    nodePlanArea; // plan view area
  SC_DoubleArray    nodeFlowArea; // i contains flow area between i & i +1
  double        lastSkinVolume, lastSkinArea;

  // conversion - slight kluge but convenient
  double        kpaConst;

  //  current sequence data
  bool        currentSeqIsFixed;      //  current sequence is history
  double        deltaT;
  SeqTimeVals     currSeqTime;
  SeqBoundVals    currSeqTZ, lastSeqTZ;
  WorkingSequence*  currSeqData;

public:
  WorkingParameterArray wPar;
  SC_DoubleArray      nodeRadii;
  WorkingSequenceArray  wSeq;

public:
  SingleWellSimulator();
  virtual       ~SingleWellSimulator();

  // Optimizable virtuals
  virtual bool        AllocateAll(UpdateCB upCB);  // for multiple runs -- allocates based on max par vals
  virtual bool        SetupForRun(SC_SetupErr&    errData);
  virtual void        Execute();
  virtual void        UpdateOutput();

protected:
  virtual void    SetRuntimeSize();
  virtual void    SetRunParameters();
  virtual void    DoAllocateAll();

  virtual void    MatSolve(bool& setupRequired) = 0;  // main driver
  virtual void    UpdateTSOutput() = 0;

  virtual void    SequenceSetup();      // for type specific init before sequence
  virtual void    SequenceComplete();     // for type specific cleanup after sequence
  virtual void    InitializeTZData();     // for type specific TZ initialization at first sequence
  virtual void    UpdateTZData();       // for type specific TZ update

  // for f(R) table calculation
  virtual double    GetFrCondVal(int nodeNum);  // default OK for linear liquid
  virtual double    GetFrStoreVal(int nodeNum); // default OK for linear liquid

  // utility - set to skin & formation
  void        SetRockMap(int stNode, int endNode, const ParameterGroups& currType);

  virtual double    GetStaticPressure(); // default is parameter, overriden by unconfined
  virtual double    GetWstoreCompressibility(); // default is liquid parameter, overriden by gas


  // boundary conditions for matrix
  double        GetWellBCRhs();
  double        GetWellBCLhs();

  // for factor & linear calcs - override for nonlinear factor
  virtual double    GetRockPar(int parIndx,
                               int nodeNum);
  virtual double    GetFluidPar(int parIndx);

  //  factor output
  void        CalcSkinFactor(int skIndex, int fkIndex);
  void        CalcFluidFormationFactors();

  // debug only
  int         tstepNum;   // used in gas debug output
  void        AddMessageVal(const char* txt, const double& cval);

  // slight kluge to address FB182
  virtual double    GetLastWellPressure();  // default for all but NL Gas


private:

  //  ------------------ Geometry Setup Calcs
  //  klugy approach because pascal used nested procedures. no C equiv structure
  //  vars used by geom calcs
  double        lastA, lastR, thicknessAtWell;
  void        CalcGeometry();
  double        flowCons, nVal, r1, r2, r12;
  void        CalcAreaTerms(int stNode, int endNode);  // calculates area and volumes
  // related terms for nodes

  void            CalcFlowDCons(int nodeNum);     // calculates flow dimension related
  // constants for a selected node
  void        CalcAreaVolume(const double& rVal,  // calculates enclosed volume
                             double& aVal,  // and surface area using last
                             double& vVal);  // calculated flowCons


  bool        accountForTZVol, accountForTZComp, accountForTZTemp, tzLhsChng;
  double        tzWstoreTerm, casingArea;


  void        DoNextTimeStep(bool& setupRequired);
  void        DoSequenceRun(WorkingSequence& currSeq);

  double        lastFlowArea;
  void        UpdateFRTable();
};


#endif
