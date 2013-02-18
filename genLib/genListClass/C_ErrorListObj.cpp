///////////////////////////////////////////////////////////////////////////////////
//
// C_ErrorListObj.cpp
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
//      performs listing of error conditions.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genListClass/C_ErrorListObj.h>

ErrorListObjC::ErrorListObjC(const char* listType) : ListObjC(listType)
{
    throwOnError = false;
}

void ErrorListObjC::InitListingData()
{
    ListObjC::InitListingData();
    errorFound = false;
}

void ErrorListObjC::CloseListingData()
{
    if (!errorFound)
        AddLine("No errors detected", 0);

    ListObjC::CloseListingData();
}


bool ErrorListObjC::ErrorCheckOK()
{
    throwOnError = true;
    bool retVal = true;
    try {

        CreateListing();
    }
    catch (ErrorException) {
        retVal = false;
    }
    throwOnError = false;
    return retVal;
}


void ErrorListObjC::AddError(const  SC_SetupErr& errData)
{
    AddError(errData.GetErrorText());
}

void ErrorListObjC::AddError(const char* errText)
{
    errorFound = true;
    if (throwOnError)
        throw ErrorException();

    AddText(errText, 0);
    AddNextLine();
}


