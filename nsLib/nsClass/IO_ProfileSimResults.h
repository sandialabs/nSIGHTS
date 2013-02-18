///////////////////////////////////////////////////////////////////////////////////
//
// IO_ProfileSimResults.h
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
//      File I/O for DC_ProfileSimResults
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef IO_PROFILESIMRESULTS_H
#define IO_PROFILESIMRESULTS_H

#include <genClass/C_Common.h>
#include <genClass/C_BufFile.h>

#include <nsClass/DC_ProfileSimResults.h>


class IO_ProfileSimResults : public BufFileC {
    public:
        char                fileName[stdFileStrLen];

    private:
        int                 readVersion;
        int                 nRuns;

        static const char*  fileHeader;
        static const int    writeVersion;

    public:
                            IO_ProfileSimResults();
                            ~IO_ProfileSimResults();

        bool                OKforReading();

        bool                ReadSimResults(DC_ProfileSimResults& results);

        bool                AddSimRun(const ProfileSimRunResults& results,
                                            bool             addExisting);

    private:
        bool                ReadFileHeader(bool leaveOpen);
        void                ReadOneCase(ProfileSimCaseResults& caseRes);
        void                ReadOneRun(ProfileSimRunResults& caseRes);

        void                WriteOneCase(const ProfileSimCaseResults& caseRes,
                                               int&              nextRec);
        void                WriteOneRun(const ProfileSimRunResults& runRes,
                                               int&              nextRec);

        //  not available
        IO_ProfileSimResults(const IO_ProfileSimResults& a) : BufFileC(128) {}
        IO_ProfileSimResults&    operator=(const IO_ProfileSimResults& a) {return *this;}

};

#endif //IO_PROFILESIMRESULTS_H

