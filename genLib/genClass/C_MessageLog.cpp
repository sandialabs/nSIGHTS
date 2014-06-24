///////////////////////////////////////////////////////////////////////////////////
//
// C_MessageLog.cpp
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

#include <time.h>
#include <genClass/U_Str.h>

#include <genClass/C_MessageLog.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before any objects in app
SC_StringArray  MessageLogC::loggedMessages;

void MessageLogC::AddErrorMessage(const char* errMsg)
{
    AddMessage("Error", errMsg);
}
void MessageLogC::AddInfoMessage(const char* infoMsg)
{
    AddMessage("Info", infoMsg);
}
void MessageLogC::AddOtherMessage(const char* msgType,
                                  const char* otherMsg)
{
    AddMessage(msgType, otherMsg);
}

void MessageLogC::AddMessage(const char* messageType,
                             const char* message)
{
    if (loggedMessages.IsEmpty())
        {
            loggedMessages.ForceMinStringLen(maxMsgLen);
            loggedMessages.Alloc(maxMessages);
        }

    if (loggedMessages.IsFull())
        {
            // keep ptr
            char* firstStr = loggedMessages[0];

            // push strings
            int nUB = loggedMessages.UpperBound();
            for (int i = 0; i < nUB; i++)
                loggedMessages[i] = loggedMessages[i + 1];

            // reset last string
            loggedMessages[nUB] = firstStr;
            loggedMessages.SetSize(nUB);
        }

    char timeStr[80];
    time_t currTime;
    if (time(&currTime) == -1)
        {
            CopyString(timeStr, "no time", 80);
        }
    else
        {
            tm localTime_s = *localtime(&currTime);
            strftime(timeStr, 80, "%H:%M:%S", &localTime_s);
        }

    int lastIndex =  loggedMessages.Size();

    MakeString(loggedMessages[lastIndex], timeStr, ": ", messageType, " : ", message, maxMsgLen);
    loggedMessages.SetSize(lastIndex + 1);
}

const char*     MessageLogC::GetLastMessage()
{
    if (loggedMessages.IsEmpty())
        return "No messages logged";
    return loggedMessages.LastIndex();
}

