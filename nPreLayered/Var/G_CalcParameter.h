///////////////////////////////////////////////////////////////////////////////////
//
// G_CalcParameter.h
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


#ifndef G_CALCPARAMETER_H
#define G_CALCPARAMETER_H

#include <osMain/C_OSListObj.h>
#include <Var/E_Parameter.h>

namespace nsParameterGlobals  {

class ParameterCalculator {
public:
  SC_DoubleArray    currCalcVals;
  SC_DoubleArray    minCalcVals;
  SC_DoubleArray    maxCalcVals;
  SC_IntArray     calcPars;
  UnitIndex       parUnits;
  const char*       parID;
  const SC_RealFormat*  poverrideFormat;
  SC_RealFormat     overrideFormat;
public:
  ParameterCalculator(UnitType pUnits,
                      const char* pID);
  ParameterCalculator(UnitType pUnits,
                      const char* pID,
                      const SC_RealFormat& rForm);

  inline void   AddPar(int par) {
    calcPars += par;
  }

  void      Override(UnitType pUnits, const char* pID,
                     const SC_RealFormat& rForm);
  void      Override(UnitType pUnits, const char* pID);


  bool      AllFixed();
  bool      IsFP();
  bool      IsFR();
  bool      IsFT();
  bool      IsMinMax();
  int       GetnVary();
  void      SetCalcVals(bool doVary);

  virtual double  GetCalcVal() = 0;
  virtual double  GetMinCalcVal();
  virtual double  GetMaxCalcVal();

  void      GetUserString(char* calcStr, int strLen);

protected:
  void      SetMinToCurrent();
  void      SetMinToCurrent(int par);
  void      SetMaxToCurrent();
  void      SetMaxToCurrent(int par);
  double      GetDensity();
  double      GetViscosity();

private:
  void      InitCommon();

};



class CalcParameterListing : public OSListObj {
public:
  CalcParameterListing();
  ~CalcParameterListing() {};

  virtual void  CreateListing();

private:
  void  ListCalcPar(ParameterCalculator& calc);

  bool  ListHKS(const char* subTitle,       // common for dual P/leaky
                const ParameterGroups& group);



  void  ListGasCalc();
  void  ListFluidCalc();
  void  ListLayerCalc();
  void  ListGeomCalc();


};

};

#endif
