///////////////////////////////////////////////////////////////////////////////////
//
// SC_SetupErr.h
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
// DESCRIPTION:  support class for error string processing in SetupOK routines
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef SC_SETUPERR_H
#define SC_SETUPERR_H

class SC_SetupErr {
private:
    enum            {errTextLen = 160};  // increased from 80 for mViewX
    char            errText[errTextLen];
    char const*     parErr;

public:
    SC_SetupErr() {errText[0] = '\0'; parErr = 0;}
    SC_SetupErr(const SC_SetupErr& a);
    ~SC_SetupErr() {};
    SC_SetupErr&    operator= (const SC_SetupErr& a);

    void            SetConstantError(const char* consError) {parErr = consError;}
    void            SetInternalError(const char* nonConsError); // copies to errText
    void            SetInternalError(const char* part1,
                                     const char* part2);        // copies to errText
    void            SetInternalError() {parErr = errText;}
    const char*     GetErrorText()  const     {return parErr;}  // was errText
    bool            ErrorFound()    const     {return parErr != 0;}
    bool            NoErrors()      const     {return parErr == 0;}
    void            ClearErrors()             {parErr = 0; errText[0] = '\0';}

    //  specific error messages
    bool            NullDataErr(const char*   valDesc,
                                const double& chkVal);
    bool            OutOfRangeErr(const char*   valDesc,
                                  const double& chkVal,
                                  const double& minVal,
                                  const double& maxVal);

    bool            SetupOK(const char*        errClassID,
                            const SC_SetupErr& classErrors);  // returns true if classError.parErr = 0
    // otherwise errText contains error
private:
    void            SetErrText(const char* part1,
                               const char* part2);


};

#endif // SC_SETUPERR_H

