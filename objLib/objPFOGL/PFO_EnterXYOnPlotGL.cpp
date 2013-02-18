///////////////////////////////////////////////////////////////////////////////////
//
// PFO_EnterXYOnPlotGL.cpp
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
//    Implements code to draw objPFO functional objects on a plot.
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/U_Str.h>

#include <genPlotClassPS/PC_ReportPS.h>

#include <genPFOGL/GL_Line.h>
#include <genPFOGL/GL_Symbol.h>

#include <objPFOGL/PFO_EnterXYOnPlotGL.h>

PFO_EnterXYOnPlotGL :: PFO_EnterXYOnPlotGL(PlotDefC& assPlt) :
                        PFO_EnterXYOnPlot(assPlt),
                        OGLBaseObj(this)

{
    InitCommon();
}

PFO_EnterXYOnPlotGL::PFO_EnterXYOnPlotGL(const PFO_EnterXYOnPlotGL& a, PlotDefC& assPlt) :
                        PFO_EnterXYOnPlot(a, assPlt),
                        OGLBaseObj(this)
{
    InitCommon();
}

PFO_EnterXYOnPlotGL::~PFO_EnterXYOnPlotGL()
{
}

void PFO_EnterXYOnPlotGL::InitCommon()
{
    deleteMode = true;
    changesMade = false;
    enterStartPoint = -1;
    BuildCallbackMenu();

    cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (assocPlot)));
}

PFO_EnterXYOnPlotGL& PFO_EnterXYOnPlotGL::operator= (const PFO_EnterXYOnPlotGL& a)
{
    PFO_EnterXYOnPlot::operator=(a);
    return *this;
}


void PFO_EnterXYOnPlotGL::DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    if (pointStatus.IsEmpty())
        deleteMode = false;

    GL_Line currLine;
    currLine.SetColor(currLinePen);
    currLine.SetLine(1);
    currLine.DrawLines(xyDataDC.xData, xyDataDC.yData, 0);

    int ndel, ncurr, nent;
    DrawSymbols(deletedPtSymbol, pstDeleted, ndel);
    DrawSymbols(currentPtSymbol, pstCurrent, ncurr);
    DrawSymbols(enteredPtSymbol, pstEntered, nent);

    if ((!deleteMode) && EnterStartOK())
    {
        GL_Symbol currSpec;
        PSC_SymbolSpec comboSpec(currentPtSymbol);
        comboSpec.symbolSize = enteredPtSymbol.symbolSize;
        comboSpec.symbolPen = enteredPtSymbol.symbolPen;
        currSpec.SetSymProperties(comboSpec);
        currSpec.DrawSymbol(Point2D(xyDataDC.xData[enterStartPoint], xyDataDC.yData[enterStartPoint]),
            comboSpec.symbolType);
    }

    cursorTrackData.Init();
    for (int i = 0; i < xyDataDC.Size(); i++)
        cursorTrackData.AddPointDataToTrack(xyDataDC.xData[i], xyDataDC.yData[i], i);

    CloseDrawObject();

    SetCallbackSensitive();

    if (!cursorReport->reportInitialized)
    {
        cursorReport->ReportInit("C/E/D:");
        cursorReport->ReportLabelSetup(0, "Mode:");
        cursorReport->ReportLabelSetup(1, "C/E/D:");
        cursorReport->showReport = true;
    }

    cursorReport->ReportTitleSetup(GetID());
    if (deleteMode)
        cursorReport->ReportUpdate(0, "Delete");
    else
        cursorReport->ReportUpdate(0, "Enter");

    char tempStr[40];
    IntToString(ncurr, tempStr, 40);
    ConcatString(tempStr, "/", 40);
    ConcatInt(nent, tempStr, 40);
    ConcatString(tempStr, "/", 40);
    ConcatInt(ndel, tempStr, 40);
    cursorReport->ReportUpdate(1, tempStr);
}


void PFO_EnterXYOnPlotGL::AddEnteredPoint(const double& pX, const double& pY)
{
    int nextEnterPoint = enterStartPoint + 1;
    for (int i = enterStartPoint + 1; i < pointStatus.Size(); i++)
    {
        if (pointStatus[i] == pstEntered)
        {
            nextEnterPoint++;
        }
        else
            break;
    }

    if (pointStatus.IsFull())
    {
        int newSize = (pointStatus.Size() + 1) * 2;
        pointStatus.ReAlloc(newSize);
        xyDataDC.xData.ReAlloc(newSize);
        xyDataDC.yData.ReAlloc(newSize);
    }

    pointStatus.InsertItem(pstEntered, nextEnterPoint);
    xyDataDC.xData.InsertItem(pX, nextEnterPoint);
    xyDataDC.yData.InsertItem(pY, nextEnterPoint);
}

bool PFO_EnterXYOnPlotGL::EnterStartOK()
{
    return (enterStartPoint >= 0) && (enterStartPoint < pointStatus.Size());
}

void PFO_EnterXYOnPlotGL::ProcessSelection(const Coord3D&                cursorPos,
                                                 PlotDefC::SelectionKeys inKey)
{

    if ((!doPlot) || (!isActive) || StatusNotOK() || ((inKey == PlotDefC::pdc_Drag) && (!dragOK)))
        return;

    dragOK = false;
    if (deleteMode)
    {
        int delIndx;
        if (!cursorTrackData.GetClosestIndex(cursorPos.cX, cursorPos.cY, delIndx))
            return;
        if (pointStatus[delIndx] == pstDeleted)
            pointStatus[delIndx] = pstEntered;
        else
            pointStatus[delIndx] = pstDeleted;
    }
    else
    {
        if (pointStatus.IsEmpty())
        {
            enterStartPoint = -1;
            AddEnteredPoint(cursorPos.cX, cursorPos.cY);
            enterStartPoint = 0;
        }
        else if ((!EnterStartOK()) || (inKey == PlotDefC::pdc_AddConsecutiveObject))
        {
            if (!cursorTrackData.GetClosestIndex(cursorPos.cX, cursorPos.cY, enterStartPoint))
                return;
            dragOK = (inKey == PlotDefC::pdc_AddConsecutiveObject);
        }
        else if (inKey == PlotDefC::pdc_Drag)
        {
            xyDataDC.xData[enterStartPoint] = cursorPos.cX;
            xyDataDC.yData[enterStartPoint] = cursorPos.cY;
            dragOK = true;
        }
        else
        {
            AddEnteredPoint(cursorPos.cX, cursorPos.cY);
        }
    }
    DoReplot();
}



void PFO_EnterXYOnPlotGL::DrawSymbols(const PSC_SymbolSpec& inSpec, PointStatusType statToDraw, int& ndrawn)
{
    GL_Symbol currSpec;
    currSpec.SetSymProperties(inSpec);
    ndrawn = 0;
    for (int i = 0; i < xyDataDC.Size(); i++)
        if (pointStatus[i] == statToDraw)
        {
            currSpec.DrawSymbol(Point2D(xyDataDC.xData[i], xyDataDC.yData[i]), inSpec.symbolType);
            ndrawn++;
        }
}



void PFO_EnterXYOnPlotGL::BuildCallbackMenu()
{
    // grab ref
    CallbackItemArray& cbMenu = callbackMenu.menuDesc;

    cbMenu.AddEntry("Apply", 0);
    cbMenu.AddEntry("Cancel", 1);
    cbMenu.AddSeparator();
    cbMenu.AddEntry("Delete mode", 2);
    cbMenu.AddEntry("Enter mode", 3);
    cbMenu.AddSeparator();
    cbMenu.AddEntry("Delete All", 4);
}

void PFO_EnterXYOnPlotGL::SetCallbackSensitive()
{
    CallbackItemArray& cbMenu = callbackMenu.menuDesc;
    cbMenu[0].isActive = changesMade;
    cbMenu[1].isActive = changesMade;
    cbMenu[3].isActive = !deleteMode;
    cbMenu[4].isActive = deleteMode;
    cbMenu[6].isActive = deleteMode;
}



void PFO_EnterXYOnPlotGL::DoCallback(int reason)
{
    switch (reason) {
        case 0 : {
            enterStartPoint = -1;
            PlotDefC::ExecuteAndReplot(*this);
            changesMade = false;
            break;
        }
        case 1: {
            DoCancel();
            break;
        }
        case 2 : {
            deleteMode = true;
            break;
        }
        case 3 : {
            deleteMode = false;
            break;
        }
        case 4 : {
            DoDeleteAll();
            break;
        }
    }
    SetCallbackSensitive();
}


void PFO_EnterXYOnPlotGL::DoCancel()
{
    DeleteType(pstEntered);
    DoReplot();
}

void PFO_EnterXYOnPlotGL::DoReplot()
{
    changesMade = true;
    objWasCalculated = true;
    PlotDefC::ReplotAfterCalc();
}

void PFO_EnterXYOnPlotGL::DoDeleteAll()
{
    Limit2D currLimits = GetCurrentViewLimits();

    for (int i = 0; i < xyDataDC.Size(); i++)
        if (currLimits.WithinLimits(Point2D(xyDataDC.xData[i], xyDataDC.yData[i])))
            pointStatus[i] = pstDeleted;

    DoReplot();
}

