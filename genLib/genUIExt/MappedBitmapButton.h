///////////////////////////////////////////////////////////////////////////////////
//
// MappedBitmapButton.h
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

// MappedBitmapButton.h : header file
//

#ifndef __MappedBitmapButton_h__
#define __MappedBitmapButton_h__

/////////////////////////////////////////////////////////////////////////////
// CMappedBitmapButton window

// Usage -- a replacement for the MFC CBitmapButton
// 1. include an owner-draw button in your dialog
// 2. declare a CMappedBitmapButton member in the CDialog code
// 3. hook in the CMappedBitmapButton using a call to AutoLoad
//
// the bitmap resource specified in AutoLoad must be divisable into
// 4 equally sized images that represent (left to right) the
// up, down, focused and disabled states of the button

class CMappedBitmapButton : public CButton
{
// Construction
public:
DECLARE_DYNAMIC( CMappedBitmapButton )
    CMappedBitmapButton();

// Attributes
public:

protected:
    CImageList  m_image;
    UINT        m_idResource;

// Operations
public:
    BOOL LoadBitmap( UINT idBitmapResource );
    void SizeToContent();
    BOOL AutoLoad(UINT nID, CWnd* pParent, UINT idBitmapResource);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMappedBitmapButton)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CMappedBitmapButton();

    // Generated message map functions
protected:
    //{{AFX_MSG(CMappedBitmapButton)
    afx_msg void OnSysColorChange();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif //#ifndef __MappedBitmapButton_h__

