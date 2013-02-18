///////////////////////////////////////////////////////////////////////////////////
//
// C_FuncObj.cpp
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

#include <iostream>

#ifdef _WIN32
#include <afx.h>                        // thread stuff
#endif

#include <genClass/C_DataObj.h>
#include <genClass/C_MessageLog.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_File.h>

#include <genClass/C_FuncObj.h>

//  declare static members

#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before any objects in app
FuncObjArray            FuncObjC::funcObjList(128);

FuncObjDefaultArray     FuncObjC::defaultFO;
FuncObjC::ObjStem*      FuncObjC::downStreamObj = 0;
FuncObjUpdateArray      FuncObjC::updateData;
bool                    FuncObjC::updateError;
bool                    FuncObjC::printTreeExec = false;
bool                    FuncObjC::printCalcMessages = false;


int                     FuncObjC::nFOcallback = 0;  // number of callbacks
void                  (*FuncObjC::recalcCB[FuncObjC::maxFOcallbacks]) (FuncObjC*);

//  ******************************************************** FuncObjInPort

FuncObjInPort::FuncObjInPort()
{
    objRef      = 0;
    objDOtype   = 0;
    masterSlaveFlag = 0;
    useFlag     = 0;
    checkRef    = true;
}

//  ******************************************************** FuncObjRef

FuncObjRef& FuncObjRef::operator= (const FuncObjRef& a)
{
    if (&a != this)
        {
            objRef = a.objRef;
            objIndx = a.objIndx;
        }
    return *this;
}

DataObjC* FuncObjRef::GetData(const std::type_info& reqSig) const
{
    return objRef->GetOutPortData(reqSig, objIndx);
}

bool FuncObjRef::CheckAndGetData(const std::type_info& reqSig,
                                 DataObjC*&       dataDO) const
{
    if ( objRef == 0)
        return false;
    dataDO = GetData(reqSig);
    return dataDO->DataOK();
}


void FuncObjRef::SetDefault(const std::type_info& reqSig)
{
    FuncObjC::SetDefaultFO(*this, reqSig);
}

//  ******************************************************** FuncObj

#ifdef _WIN32

static CRITICAL_SECTION recalcCriticalSection;

#endif


FuncObjC::FuncObjC(const char* oType)
{
    InitCommon(oType);
}

FuncObjC::FuncObjC(const FuncObjC& a)
{
    InitCommon(a.objType);
    LocalCopy(a);
}

void  FuncObjC :: InitCommon(const char* oType)
{

    CopyString(objType, oType, objectIDLen);
    objIDisDefault   = true;

    SetDefaultID(oType);

    objStatus        = os_OK;
    objStatusMsg[0]  = '\0';

    inPorts.SetResizable(32);
    outPorts.SetResizable(8);

    // somewhat of a kluge -- initialization before first object created
    if (funcObjList.IsEmpty())
        {
            defaultFO.SetResizable(16);
#ifdef _WIN32
            InitializeCriticalSection(&recalcCriticalSection);
#endif
        }


    //  add to list
    funcObjList.AddTo(this);

}


FuncObjC :: ~FuncObjC ()
{
    //  go through list to inform others of pending deletion
    int i;
    for (i = 0; i < funcObjList.Size(); i++)
        if (funcObjList[i] != this)
            funcObjList.GetRef(i).IsBeingDeleted(this);

    funcObjList.DeleteFrom(this);

    // finally delete from defaults if present
    for (i = 0; i < defaultFO.Size(); i++ )
        if (defaultFO[i].objRef == this)
            defaultFO[i].objRef = 0;
}


FuncObjC& FuncObjC::operator= (const FuncObjC& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void  FuncObjC::FullCopy(const FuncObjC& a)  // called by copy in derived class
{
    LocalCopy(a);
}

void FuncObjC::LocalCopy(const FuncObjC& a)
{
    CopyString(objID, a.objID, objectIDLen);
    objIDisDefault  = a.objIDisDefault;
}

//  ******************************************************** default status check

void FuncObjC::DoStatusChk()
{
    objStatus    = os_OK;
    CopyString(objStatusMsg, "All data OK", stdMsgStrLen);
}

//  ******************************************************** data availablity
void FuncObjC::AddInPort(FuncObjRef&      inPort,
                         const std::type_info& reqSig)
{
    FuncObjInPort tmp;
    tmp.objRef = &inPort;
    tmp.objDOtype = &reqSig;

    if (inPorts.IsFull() && (!inPorts.IsResizable()))
        GenAppInternalError("FuncObjC::AddInPort after set fixed on checkref");

    inPorts += tmp;

    SetDefaultFO(inPort, reqSig);       // set to default if one exists
}

void FuncObjC::AddMSInPort(FuncObjRef&           inPort,
                           const std::type_info& reqSig,
                           bool&                 msFlagAddr)
{
    AddInPort(inPort, reqSig);
    inPorts.LastIndex().masterSlaveFlag  = & msFlagAddr;
}

void FuncObjC::AddUseFlagInPort(FuncObjRef&           inPort,
                                const std::type_info& reqSig,
                                bool&                 useFlagAddr)
{
    AddInPort(inPort, reqSig);
    inPorts.LastIndex().useFlag  = &useFlagAddr;
}

bool* FuncObjC::GetLastInPortCheckRef()
{
    inPorts.SetFixed();  // prevent any resizes which could leave pointers dangling
    return &(inPorts.LastIndex().checkRef);
}


void FuncObjC::AddOutPort(DataObjC& outPort)
{
    outPorts += &outPort;
}

int FuncObjC::HasOutPort(const std::type_info& reqSig)
{
    // slight kluge - every derived object is based on this type
    // FuncObjC is only used for DFO_ExecuteObjects
    if (reqSig == typeid(FuncObjC))
        return 1;

    return AppHasOutPort(outPorts, reqSig);
}

DataObjC* FuncObjC::GetOutPortData(const std::type_info& reqSig, int indx)
{

    return AppGetOutPortData(outPorts, reqSig, indx);
}


void FuncObjC::AddDefaultFO(FuncObjC*                defFO,
                            const std::type_info&    reqSig)
{
    // is type already present ?
    for (int i = 0; i < defaultFO.Size(); i++)
        {
            const std::type_info& defSig = *(defaultFO[i].objDOtype);
            if (defSig == reqSig)
                {
                    defaultFO[i].objRef = defFO;
                    return;
                }
        }

    FuncObjDefault temp;
    temp.objRef = defFO;
    temp.objDOtype = & reqSig;
    defaultFO += temp;
}
void FuncObjC::ClearDefaultFO()
{
    for (int i = 0; i < defaultFO.Size(); i++)
        defaultFO[i].objRef = 0;
    defaultFO.SetEmpty();
}

void  FuncObjC::SetDefaultFO(FuncObjRef&           inputFO,
                             const std::type_info& reqInputDOtype)
{
    for (int i = 0; i < defaultFO.Size(); i++ )
        if ((*defaultFO[i].objDOtype) == reqInputDOtype)
            {
                inputFO.objRef = defaultFO[i].objRef;
                inputFO.objIndx = 0;
                return;
            }
}


void  FuncObjC::IsBeingDeleted(FuncObjC* delObj)
{
    for (int i = 0; i < inPorts.Size(); i++)
        if (inPorts[i].objRef->objRef == delObj)
            {
                inPorts[i].objRef->objRef = 0;
                inPorts[i].objRef->SetDefault(*inPorts[i].objDOtype);
            }
}

//  ******************************************************** default virtual functions

bool FuncObjC::UsesAsInput(const FuncObjC* inputObj) const
{
    if (inputObj != 0)
        for (int i = 0; i < inPorts.Size(); i++)
            if ((inPorts[i].checkRef) && (inPorts[i].objRef->objRef == inputObj))
                {
                    bool checkMaster  = inPorts[i].masterSlaveFlag != 0;
                    bool checkUse     = inPorts[i].useFlag != 0;

                    // dependent on any flags ?
                    if (!(checkMaster || checkUse))
                        return true;

                    // case for both -- true only if useFlag && !masterSlave
                    if (checkMaster && checkUse && (*inPorts[i].useFlag) && (!(*inPorts[i].masterSlaveFlag)))
                        return true;

                    // master only
                    if (checkMaster && (!(*inPorts[i].masterSlaveFlag)))
                        return true;

                    // use only
                    if (checkUse && (*inPorts[i].useFlag))
                        return true;
                }

    return false;
}

void FuncObjC::UsedByObjects(FuncObjArray& usedBy) const
{
    usedBy.SetResizable(16);
    for (int i = 0; i < funcObjList.Size(); i++)
        {
            FuncObjC* currObj = funcObjList[i];
            if ((currObj != this) && currObj->UsesAsInput(this))
                usedBy += currObj;
        }
}


void FuncObjC :: SetID(const char* newID)
{
    CopyString(objID, newID, objectIDLen);
    objIDisDefault = false;
}

void FuncObjC::ChangeType(const char* newType)
{
    CopyString(objType, newType, objectIDLen);
}

void FuncObjC :: SetDefaultID(const char* newID)
{
    if (objIDisDefault)
        CopyString(objID, newID, objectIDLen);
}


void FuncObjC::SetDefaultFileID(const char* inFname)
{
    char tempStr[stdFileStrLen];
    ExtractFileName(tempStr, inFname, stdFileStrLen);
    SetDefaultID(tempStr);
}

bool  FuncObjC :: CheckInputFO(FuncObjRef&  inputFO,
                               const char   FOdesc[])
{
    const std::type_info*   reqSig = 0;
    for (int i = 0; i < inPorts.Size(); i++)
        if (inPorts[i].objRef == &inputFO)
            reqSig = inPorts[i].objDOtype;

    if (reqSig == 0)
        GenAppInternalError("FuncObjC::CheckInputFO - no match");

    bool FO_OK = false;

    char      newStatusMsg[stdMsgStrLen];
    ObjStatus newStatus = os_OK;

    CopyString(newStatusMsg, FOdesc, stdMsgStrLen);
    if (inputFO.objRef == 0)
        {
            ConcatString(newStatusMsg, " not set", stdMsgStrLen);
            newStatus = os_NotSet;
        }
    else
        if (inputFO.objRef->GetStatus() != os_OK)
            {
                ConcatString(newStatusMsg, " not OK", stdMsgStrLen);
                newStatus = os_NoData;
            }
        else
            {
                DataObjC* tmpDO = inputFO.GetData(*reqSig);
                if (tmpDO == 0)
                    {
                        ConcatString(newStatusMsg, " output bad ??", stdMsgStrLen);
                        newStatus = os_NotSet;
                    }
                else
                    if (! tmpDO->DataOK())
                        {
                            ConcatString(newStatusMsg, " - ", stdMsgStrLen);
                            ConcatString(newStatusMsg, tmpDO->objErrMsg, stdMsgStrLen);
                            // newStatus = os_NotSet;
                            newStatus = os_NoData;  // prelim change JDA 15 Oct 2000
                        }
                    else
                        FO_OK = true;
            }

    if ((objStatus == os_OK) && (newStatus != os_OK))
        {
            CopyString(objStatusMsg, newStatusMsg, stdMsgStrLen);
            objStatus = newStatus;
        }

    return FO_OK;
}

DataObjC*  FuncObjC ::GetInPortData(FuncObjRef&    inputFO)
{
    if (inputFO.objRef ==  0)
        GenAppInternalError("FuncObjC::GetInPortData - 0 data");

    const std::type_info*   reqSig = 0;
    for (int i = 0; i < inPorts.Size(); i++)
        if (inPorts[i].objRef == &inputFO)
            reqSig = inPorts[i].objDOtype;

    if (reqSig == 0)
        GenAppInternalError("FuncObjC::GetInPortData - no match");

    return inputFO.GetData(*reqSig);
}

void  FuncObjC :: SetObjErrMsg(const char* objMsg)
{
    objStatus    = os_Bad;
    CopyString(objStatusMsg, objMsg, stdMsgStrLen);

    char errType[80];
    MakeString(errType, "Tree:", GetID(), "::", GetType(), 80);

    MessageLogC::AddOtherMessage(errType, objMsg);

#ifdef ADCONSOLEAPP
    GenAppInfoMsg(errType, objMsg);
#endif
}

void  FuncObjC :: SetObjNoDataMsg(const char* objMsg)
{
    objStatus    = os_NoData;
    CopyString(objStatusMsg, objMsg, stdMsgStrLen);
}

void  FuncObjC :: SetObjNotSetMsg(const char* objMsg)
{
    objStatus    = os_NotSet;
    CopyString(objStatusMsg, objMsg, stdMsgStrLen);
}

void FuncObjC::ApplyWarningMsg(FOcalcType      calcType,
                               const char*     warnMsg)
{
    if (calcType == foc_Apply)
        GenAppWarningMsg(objType, warnMsg);
}

void FuncObjC::ApplyInfoMsg(FOcalcType      calcType,
                            const char*     infoMsg)
{
    if (calcType == foc_Apply)
        GenAppInfoMsg(objType, infoMsg);
}

bool  FuncObjC::CheckFileExists(const char* inFile)
{
    if (StringLength(inFile) == 0)
        {
            SetObjErrMsg("File name not set");
            return false;;
        }

    char fileErr[120];
    if (!PathExists(inFile))
        {
            MakeString(fileErr, "File path is bad", inFile, 120);
            SetObjErrMsg(fileErr);
            return false;
        }

    if (!FileExists(inFile))
        {
            MakeString(fileErr, "File not found:", inFile, 120);
            SetObjErrMsg(fileErr);
            return false;
        }
    return true;
}


//  for constructing usedBy tree


FuncObjC::ObjStem::~ObjStem()
{
    // cleans up branches
    ObjBranch*  nextBrObj = firstBranch;
    while ( nextBrObj != 0)
        {
            ObjBranch* currBrObj = nextBrObj;
            nextBrObj = nextBrObj-> nextBranch;
            delete currBrObj;
        }
}



bool FuncObjC::ObjStem::CalcTree(FOcalcType currCalc)
{
    do
        {
            //  search through stemList for object with all empty or null branches
            //  (i.e all required input available)
            ObjStem* nextStObj = this;
            bool objFound = false;
            bool objAvail = false;
            while ( nextStObj != 0)
                {
                    //  if not in order or already done or end of list
                    if ((nextStObj->objDone) || (nextStObj->stemObj == 0))
                        {
                            nextStObj = nextStObj->nextStem;
                            continue;
                        }
                    objAvail = true;

                    if (printTreeExec)
                        {
                            using namespace std;
                            cout << "  Checking " << nextStObj->stemObj->GetID() << endl;
                        }
                    //  search through all branches for stem
                    ObjBranch*  nextBrObj = nextStObj->firstBranch;
                    bool brFound = false;
                    while ( nextBrObj != 0)
                        {
                            // non-zero branch ?
                            if (nextBrObj->branchObj != 0)
                                {
                                    if (printTreeExec)
                                        {
                                            using namespace std;
                                            cout << "       Branch " << nextBrObj->branchObj->GetID() << endl;
                                        }
                                    brFound = true;
                                    break;
                                }
                            nextBrObj = nextBrObj-> nextBranch;
                        }

                    //  if all zero then all stem input is available
                    if ( ! brFound )
                        {
                            objFound = true;
                            break;
                        }

                    nextStObj = nextStObj->nextStem;
                }

            //  if none avail (all done) then exit loop
            if (!objAvail)
                break;

            //  error if all stems in order are non-blank only happens if input is looped
            if (!objFound)
                {
                    GenAppErrorMsg("ObjCalc", "Non-blank object not found - circular reference");
                    return false;
                }

            //  get object in temp var for convenience
            FuncObjC*  currFO = nextStObj -> stemObj;

            //  recalc object
            if (printTreeExec)
                {
                    using namespace std;
                    cout << "  Calculating " << currFO->GetType() << " " << currFO->GetID() << endl;
                }


#ifndef ADCONSOLEAPP
            if (printCalcMessages)
                {
                    char tempStr[80];
                    MakeString(tempStr, currFO->GetType(), "::", currFO->GetID(), 80);
                    GenAppProlificInfoMsg("Calculating", tempStr);
                }
#else
            if (printCalcMessages)
                {
                    char tempStr[80];
                    MakeString(tempStr, currFO->GetType(), "::", currFO->GetID(), 80);
                    GenAppInfoMsg("CALC", tempStr);
                }
#endif


            try
                {
                    currFO->CalcOutput(currCalc);
                }
            catch (SC_Array::AllocationError& err)
                {
                    GenAppErrorMsg("AllocTrap", err.errMsg);
                    currFO->SetObjErrMsg("memory allocation");
                }

            currFO->objWasCalculated = true;

            //  set object as done
            nextStObj->objDone = true;

            // go through branch list to clear any branches using as input
            nextStObj = this;
            while ( nextStObj != 0)
                {
                    ObjBranch*  nextBrObj = nextStObj->firstBranch;
                    while ( nextBrObj != 0)
                        {
                            if ( nextBrObj->branchObj == currFO)
                                nextBrObj->branchObj = 0;
                            nextBrObj = nextBrObj-> nextBranch;
                        }
                    nextStObj = nextStObj->nextStem;
                }

        } while (true);


    return true;
}


void FuncObjC::ObjStem::CleanupTree()
{
    ObjStem* nextStObj = this;
    while ( nextStObj != 0)
        {
            ObjStem* currStObj = nextStObj;
            nextStObj = nextStObj->nextStem;
            delete currStObj;
        }
}

void FuncObjC::Execute()
{
    try
        {
            CalcOutput(foc_Apply);
        }
    catch (SC_Array::AllocationError& err)
        {
            GenAppErrorMsg("Execute", err.errMsg);
            SetObjErrMsg("memory allocation");
        }
}

bool FuncObjC::CalcAllObjects(FOcalcType calcType)
{
    printCalcMessages = true;
    if (printTreeExec)
        {
            using namespace std;
            cout << "Full calc" << endl;
        }
    ClearWasCalculated();

    //  allocate first stem
    ObjStem* firstStObj = new ObjStem();

    //  keep the start point;
    ObjStem* nextStObj = firstStObj;

    //  for each available object
    int i;
    for (i = 0; i < funcObjList.Size(); i++)
        {
            FuncObjC* stem = funcObjList[i];

            if (printTreeExec)
                {
                    using namespace std;
                    cout << "    Object setup: " <<  stem->GetID() << endl;
                }
            //  add object to stem
            nextStObj->stemObj = stem;

            //  add empty branch
            nextStObj->firstBranch = new ObjBranch();
            //  pointer to branch
            ObjBranch*  nextBrObj = nextStObj->firstBranch;

            //  go through all objects to create usedBy branches
            for (int j = 0; j < funcObjList.Size(); j++)
                {
                    FuncObjC* branch = funcObjList[j];
                    //  does the stem use the potential branch as input ?
                    if (stem->UsesAsInput(branch))
                        {
                            if (printTreeExec)
                                {
                                    using namespace std;
                                    cout << "    Object uses: " <<  branch->GetID() << endl;
                                }
                            //  add the branch to the branch list
                            nextBrObj->branchObj = branch;
                            //  add a new empty branch to the end
                            nextBrObj->nextBranch = new ObjBranch();
                            //  update end of list ptr
                            nextBrObj = nextBrObj->nextBranch;
                        }
                }

            //  add a new empty stem to the end
            nextStObj->nextStem = new ObjStem();
            //  update end of list ptr
            nextStObj = nextStObj->nextStem;
        }


    bool calcOK =  firstStObj->CalcTree(calcType);
    firstStObj->CleanupTree();

    for (i = 0; i < nFOcallback; i++)
        recalcCB[i](0);

    printCalcMessages = false;

    return calcOK;;
}

bool FuncObjC::CalcSelectedObjects(FOcalcType      calcType,
                                   FuncObjArray&   selObjects)
{
    // go through sel objects to remove any dependents
    for (int i = 0; i < selObjects.Size(); i++)
        {
            for (int j = 0; j < selObjects.Size(); j++)
                {
                    if ((i != j) && (selObjects[j] != 0) && (selObjects[j]->UsesAsInput(selObjects[i])))
                        selObjects[j] = 0;
                }
        }

    if (selObjects.IsEmpty())
        return false;


    ClearWasCalculated();

    // reset to just OK objects
    int nOK = 0;
    for (int i = 0; i < selObjects.Size(); i++)
        if (selObjects[i] != 0)
            selObjects[nOK++] = selObjects[i];
    selObjects.SetSize(nOK);

    //  allocate first stem
    ObjStem* firstStObj = new ObjStem();

    //  keep the start point;
    ObjStem* nextStObj = firstStObj;

    //  add a stem for each object
    for (int i = 0; i < selObjects.Size(); i++)
        {
            FuncObjC* stem = selObjects[i];
            if (printTreeExec)
                {
                    using namespace std;
                    cout << "    Object setup: " <<  stem->GetID() << endl;
                }

            //  add object to stem
            nextStObj->stemObj = stem;

            //  add a new empty stem to the end
            nextStObj->nextStem = new ObjStem();
            //  update end of list ptr
            nextStObj = nextStObj->nextStem;
        }

    // add new stems based on dependencies
    FuncObjC* currObj = firstStObj->stemObj;
    while (currObj)
        {
            if (printTreeExec)
                {
                    using namespace std;
                    cout << "Checking object: " <<  currObj->GetID() << endl;
                }

            // see which objects use currObj as input
            for (int i = 0; i < funcObjList.Size(); i++)
                {
                    FuncObjC* fo = funcObjList[i];
                    if (fo->UsesAsInput(currObj))
                        {
                            if (printTreeExec)
                                {
                                    using namespace std;
                                    cout << "        Object used by:" << fo->GetID() << endl;
                                }

                            //  already in list ?
                            bool inList = false;
                            for (ObjStem* loopObj = firstStObj; loopObj; loopObj = loopObj->nextStem)
                                if ( loopObj->stemObj == fo)
                                    {
                                        inList = true;
                                        break;
                                    }

                            //  no, so add to list
                            if (!inList)
                                {
                                    nextStObj->stemObj = fo;

                                    //  add new empty stem
                                    nextStObj->nextStem = new ObjStem();
                                    nextStObj = nextStObj->nextStem;
                                    if (printTreeExec)
                                        {
                                            using namespace std;
                                            cout << "        Added : " << fo->GetID() << endl;
                                        }
                                }
                        }
                }

            // update done field for currObj
            ObjStem* loopObj;
            for (loopObj = firstStObj; loopObj; loopObj = loopObj->nextStem)
                if (loopObj->stemObj == currObj)
                    {
                        loopObj->objDone = true;
                        break;
                    }

            //  find next object to check
            currObj = 0;
            for (loopObj = firstStObj; loopObj; loopObj = loopObj->nextStem)
                if (!loopObj->objDone)
                    {
                        currObj = loopObj->stemObj;
                        break;
                    }
        }


    //  now add branches using only objects in list
    for (ObjStem* outerLoopObj = firstStObj; outerLoopObj; outerLoopObj = outerLoopObj->nextStem)
        {
            //  at end of list ?
            FuncObjC* stem = outerLoopObj->stemObj;
            if (stem == 0)
                break;

            //  reset object done field
            outerLoopObj->objDone = false;

            //  add empty branch
            outerLoopObj->firstBranch = new ObjBranch();
            //  pointer to branch
            ObjBranch*  nextBrObj = outerLoopObj->firstBranch;
            if (printTreeExec)
                {
                    using namespace std;
                    cout << "Building object: " <<  stem->GetID() << endl;
                }

            for (ObjStem* innerLoopObj = firstStObj; innerLoopObj; innerLoopObj = innerLoopObj->nextStem)
                {
                    FuncObjC* branch = innerLoopObj->stemObj;
                    if (stem->UsesAsInput(branch))
                        {
                            if (printTreeExec)
                                {
                                    using namespace std;
                                    cout << "        Object uses:" << branch->GetID() << endl;
                                }

                            //  add the branch to the branch list
                            nextBrObj->branchObj = branch;

                            //  add a new empty branch to the end
                            nextBrObj->nextBranch = new ObjBranch();

                            //  update end of list ptr
                            nextBrObj = nextBrObj->nextBranch;
                        }
                }
        }


    bool calcOK =  firstStObj->CalcTree(calcType);
    firstStObj->CleanupTree();

    for (int i = 0; i < nFOcallback; i++)
        recalcCB[i](0);

    return calcOK;
}



bool FuncObjC::CalcDownStreamObjects()
{
#ifdef _WIN32
    EnterCriticalSection(&recalcCriticalSection);
#endif

    if (printTreeExec)
        {
            using namespace std;
            cout << "Downstream of " << GetID() << endl;
        }

    BuildDownStreamTree();
    bool calcOK = RecalcDownStreamTree();
    if (calcOK)
        objWasCalculated = true;

    for ( int i = 0; i < nFOcallback; i++)
        recalcCB[i](this);


#ifdef _WIN32
    LeaveCriticalSection(&recalcCriticalSection);
#endif

    return calcOK;
}


bool FuncObjC::DownStreamObjectsExist() const
{
    for (int i = 0; i < funcObjList.Size(); i++)
        if (funcObjList.GetRef(i).UsesAsInput(this))
            return true;
    return false;
}

bool FuncObjC::IsInUse() const
{
    return DownStreamObjectsExist();
}

void FuncObjC::BuildDownStreamTree()
{
    //  clear all objects
    ClearWasCalculated();

    //  cleanup old tree if it exists
    if ( downStreamObj != 0)
        downStreamObj->CleanupTree();
    downStreamObj = 0;

    //  2 step process - calc first list of all objects downstream
    downStreamObj = new ObjStem();
    ObjStem* nextStObj = downStreamObj;
    FuncObjC* currObj = this;
    while (currObj)
        {
            if (printTreeExec)
                {
                    using namespace std;
                    cout << "Checking object: " <<  currObj->GetID() << endl;
                }

            // see which objects use currObj as input
            for (int i = 0; i < funcObjList.Size(); i++)
                {
                    FuncObjC* fo = funcObjList[i];
                    if (fo->UsesAsInput(currObj))
                        {
                            if (printTreeExec)
                                {
                                    using namespace std;
                                    cout << "        Object used by:" << fo->GetID() << endl;
                                }
                            //  already in list ?
                            bool inList = false;
                            for (ObjStem* loopObj = downStreamObj; loopObj; loopObj = loopObj->nextStem)
                                if ( loopObj->stemObj == fo)
                                    {
                                        inList = true;
                                        break;
                                    }

                            //  no, so add to list
                            if (!inList)
                                {
                                    nextStObj->stemObj = fo;

                                    //  add new empty stem
                                    nextStObj->nextStem = new ObjStem();
                                    nextStObj = nextStObj->nextStem;
                                    if (printTreeExec)
                                        {
                                            using namespace std;
                                            cout << "        Added : " << fo->GetID() << endl;
                                        }
                                }
                        }
                }

            // update done field for currObj
            ObjStem* loopObj;
            for (loopObj = downStreamObj; loopObj; loopObj = loopObj->nextStem)
                if ( loopObj->stemObj == currObj)
                    {
                        loopObj->objDone = true;
                        break;
                    }

            //  find next object to check
            currObj = 0;
            for (loopObj = downStreamObj; loopObj; loopObj = loopObj->nextStem)
                if ( !loopObj->objDone)
                    {
                        currObj = loopObj->stemObj;
                        break;
                    }
        }

    ObjStem* loopObj;
    //  now add branches using only downstream objects
    for (loopObj = downStreamObj; loopObj; loopObj = loopObj->nextStem)
        {
            //  at end of list ?
            FuncObjC* stem = loopObj->stemObj;
            if (stem == 0)
                break;

            //  reset object done field
            loopObj->objDone = false;

            //  add empty branch
            loopObj->firstBranch = new ObjBranch();
            //  pointer to branch
            ObjBranch*  nextBrObj = loopObj->firstBranch;
            if (printTreeExec)
                {
                    using namespace std;
                    cout << "Building object: " <<  stem->GetID() << endl;
                }

            for (ObjStem* iloopObj = downStreamObj; iloopObj; iloopObj = iloopObj->nextStem)
                {
                    FuncObjC* branch = iloopObj->stemObj;
                    if ( stem->UsesAsInput(branch))
                        {
                            if (printTreeExec)
                                {
                                    using namespace std;
                                    cout << "        Object uses:" << branch->GetID() << endl;
                                }

                            //  add the branch to the branch list
                            nextBrObj->branchObj = branch;

                            //  add a new empty branch to the end
                            nextBrObj->nextBranch = new ObjBranch();

                            //  update end of list ptr
                            nextBrObj = nextBrObj->nextBranch;
                        }
                }
        }
}


bool  FuncObjC::RecalcDownStreamTree()
{

    bool calcOK = false;
    if ( downStreamObj != 0)
        {
            calcOK = downStreamObj->CalcTree(foc_PortOnly);
            downStreamObj->CleanupTree();
            downStreamObj = 0;

            for (int i = 0; i < nFOcallback; i++)
                recalcCB[i](0);
        }

    return calcOK;
}

//  update stuff
void  FuncObjC::PerformUpdate(FuncObjRef&   refToUpdate)
{
    //  not set ??
    if (refToUpdate.objRef == 0)
        return;

    for (int i = 0; i < updateData.Size(); i++)
        if (refToUpdate.objRef == updateData[i].oldRef)
            {
                refToUpdate.objRef = const_cast<FuncObjC*> (updateData[i].newRef);
                // from outside object (on page/object paste)
                if (refToUpdate.objRef == 0)
                    refToUpdate = FuncObjRef();

                return;
            }

    //  set no match error
    updateError = true;
}

void  FuncObjC::UpdateFuncObjRefs()
{
    for (int i = 0; i < inPorts.Size(); i++)
        PerformUpdate(*inPorts[i].objRef);
}


void  FuncObjC::ClearWasCalculated()
{
    //  clear all calc flags
    for (int i = 0; i < funcObjList.Size(); i++)
        funcObjList.GetRef(i).objWasCalculated = false;
}

void FuncObjC::AddRecalcCallback(void (*recalcCBproc)(FuncObjC*))
{
    if ( nFOcallback == maxFOcallbacks)
        GenAppInternalError("FuncObjC::AddRecalcCallback");

    recalcCB[nFOcallback] = recalcCBproc;
    nFOcallback++;
}


void FuncObjC::UpdateAll()
{
    for (int i = 0; i < funcObjList.Size(); i++)
        funcObjList.GetRef(i).UpdateFuncObjRefs();
    for (int i = 0; i < funcObjList.Size(); i++)
        {
            //              GenAppInfoMsg("StatusChk", funcObjList.GetRef(i).GetID());
            funcObjList.GetRef(i).DoStatusChk();
        }
    //       GenAppInfoMsg("Debug", "UpdateAll - status OK");
}

