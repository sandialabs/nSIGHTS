///////////////////////////////////////////////////////////////////////////////////
//
// DO_LabelArray.cpp
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
//              wrap data classes DC_XXX: base class for plottable labels
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/DO_LabelArray.h>

char* DO_LabelArray::typeDesc = "Labels array";

DO_LabelArray::DO_LabelArray()
{
    dataCoords.SetResizable(10);
    dataLabels.SetStringLen(maxLabelLen);
    dataLabels.SetResizable(10);
}

DO_LabelArray::DO_LabelArray(const DO_LabelArray& a) : DO_Label(a)
{
    LocalCopy(a);
}

DO_LabelArray& DO_LabelArray::operator= (const DO_LabelArray& a)
{
    if (&a != this)
        {
            DO_Label::operator=(a);
            LocalCopy(a);
        }
    return *this;
}


void DO_LabelArray::Reset(const char* typeLabel)
{
    SetValueLabel("none");
    SetTypeLabel(typeLabel);
    DO_LabelArray::Clear();
}

void DO_LabelArray::Clear()
{
    dataCoords.SetEmpty();
    dataLabels.SetEmpty();
}

bool DO_LabelArray::DataOK()
{
    bool dataOK = false;

    if (dataCoords.IsEmpty())
        SetErrMsg("No coordinates in list");
    else if (dataLabels.IsEmpty())
        SetErrMsg("No labels in list");
    else
        dataOK = true;

    return dataOK;
}


void DO_LabelArray::LocalCopy(const DO_LabelArray& a)
{
    dataLabels = a.dataLabels;
    dataCoords = a.dataCoords;
}

