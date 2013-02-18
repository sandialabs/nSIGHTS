///////////////////////////////////////////////////////////////////////////////////
//
// DO_CubeIndxArray.cpp
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

#include <objClass/DO_CubeIndxArray.h>

char* DO_CubeIndxArray::typeDesc = "Cube data indexes";

DO_CubeIndxArray::DO_CubeIndxArray()
{
    dataIndexes.SetResizable();
    LocalInitialize();
}

DO_CubeIndxArray::DO_CubeIndxArray(const DO_CubeIndxArray& a) : DO_Label(a)
{
    LocalCopy(a);
}

DO_CubeIndxArray& DO_CubeIndxArray::operator= (const DO_CubeIndxArray& a)
{
    if (&a != this)
    {
        DO_Label::operator=(a);
        LocalCopy(a);
    }

    return *this;
}


void DO_CubeIndxArray::Reset(const char* typeLabel)
{
    DO_Label::Reset(typeLabel);
    baseCube = 0;
    DO_CubeIndxArray::Clear();
}

void DO_CubeIndxArray::Clear()
{
    dataIndexes.SetEmpty();
    SetValueLabel("none");
}

bool DO_CubeIndxArray::DataIndexesAvailable()
{
    return (dataIndexes.PosValueCount() > 0);
}

bool DO_CubeIndxArray::DataOK()
{
    bool dataOK = false;

    if (baseCube == 0)
        SetErrMsg("Cube  bad/not set");
    else
        dataOK = true;

    return dataOK;
}

bool DO_CubeIndxArray::GetValidIndexes(SC_IntArray& validData)
{
    validData.Alloc(dataIndexes.Size());
    for (int i = 0; i < dataIndexes.Size(); i++)
        if (dataIndexes[i] >= 0)
            validData += dataIndexes[i];

    return validData.Size() > 0;
}

void DO_CubeIndxArray::LocalCopy(const DO_CubeIndxArray& a)
{
    baseCube        = a.baseCube;
    dataIndexes     = a.dataIndexes;
}

void DO_CubeIndxArray::LocalInitialize()
{
    dataIndexes.Alloc(10);
    baseCube = 0;
}

