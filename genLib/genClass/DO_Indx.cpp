///////////////////////////////////////////////////////////////////////////////////
//
// DO_Indx.cpp
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
// DESCRIPTION: Platform independent and application independent base classes and
//              support classes.
//              lightweight Data Object (DO) classes derived from DataObjC to
//              wrap data classes DC_XXX: base class for various index types
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/DO_Indx.h>

char* DO_Indx::typeDesc    = "Indx";

DO_Indx::DO_Indx() :
    singleOK(true),
    multipleOK(false),
    indexIsSingle(true),
    indxVal(0)
{
}

DO_Indx::DO_Indx(bool snglOK, bool multOK) :
    singleOK(snglOK),
    multipleOK(multOK),
    indexIsSingle(true),
    indxVal(0)
{
    if (!(snglOK || multOK))
        GenAppInternalError("DO_Indx::DO_Indx");

    if (multipleOK)
        {
            indxArray.SetResizable();
            indxArray.Alloc(10);
            indexIsSingle = snglOK;
        }
}

DO_Indx::DO_Indx(const DO_Indx& a) :
    singleOK(a.singleOK),
    multipleOK(a.multipleOK)
{
    LocalCopy(a);
}

DO_Indx& DO_Indx::operator= (const DO_Indx& a)
{
    if (&a != this)
        {
            DO_Label::FullCopy(a);
            LocalCopy(a);
        }

    return *this;
}

void DO_Indx::SetIndexLabel()
{
    char tempStr[40];
    if (indexIsSingle)
        {
            IntToString(indxVal, tempStr, 40);
        }
    else
        {
            if (indxArray.IsEmpty())
                {
                    CopyString(tempStr, "empty", 40);
                }
            else
                {
                    IntToString(indxArray.Size(), tempStr, 40);
                    ConcatString(tempStr, " data", 40);
                }
        }
    SetValueLabel(tempStr);
}

void DO_Indx::Reset(const char* typeLabel)
{
    DO_Label::Reset(typeLabel);
    indxVal = 0;
    indxArray.SetEmpty();
}

bool DO_Indx::DataOK()
{
    if (indexIsSingle)
        return true;

    bool dataOK = false;
    if (indxArray.IsEmpty())
        SetErrMsg("No indexes set");
    else
        dataOK = true;

    return dataOK;
}


void DO_Indx::SetIndexType(bool isSngl)
{
    if (isSngl && (!singleOK))
        GenAppInternalError("DO_Indx::SetIndexType 1");

    if ((!isSngl) && (!multipleOK))
        GenAppInternalError("DO_Indx::SetIndexType 2");

    indexIsSingle = isSngl;
}

void DO_Indx::SetIndexArray(const SC_IntArray&  inArray)
{
    if (!multipleOK)
        GenAppInternalError("DO_Indx::SetIndexArray");

    indxArray = inArray;
    indxArray.SetResizable();
}

const SC_IntArray& DO_Indx::GetIndexArray() const
{
    if (!multipleOK)
        GenAppInternalError("DO_Indx::GetIndexArray");
    return indxArray;
}

void  DO_Indx::LocalCopy(const DO_Indx& a)
{
    indexIsSingle = a.indexIsSingle;
    indxVal       = a.indxVal;
    if (multipleOK)
        indxArray = a.indxArray;
}
