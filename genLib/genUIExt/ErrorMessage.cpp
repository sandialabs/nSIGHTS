///////////////////////////////////////////////////////////////////////////////////
//
// ErrorMessage.cpp
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
//      Windows specific code for MFC user-interface extension or enhancement classes.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "ErrorMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CErrorMessage::CErrorMessage()
{
    m_dwError = ::GetLastError();
    GetCurrentMessage();
}

CErrorMessage::CErrorMessage(DWORD dwError)
{
    m_dwError = dwError;
    GetCurrentMessage();
}

CErrorMessage::~CErrorMessage()
{
}

void CErrorMessage::SetError(DWORD dwError)
{
    if (m_dwError == 0) {
        m_dwError=::GetLastError();
    } else {
        m_dwError = dwError;
    }
    GetCurrentMessage();
}

void CErrorMessage::SetError(DWORD dwError, const char* msgText)
{
    m_dwError = dwError;
    m_sMessage = msgText;
}

int CErrorMessage::MessageBox(const char* szPrefixText, const char* szSuffixText, UINT nType) const
{
    CString msg;
    if (szPrefixText != NULL) {
        msg = CString(szPrefixText) + "\n";
    }
    CString sNumber;
    sNumber.Format("Error %ld", m_dwError);
    msg += sNumber;
    msg += "\n";
    msg += m_sMessage;
    if (szSuffixText != NULL) {
        msg = msg + "\n" + szSuffixText;
    }
    return AfxMessageBox(msg, nType);
}

void CErrorMessage::GetCurrentMessage()
{
    LPTSTR lpBuf;
    if (::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,
                        NULL,
                        m_dwError,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
                        (LPTSTR)&lpBuf,
                        0,
                        NULL)!=0)
    {
        m_sMessage =    lpBuf;
        ::LocalFree(lpBuf);
    } else {
        m_sMessage.Format("Error %i occurred (no further info)", m_dwError);
    }
}


/* We don't need network error codes

#include <lmerr.h>

void PASCAL ConvertErrorToString( DWORD error_code, CString& error_string )
{
    HMODULE     hModule     = NULL; // default to system source
    LPTSTR      MessageBuffer   = NULL;
    DWORD       dwBufferLength;

    //
    // Always start off with an empty string
    //
    error_string.Empty();

    //
    // if error_code is in the network range, load the message source
    //
    if (error_code >= NERR_BASE && error_code <= MAX_NERR) {
        hModule = ::LoadLibraryEx( _TEXT("netmsg.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE );
    }

    //
    // call FormatMessage() to allow for message text to be acquired
    // from the system or the supplied module handle
    //
    dwBufferLength = ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_SYSTEM |        // always consider system table
        ((hModule != NULL) ? FORMAT_MESSAGE_FROM_HMODULE : 0),
        hModule,                    // module to get message from (NULL == system)
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // default language
        (LPTSTR) &MessageBuffer, 0, NULL );

    if ( MessageBuffer ) {
        if ( dwBufferLength )
            error_string = (LPCTSTR)MessageBuffer;

        //
        // free the buffer allocated by the system
        //
        ::LocalFree(MessageBuffer);
    }

    //
    // if you loaded a message source, unload it
    //
    if (hModule != NULL)
        ::FreeLibrary(hModule);

    if ( error_string.GetLength() == 0 ) {
        error_string.Format( TEXT( "Unknown System Error %lu" ), error_code );
    }
}
*/

