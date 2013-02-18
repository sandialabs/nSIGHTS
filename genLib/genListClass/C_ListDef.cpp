///////////////////////////////////////////////////////////////////////////////////
//
// C_ListDef.cpp
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
//      basic definition of a listing window.   Maintains lists of connected listing functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/C_TxtFile.h>

#include <genListClass/C_ListObj.h>

#include <genListClass/C_ListDef.h>

//  statics
T_SC_BasicPtrArray<ListDefC> ListDefC::listDefList;

ListObjArray ListDefC::defaultListObj;

ListDefC :: ListDefC(const char* dType) : FuncObjC(dType)
{
    InitCommon();
}

ListDefC :: ListDefC(const ListDefC& a) : FuncObjC(a)
{
    InitCommon();
    LocalCopy(a);
}

ListDefC :: ~ListDefC ()
{
    listDefList.DeleteFrom(this);
}

void ListDefC::InitCommon()
{
    displayDefaults = true;
    listDefList.AddTo(this);
}

ListDefC& ListDefC::operator= (const ListDefC& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void ListDefC::LocalCopy(const ListDefC& a)
{
}


void  ListDefC:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (connectedListObj.IsEmpty())
        SetObjErrMsg("no objects to list");
}

void ListDefC::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
    {
        SC_StringArray extraHTML(132);
        extraHTML.Alloc(20);

        extraHTML += "<head>";
        extraHTML += "<title>AppDev HTML Listing</title>";
        extraHTML += "</head>";
        extraHTML += "<body>";

        // clear
        ClearDisplay();

        SendToDisplay(extraHTML);

        ListObjArray displayList;
        displayList.SetResizable(8);
        if (displayDefaults)
        {
            // update header info
            for (int i = 0; i < defaultListObj.Size(); i++)
                defaultListObj[i]->DoListing();

            displayList += defaultListObj;
        }

        displayList += connectedListObj;

        for (int i = 0; i < displayList.Size(); i++)
        {
            ListObjC& listObj = (*displayList[i]);
            extraHTML.SetEmpty();
            if (listObj.StatusOK())
            {
                SendToDisplay(listObj.listingData);
            }
            else
            {
                char str[80];
                MakeString(str, "<pre>Error: ", listObj.GetID(), " - ", listObj.GetStatusMsg(), "</pre>", 80);
                extraHTML += str;
            }

            if (i < displayList.UpperBound())
                extraHTML += "<HR>";

            SendToDisplay(extraHTML);
        }

        extraHTML.SetEmpty();
        extraHTML += "</body>";
        SendToDisplay(extraHTML);
    }
}


bool ListDefC::UsesAsInput(const FuncObjC* inputObj) const
{
    for (int i = 0; i < connectedListObj.Size(); i++)
        if (inputObj == connectedListObj[i])
            return true;

    return false;
}


void ListDefC::ListToFile(const char* fName)
{
    try
    {
        TxtFileC listFile(fName, false);
        ListObjArray displayList(defaultListObj);
        displayList += connectedListObj;

        for (int i = 0; i < displayList.Size(); i++)
        {
            if (displayList[i]->StatusOK())
            {
                SC_StringArray& currText = displayList[i]->listingData;
                for (int j = 0; j < currText.Size(); j++)
                {
                    // trim extra new line if present
                    char tempStr[ListObjC::listingLineLen];
                    CopyString(tempStr, currText[j], ListObjC::listingLineLen);
                    char* lfPos = strchr(tempStr, '\n');
                    if (lfPos != NULL)
                        *lfPos = '\0';

                    if (IsNull(tempStr))
                        listFile.WriteLine();
                    else
                        listFile.WriteText(tempStr);
                }
                listFile.WriteLine();
            }
        }
    }
    catch (TextC::TextError& err)
    {
        GenAppErrorMsg("List", err.errMsg);
    }
}

