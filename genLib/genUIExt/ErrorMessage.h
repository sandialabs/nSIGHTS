///////////////////////////////////////////////////////////////////////////////////
//
// ErrorMessage.h
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
// Purpose      : Wrapper for error message
//
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERRORMESSAGE_H__51CAC9F1_9ECE_11D1_A06C_0000832CDDC7__INCLUDED_)
#define AFX_ERRORMESSAGE_H__51CAC9F1_9ECE_11D1_A06C_0000832CDDC7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
//
// Programmer   : Radim Krampol,    DE&S
//
//
// Last Modification Date:  09/03/2000
//
// Revision History:
//      09/01/2000 Initial version
//
// Description:

class CErrorMessage
{
public:
    // calls GetLastError to get the current errorcode
    CErrorMessage();
    // specify your own errorcode
    CErrorMessage(DWORD dwError);
    ~CErrorMessage();

    //  setup a new error (0=call GetLastError)
    void SetError(DWORD dwError = 0);
    void SetError(DWORD dwError, const char* msgText);

    CString GetMessage() const;
    DWORD   GetMessageCode() const;

    //  AfxMessageBox with the text pfxtext + "\n" + m_sMessage + "\n" + suftext
    int MessageBox(const char* szPrefixText=NULL, const char* szSuffixText=NULL, UINT nType=MB_OK) const;
protected:
    DWORD   m_dwError;
    CString m_sMessage;

    //  get the message string from the system
    void    GetCurrentMessage();

};

inline CString  CErrorMessage::GetMessage() const
{
    return m_sMessage;
}

inline DWORD CErrorMessage::GetMessageCode() const
{
    return m_dwError;
}

#endif // !defined(AFX_ERRORMESSAGE_H__51CAC9F1_9ECE_11D1_A06C_0000832CDDC7__INCLUDED_)



