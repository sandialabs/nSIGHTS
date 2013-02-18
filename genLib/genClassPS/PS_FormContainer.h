///////////////////////////////////////////////////////////////////////////////////
//
// PS_FormContainer.h
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

#ifndef PS_FORMCONTAINER_H
#define PS_FORMCONTAINER_H

//
//
// Programmer   : Radim Krampol,   DE&S
// Purpose      : Basic abstraction for Form(aka menu/property page) container
//
//
// Last Modification Date:
//
// Revision History:
//
//  09/01/2000  Changed from strictly one pane implementation to up
//              to three vertical panes
//
//

#include <genClassPS/PS_UIBase.h>

class FormContainer
{
public:
    FormContainer();
    virtual ~FormContainer();

    virtual CWnd* GetWindow() = 0;
    void SetForm(genClassPS::LayoutManager*    pForm);
    void SetLowerForm(genClassPS::FormManager* pForm, int nHeight);
    void SetUpperForm(genClassPS::FormManager* pForm, int nHeight);
    void ResetForm();
    void Resize(int x1, int y1, int x2, int y2);

    // Global must be defined for application
    // eg. class CFormView : public CView, public CFormContainer
    static FormContainer* GetFormContainer();

protected:
    void ResizeForm(genClassPS::LayoutManager* pForm, int x1, int x2, int y1, int y2);

protected:
    genClassPS::LayoutManager*   m_pForm;
    genClassPS::FormManager*     m_pLowerForm;
    int                          m_nLowerFormHeight;
    genClassPS::FormManager*     m_pUpperForm;
    int                          m_nUpperFormHeight;
};


#endif // _PS_FORMCONTAINER_H_

