///////////////////////////////////////////////////////////////////////////////////
//
// C_ListPage.cpp
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
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genListClass/C_ListObj.h>

#include <genListClassPS/C_ListDefPS.h>

#include <genApp/C_MenuObj.h>

#include <genApp/C_ListPage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



ListPageC::ListPageC()
    : MenuPageC("List")
{
    assocList = 0;
}

ListPageC::~ListPageC()
{
    //  delete all attached objects dependent on assoc list
    // usually goes faster from last to 1st
    int nobj = pageObjects.UpperBound();

    // clear page objects - prevents problems in tree draw
    pageObjects.SetEmpty();
    for (int i = nobj; i >= 0; i--)
        {
            delete pageObjects[i];
        }

    // OK to delete list page now all objects are gone
    delete assocList;
}

void ListPageC::UpdatePage()
{
    MenuPageC::UpdatePage();
    if (!pageObjects.IsEmpty())
        assocList->SetID(pageID);
}


void ListPageC::ResetChildren()
{
    if (pageObjects.Size() != assocList->connectedListObj.Size())
        GenAppInternalError("ListPageC::ResetChildren()-1");

    bool doReset = false;
    for (int i = 0; i < pageObjects.Size(); i++)
        {
            ListObjC* treeObj = dynamic_cast<ListObjC*>(pageObjects[i]->objFunction);
            if (treeObj == 0)
                GenAppInternalError("ListPageC::ResetChildren()-1");
            ListObjC* currObj = assocList->connectedListObj[i];
            doReset = doReset || (treeObj != currObj);
            assocList->connectedListObj[i] = treeObj;
        }

    if (doReset)
        assocList->Execute();

}


const char* ListPageAlloc::pageTypeDesc = "PG_List";

MenuPageC*  ListPageAlloc::CreatePage() const
{
    ListPageC* newPage = new ListPageC();

    ListDefCPS* newList = new ListDefCPS(false);
    newPage->assocList = newList;
    newPage->assocWindow = newList;

    return newPage;
}

const char* ListPageAlloc::GetPageTypeName() const
{
    return pageTypeDesc;
}

