///////////////////////////////////////////////////////////////////////////////////
//
// PPO_PlotFolderPg.cpp
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>
#include <genApp/C_MenuPage.h>
#include <genApp/AppConfigFile.h>

#include "PPO_PlotFolderPgPS.h"
#include "PPO_PlotFolderPg.h"

const char* PPO_PlotFolderPg::objType = "PlotPgOps";
const char* PPO_PlotFolderPg::objLongDesc = "Plot Folder";
const char* PPO_PlotFolderPg::allocID = "PPO_PlotFolderPg";

PPO_PlotFolderPg :: PPO_PlotFolderPg(char* oID) :
    MenuObjC(objID, objType, objLongDesc, oID)
{
    AutoObjectSetup(allocID);
}

PPO_PlotFolderPg :: PPO_PlotFolderPg(const PPO_PlotFolderPg& a) :
    MenuObjC(objID, objType, objLongDesc, a.objID)
{
    AutoObjectSetup(allocID);
}

PPO_PlotFolderPg& PPO_PlotFolderPg::operator= (const PPO_PlotFolderPg& a)
{
    return *this;
}


bool  PPO_PlotFolderPg::ReadFromFile()
{
    ReadDefaultObjectHeader();

    CheckForInputFailure("Reading PPO_PlotFolderPg");
    return true;
}


void PPO_PlotFolderPg::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
}


PPO_PlotFolderPgPS& PPO_PlotFolderPg::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_PlotFolderPgPS&>(*assocMenu);
}


void PPO_PlotFolderPg::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_PlotFolderPg::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_PlotFolderPgPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(false, false);
    PropUpdate();
}


void PPO_PlotFolderPg::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();

    // special case for PlotFolderPg
    MenuPageC::currPageObj->UpdatePage();
}

