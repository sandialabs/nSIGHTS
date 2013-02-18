///////////////////////////////////////////////////////////////////////////////////
//
// G_FileNames.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#ifndef ADCONSOLEAPP
#include <genPlotClass/C_PlotDef.h>
#include <genApp/C_ExposedObj.h>
#include <genApp/C_MenuRoot.h>
#endif

#include <genApp/G_FileNames.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before alloc objects in app

namespace appFileNameGlobals {

    FileNameArray appFileNameData;
    SC_StringArray  oldFileNames;
    bool writeLinux = false;
    bool changePaths = true;

    FileNameArray::FileNameArray()
    {
        SetResizable(10);
        writeLinux = false;
        changePaths = true;
    }

    void FileNameArray::AddTo(char* newFname)
    {
        (*this) += FNUpdateRec(newFname, 0);
    }

    void FileNameArray::AddTo(char* newFname,
                              FuncObjC* newFO)
    {
        (*this) += FNUpdateRec(newFname, newFO);
    }

    void FileNameArray::DeleteFrom(const char* newFname)
    {
        for (int i = 0; i < Size(); i++)
            if ((*this)[i].fName == newFname)
                {
                    DeleteItem(i);
                    return;
                }
        GenAppInternalError("FileNameArray::DeleteFrom");
    }


    void FileNameArray::GetCurrentPaths(SC_StringArray& currPaths)
    {
        currPaths.ForceMinStringLen(stdFileStrLen);
        currPaths.SetResizable(32);
        for (int i = 0; i < appFileNameData.Size(); i++)
            {
                char fPath[stdFileStrLen];
                ExtractFilePath(fPath, appFileNameData[i].fName, stdFileStrLen);
                if (StringLength(fPath) == 0)
                    continue;

                bool pathFound = false;
                for (int j = 0; j < currPaths.Size(); j++)
                    if (SameString(fPath, currPaths[j]))
                        {
                            pathFound = true;
                            break;
                        }
                if (!pathFound)
                    currPaths += fPath;
            }

    }


    bool FileNameArray::UpdatePaths(const char* existingPath,
                                    const char* newPath,
                                    bool  executeAfterUpdate)
    {
        oldFileNames.SetStringLen(stdFileStrLen);
        oldFileNames.Alloc(appFileNameData.Size());
        oldFileNames.SetEmpty();

        FuncObjArray exeFuncs;
        int existingLen = StringLength(existingPath);
        bool updateFound = false;
        for (int i = 0; i < appFileNameData.Size(); i++)
            {
                char currFile[stdFileStrLen];
                CopyString(currFile, appFileNameData[i].fName, stdFileStrLen);
                oldFileNames += currFile;

                // existing path in file starting at 0
                if (strstr(currFile, existingPath) == currFile)
                    {
                        updateFound = true;
                        DeleteChar(currFile, 0, existingLen);
                        MakeString(appFileNameData[i].fName, newPath, currFile, stdFileStrLen);
                        if (writeLinux)  //ensure directory slashes are correct
                            {
                                int indx = 0;
                                while (true)
                                    {
                                        indx = CharPositn(appFileNameData[i].fName, '\\', indx);
                                        if (indx >= 0)
                                            appFileNameData[i].fName[indx] = '/';
                                        else
                                            break;
                                    }
                            }
                        if (executeAfterUpdate && appFileNameData[i].assocFO != 0)
                            {
                                bool foFound = false;
                                for (int j = 0; j < exeFuncs.Size(); j++)
                                    if (exeFuncs[j] == appFileNameData[i].assocFO)
                                        {
                                            foFound = true;
                                            break;
                                        }
                                if (!foFound)
                                    exeFuncs += appFileNameData[i].assocFO;
                            }
                    }
            }

        if (!updateFound)
            {
                GenAppErrorMsg("Update Paths", "no files updated - check paths");
                return false;
            }

#ifndef ADCONSOLEAPP

        if (!writeLinux && executeAfterUpdate && exeFuncs.IsNotEmpty())
            {
                CWaitCursor w;

                FuncObjC::CalcSelectedObjects(foc_Full, exeFuncs);
                PlotDefC::ReplotAfterCalc();
                ExposedObjC::UpdateAllExposed();
                MenuRootC::UpdateAllUI();
            }
#endif

        return true;
    }

    void FileNameArray::ReturnPaths()
    {
        if (!writeLinux || !changePaths)
            return;

        if (oldFileNames.IsEmpty())
            {
                GenAppErrorMsg("Return Paths", "no old file name saved");
                return;
            }

        for (int i = 0; i < appFileNameData.Size(); i++)
            CopyString(appFileNameData[i].fName, oldFileNames[i], stdFileStrLen);
    }

};  // end namespace

