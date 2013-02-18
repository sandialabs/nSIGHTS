///////////////////////////////////////////////////////////////////////////////////
//
// DC_CurveArray.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <string.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/C_Common.h>

#include <objClass/DC_CurveArray.h>


DC_CurveWithID::DC_CurveWithID()
{
    curveID[0] = '\0';
}

DC_CurveWithID::DC_CurveWithID(const DC_CurveWithID& a) : DC_Curve(a)
{
    LocalCopy(a);
}

DC_CurveWithID::~DC_CurveWithID()
{
    // empty
}

DC_CurveWithID& DC_CurveWithID::operator= (const DC_CurveWithID& a)
{
    if (&a != this)
    {
        DC_Curve::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

DC_CurveWithID& DC_CurveWithID::operator= (const DC_Curve& a)
{
    if (&a != this)
    {
        DC_Curve::operator=(a);
    }
    return *this;
}


void  DC_CurveWithID::LocalCopy(const DC_CurveWithID& a)
{
    CopyString(curveID, a.curveID, curveIDlen);
}


void DC_CurveWithID::SetID(const char id[])
{
    CopyString(curveID, id, curveIDlen);
}

void DC_CurveWithID::GetID(char id[],
                           int  maxIDlen)
{
    CopyString(id, curveID, maxIDlen);
}


void DC_CurveArray::GetIDs(SC_StringArray& ids)
{
    ids.DeAlloc();
    ids.SetStringLen(DC_CurveWithID::curveIDlen);
    ids.Alloc(Size());
    for ( int i = 0; i < Size(); i++)
        ids += (*this)[i].curveID;
}


bool DC_CurveArray::GetIDIndex(const char* inID,
                              int&          idIndex)
{
    for ( int i = 0; i < Size(); i++)
        if (strcmp(inID, (*this)[i].curveID) == 0)
        {
            idIndex = i;
            return true;
        }

    idIndex = -1;
    return false;
}

