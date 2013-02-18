///////////////////////////////////////////////////////////////////////////////////
//
// DO_XYArray.cpp
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
//      Wrapper for DC_XXX.
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <objClass/DO_XYArray.h>

char* DO_XYArray::typeDesc = "Point array";

DO_XYArray::DO_XYArray()
{
    xyPoints.SetResizable();
    LocalInitialize();
}

DO_XYArray::DO_XYArray(const DO_XYArray& a)
{
    LocalCopy(a);
}

DO_XYArray& DO_XYArray::operator= (const DO_XYArray& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void DO_XYArray::Reset(const char* typeLabel)
{
    SetTypeLabel(typeLabel);
    DO_XYArray::Clear();
}

void DO_XYArray::Clear()
{
    xyPoints.SetEmpty();
    SetValueLabel("none");
}

bool DO_XYArray::DataOK()
{
    bool dataOK = false;

    if (xyPoints.IsEmpty())
        SetErrMsg("No points in list");
    else
        dataOK = true;

    return dataOK;
}


void DO_XYArray::LocalCopy(const DO_XYArray& a)
{
    xyPoints      = a.xyPoints;
}

void DO_XYArray::FullCopy(const DO_XYArray& a)
{
    LocalCopy(a);
    DO_Label::FullCopy(a);
}

void DO_XYArray::LocalInitialize()
{
    xyPoints.Alloc(10);
}

