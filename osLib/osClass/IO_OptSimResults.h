///////////////////////////////////////////////////////////////////////////////////
//
// IO_OptSimResults.h
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
//      binary file I/O for DC_OptSimResults.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef IO_OPTSIMRESULTS_H
#define IO_OPTSIMRESULTS_H

#include <genClass/C_Common.h>
#include <genClass/C_BufFile.h>

#include <osClass/DC_OptSimResults.h>


class IO_OptSimResults : public BufFileC {
    public:
        char                fileName[stdFileStrLen];

    private:
        int                 readVersion;
        int                 nRuns;
        OptSimResultsHeader currInfo;

        static const char*  fileHeader;
        static const int    writeVersion;

    public:
                            IO_OptSimResults();
                            ~IO_OptSimResults();

        bool                OKforReading();
        bool                OKtoAdd(const OptSimResultsHeader&  addHeaderInfo);

        bool                ReadSimResults(DC_OptSimResults& results);

        bool                AddSimRun(const OptSimResultsHeader&  runHeaderInfo,
                                      const OptSimRunResults&     results,
                                            int                   maxCase,      // max case in run
                                            bool                  addExisting);

        bool                AddNextCase(const OptCaseResults&   results);

    private:
        bool                ReadFileHeader(bool leaveOpen);
        void                WriteFileHeader();

        void                WriteFitComponent(const OptFitComponent&    fitComp);
        void                ReadFitComponent(OptFitComponent&    fitComp);

        void                WriteSingleFit(const OptSingleFit&       singleFit,
                                                 int&                nextRec);
        void                ReadSingleFit(OptSingleFit&       singleFit);

        void                WriteSingleCase(const OptCaseResults&     singleCase,
                                                 int&                 nextRec);
        void                ReadSingleCase(OptCaseResults&     singleCase);


        void                ReadOneRun(OptSimRunResults&     singleRun);

        void                WriteRunHeader(const OptSimRunResults&     singleRun,
                                           int                   maxCase,
                                           int&                  nextRec);  // only writes run header info

        void                WriteNextCase(const OptCaseResults&  results,
                                                int              runStartRec,
                                                int&             nextRec);


        //  not available
                            IO_OptSimResults(const IO_OptSimResults& a) : BufFileC(128) {}
        IO_OptSimResults&    operator=(const IO_OptSimResults& a) {return *this;}

};

#endif // !IO_OPTSIMRESULTS_H

