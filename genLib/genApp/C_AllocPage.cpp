///////////////////////////////////////////////////////////////////////////////////
//
// C_AllocPage.cpp
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
//              object factory for creating new menu pages.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include <iostream>
#include <typeinfo>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <genApp/C_MenuPage.h>
#include <genApp/C_AllocObj.h>

#ifndef ADCONSOLEAPP
#include <genApp/C_AppMenu.h>
#include <genApp/C_PlotFolderPage.h>
#endif

#include "C_AllocPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before alloc objects in app

T_SC_BasicPtrArray<AllocPageC> AllocPageC::allocPages(8);

AllocPageC::AllocPageC()
{
    pageObjectIDs.SetResizable(8);
    allocPages += this;
}

#ifndef ADCONSOLEAPP

void AllocPageC::SetMenuData(const ObjectCascadeArray& inObjects)
{
    pageObjectMenu.Add(inObjects);
    for (int i = 0; i < inObjects.Size(); i++)
        {
            const ObjectItemArray&  cascadeItems = inObjects[i].cascadeItems;
            for (int j = 0; j < cascadeItems.Size(); j++)
                {
                    const char* nextID = cascadeItems[j].objID;
                    if (AllocObjC::MenuObjectExists(nextID))
                        pageObjectIDs += nextID;
                }
        }
}

bool AllocPageC::IsSameType(const char* inType) const
{
    return PageNameMatch(inType);
}

bool AllocPageC::IsSameType(const AllocPageC& inAlloc) const
{
    return &inAlloc == this;
}

bool AllocPageC::ObjectOKForPage(const char*  objID) const
{
    for (int i = 0; i < pageObjectIDs.Size(); i++)
        if (SameString(pageObjectIDs[i], objID))
            return true;
    return false;
}

bool AllocPageC::AreSameTypes(const char* pageTypeA,
                              const char* pageTypeB)
{
    AllocPageC* currPtr = GetPageAllocPtr(pageTypeA);

    // must be a page
    if (currPtr == 0)
        return false;

    return currPtr == GetPageAllocPtr(pageTypeB);
}

bool AllocPageC::AreSameTypes(const char* pageIdentifier,
                              const MenuPageC* pageData)
{
    return AreSameTypes(pageIdentifier, pageData->GetPageAllocator()->GetPageTypeName());
}
#endif // ADCONSOLEAPP

MenuPageC*  AllocPageC::CreateMenuPage(const char*  pageToCreate)
{
    AllocPageC* currPtr = GetPageAllocPtr(pageToCreate);
    if (currPtr == 0)
        GenAppInternalError("AllocPageC::CreatePageObject");

    MenuPageC* newPage = currPtr->CreatePage();
    newPage->pageAllocator = currPtr;

    return newPage;
}

AllocPageC*     AllocPageC::GetPageAllocPtr(const char*  typeToFind)
{
    for (int i = 0; i < allocPages.Size(); i++)
        if (allocPages[i]->PageNameMatch(typeToFind))
            return allocPages[i];
    return 0;
}

bool AllocPageC::MenuPageExists(const char*  pageToCheck)
{
    return GetPageAllocPtr(pageToCheck) != 0;
}

bool  AllocPageC::PageNameMatch(const char*  nameToMatch) const
{
    return SameString(nameToMatch, GetPageTypeName());
}


#ifndef ADCONSOLEAPP

bool AllocPageC::IsPlotPage(const char*  pageToCheck)
{
    AllocPageC* currPtr = GetPageAllocPtr(pageToCheck);
    return (currPtr != 0) && currPtr->IsPlotPage();
}

bool AllocPageC::AcceptAsChild(const char*  parentPageType,
                               const char*  childPageType)
{
    AllocPageC* currPtr = GetPageAllocPtr(parentPageType);
    return (currPtr != 0) && currPtr->AcceptAsChild(childPageType);
}

const PullMenuC* AllocPageC::GetObjectMenuForPage(const char*  pageTypeDesc)
{
    AllocPageC* currPtr = GetPageAllocPtr(pageTypeDesc);
    if (currPtr)
        return &(currPtr->pageObjectMenu);

    return 0;
}

bool AllocPageC::ObjectOKForPage(const char*  pageTypeDesc,
                                 const char*  objID)
{
    AllocPageC* currPtr = GetPageAllocPtr(pageTypeDesc);
    return (currPtr != 0) && currPtr->ObjectOKForPage(objID);
}

// basic protected

bool AllocPageC::IsPlotPage() const
{
    return false;
}

bool AllocPageC::AcceptAsChild(const char*  pageToCheck) const
{
    return false;
}


bool AllocPlotPage::AcceptAsChild(const char*  pageToCheck) const
{
    return AreSameTypes(PlotFolderPageAlloc::PageTypeName(), pageToCheck);
}
#endif // ADCONSOLEAPP

