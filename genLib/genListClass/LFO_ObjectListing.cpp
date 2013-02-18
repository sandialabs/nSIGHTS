///////////////////////////////////////////////////////////////////////////////////
//
// LFO_ObjectListing.cpp
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
//      creates listing of defined FuncObjC connections.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include <genApp/C_MenuPage.h>
#include <genApp/C_MenuRoot.h>

#include <genListClass/LFO_ObjectListing.h>


LFO_ObjectListing::LFO_ObjectListing(const char* inID)
    :ListObjC(inID)
{
}

void LFO_ObjectListing::PrepareVars()
{
    int nroot = MenuRootC::rootList.Size();

    oneRoot =  nroot == 1;
    nobj = funcObjList.Size();

    pageObjects.Alloc(nobj);
    objNameIndexes.AllocAndFill(nobj, -1);
    pageObjectNames.SetStringLen(objStrLen);
    pageObjectNames.Alloc(nobj);

    nextPageFO = 0;
    for (int i = 0; i <  nroot; i++)
    {
        MenuRootC& currRoot = *(MenuRootC::rootList[i]);
        rootID = currRoot.shortID;
        for (int j = 0; j <  currRoot.pageLeafs.Size(); j++)
            AddPageToList(*(currRoot.pageLeafs[j]));
    }

    systemObjectNames.SetStringLen(objStrLen);
    systemObjectNames.Alloc(nobj - nextPageFO);
    for (int i = 0; i <  nobj; i++)
        if (objNameIndexes[i] < 0)
        {
            char tempStr[objStrLen];
            MakeString(tempStr, "System::", funcObjList[i]->GetID(), objStrLen);
            ConcatString(tempStr, " #", objStrLen);
            ConcatInt(i, tempStr, objStrLen);
            systemObjectNames += tempStr;
            objNameIndexes[i] = -systemObjectNames.Size();
        }
}

void LFO_ObjectListing::AddPageToList(const MenuPageC& currPage)
{
    FuncObjArray pageFO;
    currPage.GetPageFO(pageFO);
    int npageObj = pageFO.Size();

    for (int k = 0; k < npageObj; k++)
    {
        FuncObjC& currObj = *(pageFO[k]);
        const char* colorStr = 0;
        switch (currObj.GetStatus()) {
            case os_OK : {
                break;
            }
            case os_Bad : {
                colorStr =  "<FONT COLOR=RED>";
                break;
            }
            case os_NotSet : {
                colorStr =  "<FONT COLOR=BLUE>";
                break;
            }
            default : {
                colorStr =  "<FONT COLOR=GREEN>";
                break;
            }
        }

        char tempStr[objStrLen];
        MakeString(tempStr, colorStr, currObj.GetID(), "::", currObj.GetType(), "::", currPage.pageID, objStrLen);
        if (!oneRoot)
        {
            ConcatString(tempStr, "::", objStrLen);
            ConcatString(tempStr, rootID, objStrLen);
        }

        int foIndex = funcObjList.FoundAt(&currObj);
        // ConcatString(tempStr, " #", objStrLen);
        // ConcatInt(foIndex, tempStr, objStrLen);

        if (colorStr != 0)
            ConcatString(tempStr, "</FONT>", objStrLen);

        pageObjectNames += tempStr;
        pageObjects += &currObj;

        objNameIndexes[foIndex] = nextPageFO;
        nextPageFO++;
    }

    for (int i = 0; i < currPage.childPages.Size(); i++)
        AddPageToList(*(currPage.childPages[i]));
}


const char* LFO_ObjectListing::GetObjString(int foIndex)
{
    int index = objNameIndexes[foIndex];
    if (index < 0)
    {
        index = -index;
        index--;
        return systemObjectNames[index];
    }

    return pageObjectNames[index];
}

bool LFO_ObjectListing::ListConnections(FuncObjC* objToList,
                                        bool listNone,
                                        const char* listTitle)
{

    bool inputFound = false;
    int i;
    for (i = 0; i < nobj; i++)
        if (objToList->UsesAsInput(funcObjList[i]))
        {
            if (!inputFound)
            {
                AddHTML("<strong>", listTitle, "</strong>\n");
                AddLine("<em>Input From</em>", 5);
                inputFound = true;
            }
            AddLine(GetObjString(i), 10);
        }

    bool outputFound = false;
    for (i = 0; i < nobj; i++)
        if (funcObjList[i]->UsesAsInput(objToList))
        {
            if (!outputFound)
            {
                if (!inputFound)
                {
                    AddHTML("<strong>", listTitle, "</strong>\n");
                    AddLine("<U>no input</U>", 5);
                }
                AddLine("<em>Output To</em>", 5);
                outputFound = true;
            }
            AddLine(GetObjString(i), 10);
        }

    if (!(outputFound || inputFound))
    {
        if (!listNone)
            return false;

        AddLine(listTitle, 0);
        AddLine("<U>no input or output</U>", 5);
    }
    else
        if (!outputFound)
            AddLine("<U>no output</U>", 5);

    return true;
}


void LFO_ObjectListing::SetBaseFont()
{
    AddLine("<basefont face=\"Arial\" size=\"5\">");
}


LFO_ListOneObject::LFO_ListOneObject(FuncObjC*   inObj)
    : LFO_ObjectListing("SingleObj"), objToList(inObj)
{
}


void  LFO_ListOneObject::CreateListing()
{
    PrepareVars();
    SetBaseFont();
    int foIndex = funcObjList.FoundAt(objToList);

    ListConnections(objToList, true, GetObjString(foIndex));
};

LFO_ListAllObjects::LFO_ListAllObjects()
    : LFO_ObjectListing("AllObj")
{
}

void  LFO_ListAllObjects::CreateListing()
{
    PrepareVars();
    SetBaseFont();

    FuncObjC*       objToList;

    bool connFound = false;
    // page listing first
    for (int i = 0; i < pageObjects.Size(); i++)
    {
        objToList = pageObjects[i];
        int foIndex = funcObjList.FoundAt(objToList);
        if (ListConnections(objToList, false, GetObjString(foIndex)))
            connFound = true;
    }

    // system objects
    // ignore last two -- they are this and modal listing
    nobj -= 2;
    for (int i = 0; i < nobj; i++)
        if (objNameIndexes[i] < 0)
        {
            if (ListConnections(funcObjList[i], false, GetObjString(i)))
                connFound = true;
        }

    if (!connFound)
        AddLine("All objects unconnected ", 5);


};

