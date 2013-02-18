///////////////////////////////////////////////////////////////////////////////////
//
// nPostFile.cpp
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
// DESCRIPTION: implementation of GenLib genApp derived code
//              writes/reads nPost menu tree functional objects to/from
//              ASCII configuration file
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include <genClass/U_Msg.h>

#include <genClass/C_FuncObj.h>
#include <genPlotClass/C_PlotDef.h>

#include <genApp/AppConfigFile.h>

#include <genApp/C_MenuRoot.h>
#include <genApp/C_MenuObj.h>
#include <genApp/C_MenuPage.h>
#include <genApp/C_PlotPage.h>
#include <genApp/C_AppMenu.h>
#include <genApp/C_ExposedObj.h>

#include "nPostRoot.h"

#include "nPostFile.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ------------------------------------------  file header

static const char*  fileID  = "nSIGHTS Configuration File";
enum    {majorVersionNum = 0};
enum    {minorVersionNum = 0};

static void WriteCFGHeader()
{
    WriteFileHeader(fileID, majorVersionNum, minorVersionNum);

    WriteAppGlobals();

}


static void ReadCFGHeader()
{
    ReadFileHeader(fileID,
                   currentCfgFileReadMajorVersion,
                   currentCfgFileReadMinorVersion,
                   "nSIGHTS configuration");

    if ((currentCfgFileReadMajorVersion < 0) ||
        (minorVersionNum != currentCfgFileReadMinorVersion))
        {
            InputError("Bad version #");
        }

    ReadAppGlobals();

}


bool nPostFileUtilities::OpenFile(const char* fName)
{
    bool currPagesDeleted = false;
    bool fileReadOK = false;
    try
        {
            appConfigFile::OpenAsFile(fName, true);

            //  header
            ReadCFGHeader();

            //  delete existing
            projectRoot.ClearAll();
            currPagesDeleted = true;
            MenuPageC::currPageObj = 0;
            MenuObjC::currMenuObj = 0;

            projectRoot.ReadFromFile();

            appConfigFile::Close();

            fileReadOK = true;
        }
    catch (TextC::TextError re) {
        appConfigFile::Close();
        GenAppErrorMsg("OpenFile", re.errMsg);
    }

    if (currPagesDeleted)
        {
            FuncObjC::UpdateAll();
            projectRoot.SelectCurrentPage();

            if (fileReadOK)
                {
                    GenAppClearMsgs();
                    PlotDefC::InitializeAllPlots();
                    FuncObjC::CalcAllObjects(foc_Full);
                    PlotDefC::ReplotAfterCalc();
                    ExposedObjC::UpdateAllExposed();

                    MenuRootC::UpdateAllUI();

                    PlotPageC::MinimizeSubplots();
                }

        }

    if (MenuPageC::currPageObj != 0)
        {
            MenuPageC::currPageObj->UpdatePage();

            // no menu object selected yet (would be selected by SelectPage())
            if ((MenuObjC::currMenuObj == 0) &&
                (MenuPageC::currPageObj->pageObjects.IsNotEmpty()))
                MenuPageC::currPageObj->pageObjects[0]->PropOpenMenu();

            // update
            MenuObjC::UpdateCurrentObj();
        }
    MenuRootC::UpdateAllUI();

    DeallocFuncObjList();
    return fileReadOK;
}

void nPostFileUtilities::SaveFile(const char* fName)
{
    appConfigFile::OpenAsFile(fName, false);
    WriteCFGHeader();
    projectRoot.WriteToFile();
    appConfigFile::Close();
}

