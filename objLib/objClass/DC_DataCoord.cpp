///////////////////////////////////////////////////////////////////////////////////
//
// DC_DataCoord.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <objClass/DC_DataCoord.h>

DC_DataCoord :: DC_DataCoord()
{
    SetToNull(dataID);
    dataIsLog = false;
}

DC_DataCoord::DC_DataCoord(const DC_DataCoord& a) :
    SC_DoubleArray(a)
{
    LocalCopy(a);
}

bool DC_DataCoord::IsCompatible(const DC_DataCoord& a) const
{
    return (Size() == a.Size());
}

DC_DataCoord :: ~DC_DataCoord()
{
}

DC_DataCoord&  DC_DataCoord::operator=(const DC_DataCoord& a)
{
    if (&a != this)
    {
        SC_DoubleArray::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

DC_DataCoord&  DC_DataCoord::operator=(const SC_DoubleArray& a)
{
    SC_DoubleArray::operator=(a);
    return *this;
}

void DC_DataCoord::LocalCopy(const DC_DataCoord& a)
{
    CopyString(dataID, a.dataID, dataIDLen);
    dataIsLog = a.dataIsLog;
}


bool DC_DataCoord::Alloc(int nData)
{
    try
    {
        AllocAndSetSize(nData);
    }
    catch (SC_Array::AllocationError& )
    {
        DeAlloc();
        return false;
    }

    for (int i = 0; i < nData; i++)
        (*this)[i] = double(i);

    return true;
}


bool DC_DataCoord::CreateFrom(DC_DataCoord& a)
{
    try
    {
        SC_DoubleArray::operator=(a);
    }
    catch (SC_Array::AllocationError& )
    {
        DeAlloc();
        return false;
    }

    LocalCopy(a);

    return true;
}


void DC_DataCoord::SetID(const char id[])
{
    CopyString(dataID, id, dataIDLen);
}

void DC_DataCoord::GetID(char id[],
                      int  maxIDlen) const
{
    CopyString(id, dataID, maxIDlen);
}


double DC_DataCoord::GetMid(int indx) const
{
    double midVal;
    if (dataIsLog)
        midVal = log10((*this)[indx]) + log10((*this)[indx + 1]);
    else
        midVal = (*this)[indx] + (*this)[indx + 1];

    midVal /= 2.0;

    if (dataIsLog)
        return InvLgt(midVal);

    return midVal;
}



bool DC_DataCoord::GetIndex(double inVal,
                            int&   indx) const
{
    if ((inVal < (*this)[0]) ||  (inVal > LastIndex()))
        return false;

    if (dataIsLog)
    {
        if (inVal < stdEps)
            return false;
        inVal = log10(inVal);
    }

    inVal *= 2.0;

    for (int i = 0; i < UpperBound(); i++)
    {
        double nextCoord;
        if (dataIsLog)
            nextCoord = log10((*this)[i]) + log10((*this)[i + 1]);
        else
            nextCoord = (*this)[i] + (*this)[i + 1];

        if (inVal <= nextCoord)
        {
            indx = i;
            return true;
        }
    }

    indx = UpperBound();
    return true;
}


bool DC_DataCoord::GetExactIndex(double inVal,
                                  int&   indx) const
{
    if (dataIsLog)
    {
        if (inVal < stdEps)
            return false;
        inVal = log10(inVal);
    }

    for (int i = 0; i < Size(); i++)
    {
        double dval = (*this)[i];
        if (dataIsLog)
            dval = log10(dval);
        if (fabs(dval - inVal) < 1.0E-05)
        {
            indx = i;
            return true;
        }
    }
    return false;
}


bool DC_DataCoord::TransformVal(double& inVal) const
{
    if (dataIsLog)
    {
        if (inVal < stdEps)
            return false;

        inVal = log10(inVal);
    }

    return true;
}


void DC_DataCoord::UnTransformVal(double& inVal) const
{
    if (dataIsLog)
        inVal = InvLgt(inVal);
}


void DC_DataCoord::GetStartEnd(int indx,
                               double& sVal,
                               double& eVal) const
{
    if (indx == 0)
        sVal = (*this)[0];
    else
        sVal = GetMid(indx - 1);

    if (indx == UpperBound())
        eVal = (*this)[indx];
    else
        eVal = GetMid(indx);

}


bool DC_DataCoord::GetInterpT(const double& inVal,
                                    int&    stIndx,
                                    double& tVal) const
{
    if (inVal < (*this)[0])
    {
        stIndx = 0;
        tVal = 0.0;
        return false;
    }

    if (inVal > LastIndex())
    {
        stIndx = UpperBound() - 1;
        tVal = 1.0;
        return false;
    }

    stIndx = UpperBound() - 1;
    for (int i = 1; i < UpperBound(); i++)
        if (inVal < (*this)[i])
        {
            stIndx = i - 1;
            break;
        }

    double stVal = (*this)[stIndx];
    if (dataIsLog)
    {
        stVal = log10(stVal);
        tVal = (log10(inVal) - stVal) / (log10((*this)[stIndx + 1]) - stVal);
    }
    else
    {
        tVal = (inVal - stVal) / ((*this)[stIndx + 1] - stVal);
    }

    return true;

}



void DC_DataCoord::Decimate(int decModulus)
{
    if (decModulus < 2)
        return;

    // always keep first
    int next = 1;
    for (int i = 1; i < Size(); i++)
        if ((i % decModulus) == 0)
            (*this)[next++] = (*this)[i];

    // add last if not there
    if ((UpperBound() % decModulus) != 0)
        (*this)[next++] = (*this)[UpperBound()];

    SetSize(next);
}

