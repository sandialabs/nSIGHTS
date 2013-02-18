///////////////////////////////////////////////////////////////////////////////////
//
// C_PlotFolderPage.cpp
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
//              Page type which is child of plot page and contains plot objects only
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClass/U_Msg.h>
#include <genPPO/PPO_PlotFolderPg.h>

#include <genApp/C_PlotFolderPage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


PlotFolderPageC::PlotFolderPageC()
    :MenuPageC("Folder")
{
}

const MenuPageC* PlotFolderPageC::GetActualParent()const
{
    // recurses until a non folder parent is found
    return parentPage->GetActualParent();
}

void PlotFolderPageC::ResetChildren()
{
    MenuPageC* actParent = const_cast<MenuPageC*>(GetActualParent());
    if (actParent->IsPlotPage()) // should always be true
        actParent->ResetChildren();
}

bool PlotFolderPageC::AcceptAsChild(MenuPageC& newChild) const
{
    // only other plot folders
    return IsSameType(&newChild);
}

bool PlotFolderPageC::AcceptAsParent(MenuPageC* newParent) const
{
    if ((newParent == NULL) ||
        (!(newParent->IsPlotPage() || IsSameType(newParent))))
        return false;

    // if parent isn't set yet assume OK (occurs in PastePage)
    if (parentPage == 0)
        return true;

    // always OK to shift on same plot
    if (newParent->GetActualParent() == GetActualParent())
        return true;

    // otherwise objects must be OK to move and OK on plot
    return CheckObjectsOnPage(*newParent->GetActualParent(), *this);
}

bool PlotFolderPageC::CheckObjectsOnPage(const MenuPageC& newParent,
                                         const MenuPageC& currPage) const
{
    if (!currPage.IsSameType(PlotFolderPageAlloc::PageTypeName()))
        GenAppInternalError("PPO_PlotFolderPgPS::SetObjectsOnPage");

    for (int i = 1; i < currPage.pageObjects.Size(); i++)
        {
            if (!currPage.pageObjects[i]->CanDragToDifferentPage())
                return false;

            if (!newParent.ObjectOKForPage(currPage.pageObjects[i]->GetAllocName()))
                return false;
        }

    for (int i = 0; i < currPage.childPages.Size(); i++)
        if (!CheckObjectsOnPage(newParent, *currPage.childPages[i]))
            return false;

    return true;
}


void PlotFolderPageC::ParentChanging(MenuPageC* newParent)
{
    // defined & shifting  on same plot
    if ((parentPage != 0) && (newParent->GetActualParent() == GetActualParent()))
        {
            parentPage = newParent;
            ResetChildren();
            return;
        }

    if (!AcceptAsParent(newParent))
        GenAppInternalError("PlotFolderPageC::ParentChanging");

    ChangeObjectsOnPage(*const_cast<MenuPageC*>(newParent->GetActualParent()), *this);

    parentPage = newParent;
}

void PlotFolderPageC::ChangeObjectsOnPage(MenuPageC& newParent,
                                          MenuPageC& currPage) const
{
    for (int i = 1; i < currPage.pageObjects.Size(); i++)
        {
            currPage.pageObjects[i]->ChangePage(currPage, newParent);
        }

    for (int i = 0; i < currPage.childPages.Size(); i++)
        ChangeObjectsOnPage(newParent, *currPage.childPages[i]);
}

void PlotFolderPageC::ObjectPageChangeComplete(MenuPageC* oldParent) const
{
    if (oldParent->GetActualParent() != GetActualParent())
        GetActualParent()->ObjectPageChangeComplete(oldParent);
}

// allocator code

const char* PlotFolderPageAlloc::pageTypeDesc = "PG_PlotFolder";

MenuPageC*  PlotFolderPageAlloc::CreatePage() const
{
    PlotFolderPageC* newPage = new PlotFolderPageC();
    newPage->AddObject(new PPO_PlotFolderPg("Objects"));
    return newPage;
}

const char* PlotFolderPageAlloc::GetPageTypeName() const
{
    return pageTypeDesc;
}

