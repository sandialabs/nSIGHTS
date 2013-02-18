///////////////////////////////////////////////////////////////////////////////////
//
// C_MPISupport.cpp
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
// DESCRIPTION: Platform independent and application independent base claseess and
//              support classes.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_FuncObj.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_File.h>

#include <genClass/C_MPISupport.h>

// **************************************** member classes

int NodeFile::mpiNodeRank = -1;
bool NodeFile::mpiDebugRun = false;

NodeFile::NodeFile()
{
    nodeBasedFile = false;
    fileIsUsed = true;
    fileIsInput = true;
    fileName = 0;
    nodeDesc = "NodeFile not set";
}

NodeFile::NodeFile(const NodeFile& a)
{
    LocalCopy(a);
}

NodeFile::NodeFile(char* inFile, const char* inDesc, bool isInputF)
{
    nodeBasedFile = false;
    fileName = inFile;
    nodeDesc = inDesc;
    fileIsUsed = true;
    fileIsInput = isInputF;
}

NodeFile& NodeFile::operator= (const NodeFile& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

bool NodeFile::CheckNodeFile(FuncObjC& inObj,
                             bool isDirectory)
{
    if (fileIsUsed && IsNull(fileName))
        {
            char errStr[80];
            MakeString(errStr, "File name not set: ", nodeDesc, 80);
            inObj.SetObjErrMsg(errStr);
            return false;
        }

    if ((!nodeBasedFile) || (mpiNodeRank >= 0) || (!fileIsUsed))
        return true;

    // master -- assume Win GUI - do checks for 00
    bool strOK = false;
    if (isDirectory)
        strOK = (strstr(fileName, "\\00") != NULL);
    else
        strOK = (strstr(fileName, "\\00\\") != NULL);

    if (!strOK)
        {
            char msgStr[80];
            MakeString(msgStr, "Cannot find string \\00\\ in: ", nodeDesc, 80);
            inObj.SetObjErrMsg(msgStr);
            return false;
        }

    return true;

}

void NodeFile::UpdateNodeFile(bool singleCheckOnly,
                              bool isDirectory)
{
    if ((!nodeBasedFile) || (mpiNodeRank < 0) || (!fileIsUsed))
        return;

    char* st = fileName;

    while (st != NULL)
        {
#ifdef _WINDOWS
            if (isDirectory)
                {
                    st = strstr(st, "\\00");
                }
            else
                {
                    st = strstr(st, "\\00\\");
                }
#else
            if (isDirectory)
                {
                    st = strstr(st, "/00");
                }
            else
                {
                    st = strstr(st, "/00/");
                }
#endif

            char repStr[8];
            if (st != NULL)
                {
                    IntToString(mpiNodeRank, repStr, 2, 8);
                    st++;
                    *st = repStr[0];
                    st++;
                    *st = repStr[1];
                }

            if (singleCheckOnly)
                return;
        }
}

bool NodeFile::CheckExists(FuncObjC& inObj,
                           bool      isDirectory)
{
    if (fileIsUsed && fileIsInput)
        {
            if (!isDirectory)
                return inObj.CheckFileExists(fileName);

            if (PathExists(fileName))
                return true;

            char errMsg[80];
            MakeString(errMsg, "Path not found: ", fileName, 80);
            inObj.SetObjErrMsg(errMsg);
            return false;
        }

    return true;
}


void NodeFile::LocalCopy(const NodeFile& a)
{
    nodeBasedFile = a.nodeBasedFile;
    fileIsUsed = a.fileIsUsed;
    fileName = a.fileName;
    nodeDesc = a.nodeDesc;
    fileIsInput = a.fileIsInput;
}


NodeFileArray::NodeFileArray()
{
    SetResizable(8);
}

bool NodeFileArray::CheckNodeFile(FuncObjC& inObj)
{
    for (int i = 0; i < Size(); i++)
        if (!(*this)[i].CheckNodeFile(inObj))
            return false;

    return true;
}

void NodeFileArray::UpdateNodeFile(bool singleCheckOnly)
{
    for (int i = 0; i < Size(); i++)
        (*this)[i].UpdateNodeFile(singleCheckOnly);
}

void NodeFileArray::CopyFrom(const NodeFileArray& a)
{
    if (a.Size() != Size())
        GenAppInternalError("NodeFileArray::CopyFrom");
    for (int i = 0; i < Size(); i++)
        (*this)[i].nodeBasedFile = a[i].nodeBasedFile;
}

bool NodeFileArray::StdCheckAndUpdate(FuncObjC& inObj)
{
    for (int i = 0; i < Size(); i++)
        {
            NodeFile& currNodeF = (*this)[i];

            if (!currNodeF.CheckNodeFile(inObj))
                return false;

            currNodeF.UpdateNodeFile(true);

            if (!currNodeF.CheckExists(inObj))
                return false;
        }
    return true;
}

