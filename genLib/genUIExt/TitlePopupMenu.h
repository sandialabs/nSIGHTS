///////////////////////////////////////////////////////////////////////////////////
//
// TitlePopupMenu.h
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


#if !defined(AFX_TITLEPOPUPMENU_H__96718223_7A5A_11D4_BCFE_00104B4C74FD__INCLUDED_)
#define AFX_TITLEPOPUPMENU_H__96718223_7A5A_11D4_BCFE_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
//
// Programmer   : Radim Krampol,    DE&S
// Purpose      : TitlePopupMenu - CMenu ancestor which allows to add title at the top
//
//
// Last Modification Date:  08/25/2000
//
// Revision History:
//      08/25/2000 Initial version
//
// Description:
// Title is disabled ie. user can't select it or highlight it,
// but drawn with bold font and not dimmed.
// Class uses owner draw technique.
//

class TitlePopupMenu : public CMenu
{
public:
    TitlePopupMenu();
    virtual ~TitlePopupMenu();

    void AddMenuTitle(LPSTR szTitle);

protected:
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
    HFONT CreatePopupMenuTitleFont();

protected:
    HFONT   m_hFont;
    CString m_sTitle;
};

#endif // !defined(AFX_TITLEPOPUPMENU_H__96718223_7A5A_11D4_BCFE_00104B4C74FD__INCLUDED_)

