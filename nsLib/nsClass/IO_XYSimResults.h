///////////////////////////////////////////////////////////////////////////////////
//
// IO_XYSimResults.h
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
//      File I/O for DC_XYSimResults
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef IO_XYSIMRESULTS_H
#define IO_XYSIMRESULTS_H

#include <genClass/C_Common.h>
#include <genClass/C_BufFile.h>

#include <nsClass/DC_XYSimResults.h>


    class IO_XYSimResults : public BufFileC {
        public:
            char                fileName[stdFileStrLen];

        private:
            int                 readVersion;
            int                 nRuns;

            //  all runs in file must share these characteristics
            int                 nxyData;     
            int                 ncaseValues;     

            static const char*  fileHeader;
            static const int    writeVersion;

        public:
                                IO_XYSimResults();
                                ~IO_XYSimResults();

            bool                OKforReading();
            bool                OKtoAdd(int nxyIn, int ncaseIn);

            bool                ReadSimResults(DC_XYSimResults& results);

            bool                AddSimRun(const XYSimRunResults& results,
                                                bool             addExisting);

        private:
            bool                IsCompatible(int nxyIn, int ncaseIn);
            bool                ReadFileHeader(bool leaveOpen);
            void                ReadOneCase(XYSimCaseResults& caseRes);
            void                ReadOneRun(XYSimRunResults& caseRes);

            void                WriteFileHeader();

            void                WriteOneCase(const XYSimCaseResults& caseRes,
                                                   int&              nextRec);
            void                WriteOneRun(const XYSimRunResults& runRes,
                                                   int&              nextRec);

            //  not available
                                IO_XYSimResults(const IO_XYSimResults& a) : BufFileC(128) {}
            IO_XYSimResults&    operator=(const IO_XYSimResults& a) {return *this;}

    };

#endif //IO_XYSIMRESULTS_H

