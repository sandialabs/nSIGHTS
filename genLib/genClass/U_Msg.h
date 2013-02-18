///////////////////////////////////////////////////////////////////////////////////
//
// U_Msg.h
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
// DESCRIPTION: generic messages to the UI. Definition only.  Implementation is
//              in genApp/U_MsgPS
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef U_MSG_H
#define U_MSG_H

namespace genMsgSupport {

    extern void GenAppErrorMsg(const char* msgSource,
                               const char* errMsg);

    extern void GenAppWarningMsg(const char* msgSource,
                                 const char* warnMsg);

    extern void GenAppInfoMsg(const char* msgSource,
                              const char* infoMsg);
    //  normal info messages

    extern void GenAppParameterRangeMsg(const char* minText, const char* maxText);
    // parameter values

    extern void GenAppProlificInfoMsg(const char* msgSource,
                                      const char* infoMsg);
    //  info messages such as line counts when reading files

    extern int GenAppGetMessageCount();
    // total count of displayed messages used mostly to see if any were generated

    //  mostly debug use
    extern void GenAppInternalError(const char* errMsg);  // prints msg and aborts (exit(1))

    extern void GenAppInternalWarning(const char* warnMsg); // prints msg and continues

    extern void GenAppClearMsgs();

    // UI message boxes
    extern void GenAppOKButtonDialog(const char* msgText);
    extern bool GenAppYesNoButtonDialog(const char* msgText); // true if yes button pressed
};

using namespace genMsgSupport;


#endif // U_MSG_H

