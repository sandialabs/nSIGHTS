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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <limits.h>

#include <genClass/U_Msg.h>
#include <genClass/C_MessageLog.h>
#include <genApp/GenAppMessages.h>
#include <genUIExt/ErrorMessage.h>

#include <genApp/ProjectFrame.h>

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
        CString str;
        str.Format("%s: %s", msgSource, errMsg);
        AfxGetMainWnd()->SendMessage(WM_GENAPP_ERROR, WPARAM(LPCSTR(str)));
        MessageLogC::AddErrorMessage(str);
        IncrementCount();
    }


    void GenAppWarningMsg(const char* msgSource,
                          const char* warnMsg)
    {
        CString str;
        str.Format("%s: %s", msgSource, warnMsg);
        AfxGetMainWnd()->SendMessage(WM_GENAPP_WARNING, WPARAM(LPCSTR(str)));
        IncrementCount();
    }

    void GenAppInfoMsg(const char* msgSource,
                       const char* infoMsg)
    {
        /*
          CString str;
          str.Format("%s:%s", msgSource, infoMsg);
          AfxGetMainWnd()->SendMessage(WM_GENAPP_INFO, WPARAM(LPCSTR(str)));;
        */
        // all are prolific ...
        GenAppProlificInfoMsg(msgSource, infoMsg);
    }

    void GenAppProlificInfoMsg(const char* msgSource,
                               const char* infoMsg)
    {
        IncrementCount();

        CString str;
        str.Format("%s: %s", msgSource, infoMsg);
        CWnd* pMainWnd = AfxGetMainWnd();
        ASSERT(pMainWnd != NULL);

        // klugy approach to avoid errors when called from other thread
        CProjectFrame* mainFrame = dynamic_cast<CProjectFrame*>(pMainWnd);
        if (mainFrame == NULL)
            {
                // from a worker thread -- just send a message
                pMainWnd->SendMessage(WM_GENAPP_INFO, WPARAM(LPCSTR(str)));
                return;
            }

        // in the main frame -- send it to the message bar
        CInfoBarStatus& wndMViewMessagebar = mainFrame->m_wndMViewMessagebar;
        wndMViewMessagebar.SetStatus(msInformation, LPCSTR(str));
        // redraw immediately
        wndMViewMessagebar.Invalidate();
        wndMViewMessagebar.UpdateWindow();
        TRACE("%s: %s\r\n", msgSource, infoMsg);
    }

    void GenAppInternalError(const char* errMsg)
    {
        CErrorMessage errorMessage;
        errorMessage.SetError(1000, "mView/nSights internal error");
        errorMessage.MessageBox("Internal Software Error", errMsg, MB_ICONSTOP);
        GenAppErrorMsg("Internal", errMsg);
        ASSERT(false);
        exit(1000);
    }

    void GenAppInternalWarning(const char* warnMsg)
    {
        IncrementCount();
        GenAppWarningMsg("Internal", warnMsg);
        //      ASSERT(false);
    }

    void GenAppClearMsgs()
    {
        messageCount = 0;
        AfxGetMainWnd()->SendMessage(WM_GENAPP_NONE);
        AfxGetMainWnd()->SendMessage(WM_GENAPP_RANGE, WPARAM(NULL), LPARAM(NULL));
    }

    void GenAppParameterRangeMsg(const char* minText, const char* maxText)
    {
        AfxGetMainWnd()->SendMessage(WM_GENAPP_RANGE, WPARAM(minText), LPARAM(maxText));
    }

    void GenAppOKButtonDialog(const char* msgText)
    {
        AfxMessageBox(msgText, MB_OK | MB_ICONEXCLAMATION);
    }

    bool GenAppYesNoButtonDialog(const char* msgText)
    {
        int retVal = AfxMessageBox(msgText, MB_YESNO  | MB_ICONQUESTION);

        return ((retVal != 0) && (retVal == IDYES));
    }

};

