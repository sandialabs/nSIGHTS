///////////////////////////////////////////////////////////////////////////////////
//
// SymbolTypeCB.cpp
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

#include "SymbolTypeCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSymbolTypeComboBox::CSymbolTypeComboBox()
{
    symColor = RGB(00,00,00);
    symSize = 12;
    symLThick = 1;
}


CSymbolTypeComboBox::~CSymbolTypeComboBox()
{
}


BEGIN_MESSAGE_MAP(CSymbolTypeComboBox, CComboBox)
    //{{AFX_MSG_MAP(CSymbolTypeComboBox)
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSymbolTypeComboBox message handlers

int CSymbolTypeComboBox::OnCreate(LPCREATESTRUCT pCStruct)
{
    if (CComboBox::OnCreate(pCStruct) == -1) {
        return -1;
    }
    ASSERT((pCStruct->style & (CBS_HASSTRINGS|CBS_OWNERDRAWFIXED)) != 0);
    return 0;
}

// In the case of dialog created from resources PreSubclassWindow
// is the only chance to check style
void CSymbolTypeComboBox::PreSubclassWindow()
{
    CComboBox::PreSubclassWindow();
    ASSERT((GetStyle() & (CBS_HASSTRINGS|CBS_OWNERDRAWFIXED)) != 0);
}

// MeasureItem isn't called, when owner-draw combo box is created
// from resources
void CSymbolTypeComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
    lpMIS->itemHeight = 2 + 12 + 2;
}


void CSymbolTypeComboBox::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
    CRect rItemRect = pDIStruct->rcItem;

    COLORREF clrNormal   = ::GetSysColor(COLOR_WINDOW);
    COLORREF clrSelected = ::GetSysColor(COLOR_HIGHLIGHT);
    COLORREF drawColor   = symColor;

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

        PlotSymbol currSym = PlotSymbol(iItem);
        bool filledSym = PlotSymbolOps::SymbolIsFilled(currSym);

        rItemRect.DeflateRect(4, 0);
        int lThick = IMin(rItemRect.bottom - rItemRect.top - 2, symLThick);

        if (filledSym)
            lThick = 1;
        CPen drawPen(PS_SOLID, lThick, drawColor);
        CPen* pOldPen = dcContext.SelectObject(&drawPen);

        CBrush brush(drawColor);
        CBrush* pOldBrush = dcContext.SelectObject(&brush);

        int drawSymSize  = IMin((rItemRect.bottom - rItemRect.top), symSize);

        int rectMidY = (rItemRect.top + rItemRect.bottom) / 2;
        int rectMidX = (rItemRect.left + rItemRect.right) / 2;

        switch (currSym) {
            case ps_X : {
                drawSymSize /= 2;
                dcContext.MoveTo(rectMidX - drawSymSize, rectMidY - drawSymSize);
                dcContext.LineTo(rectMidX + drawSymSize, rectMidY + drawSymSize);
                dcContext.MoveTo(rectMidX + drawSymSize, rectMidY - drawSymSize);
                dcContext.LineTo(rectMidX - drawSymSize, rectMidY + drawSymSize);
                break;
            }
            case ps_Plus : {
                drawSymSize /= 2;
                dcContext.MoveTo(rectMidX - drawSymSize, rectMidY);
                dcContext.LineTo(rectMidX + drawSymSize, rectMidY);
                dcContext.MoveTo(rectMidX, rectMidY - drawSymSize);
                dcContext.LineTo(rectMidX, rectMidY + drawSymSize);
                break;
            }
            case ps_Square : case ps_FSquare :{

                drawSymSize /= 2;
                CPoint pts[4];
                pts[0].x = rectMidX - drawSymSize;
                pts[0].y = rectMidY - drawSymSize;
                pts[1].x = rectMidX + drawSymSize;
                pts[1].y = rectMidY - drawSymSize;
                pts[2].x = rectMidX + drawSymSize;
                pts[2].y = rectMidY + drawSymSize;
                pts[3].x = rectMidX - drawSymSize;
                pts[3].y = rectMidY + drawSymSize;
                DrawSymbol(dcContext, pts, 4, filledSym);

                break;
            }
            case ps_Circle : case ps_FCircle: {
                static const Point2D cicoords[] = {Point2D(0.00000 ,0.50000),
                                               Point2D(0.25000 ,0.43301),
                                               Point2D(0.43301 ,0.25000),
                                               Point2D(0.50000 ,0.00000),
                                               Point2D(0.43301 ,-0.25000),
                                               Point2D(0.25000 ,-0.43301),
                                               Point2D(0.00000 ,-0.50000),
                                               Point2D(-0.25000,-0.43301),
                                               Point2D(-0.43301,-0.25000),
                                               Point2D(-0.50000,0.00000),
                                               Point2D(-0.43301,0.25000),
                                               Point2D(-0.25000,0.43301)};

                CPoint pts[12];

                for (int i = 0; i < 12; i++)
                {
                    int offX = int(double(drawSymSize) * cicoords[i].pX);
                    int offY = int(double(drawSymSize) * cicoords[i].pY);
                    pts[i].x = rectMidX + offX;
                    pts[i].y = rectMidY + offY;
                }

                DrawSymbol(dcContext, pts, 12, filledSym);
                break;
            }
            case ps_Triangle : case ps_FTriangle : {

                int offX = int(double(drawSymSize) * 0.4330);
                int offY = int(double(drawSymSize) * 0.25);

                CPoint pts[3];
                pts[0].x = rectMidX - offX;
                pts[0].y = rectMidY + offY;
                pts[1].x = rectMidX + offX;
                pts[1].y = rectMidY + offY;
                pts[2].x = rectMidX;
                pts[2].y = rectMidY - drawSymSize / 2;
                DrawSymbol(dcContext, pts, 3, filledSym);
                break;
            }
            case ps_Diamond : case ps_FDiamond : {
                drawSymSize /= 2;

                CPoint pts[4];
                pts[0].x = rectMidX;
                pts[0].y = rectMidY - drawSymSize;
                pts[1].x = rectMidX + drawSymSize;
                pts[1].y = rectMidY;
                pts[2].x = rectMidX;
                pts[2].y = rectMidY + drawSymSize;
                pts[3].x = rectMidX - drawSymSize;
                pts[3].y = rectMidY;
                DrawSymbol(dcContext, pts, 4, filledSym);
                break;
            }

        }
    }


    dcContext.Detach();
}

void CSymbolTypeComboBox::DrawSymbol(CDC&   dcContext,
                             LPPOINT lpPoints,
                             int npts,
                             bool filledSym)

{
    if (filledSym)
    {
        dcContext.SetPolyFillMode(WINDING);
        dcContext.Polygon(lpPoints, npts);
    }
    else
    {
        dcContext.MoveTo(lpPoints[0].x, lpPoints[0].y);
        for (int i = 1; i < npts; i++)
            dcContext.LineTo(lpPoints[i].x, lpPoints[i].y);
        dcContext.LineTo(lpPoints[0].x, lpPoints[0].y);
    }
}

int CSymbolTypeComboBox::AddSymbol(PlotSymbol plType)
{
    int iItem = InsertString(-1, "dummy");
    if ( iItem != LB_ERR ) {
        if (SetItemData(iItem, plType) != CB_ERR && iItem == 0) {
            SetCurSel(0);
        }
    }
    return iItem;
}


