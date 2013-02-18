///////////////////////////////////////////////////////////////////////////////////
//
// PC_Platform2DPS.h
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
//      adds capabilities for 2D plotting cursor reporting area.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PC_PLATFORM2DPS_H
#define PC_PLATFORM2DPS_H

#include <genPlotClass/PD_2D.h>

#include <genClassPS/PS_UIBase.h>
#include <genPlotClassPS/OGL_2DBase.h>
#include <genPlotClassPS/PC_PlatformPS.h>

#include <genClassPS/PS_UIBase.h>

using namespace genClassPS;

class PC_Platform2DPS : public PC_PlatformPS
{
public:
    PD_2D&              base2DplotDef;

    BaseUIC*            lastWidgetAttached;

public:
                    PC_Platform2DPS(const char* plotType, PD_2D& assPlt, OGL2DBase& ogl2DObj);
                    ~PC_Platform2DPS();

    void            InitGraphicsWindow();
    void            ResetGraphicsWindow();
    virtual void    UpdateViewSettings();  // get top view and set sliders/status appropriately

protected:
    virtual UINT    GetMenuResourceID();
    virtual void    UpdateControlBarStatus();


public:
    virtual int     CalcStatusBarHeight();
    virtual int     CalcButtonBarHeight();

    // Generated message map functions
protected:
    //{{AFX_MSG(PC_Platform2DPS)
    afx_msg void OnUpdatePlotControlbar(CCmdUI* pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // !PC_PLATFORM2DPS_H

