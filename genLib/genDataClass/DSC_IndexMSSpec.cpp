///////////////////////////////////////////////////////////////////////////////////
//
// DSC_IndexMSSpec.cpp
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

#include <genClass/DO_Indx.h>
#include <genDataClass/DSC_IndexMSSpec.h>


DSC_IndexMSSpec::DSC_IndexMSSpec(const char* ndxID, DO_Indx& inDO) :
    indexID(ndxID), indexDO(inDO)
{
    indexIsMaster           = true;
    enteredIndexValue       = 0;
    enteredIndexIsSingle    = true;

    enteredIndexArray.SetResizable();
    enteredIndexArray.Alloc(10);

    slaveIsSingle           = true;
    slaveIndexValue         = 0;
}

DSC_IndexMSSpec::~DSC_IndexMSSpec()
{
}

DSC_IndexMSSpec& DSC_IndexMSSpec::operator= (const DSC_IndexMSSpec& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void DSC_IndexMSSpec::Clear()
{
    indexIsMaster           = true;
    enteredIndexValue       = 0;

    enteredIndexIsSingle    = true;
    enteredIndexArray.SetEmpty();

    slaveIndexValue         = 0;
    slaveIsSingle           = true;
    slaveIndexArray.SetEmpty();
}

void DSC_IndexMSSpec::LocalCopy(const DSC_IndexMSSpec& a)
{
    indexIsMaster           = a.indexIsMaster;
    indexMasterObjRef       = a.indexMasterObjRef;
    enteredIndexValue       = a.enteredIndexValue;
    enteredIndexIsSingle    = a.enteredIndexIsSingle;
    slaveIndexValue         = a.slaveIndexValue;
    enteredIndexArray       = a.enteredIndexArray;
    slaveIndexArray         = a.slaveIndexArray;
}

void  DSC_IndexMSSpec::DoCheck(const char*      specID,
                                    FuncObjC&  specFO)
{
    indexDO.Reset(indexID);
    if ((!indexIsMaster) && specFO.CheckInputFO(indexMasterObjRef,  specID))
    {
        DO_Indx& tmpDO = *(static_cast<DO_Indx*> (specFO.GetInPortData(indexMasterObjRef)));

        slaveIndexValue  = tmpDO.GetIndexValue();

        if (MultipleOK())
        {
            slaveIsSingle    = tmpDO.IsSingle();
            if (tmpDO.MultipleOK())
                slaveIndexArray = tmpDO.GetIndexArray();
            if ((!slaveIsSingle) && slaveIndexArray.IsEmpty())
                specFO.SetObjErrMsg("no slave indexes selected");
        }
    }
    else
        if (MultipleOK() && (!enteredIndexIsSingle) && enteredIndexArray.IsEmpty())
        {
            specFO.SetObjErrMsg("no indexes selected");
        }


    if (SingleOK())
        indexDO.SetIndexValue(GetCurrentIndex());

    if (MultipleOK())
        indexDO.SetIndexArray(GetCurrentIndexArray());

    if (SingleOK() && MultipleOK())
        indexDO.SetIndexType(CurrentIndexIsSingle());

}

void DSC_IndexMSSpec::AddPort(FuncObjC&   specFO)
{
    specFO.AddMSInPort(indexMasterObjRef, typeid(indexDO), indexIsMaster);
    specFO.AddOutPort(indexDO);
}

bool DSC_IndexMSSpec::CurrentIndexIsSingle() const
{
    if (indexIsMaster)
        return enteredIndexIsSingle;
    return slaveIsSingle;
}

int DSC_IndexMSSpec::GetCurrentIndex() const
{
    if (indexIsMaster)
        return enteredIndexValue;

    return slaveIndexValue;
}

const SC_IntArray&  DSC_IndexMSSpec::GetCurrentIndexArray()   const
{
    if (!MultipleOK())
    {
//      cout << indexDO.GetType() << endl;
//      cout << indexDO.multipleOK << " " << indexDO.singleOK  << endl;

        GenAppInternalError("DSC_IndexMSSpec::GetCurrentIndexArray()");
    }

    if (indexIsMaster)
        return enteredIndexArray;

    return slaveIndexArray;
}

void DSC_IndexMSSpec::GetIndexArray(SC_IntArray& selIndexes,
                                    int            maxIndex)
{
    if (CurrentIndexIsSingle())
    {
        selIndexes.Alloc(1);
        selIndexes += GetCurrentIndex();
    }
    else
        selIndexes = GetCurrentIndexArray();


    int nOK = 0;
    for (int i = 0; i < selIndexes.Size(); i++)
        if (selIndexes[i] < maxIndex)
            selIndexes[nOK++] = selIndexes[i];
    selIndexes.SetSize(nOK);
}


void DSC_IndexMSSpec::SetValid(int        maxIndex,
                              FuncObjC&  specFO)
{
    if (indexIsMaster)
    {
        // masters should always be OK but we check anyway ...
        if (enteredIndexValue >= maxIndex)
            enteredIndexValue = 0;
        int nOK = 0;
        for (int i = 0; i < enteredIndexArray.Size(); i++)
            if (enteredIndexArray[i] < maxIndex)
                enteredIndexArray[nOK++] = enteredIndexArray[i];
        enteredIndexArray.SetSize(nOK);
        if ((!enteredIndexIsSingle) && enteredIndexArray.IsEmpty())
            specFO.SetObjErrMsg("no indexes valid");
    }
    else
    {
        if (slaveIndexValue >= maxIndex)
            slaveIndexValue = 0;
        int nOK = 0;
        for (int i = 0; i < slaveIndexArray.Size(); i++)
            if (slaveIndexArray[i] < maxIndex)
                slaveIndexArray[nOK++] = slaveIndexArray[i];
        slaveIndexArray.SetSize(nOK);
        if ((!slaveIsSingle) && slaveIndexArray.IsEmpty())
            specFO.SetObjErrMsg("no slave indexes valid");
    }

}

