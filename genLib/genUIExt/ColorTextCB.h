///////////////////////////////////////////////////////////////////////////////////
//
// ColorTextCB.h
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
// Programmer   : Radim Krampol,    DE&S
// Purpose      : ComboBox which adds color rectangle to the text only items
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef COLORTEXTCB_H
#define COLORTEXTCB_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
//
//
//
// Last Modification Date:  08/01/2000
//
// Revision History:
//      08/01/2000 Initial version
//      03/05/2006 Removed unused members
//
// Description:

class CColorTextComboBox : public CComboBox
{
// Construction
public:
    CColorTextComboBox();
    virtual ~CColorTextComboBox();

    COLORREF GetSelectedColorValue();

    int AddColor(PCSTR szName, COLORREF clrColor);

// Attributes
protected:
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColorTextComboBox)
    protected:
    virtual void PreSubclassWindow();
    public:
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    //}}AFX_VIRTUAL

// Implementation
public:

    // Generated message map functions
protected:
    //{{AFX_MSG(CColorTextComboBox)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


#endif // _COLORTEXTCB_H_

