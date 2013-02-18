///////////////////////////////////////////////////////////////////////////////////
//
// C_AllocObj.cpp
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              object factory for creating DPO_XXX, LPO_XXX, PPO_XXX, and
//              UPO_XXX functional objects given an object identifier.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <iostream>
#include <typeinfo>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <genApp/C_MenuObj.h>
#include <genApp/C_MenuPage.h>

#include "C_AllocObj.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before alloc objects in app

T_SC_BasicPtrArray<AllocObjC> AllocObjC::allocObjs(256);


AllocObjC::AllocObjC()
{
    objectNames[0] = 0;
    objectNames[1] = 0;
    objectNames[2] = 0;
    objectNames[3] = 0;
    objectNames[4] = 0;

    allocObjs += this;
}


MenuObjC*  AllocObjC::CreateMenuObject(const char*  objToCreate,
                                       const MenuPageC*   objPage)
{
    char errStr[80];
    //      MakeString(errStr, "CreateMenuObject: creating :", objToCreate, 80);
    //      GenAppInfoMsg("AllocObjC:CreateMenuObject", errStr);

    MenuObjC* newObject;
    for (int i = 0; i < allocObjs.Size(); i++)
        {
            AllocObjC& currObj = *(allocObjs[i]);
            if (currObj.ObjectNameMatch(objToCreate))
                {
                    newObject = currObj.CreateObject(objPage);
                    // because MS type info doesnt work like Unix ...
                    newObject->allocName = currObj.objectNames[0];
                    return newObject;
                }
        }

    MakeString(errStr, "CreateMenuObject: cannot create :", objToCreate, 80);
    GenAppInternalError(errStr);
    return 0;
}

bool AllocObjC::MenuObjectExists(const char*  objToCheck)
{
    for (int i = 0; i < allocObjs.Size(); i++)
        if (allocObjs[i]->ObjectNameMatch(objToCheck))
            return true;

    return false;
}

MenuObjC*  AllocObjC::CopyMenuObject(const MenuObjC&  objToCopy,
                                     const MenuPageC*   objPage)
{
    MenuObjC* newObject;
    for (int i = 0; i < allocObjs.Size(); i++)
        {
            AllocObjC& currObj = *(allocObjs[i]);
            if (objToCopy.allocName == currObj.objectNames[0])
                {
                    newObject = currObj.CopyObject(objToCopy, objPage);
                    newObject->allocName = currObj.objectNames[0];
                    return newObject;
                }
        }

    GenAppInternalError("AllocObjC::CopyMenuObject");
    return 0;
}



bool  AllocObjC::ObjectNameMatch(const char*  nameToMatch)
{
    for (int i = 0; i < maxObjectNames; i++)
        {
            if (objectNames[i] == 0)
                return false;
            if (strcmp(objectNames[i], nameToMatch) == 0)
                return true;
        }
    return false;
}

