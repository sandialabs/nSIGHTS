///////////////////////////////////////////////////////////////////////////////////
//
// SizingControlBarGrip.h
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

/////////////////////////////////////////////////////////////////////////
//
// CSizingControlBarGrip           Version 2.43
//
// Created: Jan 24, 1998        Last Modified: August 03, 2000
//
// See the official site at www.datamekanix.com for documentation and
// the latest news.
//
/////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998-2000 by Cristi Posea. All rights reserved.
//
// This code is free for personal and commercial use, providing this
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc. to
// cristi@datamekanix.com or post them at the message board at the site.
/////////////////////////////////////////////////////////////////////////

#if !defined(__SIZINGCONTROLBARGRIP_H__)
#define __SIZINGCONTROLBARGRIP_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <genClassPS/PS_NCHitTest.h>

#include "SizingControlBar.h"

/////////////////////////////////////////////////////////////////////////
// CSCBButton (button info) helper class

class CSCBButton
{
public:
    CSCBButton();

    void Move(CPoint ptTo) {ptOrg = ptTo; };
    CRect GetRect() { return CRect(ptOrg, CSize(11, 11)); };
    void Paint(CDC* pDC);

    BOOL    bPushed;
    BOOL    bRaised;

protected:
    CPoint  ptOrg;
};

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarGrip control bar

class CSizingControlBarGrip : public CSizingControlBar
{
    DECLARE_DYNAMIC(CSizingControlBarGrip);

// Construction
public:
    CSizingControlBarGrip();

// Attributes
public:
    virtual BOOL HasGripper() const;

// Operations
public:

// Overridables
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

// Overrides
public:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSizingControlBarGrip)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CSizingControlBarGrip();

protected:
    // implementation helpers
    virtual void NcPaintGripper(CDC* pDC, CRect rcClient);
    virtual void NcCalcClient(LPRECT pRc, UINT nDockBarID);

protected:
    int     m_cyGripper;

    CSCBButton m_biHide;

// Generated message map functions
protected:
    //{{AFX_MSG(CSizingControlBarGrip)
    afx_msg NCHITRETVAL OnNcHitTest(CPoint point);
    afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

#endif // !defined(__SIZINGCONTROLBARGRIP_H__)

