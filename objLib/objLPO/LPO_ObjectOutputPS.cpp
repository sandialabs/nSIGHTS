///////////////////////////////////////////////////////////////////////////////////
//
// LPO_ObjectOutputPS.cpp
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

#include "ObjLibAfx.h"

#include <genClass/U_Str.h>

#include <genClass/DO_Listing.h>

#include "LPO_ObjectOutput.h"
#include "LPO_ObjectOutputPS.h"

LPO_ObjectOutputPS::LPO_ObjectOutputPS() :
    objectOutputObjRefUI(typeid(DO_Listing))
{
}


void LPO_ObjectOutputPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    objectOutputObjRefUI.InitFramedUI("Object Output Data To List", URform.GetWidget(), args);
}


void LPO_ObjectOutputPS::MenuOpen(LPO_ObjectOutput&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    objectOutputObjRefUI.StdMenuInit(inData.objectOutputObjRef);
}


void LPO_ObjectOutputPS::SetMenuData(LPO_ObjectOutput&    inData)
{
    MenuBaseC::SetMenuData(inData);
    objectOutputObjRefUI.SetFuncObjRef(inData.objectOutputObjRef);
}

bool LPO_ObjectOutputPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    objectOutputObjRefUI.FuncObjRefOK(allOK, "Object output data source");
    return allOK;
}

void LPO_ObjectOutputPS::GetMenuData(LPO_ObjectOutput&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.objectOutputObjRef = objectOutputObjRefUI.GetFuncObjRef();

}

