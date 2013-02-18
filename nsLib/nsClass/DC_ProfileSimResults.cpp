///////////////////////////////////////////////////////////////////////////////////
//
// DC_ProfileSimResults.cpp
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
//      Class for packaging P(r) data for file I/O
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/SC_RealFormat.h>

#include <nsClass/DC_ProfileSimResults.h>

ExtendedProfileTimeData::ExtendedProfileTimeData()
{
    profileTime = 0.0;
}

ExtendedProfileTimeData::ExtendedProfileTimeData(const ExtendedProfileTimeData& a)
{
    LocalCopy(a);
}
ExtendedProfileTimeData&  ExtendedProfileTimeData::operator=(const ExtendedProfileTimeData& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void ExtendedProfileTimeData::LocalCopy(const ExtendedProfileTimeData& a)
{
    waterTablePos = a.waterTablePos;
    nodalPressures = a.nodalPressures;
    profileTime = a.profileTime;
}

ExtendedProfileGeomData::ExtendedProfileGeomData()
{
    ntotalVert = 0;
    nradial = 0;
    nconstantVert = 0;
    nvariableVert = 0;
}

ExtendedProfileGeomData::ExtendedProfileGeomData(const ExtendedProfileGeomData& a)
{
    LocalCopy(a);
}
ExtendedProfileGeomData&  ExtendedProfileGeomData::operator=(const ExtendedProfileGeomData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void ExtendedProfileGeomData::LocalCopy(const ExtendedProfileGeomData& a)
{
    ntotalVert = a.ntotalVert;
    nradial = a.nradial;
    nconstantVert = a.nconstantVert;
    nvariableVert = a.nvariableVert;
    constantVertZPos = a.constantVertZPos;
    nodeRadii = a.nodeRadii;
}

ExtendedProfile::ExtendedProfile()
{
}

ExtendedProfile::ExtendedProfile(const ExtendedProfile& a)
    : T_SC_Array<ExtendedProfileTimeData>(a), ExtendedProfileGeomData(a)
{
    LocalCopy(a);
}
ExtendedProfile&  ExtendedProfile::operator=(const ExtendedProfile& a)
{
    if (&a != this)
    {
        T_SC_Array<ExtendedProfileTimeData>::operator=(a);
        ExtendedProfileGeomData::operator =(a);
        LocalCopy(a);
    }
    return *this;
}

void ExtendedProfile::LocalCopy(const ExtendedProfile& a)
{
}


void ExtendedProfile::GetTimeSteps(SC_StringArray& tsVals) const
{
    tsVals.Alloc(Size());

    SC_RealFormat rForm;
    for (int i = 0; i < Size(); i++)
    {
        char tempStr[40];
        rForm.RealToString((*this)[i].profileTime, tempStr, 40);
        tsVals += tempStr;
    }

}


ProfileSimCaseResults::ProfileSimCaseResults()
{
    caseID[0] = '\0';
    staticPressure = 0.0;
    caseDataIsExtended = false;
}

ProfileSimCaseResults::ProfileSimCaseResults(const ProfileSimCaseResults& a)
{
    LocalCopy(a);
}

ProfileSimCaseResults::~ProfileSimCaseResults()
{
}

ProfileSimCaseResults& ProfileSimCaseResults::operator=(const ProfileSimCaseResults& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void ProfileSimCaseResults::LocalCopy(const ProfileSimCaseResults& a)
{
    CopyString(caseID, a.caseID, caseIDLen);
    caseValues      = a.caseValues;
    caseData        = a.caseData;
    staticPressure  = a.staticPressure;
    caseDataIsExtended = a.caseDataIsExtended;
    extendedCaseData = a.extendedCaseData;
}


ProfileSimRunResults::ProfileSimRunResults() : T_SC_Array<ProfileSimCaseResults>()
{
    runID[0] = '\0';
    caseValDesc.SetStringLen(80);
    caseValShortDesc.SetStringLen(40);
}

ProfileSimRunResults::ProfileSimRunResults(const ProfileSimRunResults& a) :
    T_SC_Array<ProfileSimCaseResults>(a)
{
    LocalCopy(a);
}

ProfileSimRunResults::~ProfileSimRunResults()
{
}

ProfileSimRunResults& ProfileSimRunResults::operator=(const ProfileSimRunResults& a)
{
    if (&a != this)
    {
        T_SC_Array<ProfileSimCaseResults>::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void ProfileSimRunResults::LocalCopy(const ProfileSimRunResults& a)
{
    CopyString(runID, a.runID, runIDLen);
    caseValDesc      = a.caseValDesc;
    caseValShortDesc = a.caseValShortDesc;
    caseSeqData      = a.caseSeqData;
}

void ProfileSimRunResults::GetCaseIDs(SC_StringArray&  caseIDs) const
{
    caseIDs.DeAlloc();
    caseIDs.SetStringLen(ProfileSimCaseResults::caseIDLen);
    caseIDs.Alloc(Size());
    for (int i = 0; i < Size(); i++)
        caseIDs += (*this)[i].caseID;
};

int  ProfileSimRunResults::GetnProfile() const
{
    int nPro = 0;
    for (int i = 0; i < Size(); i++)
        nPro += (*this)[i].GetnProfile();
    return nPro;
}


DC_ProfileSimResults::DC_ProfileSimResults() : T_SC_Array<ProfileSimRunResults>()
{
}

DC_ProfileSimResults::DC_ProfileSimResults(const DC_ProfileSimResults& a) :
    T_SC_Array<ProfileSimRunResults>(a)
{
    LocalCopy(a);
}

DC_ProfileSimResults::~DC_ProfileSimResults()
{
}

DC_ProfileSimResults& DC_ProfileSimResults::operator=(const DC_ProfileSimResults& a)
{
    if (&a != this)
    {
        T_SC_Array<ProfileSimRunResults>::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void DC_ProfileSimResults::LocalCopy(const DC_ProfileSimResults& a)
{
}

void  DC_ProfileSimResults::GetRunIDs(SC_StringArray&  runIDs) const
{
    runIDs.ForceMinStringLen(ProfileSimRunResults::runIDLen);
    runIDs.Alloc(Size());
    for (int i = 0; i < Size(); i++)
        runIDs += (*this)[i].runID;
};

int DC_ProfileSimResults::GetnCases() const
{
    int nCase = 0;
    for (int i = 0; i < Size(); i++)
        nCase += (*this)[i].Size();
    return nCase;
}


int  DC_ProfileSimResults::GetnProfile() const
{
    int nPro = 0;
    for (int i = 0; i < Size(); i++)
        nPro += (*this)[i].GetnProfile();
    return nPro;
}

void DC_ProfileSimResults::GetCaseIDs(SC_StringArray&  caseIDs) const
{
    static const int maxStrLen = ProfileSimRunResults::runIDLen + 1 + ProfileSimCaseResults::caseIDLen;
    caseIDs.ForceMinStringLen(maxStrLen);
    caseIDs.Alloc(GetnCases());
    char tempStr[maxStrLen];

    for (int i = 0; i < Size(); i++)
    {
        ProfileSimRunResults& currRun = (*this)[i];
        for (int j = 0; j < currRun.Size(); j++)
        {
            CopyString(tempStr, currRun.runID, maxStrLen);
            ConcatString(tempStr, "\t", maxStrLen);
            ConcatString(tempStr, currRun[j].caseID, maxStrLen);
            caseIDs += tempStr;
        }
    }
}

