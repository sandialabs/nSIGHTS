///////////////////////////////////////////////////////////////////////////////////
//
// SC_SetupErr.cpp
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
// DESCRIPTION: support class for error string processing in SetupOK routines
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/SC_SetupErr.h>


SC_SetupErr::SC_SetupErr(const SC_SetupErr& a)
{
    CopyString(errText, a.errText, errTextLen);
    if (a.parErr == a.errText)
        parErr = errText;
    else
        parErr = a.parErr;
}
SC_SetupErr& SC_SetupErr::operator= (const SC_SetupErr& a)
{
    if (&a != this)
        {
            CopyString(errText, a.errText, errTextLen);
            if (a.parErr == a.errText)
                parErr = errText;
            else
                parErr = a.parErr;
        }
    return *this;
}


void SC_SetupErr::SetErrText(const char* part1,
                             const char* part2)
{
    CopyString(errText, part1, errTextLen);
    ConcatString(errText, part2, errTextLen);
    parErr = errText;
}


void   SC_SetupErr::SetInternalError(const char* nonConsError)
{
    CopyString(errText, nonConsError, errTextLen);
    parErr = errText;
}

void SC_SetupErr::SetInternalError(const char* part1,
                                   const char* part2)
{
    CopyString(errText, part1, errTextLen);
    ConcatString(errText, part2, errTextLen);
    parErr = errText;
}


bool SC_SetupErr::NullDataErr(const char*   valDesc,
                              const double& chkVal)
{
    if (RealIsNull(chkVal))
        {
            SetErrText(valDesc, " is not set");
            return true;
        }
    return false;
}

bool SC_SetupErr::OutOfRangeErr(const char*   valDesc,
                                const double& chkVal,
                                const double& minVal,
                                const double& maxVal)
{
    if ((!NullDataErr(valDesc, chkVal)) &&
        ((chkVal < minVal) || (chkVal > maxVal)))
        {
            SetErrText(valDesc, " is out of range");
            return true;
        }
    return false;
}

bool SC_SetupErr::SetupOK(const char*        errClassID,  // returns true if parErr = 0
                          const SC_SetupErr& classErrors)
{
    if (!classErrors.ErrorFound())
        return true;

    CopyString(errText, errClassID, errTextLen);
    ConcatString(errText, "::" , errTextLen);
    ConcatString(errText, classErrors.parErr, errTextLen);
    SetInternalError();
    return false;
}

