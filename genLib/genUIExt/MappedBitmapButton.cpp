///////////////////////////////////////////////////////////////////////////////////
//
// MappedBitmapButton.cpp
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

// MappedBitmapButton.cpp : implementation file
//
// Draws color-correct buttons
//
// Copyright (c) 1999 Robin J. Leatherbarrow, Erithacus Software Limited
//
// Use and distribute freely, except: don't remove my name from the
// source or documentation, mark your changes, don't alter
// or remove this notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc.,
// and I'll try to keep a version up to date.  I can be reached at:
//    robin@erithacus.com
//
// Usage -- a replacement for the MFC CBitmapButton
// 1. include an owner-draw button in your dialog
// 2. declare a CMappedBitmapButton member in the CDialog code
// 3. hook in the CMappedBitmapButton using a call to AutoLoad
//
// the bitmap resource specified in AutoLoad must be divisable into
// 4 equally sized images that represent (left to right) the
// up, down, focused and disabled states of the button


#include "MappedBitmapButton.h"

IMPLEMENT_DYNAMIC(CMappedBitmapButton, CButton)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMappedBitmapButton

CMappedBitmapButton::CMappedBitmapButton()
{
    m_idResource = (UINT)-1;
}

CMappedBitmapButton::~CMappedBitmapButton()
{
}


BEGIN_MESSAGE_MAP(CMappedBitmapButton, CButton)
    //{{AFX_MSG_MAP(CMappedBitmapButton)
    ON_WM_SYSCOLORCHANGE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CMappedBitmapButton::LoadBitmap( UINT idBitmapResource )
{
    m_image.DeleteImageList();

    m_idResource = idBitmapResource;

    CBitmap bitImage;
    if (!bitImage.LoadMappedBitmap(idBitmapResource)) {
        ASSERT( FALSE ); // failed to load the bitmap. Does this resource ID exist?
        return FALSE;
    }

    BITMAP bm;
    if (bitImage.GetBitmap(&bm) == 0) {
        return FALSE;
    }

    // we don't use a mask for the bitmaps in this case
    UINT flags = 0;
    if (bm.bmBitsPixel <= 4) {
        flags |= ILC_COLOR4;
    } else if (bm.bmBitsPixel <= 8) {
        flags |= ILC_COLOR8;
    } else if (bm.bmBitsPixel <= 16) {
        flags |= ILC_COLOR16;
    } else {
        flags |= ILC_COLOR24;
    }

    // image should contain EXACTLY four subimages
    ASSERT( bm.bmWidth % 4 == 0 );

    if (!m_image.Create(bm.bmWidth/4, bm.bmHeight, flags, 4, 0)) {
        return FALSE;
    }

    // the mask bitmap will be ignored here, so use the bitImage for both
    if (m_image.Add(&bitImage, &bitImage) == -1) {
        return FALSE;
    }

    return TRUE;
}


// SizeToContent will resize the button to the size of the bitmap
void CMappedBitmapButton::SizeToContent()
{
    ASSERT(m_image.GetImageCount() == 4);

    IMAGEINFO info;
    VERIFY( m_image.GetImageInfo( 0, &info ) );
    VERIFY(SetWindowPos(NULL, -1, -1, info.rcImage.right, info.rcImage.bottom,
        SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
}

// Autoload will load the bitmap resources
BOOL CMappedBitmapButton::AutoLoad(UINT nID, CWnd* pParent, UINT idBitmapResource)
{
    // first attach the CMappedBitmapButton to the dialog control
    if (!SubclassDlgItem(nID, pParent))
        return FALSE;

    if (!LoadBitmap(idBitmapResource))
        return FALSE;

    // size to content
    SizeToContent();
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMappedBitmapButton message handlers
void CMappedBitmapButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    ASSERT(lpDrawItemStruct != NULL);
    ASSERT(m_image.GetImageCount() == 4);

    // images are:
    // 0 = up
    // 1 = down
    // 2 = focused
    // 3 = disabled
    int nImage = 0;
    UINT state = lpDrawItemStruct->itemState;
    if (state & ODS_SELECTED) {
        nImage = 1;
    } else if (state & ODS_FOCUS) {
        nImage = 2;   // third image for focused
    } else if (state & ODS_DISABLED) {
        nImage = 3;   // last image for disabled
    }

    // draw the whole button
    CRect rect;
    rect.CopyRect(&lpDrawItemStruct->rcItem);

    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    VERIFY( m_image.Draw(pDC, nImage, rect.TopLeft(), ILD_NORMAL) );
}

void CMappedBitmapButton::OnSysColorChange()
{
    CButton::OnSysColorChange();

    if ( m_image.GetImageCount() == 4 ) {
        LoadBitmap(m_idResource);
    }

    Invalidate();
}

