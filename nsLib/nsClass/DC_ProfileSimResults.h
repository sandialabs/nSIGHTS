///////////////////////////////////////////////////////////////////////////////////
//
// DC_ProfileSimResults.h
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
//      class for packaging P(r) data for file I/O
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_PROFILESIMRESULTS_H
#define DC_PROFILESIMRESULTS_H

#include <objClass/DC_GridData.h>
#include <genClass/SC_StringArray.h>
#include <nsClass/DC_SequenceTimes.h>


// 2D profiles are 2D but not necessarily grids ...
// at each stored time
class ExtendedProfileTimeData {
    public:
        double          profileTime;
        SC_DoubleArray  waterTablePos;  // at each radii - not used for confined
        SC_DoubleMatrix nodalPressures; // at each radii, at each vert

    public:
        ExtendedProfileTimeData();
        ExtendedProfileTimeData(const ExtendedProfileTimeData& a);
        ExtendedProfileTimeData&   operator=(const ExtendedProfileTimeData& a);

    private:
        void    LocalCopy(const ExtendedProfileTimeData& a);

};

class ExtendedProfileGeomData {

    public:
        int ntotalVert;                  // nconstant + nvary
        int nradial;
        int nconstantVert;               // always 1 or greater
        int nvariableVert;               //
        SC_DoubleArray constantVertZPos; // alloc to nconstant vert
        SC_DoubleArray nodeRadii;

    public:
        ExtendedProfileGeomData();
        ExtendedProfileGeomData(const ExtendedProfileGeomData& a);
        ExtendedProfileGeomData&   operator=(const ExtendedProfileGeomData& a);

    private:
        void    LocalCopy(const ExtendedProfileGeomData& a);

};


class ExtendedProfile : public ExtendedProfileGeomData, public T_SC_Array<ExtendedProfileTimeData> {

    public:
        ExtendedProfile();
        ExtendedProfile(const ExtendedProfile& a);
        ExtendedProfile&   operator=(const ExtendedProfile& a);
                            // for selection menus
        void                GetTimeSteps(SC_StringArray& tsVals) const;

    private:
        void    LocalCopy(const ExtendedProfile& a);
};



// results for a single case
class ProfileSimCaseResults {
    public:
        enum                {caseIDLen = 80};
        char                caseID[caseIDLen];

        double              staticPressure;  // always output for each case
        SC_DoubleArray      caseValues;      // sim varying values -- may be empty

        bool                caseDataIsExtended;
        DC_GridData         caseData;           // the actual output for 1D case
        ExtendedProfile     extendedCaseData;   // for 2D profiles

    public:
        ProfileSimCaseResults();
        ProfileSimCaseResults(const ProfileSimCaseResults& a);
        ~ProfileSimCaseResults();

        ProfileSimCaseResults&   operator=(const ProfileSimCaseResults& a);
        int                 GetnProfile() const {return caseData.Size();}

    private:
        void                LocalCopy(const ProfileSimCaseResults& a);
};


class ProfileSimRunResults : public T_SC_Array<ProfileSimCaseResults> {
    public:
        enum                {runIDLen = 40};
        char                runID[runIDLen];
        SC_StringArray      caseValDesc;         // descriptions for case varying values
        SC_StringArray      caseValShortDesc;    // abbreviated descriptions for case varying values
        DC_SequenceTimes    caseSeqData;         // sequences for run

    public:
        ProfileSimRunResults();
        ProfileSimRunResults(const ProfileSimRunResults& a);
        ~ProfileSimRunResults();
        ProfileSimRunResults&    operator=(const ProfileSimRunResults& a);

        int                 GetnCases() const {return Size();}
        int                 GetnProfile() const;
        void                GetCaseIDs(SC_StringArray&  caseIDs) const;

    private:
        void                LocalCopy(const ProfileSimRunResults& a);
};


class DC_ProfileSimResults : public T_SC_Array<ProfileSimRunResults> {


    public:
        DC_ProfileSimResults();
        DC_ProfileSimResults(const DC_ProfileSimResults& a);
        ~DC_ProfileSimResults();

        DC_ProfileSimResults&    operator=(const DC_ProfileSimResults& a);

        int                 GetnRuns() const {return Size();}
        int                 GetnCases() const;
        int                 GetnProfile() const;

        void                GetRunIDs(SC_StringArray&  runIDs) const;
        void                GetCaseIDs(SC_StringArray&  caseIDs) const;

    private:
        void                LocalCopy(const DC_ProfileSimResults& a);

};

#endif // DC_PROFILESIMRESULTS_H

