///////////////////////////////////////////////////////////////////////////////////
//
// BitmapMenu.h
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

#if !defined(AFX_BITMAPMENU_H__BB6F2F01_91FA_11D1_8B78_0000B43382FE__INCLUDED_)
#define AFX_BITMAPMENU_H__BB6F2F01_91FA_11D1_8B78_0000B43382FE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CBitmapMenu : public CMenu
{
public:
    struct ToolBarData  // struct for toolbar resource
    {
        WORD wVersion;
        WORD wWidth;
        WORD wHeight;
        WORD wItemCount;
    };
    struct SpawnItem
    {
        int iImageIdx;
        int iCmd;
        char cText[128];
    };
    struct ImageItem
    {
        int iImageIdx;
        int iCmd;
    };
public:
    CBitmapMenu();
    virtual ~CBitmapMenu();


    virtual bool ExcludeItem(int nItemID); // if you need exclude some menu items

    CSize szImage;
    void DrawDisabledButton( CDC& dc, const CRect& rc ) const;
    int FindImageItem(int cmd);
    SpawnItem* AddSpawnItem(const char* txt, int cmd);
    void RemapMenu(CMenu* pMenu);
    void AddImageItem(int idx, WORD cmd);

    bool LoadToolBarResource(unsigned int resId);
    bool AddToolBarResource(unsigned int resId);
    void EnableMenuItems(CMenu* pMenu, CWnd* pParent);

    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpm);
    virtual void DrawItem(LPDRAWITEMSTRUCT lp);
protected:
    HFONT       m_hMenuFont;
    CImageList  ilList;
    CImageList  ilOther;

    int iSpawnItem;
    SpawnItem** pSpawnItem;

    int iImageItem;
    ImageItem* pImageItem;
};

#endif // !defined(AFX_BITMAPMENU_H__BB6F2F01_91FA_11D1_8B78_0000B43382FE__INCLUDED_)

