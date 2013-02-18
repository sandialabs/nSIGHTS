///////////////////////////////////////////////////////////////////////////////////
//
// AppCopyPaste.cpp
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
//              uses object serialization to/from the Windows clipboard to provide
//              copy, paste, and duplicate support for individual or groups of objects and pages.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Msg.h>

#include <genClass/C_FuncObj.h>
#include <genPlotClass/C_PlotDef.h>

#include <genApp/C_ExposedObj.h>
#include <genApp/C_MenuObj.h>
#include <genApp/C_MenuPage.h>
#include <genApp/C_MenuRoot.h>
#include <genApp/C_AppMenu.h>
#include <genApp/C_AllocObj.h>
#include <genApp/C_AllocPage.h>

#include <genApp/AppConfigFile.h>

#include "AppCopyPaste.h"

// ------------------------------------------  file header


static int GetAppID()
{
    //      return int(AfxGetThread()->m_nThreadID);
    return int(AfxGetMainWnd()->m_hWnd);
}

static void WriteHeader(const char*  fileID)
{
    WriteFileHeader(fileID, 0, 0);
    WriteInt(GetAppID());
    WriteLine();
}

static bool isCurrentApp;

static void ReadHeader(const char*  fileID, const char*  typeID)
{
    int majVer, minVer;
    ReadFileHeader(fileID, majVer, minVer, typeID);

    if ((majVer != 0) || (minVer != 0))
        InputError("Bad version #");

    isCurrentApp = (ReadInt() == GetAppID());
}


static void UpdateAppObjects()
{
    if (!isCurrentApp)
        return;

    for (int i = 0; i < FuncObjC::funcObjList.Size(); i++)
        {
            FuncObjC* currObj = FuncObjC::funcObjList[i];
            for (int j = 0; j < FuncObjC::updateData.Size(); j++)
                {
                    FuncObjUpdateRecord& currRec  = FuncObjC::updateData[j];
                    if ((currRec.oldRef == currObj) &&  (currRec.newRef == 0))
                        currRec.newRef = currObj;
                }
        }
}


// page ops

static const char*  pageFileID  = "AppDev Page Data Clipboard Configuration";

static void WritePageHeader()
{
    WriteHeader(pageFileID);
}


static void ReadPageHeader()
{
    ReadHeader(pageFileID, "AppDev page data configuration");
}

// all for page recursion support

static void UpdatePagePointers(MenuPageC& currPage)
{
    // make sure page isn't selected and is deleteable
    currPage.pageSelected = false;
    currPage.pageDeleteable = true;

    for (int i = 0; i < currPage.pageObjects.Size(); i++)
        {
            MenuObjC& currObj = currPage.pageObjects.GetRef(i);
            if (currObj.objFunction != 0)
                {
                    // update refs
                    currObj.objFunction->UpdateFuncObjRefs();
                    // all objects except calc & plot pages main objects are deleteable
                    const char* objName = currObj.GetAllocName();
                    objName++; // start at second char
                    if (strncmp(objName, "PD", 2) != 0)
                        currObj.objDeleteable = true;
                }
        }

    for (int i = 0; i < currPage.childPages.Size(); i++)
        UpdatePagePointers(currPage.childPages.GetRef(i));
}

static void UpdatePageStatus(MenuPageC& currPage)
{
    for (int i = 0; i < currPage.pageObjects.Size(); i++)
        {
            MenuObjC& currObj = currPage.pageObjects.GetRef(i);
            if (currObj.objFunction != 0)
                currObj.objFunction->DoStatusChk();
            currObj.UpdateTreeData();
        }

    for (int i = 0; i < currPage.childPages.Size(); i++)
        UpdatePageStatus(currPage.childPages.GetRef(i));
}

static void GetCalcObjects(const MenuPageC&  currPage,
                           FuncObjArray&  objsToCalc)
{
    for (int i = 0; i < currPage.pageObjects.Size(); i++)
        {
            MenuObjC& currObj = currPage.pageObjects.GetRef(i);
            if (currObj.objFunction != 0)
                objsToCalc += currObj.objFunction;
        }

    for (int i = 0; i < currPage.childPages.Size(); i++)
        GetCalcObjects(currPage.childPages.GetRef(i), objsToCalc);
}


void appCopyPaste::PastePages()
{
    bool fileReadOK = false;
    MenuPageArray pagesRead(16);
    int nObjects;
    try
        {
            appConfigFile::OpenAsClipboard(true);

            //  header
            ReadPageHeader();

            int nPages   = ReadInt();
            int ntotalPages = ReadInt();
            nObjects = ReadInt();

            if (nPages == 0)
                InputError("No pages ??");

            // check that all page types are supported
            char tempStr[160];
            for (int i = 0; i < ntotalPages; i++)
                {
                    ReadText(tempStr, 160);
                    if (!AllocPageC::MenuPageExists(tempStr))
                        InputError("Page type not supported by application");
                }

            // and all object types
            for (int i = 0; i < nObjects; i++)
                {
                    ReadText(tempStr, 160);
                    if (!AllocObjC::MenuObjectExists(tempStr))
                        InputError("Object type not supported by application");
                }

            AppMenuC::GetApp().PrePasteSetup();

            // read app globals
            ReadAppGlobals();

            // read pages
            pagesRead.Alloc(nPages);

            for (int i = 0; i < nPages; i++)
                {
                    ReadText(tempStr, 160);

                    //  convert to page type
                    if (!AllocPageC::MenuPageExists(tempStr))
                        InputError("Page type conversion error ??");

                    //  create new page
                    MenuPageC* newPage = AllocPageC::CreateMenuPage(tempStr);

                    bool childOK = (MenuPageC::currPageObj == 0) ||
                        MenuPageC::currPageObj->AcceptAsChild(*newPage);

                    // test to see if it can be added as child of current page
                    if (childOK && newPage->AcceptAsParent(MenuPageC::currPageObj))
                        {
                            // parent OK -- finish up
                            newPage->ParentChanging(MenuPageC::currPageObj);
                            if (MenuPageC::currPageObj != 0)
                                {
                                    MenuPageC::currPageObj->childPages += newPage;
                                    MenuPageC::currPageObj->ResetChildren();
                                }
                        }
                    else if (!newPage->AcceptAsParent(NULL))
                        {
                            delete newPage;
                            InputError("Page type cannot be pasted");
                        }

                    // add it to list
                    pagesRead.AddTo(newPage);

                    // read it
                    newPage->ReadFromFile();
                }

            AppMenuC::GetApp().PostPasteFixup();

            fileReadOK = true;
        }
    catch (TextC::TextError re) {
        GenAppErrorMsg("PastePages", re.errMsg);
    }

    appConfigFile::Close();

    if (!pagesRead.IsEmpty())
        {
            UpdateAppObjects();

            // update all pointers
            for (int i = 0; i < pagesRead.Size(); i++)
                UpdatePagePointers(pagesRead.GetRef(i));

            // set all FO status && update ui
            for (int i = 0; i < pagesRead.Size(); i++)
                UpdatePageStatus(pagesRead.GetRef(i));

            if (fileReadOK)
                {
                    GenAppClearMsgs();

                    FuncObjArray objsToCalc(nObjects);
                    objsToCalc.SetEmpty();
                    for (int i = 0; i < pagesRead.Size(); i++)
                        GetCalcObjects(pagesRead.GetRef(i), objsToCalc);

                    FuncObjC::CalcSelectedObjects(foc_Full, objsToCalc);
                    PlotDefC::ReplotAfterCalc();
                    ExposedObjC::UpdateAllExposed();
                }

        }

    DeallocFuncObjList();
}


bool appCopyPaste::PagePasteOK()
{
    bool pasteOK = false;
    try
        {
            appConfigFile::OpenAsClipboard(true);
            ReadPageHeader();
            pasteOK = true;
        }
    catch (TextC::TextError)
        {
        }

    appConfigFile::Close();

    return pasteOK;
}


// all for page recursion support
static int GetChildPageCount(const MenuPageC& parentPage)
{
    int childpageCount = parentPage.childPages.Size();

    for (int i = 0; i < parentPage.childPages.Size(); i++)
        childpageCount += GetChildPageCount(parentPage.childPages.GetRef(i));

    return childpageCount;
}

static int GetChildObjectCount(const MenuPageC& parentPage)
{
    int childobjCount = 0;
    for (int i = 0; i < parentPage.pageObjects.Size(); i++)
        if (parentPage.pageObjects[i]->objFunction != 0)
            childobjCount++;

    for (int i = 0; i < parentPage.childPages.Size(); i++)
        childobjCount += GetChildObjectCount(parentPage.childPages.GetRef(i));

    return childobjCount;
}

static void WritePageType(const MenuPageC& parentPage)
{
    WriteText(parentPage.GetPageAllocator()->GetPageTypeName());
    for (int i = 0; i < parentPage.childPages.Size(); i++)
        WritePageType(parentPage.childPages.GetRef(i));
}
static void WriteObjectType(const MenuPageC& parentPage)
{
    for (int i = 0; i < parentPage.pageObjects.Size(); i++)
        {
            const MenuObjC& currObj = parentPage.pageObjects.GetRef(i);
            if (currObj.objFunction != 0)
                {
                    const char* objName = currObj.GetAllocName();

                    // kluge for main plot objects -- set to DPO_PenSet which all apps should have
                    if ((strncmp(objName, "PPD", 3) == 0) || (strncmp(objName, "CPD", 3) == 0))
                        objName = "DPO_PenSet";

                    WriteText(objName);
                }
        }
    for (int i = 0; i < parentPage.childPages.Size(); i++)
        WriteObjectType(parentPage.childPages.GetRef(i));
}

void appCopyPaste::CopyPages(const MenuPageArray& pageData)
{
    if (pageData.IsEmpty())
        return;

    try
        {
            appConfigFile::OpenAsClipboard(false);
            WritePageHeader();

            // get counts
            int nPages = pageData.Size();
            int ntotalPages  = nPages;
            int ntotalObjects  = 0;
            for (int i = 0; i < nPages; i++)
                {
                    const MenuPageC& currPage = pageData.GetRef(i);
                    ntotalPages += GetChildPageCount(currPage);
                    ntotalObjects += GetChildObjectCount(currPage);
                }

            // write counts
            WriteInt(nPages);
            WriteInt(ntotalPages);
            WriteInt(ntotalObjects);
            WriteLine();

            // page types
            for (int i = 0; i < nPages; i++)
                WritePageType(pageData.GetRef(i));

            // object types
            for (int i = 0; i < nPages; i++)
                WriteObjectType(pageData.GetRef(i));

            // object list -- this has to include all objects to avoid problems on read
            WriteAppGlobals();

            // and finally pages
            for (int i = 0; i < nPages; i++)
                pageData[i]->WriteToFile();

        }
    catch (TextC::TextError re)
        {
            GenAppErrorMsg("CopyPages", re.errMsg);
        }

    appConfigFile::Close();
}

void appCopyPaste::CopyPage(MenuPageC* pageData)
{
    if ((pageData == 0) || (!pageData->IsCopyable()))
        return;
    MenuPageArray pages(1);
    pages.AddTo(pageData);
    CopyPages(pages);
}

// object ops

static const char*  objectFileID  = "AppDev Object Data Clipboard Configuration";

static void WriteObjHeader()
{
    WriteHeader(objectFileID);
}


static void ReadObjHeader()
{
    ReadHeader(objectFileID, "AppDev object data configuration");
}


void appCopyPaste::CopyObjects(const MenuObjArray& objData,
                               const AllocPageC*   objPageAllocator)
{
    if (objData.IsEmpty())
        return;

    try
        {
            appConfigFile::OpenAsClipboard(false);
            WriteObjHeader();

            WriteInt(objData.Size());
            WriteLine();

            // page type
            WriteText(objPageAllocator->GetPageTypeName());

            // object types
            int i;
            for (i = 0; i < objData.Size(); i++)
                {
                    MenuObjC& currObj =     *(objData[i]);

                    if (!currObj.IsCopyable())
                        GenAppInternalError("appCopyPaste::CopyObjects#1");
                    if (currObj.objFunction == 0)
                        GenAppInternalError("appCopyPaste::CopyObjects#2");

                    WriteText(currObj.GetAllocName());
                }

            // object list -- this has to include all objects to avoid problems on read
            WriteAppGlobals();

            // and finally objects
            for (i = 0; i < objData.Size(); i++)
                objData[i]->WriteToFile();

        }
    catch (TextC::TextError re)
        {
            GenAppErrorMsg("CopyObjects", re.errMsg);
        }

    appConfigFile::Close();
}


void appCopyPaste::CopyObject(const MenuObjC*     objData,
                              const AllocPageC*   objPageAllocator)
{
    if ((objData == 0) || (!objData->IsCopyable()))
        return;

    MenuObjArray objs(1);
    objs[0] = const_cast<MenuObjC*>(objData);
    CopyObjects(objs, objPageAllocator);
}


void appCopyPaste::PasteObjects()
{
    bool fileReadOK = false;
    MenuObjArray objectsRead;
    try
        {
            appConfigFile::OpenAsClipboard(true);

            //  header
            ReadObjHeader();

            int nObjects = ReadInt();
            if (nObjects == 0)
                InputError("No objects ??");

            // check that page type is supported
            char pageStr[160];
            ReadText(pageStr, 160);
            if (!AllocPageC::MenuPageExists(pageStr))
                InputError("Page type not supported by application");

            // and all object types
            for (int i = 0; i < nObjects; i++)
                {
                    char objStr[160];
                    ReadText(objStr, 160);
                    if (!AllocObjC::MenuObjectExists(objStr))
                        InputError("Object type not supported by application");
                    if (!MenuPageC::currPageObj->ObjectOKForPage(objStr))
                        InputError("Object not supported on current page");
                }

            // read app globals
            ReadAppGlobals();
            AppMenuC::GetApp().PrePasteSetup();


            //  read objects
            objectsRead.Alloc(nObjects);
            for (int i = 0; i < nObjects; i++)
                {
                    char objStr[160];
                    ReadText(objStr, 160);
                    MenuObjC* currObj = AllocObjC::CreateMenuObject(objStr, MenuPageC::currPageObj->GetActualParent());
                    currObj->ReadFromFile();

                    objectsRead += currObj;
                    MenuPageC::currPageObj->AddObject(currObj);
                    MenuRootC::currTreeObj->AddObjectLeafToRoot(MenuPageC::currPageObj, currObj);
                }

            AppMenuC::GetApp().PrePasteSetup();

            fileReadOK = true;
        }
    catch (TextC::TextError re) {
        GenAppErrorMsg("PasteObjects", re.errMsg);
    }

    appConfigFile::Close();

    if (!objectsRead.IsEmpty())
        {

            UpdateAppObjects();


            MenuObjC* currObj;

            // update all pointers
            int i;
            for (i = 0; i < objectsRead.Size(); i++)
                {
                    currObj = objectsRead[i];

                    // update refs
                    currObj->objFunction->UpdateFuncObjRefs();
                    currObj->objDeleteable = true;
                    currObj->objSelected = false;
                }

            // set all FO status and UI
            for (i = 0; i < objectsRead.Size(); i++)
                {

                    currObj = objectsRead[i];
                    currObj->objFunction->DoStatusChk();
                    currObj->UpdateTreeData();
                }

            if (fileReadOK)
                {
                    GenAppClearMsgs();

                    FuncObjArray objsToCalc(objectsRead.Size());
                    objsToCalc.SetEmpty();
                    for (i = 0; i < objectsRead.Size(); i++)
                        objsToCalc += objectsRead[i]->objFunction;

                    FuncObjC::CalcSelectedObjects(foc_Full, objsToCalc);

                    PlotDefC::ReplotAfterCalc();
                    ExposedObjC::UpdateAllExposed();

                    objectsRead.LastIndex()->SelectNode();
                }

        }

    DeallocFuncObjList();

}

bool appCopyPaste::ObjectPasteOK()
{
    bool pasteOK = false;
    try
        {
            appConfigFile::OpenAsClipboard(true);
            ReadObjHeader();
            pasteOK = true;
        }
    catch (TextC::TextError)
        {
        }
    appConfigFile::Close();
    return pasteOK;
}

