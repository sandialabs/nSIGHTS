///////////////////////////////////////////////////////////////////////////////////
//
// C_MessageLog.h
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
// DESCRIPTION: Platform independent and application independent base claseess and
//              support classes.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_MESSAGELOG_H
#define C_MESSAGELOG_H

#include <genClass/SC_StringArray.h>

// basic class to add common logging support
// all static members -- in class only for public/private support
class MessageLogC {
public:
  enum                    {maxMessages = 5000};
  enum                    {maxMsgLen   = 120};


  static SC_StringArray   loggedMessages;

public:
  static void             AddErrorMessage(const char* errMsg);
  static void             AddInfoMessage(const char* infoMsg);
  static void             AddOtherMessage(const char* msgType,
                                          const char* otherMsg);

  static const char*      GetLastMessage();


private:
  // no reason to make or copyone
  MessageLogC() {};
  MessageLogC(const MessageLogC& a) {};

  static void            AddMessage(const char* messageType,
                                    const char* message);


};

#endif // C_MESSAGELOG_H

