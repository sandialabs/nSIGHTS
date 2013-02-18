///////////////////////////////////////////////////////////////////////////////////
//
// DO_Label.cpp
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
//              wrap data classes DC_XXX: base class for various label types
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/C_Common.h>
#include <genClass/SC_RealFormat.h>

#include <genClass/DO_Label.h>

char* DO_Label::typeDesc    = "Label";

DO_Label::DO_Label()
{
    LocalInitialize();
}
DO_Label::DO_Label(const DO_Label& a)
{
    LocalCopy(a);
}

DO_Label& DO_Label::operator= (const DO_Label& a)
{
    if (this != &a)
        LocalCopy(a);
    return *this;
}

void  DO_Label::LocalInitialize()
{
    dataTypeLabel[0]  = '\0';
    dataValueLabel[0] = '\0';
}


void DO_Label::LocalCopy(const DO_Label& a)
{
    CopyString(dataTypeLabel,   a.dataTypeLabel, maxLabelLen);
    CopyString(dataValueLabel, a.dataValueLabel, maxLabelLen);
}

void DO_Label::Reset(const char* typeLabel)
{
    SetTypeLabel(typeLabel);
    SetValueLabel("null");
}

void  DO_Label::SetTypeLabel(const char inType[])
{
    CopyString(dataTypeLabel, inType, maxLabelLen);
}
void  DO_Label::SetValueLabel(const char inValue[])
{
    CopyString(dataValueLabel, inValue, maxLabelLen);
}
void  DO_Label::SetValueLabel(const double& inValue)
{
    SC_RealFormat rForm;
    rForm.RealToString(inValue, dataValueLabel, maxLabelLen);
}

void DO_Label::SetValueLabel(const double&        inValue,
                             const SC_RealFormat& rForm)
{
    rForm.RealToString(inValue, dataValueLabel, maxLabelLen);
}


void DO_Label::SetValueLabel(int        nValue,
                             const char valDesc[])
{
    IntToString(nValue, dataValueLabel, maxLabelLen);
    ConcatString(dataValueLabel, " ", maxLabelLen);
    ConcatString(dataValueLabel, valDesc, maxLabelLen);
}

void DO_Label::SetFileValueLabel(const char inFname[])
{
    char tempStr[stdFileStrLen];
    ExtractFileName(tempStr, inFname, stdFileStrLen);
    SetValueLabel(tempStr);
}

const char* DO_Label::GetID()
{
    return GetTypeLabel();
}

