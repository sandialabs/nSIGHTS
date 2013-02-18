///////////////////////////////////////////////////////////////////////////////////
//
// U_MsgPS.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <limits.h>
#include <iostream>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/C_MessageLog.h>
#include <genClass/C_MPISupport.h>

namespace genMsgSupport {

    static int messageCount = 0;

    int GenAppGetMessageCount()
    {
        return messageCount;
    }

    static void IncrementCount()
    {
        if (messageCount == INT_MAX)
            messageCount = 0;

        messageCount++;
    }

    void GenAppErrorMsg(const char* msgSource,
                       const char* errMsg)
    {
        char outStr[255];
        MakeString(outStr, "ERR:", msgSource, ":", errMsg, 255);
        std::cout << outStr << std::endl;
        //_flushall();

        MessageLogC::AddErrorMessage(outStr);
        IncrementCount();
    }


    void GenAppWarningMsg(const char* msgSource,
                         const char* warnMsg)
    {
        char outStr[255];
        MakeString(outStr, "WARN:", msgSource, ":", warnMsg, 255);
        std::cout << outStr << std::endl;
        //_flushall();

        IncrementCount();
    }

    void GenAppInfoMsg(const char* msgSource,
                      const char* infoMsg)
    {
        IncrementCount();
        char outStr[255];
        MakeString(outStr, "INFO:", msgSource, ":", infoMsg, 255);
        if (NodeFile::mpiNodeRank > -1)
        {
            if (NodeFile::mpiDebugRun)
                std::cout << NodeFile::mpiNodeRank << ": " << outStr << std::endl;
        }
        else
            std::cout << outStr << std::endl;
        //_flushall();

    }

    // static const char* lastSource = 0;

    void GenAppProlificInfoMsg(const char* msgSource,
                              const char* infoMsg)
    {
    /*  if (lastSource != msgSource)
        {
            GenAppInfoMsg(msgSource, infoMsg);
            lastSource = msgSource;
        }
    */
    }

    void GenAppInternalError(const char* errMsg)
    {
        std::cout << "FATAL:" << NodeFile::mpiNodeRank << " : " << errMsg << std::endl;
        //_flushall();

        exit(1000);
    }

    void GenAppInternalWarning(const char* warnMsg)
    {
        IncrementCount();
        GenAppWarningMsg("Internal", warnMsg);
    //  ASSERT(false);
    }

    void GenAppClearMsgs()
    {
        messageCount = 0;
    }


    void GenAppParameterRangeMsg(const char* minText, const char* maxText)
    {
        std::cout << "??RANGEMSG??" << minText << maxText << std::endl;
        exit(1000);
    }

    void GenAppOKButtonDialog(const char* msgText)
    {
        GenAppInternalError("GenAppOKButtonDialog");
    }

    bool GenAppYesNoButtonDialog(const char* msgText)
    {
        GenAppInternalError("GenAppYesNoButtonDialog");
        return false;
    }

};

