///////////////////////////////////////////////////////////////////////////////////
//
// PS_FormContainer.cpp
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
//              Basic abstraction of Unix X Form widget used for MFC implementation
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClassPS/PS_FormContainer.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


FormContainer::FormContainer() :
    m_pForm(NULL),
    m_pLowerForm(NULL),
    m_pUpperForm(NULL)
{
}

FormContainer::~FormContainer()
{
}

void FormContainer::SetForm(genClassPS::LayoutManager* pForm)
{
    m_pForm = pForm;
}

void FormContainer::SetLowerForm(genClassPS::FormManager* pForm, int nHeight)
{
    m_pLowerForm = pForm;
    m_nLowerFormHeight = nHeight;
}

void FormContainer::SetUpperForm(genClassPS::FormManager* pForm, int nHeight)
{
    m_pUpperForm = pForm;
    m_nUpperFormHeight = nHeight;
}

void FormContainer::ResizeForm(genClassPS::LayoutManager* pForm, int x1, int x2, int y1, int y2)
{
    pForm->Resize(x1, x2, y1, y2);
}

void FormContainer::Resize(int x1, int y1, int x2, int y2)
{
    if (m_pForm != NULL) {
        if (m_pLowerForm != NULL) {
            if (m_pUpperForm != NULL) {
                ResizeForm(m_pUpperForm, x1, x2, y1, m_nUpperFormHeight);
                ResizeForm(m_pForm, x1, x2, y1+m_nUpperFormHeight, y2-m_nLowerFormHeight);
                ResizeForm(m_pLowerForm, x1, x2, y2-m_nLowerFormHeight, y2);
            } else {
                ResizeForm(m_pForm, x1, x2, y1, y2-m_nLowerFormHeight);
                ResizeForm(m_pLowerForm, x1, x2, y2-m_nLowerFormHeight, y2);
            }
        } else {
            ResizeForm(m_pForm, x1, x2, y1, y2);
        }
    }
}

void FormContainer::ResetForm()
{
    CWnd* pWnd = GetWindow();
    if (::IsWindow(pWnd->m_hWnd)) { // 1/9/2001 RK - bug when opening new and new files
        CRect rect;
        pWnd->GetClientRect(rect);
        pWnd->SendMessage(WM_SIZE, 0, MAKELPARAM(rect.Width(), rect.Height()));
    }
}

