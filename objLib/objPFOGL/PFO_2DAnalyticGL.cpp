///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DAnalyticGL.cpp
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
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/U_Str.h>

#include <genPFOGL/GL_Line.h>
#include <genPFOGL/GL_Symbol.h>

#include <genPlotClassPS/PC_ReportPS.h>

#include <objPFOGL/PFO_2DAnalyticGL.h>

PFO_2DAnalyticGL :: PFO_2DAnalyticGL(PlotDefC& assPlt) :
                        PFO_2DAnalytic(assPlt),
                        OGLBaseObj(this)

{
    InitCommon();
}

PFO_2DAnalyticGL::PFO_2DAnalyticGL(const PFO_2DAnalyticGL& a, PlotDefC& assPlt) :
                        PFO_2DAnalytic(a, assPlt),
                        OGLBaseObj(this)
{
    InitCommon();
}

PFO_2DAnalyticGL::~PFO_2DAnalyticGL()
{
}

void PFO_2DAnalyticGL::InitCommon()
{
    lastSelection = -1;
    BuildCallbackMenu();
}

PFO_2DAnalyticGL& PFO_2DAnalyticGL::operator= (const PFO_2DAnalyticGL& a)
{
    PFO_2DAnalytic::operator=(a);
    return *this;
}


void PFO_2DAnalyticGL::DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    if (StatusOK())
    {
        GL_Line currLine;
        currLine.SetLineProperties(lineSpec);

        // to get hard copy
        SC_PointArray tempPoints(2);
        tempPoints.SetSize(2);
        tempPoints[0] = lineStart;
        tempPoints[1] = lineEnd;
        currLine.DrawLines(tempPoints);

        if (isActive)
        {
            GL_Symbol currSym;
            currSym.SetSymProperties(endPtSymbol);
            currSym.DrawSymbols(tempPoints, endPtSymbol.symbolType);

            currSym.SetSymProperties(rotationPtSymbol);
            tempPoints[0] = lineRotPointA;
            tempPoints[1] = lineRotPointB;
            currSym.DrawSymbols(tempPoints, rotationPtSymbol.symbolType);

            tempPoints.SetSize(1);
            tempPoints[0] = lineCenter;
            currSym.SetSymProperties(centerPtSymbol);
            currSym.DrawSymbols(tempPoints, centerPtSymbol.symbolType);

            cursorTrackData.Init();
            cursorTrackData.AddPointDataToTrack(lineStart, 0);
            cursorTrackData.AddPointDataToTrack(lineRotPointA, 1);
            cursorTrackData.AddPointDataToTrack(lineCenter, 2);
            cursorTrackData.AddPointDataToTrack(lineRotPointB, 3);
            cursorTrackData.AddPointDataToTrack(lineEnd, 4);
        }
    }
    CloseDrawObject();

    SetCallbackSensitive();
}


void PFO_2DAnalyticGL::ProcessSelection(const Coord3D&                cursorPos,
                                              PlotDefC::SelectionKeys inKey)
{
    if ((!doPlot) || (!isActive) || StatusNotOK())
        return;


    if (lineStart.PointIsNull())
    {
        DoReset();
        return;
    }

    int currSelection;
    if (!cursorTrackData.GetClosestIndex(cursorPos.cX, cursorPos.cY, currSelection))
        return;

    if ((currSelection != lastSelection) && (inKey == PlotDefC::pdc_Drag))
    {
        lastSelection =  currSelection;
        DoReplot();
        return;
    }

    lastSelection =  currSelection;
    GetAxesTypes(xLin, yLin);  // just to be sure ..

    Point2D newPt(cursorPos);

    Transform(newPt);

    switch (currSelection)
    {
        case 0 :  {
            // move relative to endpoint
            Point2D tempPt(lineEnd);
            Transform(tempPt);
            Line2D tempLine(newPt, tempPt);
            lineCenter = tempLine.PointOnLine(0.5);
            UnTransform(lineCenter);
            lineStart = cursorPos;
            break;
        }

        case 1 :  {
            // rotate
            Point2D tempPt(lineCenter);
            Transform(tempPt);
            Line2D tempLine(newPt, tempPt);
            lineStart = tempLine.PointOnLine(-1.0);
            UnTransform(lineStart);
            break;
        }

        case 2 :  {
            // shift
            Point2D tempPt(lineCenter);
            Transform(tempPt);
            double dX = newPt.pX - tempPt.pX;
            double dY = newPt.pY - tempPt.pY;

            Transform(lineStart);
            lineStart.pX += dX;
            lineStart.pY += dY;
            UnTransform(lineStart);
            lineCenter = cursorPos;

            break;
        }

        case 3 :  {
            // rotate
            Point2D tempPt(lineCenter);
            Transform(tempPt);
            Line2D tempLine(newPt, tempPt);
            lineStart = tempLine.PointOnLine(3.0);
            UnTransform(lineStart);
            break;
        }

        case 4 :  {
            // move relative to endpoint
            Point2D tempPt(lineStart);
            Transform(tempPt);
            Line2D tempLine(newPt, tempPt);
            lineCenter = tempLine.PointOnLine(0.5);
            UnTransform(lineCenter);
            break;
        }

    }
    CalcPoints();
    DoReplot();
}



void PFO_2DAnalyticGL::BuildCallbackMenu()
{
    // grab ref
    CallbackItemArray& cbMenu = callbackMenu.menuDesc;

    cbMenu.AddEntry("Apply", 0);
    cbMenu.AddSeparator();
    cbMenu.AddEntry("Reset", 1);
}

void PFO_2DAnalyticGL::SetCallbackSensitive()
{
    CallbackItemArray& cbMenu = callbackMenu.menuDesc;
    cbMenu[0].isActive = !lineCenter.PointIsNull();
}


void PFO_2DAnalyticGL::DoCallback(int reason)
{
    switch (reason) {
        case 0 : {
            PlotDefC::ExecuteAndReplot(*this);
            break;
        }
        case 1: {
            DoReset();
            break;
        }
    }
    SetCallbackSensitive();
}


void PFO_2DAnalyticGL::DoReset()
{
    Limit2D currLimits = GetCurrentViewLimits();
    GetAxesTypes(xLin, yLin);  // just to be sure ..

    // kluge
    lineCenter = static_cast<PD_2D*>(assocPlot)->cursorCoord;
    lineStart = lineCenter;

    Transform(lineStart);
    Transform(currLimits.maxLim);
    Transform(currLimits.minLim);
    lineStart.pX -= (currLimits.maxLim.pX  - currLimits.minLim.pX) / 4.0;
    UnTransform(lineStart);

    CalcPoints();
    DoReplot();
}

void PFO_2DAnalyticGL::DoReplot()
{
    objWasCalculated = true;
    PlotDefC::ReplotAfterCalc();
}


