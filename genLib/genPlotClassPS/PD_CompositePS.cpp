///////////////////////////////////////////////////////////////////////////////////
//
// PD_CompositePS.cpp
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
//      merges OGL_Base with PD_Composite 
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClassPS/PS_Color.h>

#include <genPlotClass/C_PlotObj.h>

#include <genPlotClassPS/PC_PlatformPS.h>

#include <genPlotClassPS/PD_CompositePS.h>


class PC_PlatformCompositePS : public PC_PlatformPS {

    public:
                    PC_PlatformCompositePS(PD_Composite& assPlt, OGLBase& oglObj);
                    ~PC_PlatformCompositePS();

            virtual void InitGraphicsWindow();


    protected:
        virtual UINT    GetMenuResourceID();
        virtual int     CalcStatusBarHeight() {return 0;}
        virtual int     CalcButtonBarHeight() {return buttonBarHeight;}

    public:

};


PC_PlatformCompositePS::PC_PlatformCompositePS(PD_Composite& assPlt, OGLBase& oglObj) :
    PC_PlatformPS("Composite", assPlt, oglObj, 26, 77, 66)
{
    m_wndStatusBar.isFixedAndVisible = false;
}

PC_PlatformCompositePS::~PC_PlatformCompositePS()
{
}

UINT PC_PlatformCompositePS::GetMenuResourceID()
{
    return IDR_VIEW_2D;
}

void PC_PlatformCompositePS::InitGraphicsWindow()
{
    PC_PlatformPS::InitGraphicsWindow();
    SetFrameIcon(IDI_COMPOSITEPLOT);

    layoutStatusBar.MenuOpen();

    initComplete = true;
    RecalcLayout();
}


PD_CompositePS :: PD_CompositePS() :
    OGLBase(*(static_cast<PD_Composite*>(this)))
{
    InitCommon();
}

PD_CompositePS :: ~PD_CompositePS ()
{
}

PD_CompositePS :: PD_CompositePS(const PD_CompositePS& a) :
    OGLBase(*(static_cast<PD_Composite*>(this))), PD_Composite(a)
{
    InitCommon();
}

void PD_CompositePS::InitCommon()
{
    platformData = new PC_PlatformCompositePS(*this, *this);
}

PD_CompositePS& PD_CompositePS::operator= (const PD_CompositePS& a)
{
    if (&a != this)
    {
        PD_Composite::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PD_CompositePS::LocalCopy(const PD_CompositePS& a)
{
    //  no properties yet
}

void PD_CompositePS::ResetSize()
{
    AdjustMargins();
    static_cast<PC_PlatformPS*>(platformData)->DoResize();
}

OGLBase* PD_CompositePS::GetOGLBase()
{
    return this;
}

void  PD_CompositePS::InitRenderer()
{
    //  switch context
    if (currOGLBase)
        currOGLBase->MakeCurrent();

    // set scissor area
    ScissorSetup();
    // and clear
    Clear();
}

void PD_CompositePS::StartLineSection()
{
}
void PD_CompositePS::EndLineSection()
{
}

void  PD_CompositePS::SetupDataView()
{
    InitView();
}

void  PD_CompositePS::SetupAnnoView()
{
    InitAnnoView();
}

void  PD_CompositePS::CloseRenderer()
{
    Flush();
}

void PD_CompositePS::DrawPlotAxes()
{
}

void PD_CompositePS::DrawPlotObjects()
{
    // draw everything else
    PD_Composite::DrawPlotObjects();

    // reset data view so viewport gets reset
    SetupDataView();

    // set hardcopy viewport but don't fill
    HardCopySetViewport(false);

    if (showLayout)
    {
        SetDrawColor(defaultPenSet->GetColor(layoutPen));
        SetLine(1);
        SetLineSolid();

        HardCopyBlockStart(pageLayout.Size() * 4);         // 4 axes + 2 caps per axes * 2

        // just draws layout spec
        for (int i = 0; i < pageLayout.Size(); i++)
        {
            PSC_PlotLayout& currLayout =  pageLayout[i];

            double xmin = currLayout.xOffset;
            double xmax = xmin + currLayout.xWidth;
            double ymin = currLayout.yOffset;
            double ymax = ymin + currLayout.yWidth;

            glBegin(GL_LINE_LOOP);
                glVertex2d(xmin, ymin);
                glVertex2d(xmax, ymin);
                glVertex2d(xmax, ymax);
                glVertex2d(xmin, ymax);
            glEnd();
        }
        HardCopyBlockEnd();
    }

}

void  PD_CompositePS::InitView()
{
    int width, height;
    CalcAvailablePixels(width, height);

    if (width < 10)
        width = 10;
    if (height < 10)
        height = 10;

    xPerPixel = 100.0 / double(width);
    yPerPixel = 100.0 / double(height);

    ClearClipPlanes();

    //  change projection to XY for annotation
    glMatrixMode(GL_PROJECTION);        //  set projection  mode
    glLoadIdentity();                   //  clear existing

    // set limits
    double xmin = -double(leftMargin) * xPerPixel;
    double xmax = 100.0 + double(rightMargin) * xPerPixel;

    double ymin = -double(bottomMargin) * yPerPixel;
    double ymax = 100.0 + double(topMargin) * yPerPixel;

    glOrtho(xmin, xmax, ymin, ymax, -1.0, 2.0);

    glViewport(0, 0, plotUwidth, plotVheight);

    // reset scissor area
    ScissorSetup();

    //  clear modelview
    //  set initial translation/rotation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void PD_CompositePS::DrawLine(const Coord3D& stPoint,
                              const Coord3D& endPoint) const
{
    glBegin(GL_LINES);
        glVertex3d(GLdouble(stPoint.cX), GLdouble(stPoint.cY), 0.0);
        glVertex3d(GLdouble(endPoint.cX), GLdouble(endPoint.cY), 0.0);
    glEnd();
}

Coord3D PD_CompositePS::GetIndependentScale(const double& inSize) const
{
    return Coord3D(xPerPixel * inSize, yPerPixel * inSize, 1.0);
}

void PD_CompositePS::SetClipPlanes()
{
}

