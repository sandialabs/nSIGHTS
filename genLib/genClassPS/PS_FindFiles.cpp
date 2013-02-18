///////////////////////////////////////////////////////////////////////////////////
//
// PS_FindFiles.cpp
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
// DESCRIPTION: Platform specific but application independent code.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include <genClass/C_Common.h>

#include <genClassPS/PS_FindFiles.h>
#include <genClass/U_Str.h>
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool PS_FindFiles(const char*       rootName,
                  SC_StringArray    &fileNames,
                  const char*       fileString,
                  char*                 errMsg)
{

    WIN32_FIND_DATA fileData;   // Data structure describes the file found
    HANDLE hSearch;             // Search handle returned by FindFirstFile

    char subString[stdFileStrLen];
    MakeString(subString, rootName, "\\", fileString, stdFileStrLen);

    // Start searching for files in the root directory.


    hSearch = FindFirstFile (subString, &fileData);
    if (hSearch == INVALID_HANDLE_VALUE)
        {
            MakeString(errMsg, "No ", fileString, " files found.", psFFErrMsgLen);
            return false;
        }

    //place first file name in string array
    fileNames += fileData.cFileName;

    //find remaining files and add to string array

    bool bFinished = false;  //determines whether all files have been found
    while (!bFinished)
        {
            if (FindNextFile (hSearch, &fileData))
                {
                    fileNames += fileData.cFileName;
                }
            else
                {
                    if (GetLastError () == ERROR_NO_MORE_FILES) //i.e. not all files found
                        {
                            bFinished = true;
                        }
                    else
                        {
                            CopyString(errMsg, "Unable to find next file.", psFFErrMsgLen);
                            return false;
                        }
                }
        }

    // Close the search handle.

    if (!FindClose (hSearch))
        {
            CopyString(errMsg, "Unable to close search handle.", psFFErrMsgLen);
            return false;
        }

    return true;
}

bool PS_CopyFiles(const char*       rootName,
                  const char*       destFolder,
                  const char*       fileExtension,
                  SC_StringArray&   fileList,
                  char*             errMsg)
{

    WIN32_FIND_DATA fileData;   // Data structure describes the file found
    HANDLE hSearch;             // Search handle returned by FindFirstFile

    char subString[stdFileStrLen];
    MakeString(subString, rootName, "\\*.", fileExtension, stdFileStrLen);

    // Start searching for files in the root directory.

    hSearch = FindFirstFile (subString, &fileData);
    if (hSearch == INVALID_HANDLE_VALUE)
        {
            MakeString(errMsg, "PS_CopyFiles: No *.", fileExtension, " files found.", psFFErrMsgLen);
            return false;
        }

    //copy file, find remaining files and loop

    char oldFile[stdFileStrLen];
    char newFile[stdFileStrLen];
    bool bFinished = false;  //determines whether all files have been found
    while (!bFinished)
        {
            MakeString(oldFile, rootName, "\\", fileData.cFileName, stdFileStrLen);

            if (!PathIsDirectory(oldFile) &&
                !SameString(fileData.cFileName, ".") &&
                !SameString(fileData.cFileName, ".."))
                {
                    fileList += fileData.cFileName;

                    MakeString(newFile, destFolder, "\\", fileData.cFileName, stdFileStrLen);
                    if (!CopyFile(oldFile, newFile, false))
                        {
                            MakeString(errMsg, "PS_CopyFiles: Copying ", fileData.cFileName, " failed.", psFFErrMsgLen);
                            return false;
                        }
                }

            if (!FindNextFile (hSearch, &fileData))
                {
                    if (GetLastError () == ERROR_NO_MORE_FILES) //i.e. not all files found
                        {
                            bFinished = true;
                        }
                    else
                        {
                            CopyString(errMsg, "PS_CopyFiles: Unable to find next file.", psFFErrMsgLen);
                            return false;
                        }
                }
        }

    // Close the search handle.

    if (!FindClose (hSearch))
        {
            CopyString(errMsg, "PS_DeleteFiles: Unable to close search handle.", psFFErrMsgLen);
            return false;
        }

    return true;
}

bool PS_DeleteFiles(const char* rootName,
                    const char* fileExtension,
                    char*       errMsg)
{

    WIN32_FIND_DATA fileData;   // Data structure describes the file found
    HANDLE hSearch;             // Search handle returned by FindFirstFile

    char subString[stdFileStrLen];
    MakeString(subString, rootName, "\\*.", fileExtension, stdFileStrLen);

    // Start searching for files in the root directory.

    hSearch = FindFirstFile (subString, &fileData);
    if (hSearch == INVALID_HANDLE_VALUE)
        {
            MakeString(errMsg, "PS_DeleteFiles: No *.", fileExtension, " files found.", psFFErrMsgLen);
            return false;
        }

    //delete file, find remaining files and loop

    char fileName[stdFileStrLen];
    bool bFinished = false;  //determines whether all files have been found
    while (!bFinished)
        {
            if (!SameString(fileData.cFileName, ".") && !SameString(fileData.cFileName, ".."))
                {
                    MakeString(fileName, rootName, "\\", fileData.cFileName, stdFileStrLen);
                    if (!DeleteFile(fileName))
                        {
                            MakeString(errMsg, "PS_DeleteFiles: Deleting ", fileData.cFileName, " failed.", psFFErrMsgLen);
                            return false;
                        }
                }

            if (!FindNextFile (hSearch, &fileData))
                {
                    if (GetLastError () == ERROR_NO_MORE_FILES) //i.e. not all files found
                        {
                            bFinished = true;
                        }
                    else
                        {
                            CopyString(errMsg, "PS_DeleteFiles: Unable to find next file.", psFFErrMsgLen);
                            return false;
                        }
                }
        }

    // Close the search handle.

    if (!FindClose (hSearch))
        {
            CopyString(errMsg, "PS_DeleteFiles: Unable to close search handle.", psFFErrMsgLen);
            return false;
        }

    return true;
}

bool PS_FileExists(     const char* fullFileName,
                        char*       errMsg)
{

    WIN32_FIND_DATA fileData;   // Data structure describes the file found
    HANDLE hSearch;             // Search handle returned by FindFirstFile

    char fileName[stdFileStrLen];
    ExtractFileName(fileName, fullFileName, stdFileStrLen);

    // Start searching for files in the root directory.

    hSearch = FindFirstFile(fullFileName, &fileData);

    if (hSearch == INVALID_HANDLE_VALUE)
        {
            MakeString(errMsg, "PS_FileExists: ", fileName, " not found.", psFFErrMsgLen);
            return false;
        }
    else
        MakeString(errMsg, "PS_FileExists: ", fileName," exists.", psFFErrMsgLen);

    if (!FindClose (hSearch))
        {
            CopyString(errMsg, "PS_FileExists: Unable to close search handle.", psFFErrMsgLen);
            return false;
        }

    return true;

}

