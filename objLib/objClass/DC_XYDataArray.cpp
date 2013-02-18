///////////////////////////////////////////////////////////////////////////////////
//
// DC_XYDataArray.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/C_Common.h>

#include <objClass/DC_XYDataArray.h>

DC_XYDataArray :: DC_XYDataArray()
: T_SC_Array<DC_XYData>()
{
    SetToNull(arrayDataID);
}

void DC_XYDataArray::SetID(const char id[])
{
    CopyString(arrayDataID, id, DC_XYData::dataIDLen);
}

void DC_XYDataArray::GetID(char id[],
                           int  maxIDlen) const
{
    CopyString(id, arrayDataID, maxIDlen);
}

void DC_XYDataArray::GetIDs(SC_StringArray& ids)
{
    ids.DeAlloc();
    ids.SetStringLen(DC_XYData::dataIDLen);
    ids.Alloc(Size());
    for ( int i = 0; i < Size(); i++)
        ids += (*this)[i].dataID;
}


bool DC_XYDataArray::GetIDIndex(const char* inID,
                                int&        idIndex)
{
    for ( int i = 0; i < Size(); i++)
        if (strcmp(inID, (*this)[i].dataID) == 0)
        {
            idIndex = i;
            return true;
        }

    idIndex = -1;
    return false;
}

