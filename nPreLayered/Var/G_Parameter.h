///////////////////////////////////////////////////////////////////////////////////
//
// G_Parameter.h
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


#ifndef G_PARAMETER_H
#define G_PARAMETER_H

#include <genClass/T_SC_Array.h>

#include <genApp/C_CfgFileObj.h>
#include <genApp/G_AppObj.h>

#include <osMain/C_OSListObj.h>

#include <genListClass/C_ErrorListObj.h>
#include <genListClass/C_ListFactory.h>

#include <Var/C_Units.h>
#include <Var/C_Parameter.h>
#include <Var/E_Parameter.h>

namespace nsParameterGlobals  {


class AllocatedParameterArray: public T_SC_Array<Parameter*> {

public:
  AllocatedParameterArray() : T_SC_Array<Parameter*>() {}

  Parameter&  GetRef(int indx);

};
extern  AllocatedParameterArray  allParam;


class ParameterPtrArray: public T_SC_Array<Parameter*> {
public:
  enum  {parDescLen = 40};
  char  parDesc[parDescLen];

public :
  //  constructors
  ParameterPtrArray();
  ParameterPtrArray(const char* parDesc);

  //  type specific methods
  Parameter&  GetRef(int indx) const;

  void    Init();
  void    AddActive(int parToAdd);

  void    StdGasParSetup(Parameters      pk,
                         Parameters      pPhi,
                         Parameters      pKf);

  void    StdFluidParSetup(Parameters      pk,
                           Parameters      pHK,
                           Parameters      pSs,
                           Parameters      pC,
                           Parameters      pPhi,
                           Parameters    pSy);

};

extern ParameterPtrArray  formationPar;
extern ParameterPtrArray  fracturePar;
extern ParameterPtrArray    matrixPar;
extern ParameterPtrArray    skinPar;
extern ParameterPtrArray    fluidPar;
extern ParameterPtrArray    gasPar;
extern ParameterPtrArray    testZonePar;
extern ParameterPtrArray    leakyPar;
extern ParameterPtrArray    lowerLeakyPar;
extern ParameterPtrArray    upperLeakyPar;
extern ParameterPtrArray    numericPar;
extern ParameterPtrArray  layerCommon;


class ParameterPtrMatrix: public T_SC_Array<ParameterPtrArray> {
public :
  //  constructors
  ParameterPtrMatrix():T_SC_Array<ParameterPtrArray>() {}
};

extern ParameterPtrMatrix layerPar;

class ParameterListing : public OSListObj {

public:
  ParameterListing();
  ~ParameterListing() {};

  // C_Listing virtuals
  virtual void  CreateListing();

private:
  void            ListOneParameter(const Parameter& listPar);
  void            ListParameterArray(const ParameterPtrArray& listArray);

};

class ParameterErrorListing : public ErrorListObjC {
public:
  ParameterErrorListing();
  ~ParameterErrorListing() {};

  virtual void  CreateListing();
private:
  bool            CheckParameterArray(const ParameterPtrArray& chkArray);

};

extern ParameterErrorListing  parameterErrorListing;

// singleton only
class ParameterSupport :  public ListFactory,
  public CfgFileObj
{

  friend class ParameterErrorListing;

private:
  static  bool        nonlinearParFound;
  static  bool        timeParFound;
  // object versioning support
  static const char*  groupHeader;
  static const int    majorVersion;
  static const int    minorVersion;
  static appFuncObjGlobals::AppFuncArray parameterObj;

public:
  ParameterSupport();  // constructor initializes all parameters and vars
  ~ParameterSupport();

  //  parameter control
  static void     InitAppGlobals(); // must be called once after creation and before use
  static void         SetToInitial();     // for file New
  static void         SetForFlags();      // creates parameter arrays according to flag settings
  static bool         NonLinearParameters() {
    return nonlinearParFound;
  }
  static bool         TimeParameters()  {
    return timeParFound;
  }
  static bool         ParametersOK();

  // list of f(P)/f(r) points parameters
  static bool         GetPointsParameters(ParameterPtrArray& pointParameters);
  static bool         HasPointsParameters();
  static void     ResetPointsParameters(); // special case after aborted opt


  // implementation of virtuals
  virtual void        WriteToFile();
  virtual bool        ReadFromFile();

  virtual ListObjC*   CreateListObject() {
    return new ParameterListing();
  }
  virtual ErrorListObjC*  GetErrorListObject() {
    return &parameterErrorListing;
  }

private:
  static void         CheckForTimeAndNonlinear();  // scans parameters for fT and fP parameters

  static void         InitAllParameters();  // allocates & initializes

  static void     SetLayeredForFlags();
  static void     SetSingleForFlags();
  static void     SetSingleGasForFlags();
  static void     SetSingleFluidForFlags();
  static void     SetFluidForFlags();

  //  parameter specific config file
  void            WriteParCurve(const ParameterCurveBase& inCurve);
  void            ReadParCurve(ParameterCurveBase& outCurve);

  void      WriteFixedCurve(const FixedCurve& inCurve);
  void            ReadFixedCurve(FixedCurve& outCurve);

  void            WritePointsCurve(const PointsCurve& inCurve);
  void            ReadPointsCurve(PointsCurve& outCurve);

};

// seperate class for listing/error check of points parameters

class PointsParameterListing : public OSListObj {

public:
  PointsParameterListing();
  ~PointsParameterListing() {};

  // C_Listing virtuals
  virtual void  CreateListing();


};


class PointsParameterErrorListing : public ErrorListObjC {
public:
  PointsParameterErrorListing();
  ~PointsParameterErrorListing() {};

  virtual void  CreateListing();

};

extern PointsParameterErrorListing  pointsParameterErrorListing;


class PointsParameterSupport : public ListFactory {
public:
  virtual ListObjC*   CreateListObject() {
    return new PointsParameterListing();
  }
  virtual ErrorListObjC*  GetErrorListObject() {
    return &pointsParameterErrorListing;
  }
};

// only purpose is sequence curve obj ref updating & check
class ParameterCurveFO : public FuncObjC {
public:
  ParameterCurveFO();
  ~ParameterCurveFO() {};

  virtual void    CalcOutput(FOcalcType  calcType) {};
  virtual void  IsBeingDeleted(FuncObjC* delObj);
  virtual void    UpdateFuncObjRefs();
  virtual bool  UsesAsInput(const FuncObjC* inputObj) const; // used in building  connection list

private:
  // don't need these -- make sure they dont't get used
  ParameterCurveFO(const ParameterCurveFO& a) : FuncObjC(a) {};
  ParameterCurveFO& operator= (const ParameterCurveFO& a) {
    return *this;
  }

};



// the one and only
extern  ParameterSupport        parameters;
extern  PointsParameterSupport  pointParameters;
extern  ParameterCurveFO    parameterCurveFO;
};


//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace nsParameterGlobals;


#endif
