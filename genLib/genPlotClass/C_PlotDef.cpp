///////////////////////////////////////////////////////////////////////////////////
//
// C_PlotDef.cpp
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
//      basic definition of a plot window.
//  Maintains lists of connected plot and annotation objects.
//  Controls overall plot display.
//  Handles default mouse right click menus.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <genClass/DO_PenSet.h>
#include <genClass/SC_BoolArray.h>

#include <genClass/U_Str.h>

#include <genPlotClass/C_PlotObj.h>
#include <genPlotClass/C_AnnoObj.h>
#include <genPlotClass/PD_Composite.h>

#include <genPlotClass/C_PlotDef.h>

//  statics

T_SC_BasicPtrArray<PlotDefC> PlotDefC::plotList(16);

PlotDefC :: PlotDefC(const char* dType)
    : InteractiveObj(dType), callbackMenu(*this, "Views"),
        plotObjList(32), annoObjList(32)
{
    InitCommon();
    SetDefault();
}

PlotDefC :: PlotDefC(const PlotDefC& a)
    : InteractiveObj(a), callbackMenu(*this, "Views"),
    plotObjList(32), annoObjList(32)
{
    InitCommon();
    LocalCopy(a);
}

PlotDefC :: ~PlotDefC ()
{
    if (IsSubPlot())
        static_cast<PD_Composite&>(GetMasterPlot()).RemoveSubplot(this);

    plotList.DeleteFrom(this);

    // platform data data deleted here to avoid dtor problems
    // in OGL_Base derived derived classes
    delete platformData;
}

void PlotDefC::InitCommon()
{
    platformData = 0;
    masterPlot = this;
    preID = 0;
    selectedAnno = 0;

    autoUpdateRequired = false;
    plotDefIsActive = true;
    axesHaveBeenSet = false;

    plotUOffset = 0;
    plotVOffset = 0;

    AddInPort(plotPenSetObjRef,   typeid(DO_PenSet));
    AddOutPort(defaultPenSetDO);
    defaultPenSet       = 0;

    rubberbandDrag = false;
    plotList.AddTo(this);
    BuildCallbackMenu();
}



PlotDefC& PlotDefC::operator= (const PlotDefC& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}

void  PlotDefC::FullCopy(const PlotDefC& a)
{
        FuncObjC::FullCopy(a);
        LocalCopy(a);
}

void  PlotDefC::LocalCopy(const PlotDefC& a)
{
    plotPenSetObjRef    = a.plotPenSetObjRef;
    plotBackgroundPen   = a.plotBackgroundPen;

    plotUwidth          = a.plotUwidth;
    plotVheight         = a.plotVheight;

    rightMargin         = a.rightMargin;
    leftMargin          = a.leftMargin;
    topMargin           = a.topMargin;
    bottomMargin        = a.bottomMargin;

    psOutput            = a.psOutput;
    tgaOutput           = a.tgaOutput;
}


void  PlotDefC:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (plotPenSetObjRef.objRef == 0)
        plotPenSetObjRef.SetDefault(typeid(DO_PenSet));

    if (CheckInputFO(plotPenSetObjRef, "Pen set"))
    {
        defaultPenSetDO = *((DO_PenSet*) GetInPortData(plotPenSetObjRef));
        defaultPenSet = defaultPenSetDO.penSetData;
    }
    else
    {
        defaultPenSet = 0;
        defaultPenSetDO = DO_PenSet();
    }
}

bool PlotDefC::IsInUse() const
{
    return true;
}


void PlotDefC::SetDefault()
{
    plotBackgroundPen   = 0;

    plotUwidth          = 640;
    plotVheight         = 480;
}


void PlotDefC::CalcOutput(FOcalcType  calcType)
{
    PlotDefC::DoStatusChk();
    if (objStatus != os_OK)
        return;

    if (!platformData->initComplete)
        platformData->InitGraphicsWindow();
    platformData->ResetGraphicsWindow();

    char titleStr[80];
    if (preID != 0)
    {
        MakeString(titleStr, preID, "::", GetID(), 80);
    }
    else
    {
        CopyString(titleStr, GetID(), 80);
    }

    platformData->RenameGraphicsWindow(titleStr);

    autoUpdateRequired = true;
}

Limit3D PlotDefC::CalcPlotLimits()
{
    Limit3D plotLimits;
    for (int i = 0; i < plotObjList.Size(); i++)
    {
        if (plotObjList[i]->doPlot)
        {
            PlotObjC& currObj = *(plotObjList[i]);
            Limit3D objLimits = currObj.GetPlotObjLimits();
            if (!objLimits.minLim.CoordIsNull())
            {
                objLimits.minLim += currObj.offsetCoord;
                objLimits.maxLim += currObj.offsetCoord;
            }
            plotLimits.AddToLimits(objLimits);
        }
    }
    return plotLimits;
}



void PlotDefC::SetInitialView(const PC_View& initView)
{
    plotViews.InitStack(initView);
}

void PlotDefC::ChangeView(const PC_View&  newView)
{
    plotViews.PushStack(newView);
    ResetView();
}

static void DoOp(ViewOp     axesOp,
                 double     currPt,
                 double     currMin,
                 double     currMax,
                 double     scFact,
                 double&    newMin,
                 double&    newMax)
{
    if ( axesOp == vo_na)
    {
        newMin = currMin;
        newMax = currMax;
        return;
    }

    if ( axesOp == vo_Log)
    {
        currMin = log10(currMin);
        currMax = log10(currMax);
        currPt  = log10(currPt);
    }

    double dx = (currMax - currMin) / 2.0 * scFact;
    newMin = currPt - dx;
    newMax = currPt + dx;

    if ( axesOp == vo_Log)
    {
        newMin = pow(10.0, newMin);
        newMax = pow(10.0, newMax);
    }
}


void PlotDefC::ChangeView(const Coord3D&  scaleFactor)
{
    Coord3D viewCenter = GetNewViewCenter();
    if (viewCenter.CoordIsNull())
        return;

    Limit3D oldLimits = GetCurrentViewLimits();
    Limit3D newLimits;

    PC_ViewOpRec axesSettings = GetPlotViewOps();

    DoOp(axesSettings.xOp, viewCenter.cX,
                           oldLimits.minLim.cX, oldLimits.maxLim.cX,
                           scaleFactor.cX,
                           newLimits.minLim.cX, newLimits.maxLim.cX);
    DoOp(axesSettings.yOp, viewCenter.cY,
                           oldLimits.minLim.cY, oldLimits.maxLim.cY,
                           scaleFactor.cY,
                           newLimits.minLim.cY, newLimits.maxLim.cY);
    DoOp(axesSettings.zOp, viewCenter.cZ,
                           oldLimits.minLim.cZ, oldLimits.maxLim.cZ,
                           scaleFactor.cZ,
                           newLimits.minLim.cZ, newLimits.maxLim.cZ);

    // keep existing 3D az/scale/el
    PC_View currView = GetCurrentView();

    //  clear translations
    currView.translation = Coord3D(0.0);

    //  set new limits
    currView.viewLimits = newLimits;
    plotViews.PushStack(currView);
    ResetView();
}

void PlotDefC::ChangeView(double scaleFactor)
{
    Coord3D sc3D = Coord3D(scaleFactor);
    ChangeView(sc3D);
}

PC_View PlotDefC::GetCurrentView()
{
    return plotViews.TopStack();
}

Limit3D PlotDefC::GetCurrentViewLimits()
{
    return plotViews.TopStack().viewLimits;
}


int PlotDefC::GetNPlotObject()
{
    return plotObjList.Size();
}

void PlotDefC:: ViewTop()
{
    plotViews.ClearStack();
    ResetView();
}

void PlotDefC:: ViewPrev()
{
    plotViews.PopStack();
    ResetView();
}

void PlotDefC::ViewPropagate()
{
    PC_View currView = GetCurrentView();
    for (int i = 0; i < plotList.Size(); i++)
    {
        PlotDefC& pl = plotList.GetRef(i);
        if ((&pl != this) && (typeid(pl) == typeid(*this)))
            pl.ChangeView(currView);
    }
}

void PlotDefC::SizePropagate()
{
    for (int i = 0; i < plotList.Size(); i++)
    {
        PlotDefC& pl = plotList.GetRef(i);
        if ((&pl != this) && (typeid(pl) == typeid(*this)))
        {
            pl.plotUwidth  = plotUwidth;
            pl.plotVheight = plotVheight;
            pl.ResetSize();
        }
    }
}

bool PlotDefC::CheckViewChangeReplot()
{
    bool replotRequired = false;
    for (int i = 0; i < plotObjList.Size(); i++)
    {
        PlotObjC& plotObj = plotObjList.GetRef(i);
        if (plotObj.recalcOnViewChange && plotObj.doPlot)
        {
            plotObj.ViewChangeRecalc();
            plotObj.CalcDownStreamObjects();
            // clear to avoid double plot
            plotObj.objWasCalculated = false;
            replotRequired = true;
        }
    }
    return replotRequired;
}


void PlotDefC::ResetView()
{
    bool fullReplot = CheckViewChangeReplot();

    GetMasterPlot().DrawPlot();

    // draw any affected downstream
    if (fullReplot)
        ReplotAfterCalc();
}


void PlotDefC::DrawPlot()
{
    bool fullReplot = false;

    bool wasAuto = autoUpdateRequired;
    if (autoUpdateRequired)
    {
        autoUpdateRequired = false;
        SetAxesAutoLimits();
        SetViewFromAxes();
        // to initialize controls
        ResetView();
        fullReplot = CheckViewChangeReplot();
    }

    // only masters draw themselves
    if (IsMasterPlot())
    {
        DoDrawPlot();
        if (!wasAuto)    // kluge to get around double dump on vert exag change
            CheckTGADump();
        platformData->DisplayNormalCursor();
    }

    // draw any affected downstream
    if (fullReplot)
        ReplotAfterCalc();
}

void PlotDefC::RedrawPlot()
{
    // only masters draw themselves
    if (IsMasterPlot())
    {
        DoDrawPlot();
        platformData->DisplayNormalCursor();
    }
}

void PlotDefC::DoDrawPlot()
{
    if (!platformData->initComplete)
    {
        platformData->InitGraphicsWindow();
        platformData->ResetGraphicsWindow();
    }

    // don't draw if minimize
    if (platformData->IsMinimized())
        return;

    platformData->DisplayBusyCursor();
    AdjustMargins();
    SetRubberbandOKLimits();

    BasicFullDraw();
}

void PlotDefC::BasicFullDraw()
{
    InitRenderer();
    SetupDataView();
    DrawAllObjects();
    CloseRenderer();
}

void PlotDefC::ExecuteAndReplot(FuncObjC& inObj)
{
    inObj.Execute();
    if (inObj.StatusOK())
    {
        inObj.CalcDownStreamObjects();
        ReplotAfterCalc();
    }
}


void PlotDefC::ReplotAfterCalc()
{

    // must loop through all plots first and save in array as objWasCalculated can be cleared during drawplot
    SC_BoolArray drawPlotRequired;
    drawPlotRequired.Alloc(plotList.Size());

    int i, j;
    for (i = 0; i < plotList.Size(); i++)
    {
        PlotDefC& plot = plotList.GetRef(i);

        bool plotUpdateRequired = plot.plotObjList.IsEmpty();
        for (j = 0; j < plot.plotObjList.Size(); j++)
        {
            PlotObjC& plotObj = plot.plotObjList.GetRef(j);
            if (plotObj.objWasCalculated && plotObj.doPlot)
                plotUpdateRequired = true;
        }

        for (j = 0; j < plot.annoObjList.Size(); j++)
        {
            AnnoObjC& plotObj = plot.annoObjList.GetRef(j);
            if (plotObj.objWasCalculated && plotObj.doPlot)
                plotUpdateRequired = true;
        }

        drawPlotRequired[i] = plotUpdateRequired;
    }

    for (i = 0; i < plotList.Size(); i++)
        if (drawPlotRequired[i])
            plotList[i]->DrawPlot();

}


void PlotDefC::CheckTGADump()
{
    if (tgaOutput.dumpMethod != PC_DumpTGA::dtga_FullAuto)
        return;

    char  tgafName[stdFileStrLen];
    tgaOutput.GetNextFileName(tgafName, stdFileStrLen);
    platformData->WriteBitmapFile(tgafName);
}

void PlotDefC::DrawAllObjects()
{
    DrawPlotAxes();
    DrawPlotObjects();
    SetupAnnoView();
    DrawAnnoObjects();
}


void PlotDefC::DrawAnnoObjects()
{
    for (int i = 0; i < annoObjList.Size(); i++)
    {
        AnnoObjC& currObj = annoObjList.GetRef(i);
        if (currObj.doPlot)
            currObj.DrawPlotObject();
    }
}


void PlotDefC::InitializeAllPlots()
{
    for (int i = 0; i < plotList.Size(); i++)
        plotList.GetRef(i).platformData->InitGraphicsWindow();
}

//  anno to pixel conversion
void PlotDefC::AnnoXYtoPixelXY(const Point2D& annoPt,
                               int&     annoPixX,
                               int&     annoPixY)
{
    annoPixX = int (annoPt.pX / AnnoObjC::maxAnnoCoord * double(plotUwidth));
    annoPixY = int ((1.0 - annoPt.pY / AnnoObjC::maxAnnoCoord) * double(plotVheight));
}

void PlotDefC::PixelXYtoAnnoXY(int      annoPixX,
                               int      annoPixY,
                               Point2D& annoPt)
{
    annoPt.pX = AnnoObjC::maxAnnoCoord * double(annoPixX) / double(plotUwidth);
    annoPt.pY = AnnoObjC::maxAnnoCoord * (1.0 - double(annoPixY) / double(plotVheight));
}

void  PlotDefC::CalcAvailablePixels(int&    xAvail,
                                    int&    yAvail)
{
    xAvail = plotUwidth - rightMargin - leftMargin;
    yAvail = plotVheight - topMargin - bottomMargin;
}

void PlotDefC::AdjustMargins()
{
    int dxAvail, dyAvail;
    CalcAvailablePixels(dxAvail, dyAvail);
    if (IsSubPlot())
    {
        if ((dxAvail < 5) && (dxAvail > 0))
        {
            rightMargin = (plotUwidth - 5) / 2;
            leftMargin = rightMargin;
        }

        if ((dyAvail < 5) && (dyAvail > 0))
        {
            topMargin = (plotVheight - 5) / 2;
            bottomMargin = topMargin;
        }

    }
    else
    {
        if (dxAvail < 100)
            plotUwidth = 100 + leftMargin + rightMargin;

        if (dyAvail < 100)
            plotVheight = 100 + topMargin + bottomMargin;
    }
}

void  PlotDefC::ProcessSelectionKeys(SelectionKeys inKey) {}

void  PlotDefC::SetRubberbandOKLimits()
{
    int dx, dy;
    CalcAvailablePixels(dx, dy);

    rubberbandOKrect.ulXpos = leftMargin;
    rubberbandOKrect.ulYpos = plotVheight - bottomMargin - dy;
    rubberbandOKrect.lrXpos = leftMargin + dx;
    rubberbandOKrect.lrYpos = plotVheight - bottomMargin;
}

void PlotDefC::PointerEnter(int xpos,
                            int ypos)
{
    platformData->ClearPlatformOverlay();
    selectedAnno = 0;
    rubberbandDrag = false;
}

void PlotDefC::PointerLeave()
{
    platformData->ClearPlatformOverlay();
    selectedAnno = 0;
    rubberbandDrag = false;
}

void  PlotDefC::PointerMove(int xpos,
                            int ypos)
{
    for (int i = 0; i < annoObjList.Size(); i++)
    {
        AnnoObjC& currObj = annoObjList.GetRef(i);
        if (currObj.doPlot)
        {
            PC_ObjLimitRect& currRect = currObj.rectLimits;
            if (currRect.NotNull())
                if (currRect.InRect(xpos, ypos))
                {
                    DrawPlatformRectangle(currRect, false);
                }
                else
                {
                    ClearPlatformRectangle(currRect);
                }
        }
    }

    ProcessCursorMove(xpos, ypos);
}

void PlotDefC::PointerSelect(int    xpos,
                             int    ypos,
                             bool   shifted)
{
    // cant think why this would happen ...
    if (((selectedAnno != 0) || rubberbandDrag) && (draggedObjectRect.NotNull()))
    {
        ClearPlatformRectangle(draggedObjectRect);
    }

    selectedAnno = 0;
    rubberbandDrag = false;

    for (int i = 0; i < annoObjList.Size(); i++)
    {
        AnnoObjC& currAnno = annoObjList.GetRef(i);
        if (currAnno.doPlot)
        {
            PC_ObjLimitRect& currRect = currAnno.rectLimits;
            if (currRect.NotNull()  && currRect.InRect(xpos, ypos))
            {
                // nothing selected
                if (selectedAnno == 0)
                {
                    selectedAnno = &currAnno;
                    continue;
                }

                //  already selected, see if current is better
                PC_ObjLimitRect& selRect = selectedAnno->rectLimits;

                // easy case for nested
                if (selRect.InRect(currRect.ulXpos, currRect.ulYpos) &&
                    selRect.InRect(currRect.lrXpos, currRect.lrYpos))
                {
                    selectedAnno = &currAnno;
                    continue;
                }

                //  based on distance from center to cursor when overlapping
                if (selRect.DistFromCenter(xpos, ypos) > currRect.DistFromCenter(xpos, ypos))
                    selectedAnno = &currAnno;
            }
        }
    }
    dragStartX = xpos;
    dragStartY = ypos;
    if (selectedAnno != 0)
    {
        draggedObjectRect = selectedAnno->rectLimits;
        DrawPlatformRectangle(draggedObjectRect, true);
        GetPlatformData()->DisplayGrabCursor();
    }
    else
        if (rubberbandOKrect.InRect(xpos, ypos))
        {
            draggedObjectRect = PC_ObjLimitRect();
            rubberbandDrag = true;
            GetPlatformData()->DisplayZoomCursor();
        }
}

void  PlotDefC::AdjustAnnoSelection(int& dx,
                                    int& dy,
                                    int& ulXpos,
                                    int& ulYpos)
{
    PC_ObjLimitRect& selRect = selectedAnno->rectLimits;

    // adjust if attempt to drag out of window
    if ((selRect.ulXpos + dx) < 0)
        dx = -selRect.ulXpos;

    if ((selRect.lrXpos + dx) > plotUwidth)
        dx = plotUwidth - selRect.lrXpos;

    if ((selRect.ulYpos + dy) < 0)
        dy = -selRect.ulYpos;

    if ((selRect.lrYpos + dy) > plotVheight)
        dy = plotVheight - selRect.lrYpos;

    ulXpos = selRect.ulXpos + dx;
    ulYpos = selRect.ulYpos + dy;
}


void PlotDefC::PointerDrag(int  xpos,
                           int  ypos,
                           bool shifted)
{
    ProcessCursorMove(xpos, ypos);
    if ((selectedAnno == 0) && (!rubberbandDrag))
        return;

    // clear existing
    ClearPlatformRectangle(draggedObjectRect);

    int dx = xpos - dragStartX;
    int dy = ypos - dragStartY;
    if (selectedAnno != 0)
    {
        draggedObjectRect = selectedAnno->rectLimits;

        int newX, newY;
        AdjustAnnoSelection(dx, dy, newX, newY);

        draggedObjectRect.ulXpos = newX;
        draggedObjectRect.ulYpos = newY;
        draggedObjectRect.lrXpos += dx;
        draggedObjectRect.lrYpos += dy;

        Point2D newULloc;
        PixelXYtoAnnoXY(newX, newY, newULloc);
        selectedAnno->UpdateULlocation(newULloc);
    }
    else
    {
        // if still within region update
        if (rubberbandOKrect.InRect(xpos, ypos))
        {
            if (dx > 0)
            {
                draggedObjectRect.ulXpos = dragStartX;
                draggedObjectRect.lrXpos = xpos;
            }
            else
            {
                draggedObjectRect.ulXpos = xpos;
                draggedObjectRect.lrXpos = dragStartX;
            }

            if (dy > 0)
            {
                draggedObjectRect.ulYpos = dragStartY;
                draggedObjectRect.lrYpos = ypos;
            }
            else
            {
                draggedObjectRect.ulYpos = ypos;
                draggedObjectRect.lrYpos = dragStartY;
            }
        }
    }

    DrawPlatformRectangle(draggedObjectRect, true);
}

void PlotDefC::PointerRelease(int   xpos,
                              int   ypos,
                              bool  shifted)

{
    GetPlatformData()->DisplayNormalCursor();
    if ((selectedAnno == 0) && (!rubberbandDrag))
        return;

    // clear existing
    ClearPlatformRectangle(draggedObjectRect);

    if (rubberbandDrag)
    {
        bool viewChanged = ProcessRubberbandRegion(draggedObjectRect);
        rubberbandDrag = false;
        selectedAnno = 0;
//      if (viewChanged && IsSubPlot())
//          GetMasterPlot().RedrawPlot();
        return;
    }

    //  has it moved ??
    int dx = xpos - dragStartX;
    int dy = ypos - dragStartY;
    if ((dx != 0) || (dy != 0))
    {
        int newX, newY;
        AdjustAnnoSelection(dx, dy, newX, newY);

        Point2D newULloc;
        PixelXYtoAnnoXY(newX, newY, newULloc);

        selectedAnno->UpdateULlocation(newULloc);

        // replot
        GetMasterPlot().RedrawPlot();
    }

    // clear it
    selectedAnno = 0;
}


void PlotDefC::ClearPlatformRectangle(const PC_ObjLimitRect&     rectToClear)
{
    PC_ObjLimitRect currRect = rectToClear;
    if (IsSubPlot())
    {
        currRect.Adjust(plotUOffset, masterPlot->plotVheight - (plotVOffset + plotVheight));
    }

    GetPlatformData()->ClearPlatformRectangle(currRect);
};

void PlotDefC::DrawPlatformRectangle(const PC_ObjLimitRect&     rectToDraw,
                                           bool                 useSelectColor)
{
    PC_ObjLimitRect currRect = rectToDraw;
    if (IsSubPlot())
    {
        currRect.Adjust(plotUOffset, masterPlot->plotVheight - (plotVOffset + plotVheight));
    }

    GetPlatformData()->DrawPlatformRectangle(currRect, useSelectColor);

};





void PlotDefC::GetAxesTypes(bool& xIsLinear,
                            bool& yIsLinear,
                            bool& zIsLinear)
{
    xIsLinear = true;
    yIsLinear = true;
    zIsLinear = true;
}

void PlotDefC::GetAxesTypes(bool& xIsLinear,
                            bool& yIsLinear)
{
    bool zIsLinear;
    GetAxesTypes(xIsLinear, yIsLinear, zIsLinear);
}


Coord3D PlotDefC::GetNormalizedCoord(const Coord3D& inCoord, const Coord3D& offsetCoord)
{
    return inCoord;
}

// **************************** callback processing

PC_CallbackMenu* PlotDefC::GetCallbackMenu()
{
    // see if any object has a callback
    if (!plotDefIsActive)
        for (int i = 0; i < plotObjList.Size(); i++)
        {
            PlotObjC& currObj = plotObjList.GetRef(i);
            if (currObj.doPlot)
            {
                PC_CallbackMenu* tmp = currObj.GetCallbackMenu();
                if (tmp)
                    return tmp;
            }
        }

    return &callbackMenu;
}

void PlotDefC::BuildCallbackMenu()
{
    // grab ref
    CallbackItemArray& cbMenu = callbackMenu.menuDesc;

    cbMenu.AddEntry("Zoom-in x 1.1", 0);
    cbMenu.AddEntry("Zoom-in x 2", 1);
    cbMenu.AddEntry("Zoom-in x 5", 2);
    cbMenu.AddSeparator();
    cbMenu.AddEntry("Zoom-out x 1.1", 3);
    cbMenu.AddEntry("Zoom-out x 2", 4);
    cbMenu.AddEntry("Zoom-out x 5", 5);
    cbMenu.AddSeparator();
    cbMenu.AddEntry("Axes -> Data Limits", 6);
    cbMenu.AddSeparator();
    cbMenu.AddEntry("Pan", 7);
    cbMenu.AddSeparator();
    cbMenu.AddEntry("Copy To Clipboard", 8);
}

void PlotDefC::DoCallback(int reason)
{
    static double zooms[] = {0.9091, 0.5, 0.2, 1.1, 2.0, 5.0};
    if (reason < 6)
    {
        ChangeView(zooms[reason]);
    }
    else if (reason == 6)
    {
        SetAxesFromExtents();
        SetViewFromAxes();
        ResetView();
    }
    else if (reason == 7)
    {
        ChangeView(1.0);
    }
    else
    {
        PC_Platform* currPlatform = GetPlatformData();
        if (currPlatform)
            currPlatform->CopyToClipboard(true);
        return;
    }

    if (IsSubPlot())
        masterPlot->RedrawPlot();
}


