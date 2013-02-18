///////////////////////////////////////////////////////////////////////////////////
//
// ListBoxCtrl.h
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
// DESCRIPTION:  Windows specific code for implementing basic controls with
//              callbacks on resource defined dialog forms.
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(LISTBOXCTRL_H)
#define LISTBOXCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <genClass/SC_StringArray.h>

#include <genCtrl/UpdateableCtrl.h>

// ListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl window

class CListBoxCtrl : public CListBox, public CUpdateableCtrl
{
    // Construction
public:
    CListBoxCtrl();
    virtual         ~CListBoxCtrl();

    //  must be called before set
    void        ClearEntries();
    void        AddBasicEntry(const SC_StringArray& allStrings);

    void        Set(CWnd* pParent, int nIDCombo, int* pcurrVal);
    void        Set(CWnd* pParent, int nIDCombo, SC_IntArray &intValArray);

    //  resets content to new strings after initial Set
    void        ResetStrings();

    //  clear, add, reset combined
    void        NewStrings(const SC_StringArray& newStrings);

    void        ResetSelection();
    void        ClearSelections();

    //  get multiple selection indexes
    void        GetMultipleSel(SC_IntArray& intValArray);

    // CUpdateableCtrl virtual
    void        DoCtrlReset();

    const char* GetListString() {return listStrings[*m_pintVal];}

protected:
    SC_StringArray  listStrings;

    int*            m_pintVal;
    SC_IntArray     m_intValArray;

    bool        multiSelection;


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CListBoxCtrl)
    //}}AFX_VIRTUAL


    // Generated message map functions
protected:
    //{{AFX_MSG(CListBoxCtrl)
    afx_msg void OnSelchange();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // LISTBOXCTRL_H

