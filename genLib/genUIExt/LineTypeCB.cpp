///////////////////////////////////////////////////////////////////////////////////
//
// LineTypeCB.cpp
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

#include <genClass/U_Real.h>

#include "LineTypeCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLineTypeComboBox::CLineTypeComboBox()
{
    lineColor = RGB(00,00,00);
    lineThick = 1;
}


CLineTypeComboBox::~CLineTypeComboBox()
{
}


BEGIN_MESSAGE_MAP(CLineTypeComboBox, CComboBox)
    //{{AFX_MSG_MAP(CLineTypeComboBox)
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineTypeComboBox message handlers

int CLineTypeComboBox::OnCreate(LPCREATESTRUCT pCStruct)
{
    if (CComboBox::OnCreate(pCStruct) == -1) {
        return -1;
    }
    ASSERT((pCStruct->style & (CBS_HASSTRINGS|CBS_OWNERDRAWFIXED)) != 0);
    return 0;
}

// In the case of dialog created from resources PreSubclassWindow
// is the only chance to check style
void CLineTypeComboBox::PreSubclassWindow()
{
    CComboBox::PreSubclassWindow();
    ASSERT((GetStyle() & (CBS_HASSTRINGS|CBS_OWNERDRAWFIXED)) != 0);
}

// MeasureItem isn't called, when owner-draw combo box is created
// from resources
void CLineTypeComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
    lpMIS->itemHeight = 2 + 12 + 2;
}


void CLineTypeComboBox::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
    CRect rItemRect = pDIStruct->rcItem;

    COLORREF clrNormal   = ::GetSysColor(COLOR_WINDOW);
    COLORREF clrSelected = ::GetSysColor(COLOR_HIGHLIGHT);
    COLORREF drawColor   = lineColor;

    CDC dcContext;
    if (!dcContext.Attach(pDIStruct->hDC)) {
        return;
    }

    int iState = pDIStruct->itemState;
    if (iState & ODS_SELECTED)
    {
        drawColor = 0x00FFFFFF & ~(drawColor);
        dcContext.SetBkColor(clrSelected);
        dcContext.FillSolidRect(&rItemRect, clrSelected);
    }
    else
    {
        dcContext.SetBkColor(clrNormal);
        dcContext.FillSolidRect(&rItemRect, clrNormal);
    }

    if (iState & ODS_FOCUS) {
        dcContext.DrawFocusRect(&rItemRect);
    }

    int iItem = pDIStruct->itemID;
    if (iItem != -1)
    {
        dcContext.SetBkMode(TRANSPARENT);

        if (iState & ODS_DISABLED)
        {
            drawColor = ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);
        }

        dcContext.SelectObject(GetStockObject(DC_PEN));
        dcContext.SetDCPenColor(drawColor);

        rItemRect.DeflateRect(4, 0);

        int rectMid = (rItemRect.top + rItemRect.bottom) / 2;

        unsigned short basePattern = lineTypeData.GetLineTypeSpec(iItem).linePattern;
        int factor = lineTypeData.GetLineTypeSpec(iItem).lineFactor;
        int rectWidth = rItemRect.Width();

        int maxLThick = IMin(rItemRect.bottom - rItemRect.top - 2, lineThick);

        rectMid -= maxLThick / 2;
        for (int i = 0; i < maxLThick; i++)
        {
            int linePos = 0;
            bool penOn = false;
            int bitPos = 0;
            unsigned short pattern = basePattern;
            while (linePos < rectWidth) {

                if (pattern & 0x0001)
                {
                    if (!penOn)
                    {
                        penOn = true;
                        dcContext.MoveTo(rItemRect.left + linePos, rectMid);
                    }
                }
                else
                {
                    if (penOn)
                    {
                        penOn = false;
                        dcContext.LineTo(rItemRect.left + linePos, rectMid);
                    }
                }

                linePos += factor;
                bitPos++;
                if (bitPos == 16)
                {
                    pattern = basePattern;
                    bitPos = 0;
                }
                else
                    pattern = (pattern >> 1);
            }

            if (penOn)
                dcContext.LineTo(rItemRect.right, rectMid);
            rectMid++;
        }
    }



    dcContext.Detach();
}

int CLineTypeComboBox::AddLineType(PlotLineType plType)
{
    int iItem = InsertString(-1, lineTypeData.GetLineTypeSpec(plType).patName);
    if ( iItem != LB_ERR ) {
        if (SetItemData(iItem, plType) != CB_ERR && iItem == 0) {
            SetCurSel(0);
        }
    }
    return iItem;
}


