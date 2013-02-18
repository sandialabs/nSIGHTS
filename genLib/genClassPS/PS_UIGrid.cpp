///////////////////////////////////////////////////////////////////////////////////
//
// PS_UIGrid.cpp
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
// DESCRIPTION: Platform specific but application independent code.
//              Implements grid as a BaseWidget derived object.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClassPS/PS_UIGrid.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace genClassPS {


    GridControl::GridControl()
    {
        if (m_bSmall)
            SetFont(menuLogFont2);
        else
            SetFont(menuLogFont);
    }

    CWnd* GridControl::GetWindow()
    {
        return this;
    }

    void GridControl::Create(const CRect& rect)
    {
        int nID = GetID();
        CWnd* pParent = GetParentWnd();
        VERIFY(CGridCtrlCell::Create(rect, pParent, nID, NULL, WS_CHILD|WS_VISIBLE));
        TRACE("GridControl %d\r\n", nID);

        AutoSize();
        SetEditMode();
    }

    void GridControl::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    void GridControl::SetSensitive(bool isSensitive)
    {
        // does not work
        EnableWindow(isSensitive);
    }
    void GridControl::DoSetSensitive(bool isSensitive)
    {
        EnableWindow(isSensitive);
    }

    // will be resizable ie. sizes doesn't matter
    int GridControl::GetWindowWidth()
    {
        return 0;
    }

    int GridControl::GetWindowHeight()
    {
        return 30;
    }

    BEGIN_MESSAGE_MAP(GridControl, CGridCtrlCell)
    //{{AFX_MSG_MAP(GridControl)
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

} // namespace
