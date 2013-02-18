///////////////////////////////////////////////////////////////////////////////////
//
// PullMenu.cpp
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

#include <genApp/C_AllocObj.h>

#include <genApp/PullMenu.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


PullMenuC::PullMenuC()
{
    VERIFY(CreatePopupMenu());
}

PullMenuC::~PullMenuC()
{
    Detach();
}

void PullMenuC::Add(PullMenuC& child, const char* id)
{
    VERIFY(AppendMenu(MF_POPUP | MF_ENABLED, (UINT)child.m_hMenu, id));
}

void PullMenuC::Add(const char* objDesc, const char* objID)
{
    int nextIDValue = initialIDValue + idMap.Size();
    idMap += objID;

    VERIFY(AppendMenu(MF_STRING | MF_ENABLED, nextIDValue, objDesc));

    int index = GetMenuItemCount() - 1;
    // setup MENU info for last item that was justed added
    MENUITEMINFO info;
    char aName[256];

    ZeroMemory(&info, sizeof(MENUITEMINFO));
    info.cbSize = sizeof(MENUITEMINFO);
    info.wID = nextIDValue;
    info.fMask = MIIM_ID | MIIM_TYPE;
    info.dwTypeData = aName;
    info.cch = sizeof(aName);

    VERIFY(GetMenuItemInfo(index, &info, true));
    /*  We're using idMap instead the following lines
        info.fMask |= MIIM_DATA;
        info.dwItemData = (UINT)objID;
    */
    // added for mView dev -- shows which objects remain to be ported
    if (!AllocObjC::MenuObjectExists(objID))
        {
            info.fMask |= MIIM_STATE;
            info.fState = MFS_GRAYED;
        }

    // to fix NT bug
    info.fType = MFT_STRING;
    CopyString(aName, objDesc, 255);

    VERIFY(::SetMenuItemInfo(GetSafeHmenu(), index, 1, &info));
}

void PullMenuC::Add(const ObjectCascadeArray& cascade)
{
    if (cascade.Size() == 1)
        {
            Add(cascade[0].cascadeItems);
        }
    else
        {
            for (int i = 0; i < cascade.Size(); i++)
                {
                    PullMenuC child;  // dummy
                    child.Add(cascade[i].cascadeItems);
                    Add(child, cascade[i].cascadeID);
                }
        }
}

void PullMenuC::Add(const ObjectItemArray& items)
{
    for (int i = 0; i < items.Size(); i++)
        Add(items[i].objDesc, items[i].objID);
}



const char* PullMenuC::GetCBName(UINT nItem, char* pName, UINT cch)
{
    /*  idMap obsoletes the following lines
        MENUITEMINFO info;

        ZeroMemory(&info, sizeof(MENUITEMINFO));
        info.cbSize = sizeof(MENUITEMINFO);
        info.fMask = MIIM_DATA | MIIM_ID;
        VERIFY(GetMenuItemInfo(nItem, &info, false));
    */
    return idMap[nItem - initialIDValue];
}

SC_ConstStringArray PullMenuC::idMap;
int PullMenuC::initialIDValue = 0;

void PullMenuC::InitializeMapping(int startIDValue)
{
    idMap.SetResizable(32);
    initialIDValue = startIDValue;
}

