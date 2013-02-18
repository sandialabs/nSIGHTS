///////////////////////////////////////////////////////////////////////////////////
//
// C_FuncObj.h
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains certain rights in this software.
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
//
// DESCRIPTION: Platform independent and application independent base classes
//              and support classes.
//              The fundamental framework class. It is the base class for all
//              processing/plotting/listing functional objects.  FuncObjC manages
//              all inter-object communications and creates the object execution
//              tree. Virtual functions defined in FuncObjC are implemented in
//               derived classes to provide actual object functionality.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_FUNCOBJ_H
#define C_FUNCOBJ_H

#include <typeinfo>

#include <genClass/C_Common.h>
#include <genClass/T_SC_Array.h>
#include <genClass/T_SC_BasicPtrArray.h>

//  This is the base class for functional objects


enum FOcalcType {foc_Full,           // full re-calculation to all non-utility objects
                 foc_PortOnly,       // input port data may have changed
                 foc_Apply           // full recalc for all object types
};


enum ObjStatus {os_OK,             // all object properties appear to be OK,
                // input objects OK and available
                os_Bad,            // internal properties are invalid
                os_NotSet,         // internal properties are OK
                // input objects not set (== 0)
                os_NoData          // internal properties are OK,
                // input objects are set -- input object status != os_OK
};


class DataObjC;                         // ref only

class FuncObjC;                         // forward decl

//  used within derived FuncObjC to refer to other FO used as input
class FuncObjRef{
public:
  FuncObjC*   objRef;     //  the FO
  int         objIndx;    //  the index into GetOutPortData
public:
  FuncObjRef(FuncObjC* inObj){objRef = inObj; objIndx = 0;}
  FuncObjRef(FuncObjC* inObj, int inIndx){objRef = inObj; objIndx = inIndx;}
  FuncObjRef(FuncObjC& inObj){objRef = &inObj; objIndx = 0;}
  FuncObjRef(FuncObjC& inObj, int inIndx){objRef = &inObj; objIndx = inIndx;}
  FuncObjRef(const FuncObjRef& a){objRef = a.objRef; objIndx = a.objIndx;}
  FuncObjRef(){objRef = 0; objIndx = 0;}
  ~FuncObjRef(){}
  //  copy operator
  FuncObjRef&     operator= (const FuncObjRef& a);

  bool            operator== (const FuncObjRef& a) const {return (objRef == a.objRef) && (objIndx == a.objIndx);}
  bool                    IsNull() const {return objRef == 0;}

  void            Clear() {objRef = 0; objIndx = 0;}

  DataObjC*       GetData(const std::type_info& reqSig) const;
  bool            CheckAndGetData(const std::type_info& reqSig,
                                  DataObjC*&       dataDO) const;

  // uses static list in FuncObj
  void            SetDefault(const std::type_info& reqSig);
};


typedef T_SC_Array<FuncObjRef> FuncObjRefArray;


//  used to construct inPort list for delete & default processing
class FuncObjInPort{
public:
  FuncObjRef*                 objRef;             //  ptr to the FO ref
  const std::type_info*    objDOtype;          //  type of DO required from FO
  bool*               masterSlaveFlag;    //  for master/slave type output
  //  if set then
  //      if *msFlag == false then
  //          input is required and ref is checked
  //   else
  //          input is not required but ref is checked for deletion
  bool*               useFlag;            //  for use flag type output
  //  if set then
  //      if *useFlag == true then
  //          input is required and ref is checked
  //   else
  //          input is not required but ref is checked for deletion
  // final flag for more complex cases -- combo MS and other -- defaults to true
  // pointer to flag is obtained after AddInPort, AddMSInPort or AddUseFlagInPort
  bool                checkRef;           //  if true (default)
  //          input is required and ref is checked
  //   else
  //          input is not required but ref is checked for deletion


public:
  FuncObjInPort();
  ~FuncObjInPort(){}
};

class FuncObjDefault{
public:
  FuncObjC*                   objRef;  //  ptr to the FO
  const std::type_info*    objDOtype;  //  type of DO required from FO

public:
  FuncObjDefault(){objRef = 0; objDOtype = 0;}
  ~FuncObjDefault(){}
};

class  FuncObjUpdateRecord {
public:

  const FuncObjC*       oldRef;
  const FuncObjC*       newRef;

public:
  FuncObjUpdateRecord() {oldRef = 0; newRef = 0;}
};

typedef T_SC_Array<FuncObjDefault>          FuncObjDefaultArray;
typedef T_SC_Array<FuncObjUpdateRecord> FuncObjUpdateArray;
typedef T_SC_BasicPtrArray<FuncObjC>        FuncObjArray;
typedef T_SC_BasicPtrArray<DataObjC>        DataObjArray;

class FuncObjC {
public:
  enum    {maxFOcallbacks = 8};
  enum    {objectIDLen        = 40};   // we needed more chars ....

private:
  char                objType[objectIDLen];        // objType - set at construction

public:
  //  classes used in building execution tree
  //  classes have to be public because of HP aCC error with nested classes
  class ObjBranch {
  public :
    FuncObjC*   branchObj;
    ObjBranch*  nextBranch;

    ObjBranch() {branchObj = 0; nextBranch = 0;}
    ~ObjBranch() {}
  };

  class ObjStem {
  public :
    FuncObjC*   stemObj;
    ObjBranch*  firstBranch;
    ObjStem*    nextStem;
    bool        objDone;

    ObjStem()   {stemObj = 0; firstBranch = 0; nextStem = 0; objDone = false;}
    ~ObjStem();

    bool        CalcTree(FOcalcType currCalc);
    void        CleanupTree();
  };

protected:
  static ObjStem* downStreamObj;
  ObjStatus       objStatus;
  char            objStatusMsg[stdMsgStrLen];

  //  available output data types - used to select input to other FO
  DataObjArray                                outPorts;

  //      input objects
  T_SC_Array<FuncObjInPort>   inPorts;

public:
  char             objID[objectIDLen];          // object name/ID
  bool             objIDisDefault;                 // true if SetID has not been called

  //  for use by derived types if further processing is required after a calc
  //  specifically to support replotting
  bool             objWasCalculated;

  //  list of all FO
  static FuncObjArray funcObjList;     // array of all objects

  //  func obj ref update access is required by several non-class routines
  static FuncObjUpdateArray   updateData;

  //  debug print switch set by app - default false
  static bool                 printTreeExec;

private:
  // static list of defaults
  static FuncObjDefaultArray  defaultFO;

  // static list of recalc callbacks
  static int              nFOcallback;
  static void             (*recalcCB[maxFOcallbacks]) (FuncObjC*);

  //  update error
  static bool             updateError;  // set to true if no match on update

  //  prolific info msg switch set during full calc
  friend class ObjStem;
  static bool             printCalcMessages;

public:

  // constructor/destructor
  FuncObjC(const char* dType); // normal initializer - sets objType
  FuncObjC(const FuncObjC& a); // copy initializer
  virtual             ~FuncObjC();

  //  copy operator
  FuncObjC&           operator= (const FuncObjC& a);

  // status checking routines
  virtual void        DoStatusChk();
  ObjStatus           GetStatus()     const   {return objStatus;}
  const char*         GetStatusMsg()  const   {return objStatusMsg;}
  bool                StatusOK()      const   {return objStatus == os_OK;}
  bool                StatusNotOK()   const   {return objStatus != os_OK;}

  // protected member access
  const char*         GetID()                 const   {return objID; }
  const char*         GetType()               const   {return objType; }
  void                SetID(const char* newID);               // sets ID; sets default flag to false
  void                SetDefaultID(const char* newID);        // sets ID if default flag is true
  void                SetDefaultFileID(const char* inFname);  // sets default ID to root part of fname

  //  input and output port setup used in derived object constructor only
  //  adds objects to lists

  void                AddInPort(FuncObjRef&      inPort,
                                const std::type_info& reqSig);
  //  methods to set to default are in FuncObjRef
  void                AddMSInPort(FuncObjRef&      inPort,
                                  const std::type_info& reqSig,
                                  bool&            msFlagAddr);
  void                AddUseFlagInPort(FuncObjRef&      inPort,
                                       const std::type_info& reqSig,
                                       bool&            useFlag);
  bool*               GetLastInPortCheckRef();  // returns pointer to checkRef

  void                AddOutPort(DataObjC&  inPort);

  //  default FO types are used in initialization of FO inports
  static void         AddDefaultFO(FuncObjC*        defFO,
                                   const std::type_info& reqSig);
  static void         ClearDefaultFO();  // clears count and data

  static void         SetDefaultFO(FuncObjRef&      inputFO,
                                   const std::type_info& reqInputDOtype);

  //  output data type checking and access
  // base clas version is usually OK
  // only overridden for derived type in cases where
  // where access to base type DO's is wanted
  // this is done internally for DO_Label and DO_DataIndxArray

  virtual int         HasOutPort(const std::type_info& reqSig);
  // returns number of objects matching type

  virtual DataObjC*   GetOutPortData(const std::type_info& reqSig, int indx);
  // returns data object #indx of type
  // base class version ignores indx

  //  cleanup internal data on other obj deletion - base case usually OK
  //  main task is to reset inPorts to 0 or to default
  virtual void        IsBeingDeleted(FuncObjC* delObj);

  //  object recalculation
  virtual bool        UsesAsInput(const FuncObjC* inputObj) const; // used in building calc tree
  // base case OK except when input FO
  // use is calc dependent
  virtual void        CalcOutput(FOcalcType  calcType) = 0;  // performs calculation
  void                Execute(); // alloc traps on CalcOutput(foc_Apply);}

  // list of all objects using this as input
  void                UsedByObjects(FuncObjArray& usedBy) const;

  //  tree recalculation and deletion support
  bool                DownStreamObjectsExist() const;         // ret true if any objects
  // depend on current obj

  virtual bool        IsInUse() const;
  // default returns DownStreamObjectsExist();
  // override in PlotObjBase to add plotted

  bool                CalcDownStreamObjects();               // recalculates all objects
  // dependent on this using foc_PortOnly
  //  object deletion support -- two phases
  //  build before deletion, recalc after deletion
  void                BuildDownStreamTree();                 // builds dependency tree for this
  // tree details are hidden internally
  static bool         RecalcDownStreamTree();                // recalculates dependent objects

  //  full calc after file load -- partial calc after page delete
  static bool         CalcAllObjects(FOcalcType calcType);  // recalculates all objects

  //  use after page copy/delete
  static bool         CalcSelectedObjects(FOcalcType      calcType,
                                          FuncObjArray&   selObjects);  // recalculates all in list and dependents

  static void         AddRecalcCallback(void (*recalcCBproc)(FuncObjC*));  // for app notification on CB

  //  pointer update after file read or page/block copy
  static void         ClearFuncObjRefErrors() {updateError = false;}
  static bool         FuncObjErrorFound()     {return updateError;}

  // virtual added to allow composite plot to update subplots
  virtual void        UpdateFuncObjRefs();
  static void                     UpdateAll();                    // std post-read procedure
  // does update and status chk


  bool                CheckInputFO(FuncObjRef&      inputFO,  // common status msg stuff
                                   const char       FOdesc[]);
  void                SetObjErrMsg(const char* objMsg);
  void                SetObjNoDataMsg(const char* objMsg);
  void                SetObjNotSetMsg(const char* objMsg);
  bool                CheckFileExists(const char* inFile);

  // convenience functions -- sets mViewMsg iff calcType == foc_Apply
  void                ApplyWarningMsg(FOcalcType      calcType,
                                      const char*     warnMsg);
  void                ApplyInfoMsg(FOcalcType         calcType,
                                   const char*        infoMsg);

  DataObjC*           GetInPortData(FuncObjRef&    inputFO);  // gets data obj
  // associated with input
protected:
  void                FullCopy(const FuncObjC& a);      // called by copy in derived class
  void                ChangeType(const char* newType);  // for overriding ctor set type
  static void         PerformUpdate(FuncObjRef&  refToUpdate);

private:
  static void         ClearWasCalculated();
  void                InitCommon(const char* dType);  // common to normal and copy constructor
  void                LocalCopy(const FuncObjC& a);   // common to copy constructor and = operator

  //      for app specific outport matching -- must be implemented in each App
  static int          AppHasOutPort(const DataObjArray&        objOutPorts,
                                    const std::type_info&      reqSig);

  // returns number of objects matching type

  static DataObjC*    AppGetOutPortData(const DataObjArray&    objOutPorts,
                                        const std::type_info&  reqSig,
                                        int                    indx);





};



#endif // C_FUNCOBJ_H

