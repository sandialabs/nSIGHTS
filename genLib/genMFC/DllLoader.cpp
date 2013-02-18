///////////////////////////////////////////////////////////////////////////////////
//
// DLLLOAD.CPP
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012, Sandia National Laboratories
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sandia National Laboratories nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL SANDIA NATIONAL LABORATORIES BE LIABLE FOR ANY
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

#include "GenLibAfx.h"

#include "DllLoader.h"

#include <genClass/U_Msg.h>

DllLoad::DllLoad() :
    m_hInst(NULL)
{
}

DllLoad::~DllLoad()
{
    if (m_hInst != NULL) {
        VERIFY(AfxFreeLibrary(m_hInst));
    }
    m_hInst = NULL;
}

bool DllLoad::IsLoaded()
{
    return m_hInst != NULL;
}

void DllLoad::LoadDll(LPCSTR szEntry)
{
    if (m_hInst != NULL)
        return;

    HINSTANCE hInst = NULL;
    hInst = AfxLoadLibrary( szEntry );
    if (hInst == NULL) {
        CString sMsgBuf;
        sMsgBuf.GetBuffer( 256 );
        FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,
                     NULL,
                     GetLastError(),
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     (LPTSTR)(LPCTSTR)sMsgBuf,
                     200,
                     NULL );
        sMsgBuf.ReleaseBuffer();
        sMsgBuf += "\n";
        sMsgBuf +=  szEntry;
        if (AfxGetMainWnd() != NULL) {
            GenAppErrorMsg("DllLoad", sMsgBuf);
        }
    }
    m_hInst = hInst;
}

FARPROC DllLoad::DllProcLoad(LPCSTR szEntry)
{
    ASSERT(m_hInst != NULL);
    FARPROC pProc = GetProcAddress( m_hInst, szEntry );
    if (pProc == NULL) {
        CString sMsgBuf;
        sMsgBuf.GetBuffer( 256 );
        FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,
                     NULL,
                     GetLastError(),
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     (LPTSTR)(LPCTSTR)sMsgBuf,
                     200,
                     NULL );
        sMsgBuf.ReleaseBuffer();
        sMsgBuf += "\n";
        sMsgBuf +=  szEntry;
        if (AfxGetMainWnd() != NULL) {
            GenAppErrorMsg("DllLoad", sMsgBuf);
        }
    }
    return pProc;
}

