///////////////////////////////////////////////////////////////////////////////////
//
// C_CurveFile.cpp
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
//      Class for error checking and reading curve files into DC_Curve Array structures.
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genApp/G_FileNames.h>

#include <iostream>

#include <Var/C_CurveFile.h>

CurveFile::CurveFile()
{
    fileName[0] = '\0';
    currentCurveData = 0;
    appFileNameData.AddTo(fileName);
}

CurveFile::~CurveFile()
{
    appFileNameData.DeleteFrom(fileName);
}

bool CurveFile::SetupOK(SC_SetupErr& errData)
{
    SC_SetupErr filErr;
    if (fileName[0] == '\0')
        filErr.SetConstantError("file name not set");
    return errData.SetupOK("Curve File", filErr);
}

bool CurveFile::LoadCurvesFromFile()
{
    currentCurveData = 0;
//  std::cout << fileName << std::endl;
    if (IsNotNull(fileName) && textCurveData.ReadFromFile(fileName))
    {
        currentCurveData = &textCurveData;
        return true;
    }
    return false;
}

void CurveFile::SetToInitial()
{
    currentCurveData = 0;
    textCurveData.DeAlloc();
    fileName[0] = '\0';
}


bool CurveFile::GetIDs(SC_StringArray& currIDs) const
{
    currIDs.SetEmpty();
    if (currentCurveData != 0)
        currentCurveData->GetIDs(currIDs);
    return ! currIDs.IsEmpty();
}



