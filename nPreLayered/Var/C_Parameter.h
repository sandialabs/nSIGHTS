///////////////////////////////////////////////////////////////////////////////////
//
// C_Parameter.h
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


#ifndef C_PARAMETER_H
#define C_PARAMETER_H

#include <genClass/T_SC_AllocPtrArray.h>

#include <objClass/DC_Curve.h>
#include <genClass/SC_SetupErr.h>

#include <osClass/C_OptVarUnits.h>
#include <osClass/C_SampVarUnits.h>
#include <osClass/C_VaryVarUnits.h>

#include <Var/C_CurveFile.h>

#ifdef _WIN32
#include <afx.h>            // strings for dialogs
#endif

// these classes support parameters implemented in G_Parameter

// common to ParameterCurve and working parameter curve
class ParameterCurveBase {
public:
  bool                xIsLog10;             // transform Xi before curve extract
  bool                yIsLog10;             // transform f(Xi) after curve extract
  UnitIndex           xUnitIndex;           // units of xi data
public:
  ParameterCurveBase() {};
  ~ParameterCurveBase() {};

private:
  // don't need copy ctor
  ParameterCurveBase(const ParameterCurveBase& a) {};
  ParameterCurveBase& operator= (const ParameterCurveBase& a) {
    return *this;
  }
};


//  common to file and points curve parameter types
class ParameterCurve  : public ParameterCurveBase {

  friend class WorkingParameterCurve;

public:
  UnitIndex&          yUnitIndex;           // units of yi data - linked to main param for points

public:
  ParameterCurve(UnitType   xUnitType,
                 UnitIndex& yUnitIndexInit);
  ~ParameterCurve() {};

  ParameterCurve&     operator= (const ParameterCurve& a);

protected:
  //  curve extraction
  double              GetMetricCurveY(const double&   metricXVal,
                                      DC_Curve& curve);

private:
  // don't need copy ctor
  ParameterCurve(const ParameterCurve& a);
};



// the following PointsCurve ... classes are for implementing f(P) points and f(r) points parameters
// they are complicated by the requirement to be able to optimize Xi and Yi for f(r) points and Yi for f(P) points'
//  record for single X or Y value
class PointsCurveValue {
public:
  enum        PointsCurveValueType  {pcvFixed,
                                     pcvOpt,
                                     pcvVary
                                    };


  PointsCurveValueType pType;
  double               pVal;               //  Pi,ri or Yi  for non opt cases
  //  NOTE that, unlike nearly everything else, pVal is
  //  maintained in user units (not metric) to allow
  //  conversions are required prior to use

  OptVarUnits*        pOptData;           //  allocd only if required (pType == pcvOpt)
  VaryVarUnits*       pVaryData;          //  allocd only if required (pType == pcvVary)

private:
  enum                {pointsIDLen = 32};
  char                pID[pointsIDLen];   //  for OptVar ctor
  bool                pOptIsActive;       //  used in reference for pOptData, required
  //  as separate member for case where parameter is inactive
  bool                pVaryIsActive;      //  used in reference for pVaryData, required
  //  as separate member for case where parameter is inactive
  UnitIndex&          unitIndex;          //  for OptVar & VaryVar ctor
  const bool      defaultIsLinear;

public:
  PointsCurveValue(UnitIndex&  inUnitIndex, bool defIsLinear);
  ~PointsCurveValue();  // non trivial

  PointsCurveValue&    operator= (const PointsCurveValue& a);

  inline bool     IsOpt() {
    return pType == pcvOpt;
  }
  inline bool     IsVary() {
    return pType == pcvVary;
  }
  inline bool     IsFixed() {
    return pType == pcvFixed;
  }


  void                InitOpt();    // initializes pOptData if reqd
  void                InitVary();  // initializes pVaryData if reqd

  void                SetID(const char baseName[],  // sets the pID member
                            int  index);

  void                SetActive(bool parIsActive);
  void        ClearWasActive();  // clears all old wasActive settings

  void                SetForFlags(bool parIsActive);
  void                GetVals();  // gets current opt/vary val(s), sets pVal appropriately
  void                SetVals();  // sets current opt val(s) from pVal

  bool                SetupOK(SC_SetupErr& errData,
                              bool     pIsLog);      // returns true if basic setup is OK

private:
  // don't need copy ctor
  PointsCurveValue(const PointsCurveValue& a);
};

class PointsCurveXY {
public:
  PointsCurveValue    xVal;
  PointsCurveValue    yVal;

public:
  PointsCurveXY(); // dummy for template
  PointsCurveXY(UnitIndex&  inX, UnitIndex&  inY, bool defYisLin);
  ~PointsCurveXY() {};
  PointsCurveXY&      operator= (const PointsCurveXY& a);


};

class PointsCurveXYArray: public T_SC_AllocPtrArray<PointsCurveXY> {
public:
  UnitIndex&          xUnits;     // kluge for constructability
  UnitIndex&          yUnits;     // kluge for constructability
  const bool      defYIsLinear;

public :
  PointsCurveXYArray(UnitIndex&  inX, UnitIndex&  inY, bool initDefYisLinear);
  PointsCurveXYArray& operator= (const PointsCurveXYArray& a);

  //  overrides to deal with constructor issues
  virtual void  MakeNew(int index);
};


class PointsCurve  : public ParameterCurve {

public:
  PointsCurveXYArray  pVals;
  DC_Curve            pointsCurve;          // curve spec

  //  curve opt spec data -- copied into opt vars in check routines
  double              yoptMinLim;
  double              yoptMaxLim;
  bool                yoptIsLinear;
  double              yoptParSigma;

  bool                xoptIsLinear;
  double              xoptParSigma;

private:
  const char*         xVarID;
  const char*         yVarID;

public:
  PointsCurve(const char* xVarIDInit,
              const char* yVarIDInit,
              UnitType   xUnitType,
              UnitIndex& yUnitIndexInit,
              bool    parIsLinear);
  ~PointsCurve() {};

  //  standard for parameters
  void                GetVals();  // gets current opt/vary val(s) for each xy
  void                SetVals();  // sets current opt val(s) from pVal
  void                SetActive(bool parIsActive);
  void        ClearWasActive();  // clears all old wasActive settings

  void                SetForFlags(bool parIsActive);

  //  checking
  bool                SetupOK(SC_SetupErr& errData);      // returns true if basic setup is OK

  bool                CurveOK(SC_SetupErr& errData);      // checks and creates curve
  // assumes basic setup OK

  //  curve extraction
  double              GetMetricPointCurveY(const double& metricXVal);


private:
  // don't need copy ctor
  PointsCurve(const PointsCurve& a);
  PointsCurve&        operator= (const PointsCurve& a) {
    return *this;
  }
};


//  for curves in the curve files
//  and objects -- update Sept 03 - no name change
class FixedCurve : public ParameterCurve {
public:
  // for file
  char        curveID[DC_CurveWithID::curveIDlen];
  CurveFile&          curveFile;
  // for object
  FuncObjRef      curveObjRef;


  UnitIndex           yUnitIndex;           // units of yi data - NOT linked to main param
  DC_Curve*           curveToUse;            // set by SetupOK()

private:
  const char*         parID;                // for setup errors
  int                 fileIndex;            // index into appropriate curve file

public:
  FixedCurve(     CurveFile& curveFileInit,
                  UnitType   xUnitType,
                  UnitType   yUnitType,
                  const char*      parIDInit);
  ~FixedCurve() {};
  FixedCurve& operator= (const FixedCurve& a);

  bool                SetupOK(SC_SetupErr& errData);      // returns true if basic setup is OK

  //  curve extraction
  double              GetMetricFileCurveY(const double& metricXVal);

private:
  // don't need copy ctor
  FixedCurve(const FixedCurve& a);
};


// adds range checking
class ParSampVar : public SampVarUnits {
private:
  const double& minLim;
  const double& maxLim;

public:
  ParSampVar(bool& isActiveInit,
             const char* varShortIDInit, const char* varLongIDInit,
             UnitIndex& unitIndexInit,
             const double& minL, const double& maxL, bool defIsLinear);

  virtual     bool          CheckLimits(int sampleNum) const;
};

//  this is the basic parameter class
class Parameter {

public:
  enum    ParameterType  {ptFixed,        // simple constant - always OK
                          ptVary,         // suite if Forward/Normal
                          // range if Forward/Range
                          ptOpt,          // simple opt if Optimization
                          ptSample,       // sample if Forward/Sample or Opt/Sample

                          ptFrFixed,    // f(r) from std f(r) file (in G_CurveFile) or from object
                          ptFPFixed,    // f(P) from std f(P) file (in G_CurveFile) or from object

                          ptFrPoint,      // f(r) from points
                          ptFPPoint,      // f(P) from points

                          ptFtFixed,    // f(t) from std f(t) file (in G_CurveFile) or from object
                          ptFtPoint,      // f(t) from points

                         };

  enum  CurveParameterType {cptRadial, cptPressure, cptTime};


  ParameterType       parameterType;

  UnitReal            parVal;             // fixed par value and unit spec

  bool        isVary;       //  isActive flag used by VaryVar class
  VaryVarUnits    varyParVal;     //  data for simple vary

  bool                isOpt;              //  isActive flag used by OptVar class
  OptVarUnits         optParVal;          //  data for simple optimized

  bool                isSampled;          //  isActive flag used by SampVar class
  ParSampVar          sampParVal;         //  data for sampled

  PointsCurve*        pPointsCurve;       //  created if parameterType == ptFPPoint
  PointsCurve*        rPointsCurve;       //  created if parameterType == ptFrPoint
  PointsCurve*        tPointsCurve;       //  created if parameterType == ptFtPoint

  FixedCurve      pFixedCurve;         //  data for ptFPFixed
  FixedCurve      rFixedCurve;         //  data for ptFrFixed
  FixedCurve      tFixedCurve;         //  data for ptFtFixed

  //  all set by constructor and fixed for duration
  const   char*           fullName;
  const   char*           shortName;

  const   bool            useUnitFormat;  // if true use default for unit, if false use format below
  const   SC_RealFormat   format;         // for parameter specific formats

  const   double          minLimit;       // parameter range
  const   double          maxLimit;

  const   bool            optIsOK;        // used by UI routines to set available parameter types
  const   bool            fpIsOK;
  const   bool            frIsOK;
  const   bool            ftIsOK;
  const   bool      defaultIsLinear;  // for opt/range stepping

protected:
  bool                    parIsActive;
  bool          wasEdited;    // only edited parameters are saved in cfg

public:
  // ctor sigs
  //  default  -- inits var to null real, default unit format
  Parameter(const char*   initFullName,
            const char*   initShortName,
            UnitType  initUnitType,
            const double&   initMinLimit,
            const double&   initMaxLimit,
            bool    initOptIsOK,
            bool    initFpIsOK,
            bool    initFrIsOK,
            bool    initFtIsOK,
            bool    initDefIsLinear = true);

  //  inits var to initVal, default unit format
  Parameter(const char*   initFullName,
            const char*   initShortName,
            UnitType  initUnitType,
            const double&   initMinLimit,
            const double&   initMaxLimit,
            bool    initOptIsOK,
            bool    initFpIsOK,
            bool    initFrIsOK,
            bool    initFtIsOK,
            const double&   initVal,
            bool    initDefIsLinear = true);

  //  inits var to null real, custom unit format
  Parameter(const char*           initFullName,
            const char*           initShortName,
            UnitType        initUnitType,
            const double&         initMinLimit,
            const double&         initMaxLimit,
            bool            initOptIsOK,
            bool            initFpIsOK,
            bool            initFrIsOK,
            bool            initFtIsOK,
            const SC_RealFormat&  initFormat,
            bool      initDefIsLinear = true);


  //  inits var to initVal, custom unit format
  Parameter(const char*           initFullName,
            const char*           initShortName,
            UnitType        initUnitType,
            const double&         initMinLimit,
            const double&         initMaxLimit,
            bool            initOptIsOK,
            bool            initFpIsOK,
            bool            initFrIsOK,
            bool            initFtIsOK,
            const SC_RealFormat&  initFormat,
            const double&         initVal,
            bool      initDefIsLinear = true);


  // destructor
  virtual         ~Parameter();

  // to be called to change parameter type
  // initializes pPointsCurve or rPointsCurve if reqd
  void                    SetParameterType(ParameterType       newParameterType,    // new type
      bool                updateFromOldType);  // if true set new type values from old type if possible

  // enables/disables opt/samp settings for used/unused parameters
  void                    SetActive(bool isActive);
  inline bool             IsActive() const {
    return parIsActive;
  }
  void          ClearWasActive();  // clears all old wasActive settings



  // various inlines for type access

  inline bool       IsNonLinear() const {
    return (parameterType == ptFPFixed) ||
           (parameterType == ptFPPoint);
  }

  inline bool       IsRadial() const {
    return (parameterType == ptFrFixed) ||
           (parameterType == ptFrPoint);
  }
  inline bool       IsTime()  const {
    return (parameterType == ptFtFixed) ||
           (parameterType == ptFtPoint);
  }

  inline bool       IsCurve() const {
    return (IsRadial() || IsNonLinear() || IsTime());
  }

  inline bool             IsConstant() const {
    return (!IsCurve());
  }

  inline bool             IsVary() const {
    return (parameterType == ptVary);
  }

  inline bool             IsFixed() const {
    return (parameterType == ptFixed);
  }

  inline bool             IsSampled() const {
    return (parameterType == ptSample);
  }

  inline bool             IsOpt() const {
    return (parameterType == ptOpt);
  }

  inline bool             IsPoints() const {
    return (parameterType == ptFrPoint) ||
           (parameterType == ptFPPoint) ||
           (parameterType == ptFtPoint);
  }

  inline bool             IsFPPoints() const {
    return (parameterType == ptFPPoint);
  }
  inline bool             IsFrPoints() const {
    return (parameterType == ptFrPoint);
  }
  inline bool             IsFtPoints() const {
    return (parameterType == ptFtPoint);
  }

  // checking
  virtual bool            SetupOK(SC_SetupErr& errData);
  // returns true if basic setup is OK
  // virtual to allow special checking for flow D

  // edit status
  bool                    WasEdited() const {
    return wasEdited;
  }
  void                    SetEdited() {
    wasEdited = true;
  }

  // access
  double                  GetParValue() const;  // standard

  int           GetIntMaxPar() const;  // for runtime allocation of arrays based on numeric parameters

  double                  GetVaryParValue() const;  // vary/range

  bool                    GetRadialParValue(const SC_DoubleArray& rVal,
      SC_DoubleArray& parVals,
      SC_SetupErr& errData);  // errData contains error message if any

  bool          GetPressureCurve(WorkingParameterCurve& fpCurve,
                                 SC_SetupErr& errData);  // errData contains error message if any

  bool          GetTimeCurve(WorkingParameterCurve& ftCurve,
                             SC_SetupErr& errData);  // errData contains error message if any

  const SC_RealFormat*  GetOverrideFormat() const;

  void                    ParValToString(const double&  metricVal,
                                         char     parString[],
                                         int      parStrLen);

  void                    ParValToString(const double&  metricVal,    // in dialogs were units can be chaged
                                         int      unitIndex,
                                         char     parString[],
                                         int      parStrLen);
#ifdef _WIN32
  void                    ParValToString(const double&  metricVal,
                                         CString& parString);

  void                    ParValToString(const double&  metricVal,    // in dialogs were units can be chaged
                                         int      unitIndex,
                                         CString& parString);
#endif

  PointsCurve*      GetPointsCurve();
  CurveParameterType    GetCurveType();


protected:
  virtual bool      ParValOK(const double&      pVal,
                             const char         rangeID[],
                             SC_SetupErr& errData);

private:
  void                    InitCommon(); // common to all ctors


};


//  copies of file/points curve setup used by working parameters
class WorkingParameterCurve : public ParameterCurveBase {
public:
  DC_Curve        parCurve;
  UnitIndex             yUnitIndex;

public:
  WorkingParameterCurve();
  WorkingParameterCurve&  operator= (const ParameterCurve& a);

  double                GetMetricCurveY(const double&   metricXVal);
private:
  WorkingParameterCurve(const WorkingParameterCurve& a) {};

};





#endif
