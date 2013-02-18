///////////////////////////////////////////////////////////////////////////////////
//
// DPO_DataPg.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              File I/O for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#ifndef ADCONSOLEAPP
#include "DPO_DataPgPS.h"
#include <genPlotClass/C_PlotDef.h>
#endif

#include <genClass/U_Str.h>
#include <genApp/C_MenuPage.h>
#include <genApp/AppConfigFile.h>

#include "DPO_DataPg.h"

const char* DPO_DataPg::objType = "DataPgDesc";
const char* DPO_DataPg::objLongDesc = "Data Page Description";
const char* DPO_DataPg::allocID = "DPO_DataPg";

DPO_DataPg :: DPO_DataPg(char* oID) :
    MenuObjC(objID, objType, objLongDesc, oID)
{
    AutoObjectSetup(allocID);
}

DPO_DataPg :: DPO_DataPg(const DPO_DataPg& a) :
    MenuObjC(objID, objType, objLongDesc, a.objID),
    dataDesc(a.dataDesc)
{
    AutoObjectSetup(allocID);
}

DPO_DataPg& DPO_DataPg::operator= (const DPO_DataPg& a)
{
    if (&a != this)
        dataDesc = a.dataDesc;
    return *this;
}


bool  DPO_DataPg::ReadFromFile()
{
    ReadDefaultObjectHeader();
    if (currentObjReadMajorVersion == 0)
        {
            char oldStr[512];
            ReadText(oldStr, 512);
            dataDesc.CreateFromString(oldStr, "\n");
        }
    else
        ReadStringArray(dataDesc);

    CheckForInputFailure("Reading DPO_DataPg");
    return true;
}


void DPO_DataPg::WriteToFile()
{
    const int objMajVer = 1;        // converted to string array
    //    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteStringArray(dataDesc);
}

#ifndef ADCONSOLEAPP

DPO_DataPgPS& DPO_DataPg::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_DataPgPS&>(*assocMenu);
}


void DPO_DataPg::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_DataPg::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_DataPgPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(false, true);
    PropUpdate();
}

void DPO_DataPg::AddObjectsOnPage(MenuPageC& currPage,
                                  FuncObjArray& pageObjs)
{
    for (int i = 0; i < currPage.pageObjects.Size(); i++)
        {
            FuncObjC* nextObj = currPage.pageObjects[i]->objFunction;
            if (nextObj)
                pageObjs += nextObj;
        }

    for (int i = 0; i < currPage.childPages.Size(); i++)
        AddObjectsOnPage(*currPage.childPages[i], pageObjs);
}

void DPO_DataPg::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();

    // special case for DataPg - do  full execute on all objects
    // get all funcobj
    FuncObjArray execObjs;
    execObjs.SetResizable(128);
    MenuPageC& currPage = *MenuPageC::currPageObj;
    AddObjectsOnPage(currPage, execObjs);

    FuncObjC::CalcSelectedObjects(foc_Full, execObjs);
    PlotDefC::ReplotAfterCalc();
    ExposedObjC::UpdateAllExposed();

    currPage.UpdatePage();
}

#endif

