///////////////////////////////////////////////////////////////////////////////////
//
// C_StaticCondensation.h
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


#ifndef C_STATICCONDENSATION_H
#define C_STATICCONDENSATION_H

#include <Sim/C_WorkParameter.h>


class StaticCondensation {

protected:
  SC_DoubleArray    flowFactor, gaussResults;
  SC_DoubleArray      scaleVector, rhsVector;

  SC_DoubleArray      dMainDiag, dUpperDiag, sDiag;
  SC_DoubleArray    solveMainDiag, solveUpperDiag;
  SC_DoubleArray      sFactor;
  SC_DoubleMatrix   pressure, prevPressure;

  int         nstaticNodes, nstaticSolnNodes, nmainNodes;

  double        bTerm;  // boundary term for leakage

  //  local copies for convenience
  double        kpaConst, staticPressure;

  double        qVal;  // result of calc Q val

public:
  StaticCondensation();
  ~StaticCondensation();

  virtual void    AllocateAll(int maxMainNodes, bool saveOld) = 0;
  virtual void    SetRuntimeSize(WorkingParameterArray& wPar, int innmainNodes) = 0;

  void        ConstantInit(const double& inStatic,
                           const double& inkpaConst);

  void        CalcQTerm(const double& deltaT);
  double        GetLhsQ(int mainNodeNum);
  virtual double    GetRhsQ(int mainNodeNum) = 0;
  void        GaussSolve(      int    mainNodeNum,
                               const double&  mainNodePressure);

  // dynamic TS support
  void        SaveCurrentPressure();
  void        RestoreCurrentPressure();
  void        AddToMinMax(double& minP,
                          double& maxP,
                          int   startNode);

protected:
  void        DoSetRuntimeSize(int innmainNodes);
  void        DoAllocateAll(int maxSCNodes, int maxMainNodes, bool saveOld);
  double        BasicRHSQ(int mainNodeNum);

private:

  // gauss based for static condensation
  void          GaussElim();

  void          CalcFlowFactor();


  void          SolveSetup(const double& deltaT);

};


class DualPorosity : public StaticCondensation {


public:
  // main setup

  void        SetParameters(WorkingParameterArray& wPar,
                            const ParameterGroups&  parGroup,
                            const SC_DoubleArray&   fracNodeVol);

  // SC virtuals
  virtual double    GetRhsQ(int mainNodeNum);
  virtual void        AllocateAll(int maxMainNodes, bool saveOld);
  virtual void    SetRuntimeSize(WorkingParameterArray& wPar, int innmainNodes);

};

class Leakage : public StaticCondensation {


public:
  // main setup

  void        SetParameters(WorkingParameterArray& wPar,
                            const ParameterGroups&  parGroup,
                            const SC_DoubleArray&   nodeArea);

  // SC virtuals
  virtual double    GetRhsQ(int mainNodeNum);
  virtual void        AllocateAll(int maxMainNodes, bool saveOld);
  virtual void    SetRuntimeSize(WorkingParameterArray& wPar, int innmainNodes);

};



#endif
