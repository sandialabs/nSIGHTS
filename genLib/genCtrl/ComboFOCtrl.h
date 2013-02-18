///////////////////////////////////////////////////////////////////////////////////
//
// ComboFOCtrl.h
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
// DESCRIPTION: Windows specific code for implementing basic controls with
//              callbacks on resource defined dialog forms.
//              Dropdown with list of defined FuncObjC for object connection
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMBOFOCTRL_H__332C6D09_42CF_11D4_9C49_006008909B72__INCLUDED_)
#define AFX_COMBOFOCTRL_H__332C6D09_42CF_11D4_9C49_006008909B72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <genClass/SC_StringArray.h>
#include <genClass/C_FuncObj.h>

#include <genUIExt/TabbedComboBox.h>

#include <genCtrl/UpdateableCtrl.h>

// ComboIntCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComboFOCtrl window

class CComboFOCtrl : public CTabbedComboBox, public CUpdateableCtrl
{
    // Construction
public:
    CComboFOCtrl(const type_info& inType);
    virtual     ~CComboFOCtrl();

    void        Set(CWnd* pParent, int nIDCombo, FuncObjRef&  pFOVal);

    void        ResetSelection();

    // CUpdateableCtrl virtual
    void        DoCtrlReset();

    bool                    FOAvail() {return listDataOK;}
    const FuncObjRefArray&  GetListData() {return listData;}


protected:
    FuncObjRefArray     listData;
    bool                listDataOK;
    FuncObjRef*         m_pFOVal;

private:
    const type_info& listType;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CComboFOCtrl)
    //}}AFX_VIRTUAL


    // Generated message map functions
protected:
    //{{AFX_MSG(CComboFOCtrl)
    afx_msg void OnSelchange();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_COMBOFOCTRL_H__332C6D09_42CF_11D4_9C49_006008909B72__INCLUDED_

