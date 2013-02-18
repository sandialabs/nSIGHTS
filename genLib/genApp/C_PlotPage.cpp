///////////////////////////////////////////////////////////////////////////////////
//
// C_PlotPage.cpp
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              Page type for plot objects and child plot folder pages.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClass/U_Msg.h>

#include <genPlotClass/C_PlotDef.h>
#include <genPlotClass/C_PlotObj.h>
#include <genPlotClass/C_AnnoObj.h>
#include <genPlotClass/PD_Composite.h>
#include <genPlotClassPS/PC_PlatformPS.h>

#include <genPPD/PPD_2DXY.h>
#include <genPPD/PPD_2DPlotAnno.h>
#include <genPPD/PPD_LinLogAxis.h>

#include <genPPD/PPD_3DXYZ.h>
#include <genPPD/PPD_3DAxesFormat.h>
#include <genPPD/PPD_3DAxesLabel.h>
#include <genPPD/PPD_3DLighting.h>

#include <genApp/C_CompositePage.h>
#include <genApp/C_PlotFolderPage.h>

#include <genApp/C_PlotPage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//  declare static members
#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before any objects in app

T_SC_BasicPtrArray<PlotPageC> PlotPageC::plotList(8);


PlotPageC::PlotPageC(const char* pID)
    : MenuPageC(pID)
{
    assocPlot = 0;
    plotList.AddTo(this);
}

PlotPageC::~PlotPageC()
{
    assocPlot = 0;
    plotList.DeleteFrom(this);
}

void PlotPageC::MinimizeSubplots()
{
    for (int i = 0; i < plotList.Size(); i++)
        if (plotList.GetRef(i).GetPlot()->IsSubPlot())
            plotList.GetRef(i).MinimizePlot();
}

bool PlotPageC::IsPlotPage() const
{
    return true;
}


bool PlotPageC::AcceptAsParent(MenuPageC* newParent) const
{
    return (newParent == NULL) ||
        newParent->IsSameType(CompositePlotPageAlloc::PageTypeName());
}

void PlotPageC::ResetChildren()
{
    assocPlot->annoObjList.SetEmpty();
    assocPlot->plotObjList.SetEmpty();

    for (int i = 0; i < pageObjects.Size(); i++)
        AddResetObject(pageObjects[i]->objFunction);

    for (int i = 0; i < childPages.Size(); i++)
        AddResetPage(childPages[i]);
}

void PlotPageC::AddResetObject(FuncObjC* currObj)
{
    if (currObj == 0)
        return;

    // minor kluge
    PlotObjBaseC* currPlotObj = dynamic_cast<PlotObjBaseC*>(currObj);
    if (currPlotObj == 0)
        return;

    AnnoObjC* annoObj = dynamic_cast<AnnoObjC*>(currPlotObj);
    if (annoObj != 0)
        {
            assocPlot->annoObjList += annoObj;
        }
    else
        {
            PlotObjC* plotObj = dynamic_cast<PlotObjC*>(currPlotObj);
            if (plotObj != 0)
                assocPlot->plotObjList += plotObj;
        }
}

void PlotPageC::AddResetPage(MenuPageC* currPage)
{
    if (!currPage->IsSameType(PlotFolderPageAlloc::PageTypeName()))
        return;

    for (int i = 0; i < currPage->pageObjects.Size(); i++)
        AddResetObject(currPage->pageObjects[i]->objFunction);

    for (int i = 0; i < currPage->childPages.Size(); i++)
        AddResetPage(currPage->childPages[i]);
}

void PlotPageC::ParentChanging(MenuPageC* newParent)
{
    // changing position on root ?
    if ((parentPage == NULL) && (newParent == NULL))
        return;

    PlotPageC* currPlotParent = static_cast<PlotPageC*>(parentPage);

    // changing to composite page
    if (newParent != NULL)
        {
            if (!newParent->IsSameType(CompositePlotPageAlloc::PageTypeName()))
                GenAppInternalError("PlotPageC::ParentChanging - new parent not composite");

            // moving around on same page ?
            if (parentPage == newParent)
                return;

            // was old page composite ?
            if (currPlotParent != NULL)
                RemoveFromComposite(currPlotParent);

            PD_Composite& newComp = *static_cast<PD_Composite*>(static_cast<PlotPageC*>(newParent)->assocPlot);
            newComp.AddSubplot(assocPlot);
            newComp.Execute();

            // if not created (called from paste pages)
            assocPlot->platformData->InitGraphicsWindow();

            MinimizePlot();

            assocPlot->Execute();
        }
    else
        {
            RemoveFromComposite(currPlotParent);

            assocWindow->ShowMainWindow(SW_RESTORE);

            assocPlot->ResetToMaster();
            assocPlot->plotUwidth = 640;
            assocPlot->plotUOffset = 0;
            assocPlot->plotVheight = 480;
            assocPlot->plotVOffset = 0;
            assocPlot->Execute();

            assocWindow->MainWindowToTop();
        }


    PlotDefC::ReplotAfterCalc();
    parentPage = newParent;
}

void PlotPageC::ObjectPageChangeComplete(MenuPageC* oldParent) const
{
    // draw current
    GetPlot()->GetMasterPlot().DrawPlot();

    // no need to update old
    if (oldParent == NULL)
        return;


    const PlotPageC* oldPlot = static_cast<const PlotPageC*>(oldParent->GetActualParent());

    // not composite, plot both
    if (GetPlottingParent() != oldPlot->GetPlottingParent())
        oldPlot->GetPlot()->GetMasterPlot().DrawPlot();

}

void PlotPageC::MinimizePlot()
{
    assocWindow->ShowMainWindow(SW_MINIMIZE);
}

void PlotPageC::RemoveFromComposite(PlotPageC* compParent)
{
    if (!compParent->IsSameType(CompositePlotPageAlloc::PageTypeName()))
        GenAppInternalError("PlotPageC::RemoveFromComposite");

    // moving composites
    PD_Composite& oldComp = *static_cast<PD_Composite*>(compParent->assocPlot);
    oldComp.subPlots.DeleteFrom(assocPlot);
    oldComp.Execute();
}


const PlotPageC* PlotPageC::GetPlottingParent() const
{
    // must be composite
    if (parentPage != NULL)
        return static_cast<PlotPageC*>(parentPage);

    return this;
}


// allocator code

const char* Plot2DXYPageAlloc::pageTypeDesc = "PG_2DXY_Plot";

MenuPageC*  Plot2DXYPageAlloc::CreatePage() const
{
    PlotPageC* newPage = new PlotPageC("XY");
    PPD_2DXY* plot = new PPD_2DXY();

    newPage->SetIcon(IDI_2DPLOT);
    newPage->assocPlot = plot;
    newPage->assocWindow = static_cast<PC_PlatformPS*>(plot->platformData);

    newPage->AddObject(plot);
    newPage->AddObject(new PPD_LinLogAxis(plot->xAxis, plot->setxAxis, *plot, "X Axis"));
    newPage->AddObject(new PPD_LinLogAxis(plot->yAxis, plot->setyAxis, *plot, "Y Axis"));
    newPage->AddObject(new PPD_2DPlotAnno(plot->plotAnnotation, *plot));

    return newPage;
}

const char* Plot2DXYPageAlloc::GetPageTypeName() const
{
    return pageTypeDesc;
}


const char* Plot3DXYZPageAlloc::pageTypeDesc = "PG_3DXYZ_Plot";

MenuPageC*  Plot3DXYZPageAlloc::CreatePage() const
{
    PlotPageC* newPage = new PlotPageC("XYZ");
    PPD_3DXYZ* plot = new PPD_3DXYZ();

    newPage->SetIcon(IDI_3DPLOT);
    newPage->assocPlot = plot;
    newPage->assocWindow = static_cast<PC_PlatformPS*>(plot->platformData);

    newPage->AddObject(plot);
    newPage->AddObject(new PPD_LinLogAxis(plot->xAxis, plot->setxAxis, *plot, "X Axis"));
    newPage->AddObject(new PPD_LinLogAxis(plot->yAxis, plot->setyAxis, *plot, "Y Axis"));
    newPage->AddObject(new PPD_LinLogAxis(plot->zAxis, plot->setzAxis, *plot, "Z Axis"));
    newPage->AddObject(new PPD_3DAxesLabel(plot->axesLabel, *plot));
    newPage->AddObject(new PPD_3DAxesFormat(plot->axesFormat, *plot));
    newPage->AddObject(new PPD_3DLighting(plot->plotLighting, *plot));

    return newPage;
}

const char* Plot3DXYZPageAlloc::GetPageTypeName() const
{
    return pageTypeDesc;
}

