///////////////////////////////////////////////////////////////////////////////////
//
// TreeCtrlEx.h
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
// Programmer   : Radim Krampol,    DE&S
// Purpose      : Tree control with single/double line nodes
//
//
// Last Modification Date:  08/07/2000
//
// Revision History:
//      08/07/2000 Initial version
//      08/24/2000 virtual function OnToggleNode(HTREEITEM hItem);
//      11/07/2000 When TVS_NONEVENHEIGHT not set it will be common single-line tree
//
// Description:
//
// Custom draw tree view control
// which supports combination of single and double line nodes on level 0 and deeper
// * Initializer must set iIntegral member of
//      TV_INSERTSTRUCT item;
//      ...
//      item.itemex.iIntegral = 2;
//      OR item.itemex.iIntegral = 1;
//      InsertItem(&item);
// * For single/double line nodes combination TVS_NONEVENHEIGHT must be set
// * single/double line title tips and solved properly
//      (TVS_INFOTIP must be set and TVS_NOTOOLTIPS unset)
// * Rewrite virtual CString GetOutText(HTREEITEM hItem);
//      to return empty text for single line or text of the 2nd line in the double line case
// * Rewrite virtual OnToggleNode(HTREEITEM hItem)
//      It is called when Expand(HitTestInfo.hItem, TVE_TOGGLE) is called
// * When TVS_NONEVENHEIGHT not set it will be common single-line tree
//      iIntegral member at all tree node must be set to 1 (item.itemex.iIntegral = 1;);
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef TREECTRLEX_H
#define TREECTRLEX_H


class CTreeCtrlEx : public CTreeCtrl
{
public:
    CTreeCtrlEx();
    virtual ~CTreeCtrlEx();

protected:
    virtual CString GetOutText(HTREEITEM hItem);
    virtual void OnToggleNode(HTREEITEM hItem);

    void DrawItem(CDC& dc, HTREEITEM hItem, COLORREF clrText, const CString& sOutText, UINT uItemState);
    void DrawDisabledItem(CDC& dc, const CRect& rc, COLORREF crBackGroundColor);
    void GetItemRect2(CDC& dc, HTREEITEM hItem, CRect& rc);

    bool IsInExtendedRect(HTREEITEM hItem, CPoint pt);
protected:
    CBrush m_brushBack;
    CBrush m_brushNormal;
    CBrush m_brushGrey;
    COLORREF m_clrHilightText;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTreeCtrlEx)
    protected:
    virtual void PreSubclassWindow();
    public:
    virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:
    //{{AFX_MSG(CTreeCtrlEx)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLButtonClk(WPARAM wParam, LPARAM lParam);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // _TREECTRLEX2_H_

