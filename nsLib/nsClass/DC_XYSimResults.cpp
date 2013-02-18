///////////////////////////////////////////////////////////////////////////////////
//
// DC_XYSimResults.cpp
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
//      Class for packaging F(t) data for file I/O
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/SC_RealFormat.h>

#include <nsClass/DC_XYSimResults.h>

XYSimCaseResults::XYSimCaseResults()
{
    caseIndex = -1;
    caseID[0] = '\0';
    staticPressure = 0.0;
}

XYSimCaseResults::XYSimCaseResults(const XYSimCaseResults& a)
{
    LocalCopy(a);
}

XYSimCaseResults::~XYSimCaseResults()
{
}

XYSimCaseResults& XYSimCaseResults::operator=(const XYSimCaseResults& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void XYSimCaseResults::LocalCopy(const XYSimCaseResults& a)
{
    CopyString(caseID, a.caseID, caseIDLen);
    staticPressure  = a.staticPressure;
    caseValues      = a.caseValues;
    caseData        = a.caseData;
    caseIndex       = a.caseIndex;
}


XYSimRunResults::XYSimRunResults() : T_SC_Array<XYSimCaseResults>()
{
    runID[0] = '\0';
    caseValDesc.SetStringLen(80);
    caseValShortDesc.SetStringLen(40);
}

XYSimRunResults::XYSimRunResults(const XYSimRunResults& a) :
    T_SC_Array<XYSimCaseResults>(a)
{
    LocalCopy(a);
}

XYSimRunResults::~XYSimRunResults()
{
}

XYSimRunResults& XYSimRunResults::operator=(const XYSimRunResults& a)
{
    if (&a != this)
    {
        T_SC_Array<XYSimCaseResults>::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void XYSimRunResults::LocalCopy(const XYSimRunResults& a)
{
    CopyString(runID, a.runID, runIDLen);
    caseValDesc      = a.caseValDesc;
    caseValShortDesc = a.caseValShortDesc;
    caseSeqData      = a.caseSeqData;
}

void XYSimRunResults::GetCaseIDs(SC_StringArray&  caseIDs) const
{
    caseIDs.DeAlloc();
    caseIDs.SetStringLen(XYSimCaseResults::caseIDLen);
    caseIDs.Alloc(Size());
    for (int i = 0; i < Size(); i++)
        caseIDs += (*this)[i].caseID;
};

int  XYSimRunResults::GetnXY() const
{
    return (*this)[0].GetnXY();
}


DC_XYSimResults::DC_XYSimResults() : T_SC_Array<XYSimRunResults>()
{
}

DC_XYSimResults::DC_XYSimResults(const DC_XYSimResults& a) :
    T_SC_Array<XYSimRunResults>(a)
{
    LocalCopy(a);
}

DC_XYSimResults::~DC_XYSimResults()
{
}

DC_XYSimResults& DC_XYSimResults::operator=(const DC_XYSimResults& a)
{
    if (&a != this)
    {
        T_SC_Array<XYSimRunResults>::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void DC_XYSimResults::LocalCopy(const DC_XYSimResults& a)
{
}

void  DC_XYSimResults::GetRunIDs(SC_StringArray&  runIDs) const
{
    runIDs.ForceMinStringLen(XYSimRunResults::runIDLen);
    runIDs.Alloc(Size());
    for (int i = 0; i < Size(); i++)
        runIDs += (*this)[i].runID;
};

int DC_XYSimResults::GetnCases() const
{
    int nCase = 0;
    for (int i = 0; i < Size(); i++)
        nCase += (*this)[i].Size();
    return nCase;
}


int  DC_XYSimResults::GetnXY() const
{
    return (*this)[0].GetnXY();
}

void DC_XYSimResults::GetCaseIDs(SC_StringArray&  caseIDs) const
{
    static const int maxStrLen = 120;

    caseIDs.ForceMinStringLen(maxStrLen);
    caseIDs.Alloc(GetnCases());
    char tempStr[maxStrLen];

    SC_SciFormat rf(5);
    for (int i = 0; i < Size(); i++)
    {
        XYSimRunResults& currRun = (*this)[i];
        int nval = currRun.caseValShortDesc.Size();
        if (nval > 4)
            nval = 4;

        for (int j = 0; j < currRun.Size(); j++)
        {
            XYSimCaseResults& currCase = currRun[j];

            MakeString(tempStr, currRun.runID, "\t", currCase.caseID, maxStrLen);
            if (currCase.caseIndex >= 0)
            {
                char iStr[20];
                IntToString(currCase.caseIndex, iStr, 5, 20); 
                ConcatString(tempStr, "Indx:", maxStrLen);
                ConcatString(tempStr, iStr, maxStrLen);
            }

            for (int k = 0; k < nval; k++)
            {
                ConcatString(tempStr, "\t", maxStrLen);
                ConcatString(tempStr, currRun.caseValShortDesc[k], maxStrLen);
                ConcatString(tempStr, " ", maxStrLen);
                char valStr[40];
                rf.RealToString(currCase.caseValues[k], valStr, 40);
                ConcatString(tempStr, valStr, maxStrLen);
            }                   
            caseIDs += tempStr;
        }
    }
}

void DC_XYSimResults::GetXYIDs(SC_StringArray&  xyIDs) const
{
    xyIDs.ForceMinStringLen(DC_XYData::dataIDLen);
    xyIDs.Alloc(GetnXY());

    DC_XYDataArray& firstXY = (*this)[0][0].caseData;
    for (int i = 0; i < firstXY.Size(); i++)
        xyIDs += firstXY[i].dataID;
}

