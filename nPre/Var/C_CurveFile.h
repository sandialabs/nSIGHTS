///////////////////////////////////////////////////////////////////////////////////
//
// C_CurveFile.h
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
//      Class for error checking and reading curve files into DC_CurveArray structures
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_CURVEFILE_H
#define C_CURVEFILE_H

#include <genClass/C_Common.h>
#include <objClass/DC_CurveArrayIOText.h>
#include <genClass/SC_SetupErr.h>

//  class for using curve files within nPre
class CurveFile {
    public:
        char                fileName[stdFileStrLen];  // file name

    protected:
        DC_CurveArrayIOText textCurveData;            // current data -- text only for now
        DC_CurveArray*      currentCurveData;         // to support binary in future

    public:
                        CurveFile();
                        ~CurveFile();

        bool            SetupOK(SC_SetupErr& errData);      // returns true if basic setup is OK

        bool            LoadCurvesFromFile();        //returns true if OK
                                                     // if false, currentCurveData will be 0

        DC_CurveArray*  GetCurveArray() {return currentCurveData;} 

        void            SetToInitial(); 

        bool            GetIDs(SC_StringArray& currIDs) const;

    private:
                        // no copy reqd
                        CurveFile(const CurveFile& a) {};
};


#endif // C_CURVEFILE_H

