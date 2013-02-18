///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ExecuteObjects.cpp
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
// DESCRIPTION:
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <objDFO/DFO_ExecuteObjects.h>

DFO_ExecuteObjects::ObjectData::ObjectData() 
{
    executeObject   = false;
    execDownstream  = detNone;
    statusMustBeOK  = true;
    stopOnError     = true;
}
DFO_ExecuteObjects::ObjectData::ObjectData(const ObjectData& a)
{
    LocalCopy(a);
}
DFO_ExecuteObjects::ObjectData& DFO_ExecuteObjects::ObjectData::operator= (const ObjectData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void DFO_ExecuteObjects::ObjectData::LocalCopy(const ObjectData& a)
{
    executeObject   = a.executeObject;
    execObjectRef   = a.execObjectRef;
    execDownstream  = a.execDownstream;
    stopOnError     = a.stopOnError;
    statusMustBeOK  = a.statusMustBeOK;
}

DFO_ExecuteObjects :: DFO_ExecuteObjects()
    : FuncObjC("Execute Objects")
{
    useUpstreamTrigger = false;
    InitCommon();
}

DFO_ExecuteObjects::DFO_ExecuteObjects(const DFO_ExecuteObjects& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_ExecuteObjects :: ~DFO_ExecuteObjects ()
{
}

void DFO_ExecuteObjects::InitCommon()
{
    DoStatusChk();
}

DFO_ExecuteObjects& DFO_ExecuteObjects::operator= (const DFO_ExecuteObjects& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

bool DFO_ExecuteObjects::UsesAsInput(const FuncObjC* inputObj) const
{
    return useUpstreamTrigger && (upstreamTriggerObjRef.objRef == inputObj); 
}

void  DFO_ExecuteObjects::LocalCopy(const DFO_ExecuteObjects& a)
{
    useUpstreamTrigger = a.useUpstreamTrigger;
    upstreamTriggerObjRef = a.upstreamTriggerObjRef;
    for (int i = 0; i < maxExecObject; i++)
        objectData[i] = a.objectData[i];
}

void DFO_ExecuteObjects::IsBeingDeleted(FuncObjC* delObj)
{
    if (upstreamTriggerObjRef.objRef == delObj)
        upstreamTriggerObjRef = FuncObjRef();
    for (int i = 0; i < maxExecObject; i++)
        if (objectData[i].execObjectRef.objRef == delObj)
            objectData[i].execObjectRef = FuncObjRef();
}

void DFO_ExecuteObjects::UpdateFuncObjRefs()
{
    PerformUpdate(upstreamTriggerObjRef);
    for (int i = 0; i < maxExecObject; i++)
        PerformUpdate(objectData[i].execObjectRef);
}


void DFO_ExecuteObjects::SetDefault()
{
    useUpstreamTrigger = false;
    for (int i = 0; i < maxExecObject; i++)
        objectData[i].executeObject = false;
}

void  DFO_ExecuteObjects:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    // check input objects
    nobjectExec = 0;
    for (int i = 0; i < maxExecObject; i++)
    {
        ObjectData& currObj = objectData[i];
        if (currObj.executeObject)
        {
            currObj.executeObjectFO = currObj.execObjectRef.objRef;
            if (currObj.executeObjectFO == 0)
            {
                SetObjErrMsg("Object to execute not set");
                return;
            }
            currObj.executeObjectFO->DoStatusChk();
            if (currObj.statusMustBeOK && currObj.executeObjectFO->StatusNotOK())
            {
                SetObjErrMsg("Object to execute not OK");
                return;
            }
            if (useUpstreamTrigger && (currObj.execDownstream != ObjectData::detNone))
            {
                SetObjErrMsg("Cannot use upstream trigger and downstream exec");
                return;
            }
            nobjectExec++;
        }
    }

    if (nobjectExec == 0)
    {
        SetObjErrMsg("no objects set for execution");
        return;
    }
}


void DFO_ExecuteObjects:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    // no execute on load or port unless trigger
    if (((calcType == foc_Full) || (calcType == foc_PortOnly)) && (!useUpstreamTrigger))
        return;

    bool calcOK = true;

    FuncObjArray atEndExecObjs(maxExecObject);
    atEndExecObjs.SetEmpty();

    for (int i = 0; i < maxExecObject; i++)
    {
        ObjectData& currObj = objectData[i];
        if (currObj.executeObject)
        {
            char calcMsg[80];
            MakeString(calcMsg, "Calculating: ", currObj.executeObjectFO->GetID(), 80);
            GenAppInfoMsg("ExecObj", calcMsg);
            currObj.executeObjectFO->Execute();
            if (currObj.executeObjectFO->StatusNotOK())
            {
                calcOK = false;
                if (currObj.stopOnError)
                    break;
            }
            else
            {
                if (currObj.execDownstream == ObjectData::detImmediate)
                    currObj.executeObjectFO->CalcDownStreamObjects();
                else if (currObj.execDownstream == ObjectData::detAtEnd)
                    atEndExecObjs += currObj.executeObjectFO;
            }
        }
    }

    if (calcOK)
    {
        if (atEndExecObjs.IsNotEmpty())
        {
            FuncObjC::CalcSelectedObjects(foc_PortOnly, atEndExecObjs);
        }
        GenAppInfoMsg("ExecObj", "All objects executed OK");
    }
}


