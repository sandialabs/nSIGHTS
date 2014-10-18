///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell1DNonLinear.h
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


#ifndef C_SINGLEWELL1DNONLINEAR_H
#define C_SINGLEWELL1DNONLINEAR_H

#include <genClass/SC_BoolArray.h>

#include <Sim/C_SingleWell1D.h>


class SingleWell1DNonLinear : public SingleWell1D  {


private:
  SC_DoubleArray        nrRhs, nrDx, nrWork, nrNewX, nrInitRhs;
  SC_DoubleMatrix     nrMat;

  // nonlinear control & status
  double          nrTolP;
  int           nnrIter;

protected:
  SC_DoubleArray      dLinearTerm, sLinearTerm;
  SC_BoolArray      dTermIsLinear, sTermIsLinear;
  double          nrTolQ;
  bool          lastWasNonLinear;

public:
  SingleWell1DNonLinear();
  virtual       ~SingleWell1DNonLinear();

protected:
  // optimizable virtuals
  virtual bool        SetupForRun(SC_SetupErr&    errData);

  // SingleWell virtuals
  virtual void    SetRuntimeSize();
  virtual void    SetRunParameters();
  virtual void    DoAllocateAll();

  // just used for factor calcs
  virtual double    GetRockPar(int  parIndx,
                               int  nodeNum);
  virtual double    GetFluidPar(int parIndx);

  // dynamic TS
  virtual double    GetPChange();

  void        SetDSTerms();

  double        NLGetRockPar(    int    parIndx,
                                 int    nodeNum,
                                 const double&  pVal);

  double        NLGetFluidPar(      int   parIndx,
                                    const double& pVal);


  // new virtuals for nonlinear
  virtual double    NLGetDTerm(    int    nodeNum,
                                   const double&  pN,
                                   const double&  pN1) = 0;

  virtual double    NLGetSTerm(    int    nodeNum,
                                   const double&  pN) = 0;


  virtual double    NLCalcDTerm(    int     nodeNum,
                                    const double& pN,
                                    const double& pN1) = 0;

  virtual double    NLGetWellPressure() = 0;
  virtual double    NLGetExternalPressure() = 0;


  virtual double    AnalyticPartialD(   int     nodeNum,
                                        const double&  pN) = 0;


  //  runtime calculation
  void          NonlinearMatSolve();
  double          NLCalcSTerm(     int    nodeNum,
                                   const double& pN);
  double          NLCalcFormSTerm(const double& pTZ);

private:

  // nonlinear partial calculations
  double          NLCalcFJ(int       eqnNum,
                           const double& p_i,
                           const double& p_j,
                           const double& p_k);

  double          NLWellDTerm(const double& nextP);
  double          NLExternalDTerm(const double& nodeN_1P);

  // used in calculating derivatives
  void          SetDp(const double&   p,
                      const double&   delP,
                      const double&   pSum,
                      double&   pf1,
                      double&   pf2,
                      double&   dpf);

  void          NLSetPartials();


};

#endif
