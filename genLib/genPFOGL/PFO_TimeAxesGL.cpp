///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TimeAxesGL.cpp
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
//      Implements renderer dependent code to actually draw genPFO functional
// objects on a plot.  Is platform independent but
// relies on platform dependent code in genPlotClassPS.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genPlotClassPS/PC_ReportPS.h>
#include <genPlotClassPS/PD_2DXYPS.h>

#include <genPFOGL/GL_Line.h>

#include <genPFOGL/PFO_TimeAxesGL.h>


PFO_TimeAxesGL :: PFO_TimeAxesGL(PlotDefC& assPlt) :
                            PFO_TimeAxes(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_TimeAxesGL::PFO_TimeAxesGL(const PFO_TimeAxesGL& a, PlotDefC& assPlt) :
                            PFO_TimeAxes(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}

void PFO_TimeAxesGL::InitCommon(PlotDefC& assPlt)
{
    if (!is3DObject)
    {
        cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
        reportOK     = true;
    }
}


PFO_TimeAxesGL::~PFO_TimeAxesGL()
{
}

PFO_TimeAxesGL& PFO_TimeAxesGL::operator= (const PFO_TimeAxesGL& a)
{
    if (&a != this)
        PFO_TimeAxes::operator=(a);

    return *this;
}

bool PFO_TimeAxesGL::AxesSetup()
{
    zeroTime = 0.0;
    currAxesUnits = currDateTimeSpec.timeSpecUnits;

    switch (currDateTimeSpec.timeSpecType) {
        case DC_DateTimeSpec::tstArbitraryZero : {
            zeroTime = currDateTimeSpec.arbitraryZeroDT.GetExcelTime(currAxesUnits);
            break;
        }
        case DC_DateTimeSpec::tstCalendarYear : {
            DC_DateTime tempTime;
            tempTime.year = currDateTimeSpec.calendarYearStart;
            zeroTime = tempTime.GetExcelTime(currAxesUnits);
            break;
        }
        case DC_DateTimeSpec::tstExcelDays : {
            currAxesUnits = DC_DateTime::tuDays;
            break;
        }
    }

    Limit3D currLimits = GetCurrentViewLimits();
    xstart = currLimits.minLim.cX;
    xend = currLimits.maxLim.cX;
    startTime.SetExcelTime(xstart + zeroTime, currAxesUnits);
    endTime.SetExcelTime(xend + zeroTime, currAxesUnits);
    return startTime.IsOK() && endTime.IsOK();
}


DC_DateTime PFO_TimeAxesGL::SetIncStart(AxesTimeUnits incType,
                                        int           incrementCount)
{
    DC_DateTime incTime;

    switch  (incType) {
        case atuYears : case atuQuarters: {
            incTime.year = startTime.year;
            incTime.year /= incrementCount;
            incTime.year *= incrementCount;
            break;
        }
        case atuMonths : {
            incTime.year = startTime.year;
            incTime.month = startTime.month - 1;
            incTime.month /= incrementCount;
            incTime.month *= incrementCount;
            incTime.month++;
            break;
        }
        case atuDays : {
            incTime.year = startTime.year;
            incTime.month = startTime.month;
            incTime.day = startTime.day - 1;
            incTime.day /= incrementCount;
            incTime.day *= incrementCount;
            incTime.day++;
            break;
        }
        case atuHours : {
            incTime = startTime;
            incTime.hour /= incrementCount;
            incTime.hour *= incrementCount;
            incTime.minute = 0;
            incTime.second = 0;
            break;
        }
        case atuMinutes : {
            incTime = startTime;
            incTime.minute /= incrementCount;
            incTime.minute *= incrementCount;
            incTime.second = 0;
            break;
        }
        case atuSeconds : {
            incTime = startTime;
            incTime.second /= incrementCount;
            incTime.second *= incrementCount;
            break;
        }
    }
    return incTime;
}


void PFO_TimeAxesGL::GetNextInc(DC_DateTime& nextInc,
                                AxesTimeUnits incType,
                                int          incCount)
{
    double currSec = nextInc.GetExcelTime();

    switch  (incType) {
        case atuYears: {
            nextInc.year += incCount;
            break;
        }
        case atuQuarters: {
            nextInc.month += 3 * incCount;
            if (nextInc.month > 12)
            {
                int nyear = nextInc.month / 12;
                nextInc.month = nextInc.month % 12;
                nextInc.year += nyear;
            }
            break;
        }
        case atuMonths : {
            nextInc.month += incCount;
            if (nextInc.month > 12)
            {
                int nyear = nextInc.month / 12;
                nextInc.month = nextInc.month % 12;
                nextInc.year += nyear;
            }
            break;
        }
        case atuDays : {
            nextInc.SetExcelTime(currSec + 86400.0 * double(incCount));
            break;
        }
        case atuHours : {
            nextInc.SetExcelTime(currSec + 3600.0 * double(incCount));
            break;
        }
        case atuMinutes : {
            nextInc.SetExcelTime(currSec + 60.0 * double(incCount));
            break;
        }
        case atuSeconds : {
            nextInc.SetExcelTime(currSec + double(incCount));
            break;
        }
    }

}

double  PFO_TimeAxesGL::GetIncVal(const DC_DateTime& nextInc)
{
    return nextInc.GetExcelTime(currAxesUnits) - zeroTime;
}

int PFO_TimeAxesGL::CountIncrements(AxesTimeUnits incrementType,
                                    int           incrementCount)
{
    DC_DateTime nextInc = SetIncStart(incrementType, incrementCount);
    int nincChecked = 0;
    int nincToPlot = 0;
    while (true)
    {
        double nextVal = GetIncVal(nextInc);
        if (nextVal > xend)
            return nincToPlot;

        nincChecked++;
        if (nextVal >= xstart)
            nincToPlot++;

        if (nincChecked > 500)
            return nincToPlot;

        GetNextInc(nextInc, incrementType, incrementCount);
    }
}

void PFO_TimeAxesGL::SetAutoIncrements()
{
    int delYears = endTime.year - startTime.year;

    useMajorIncrementCount = 1;
    useMinorIncrementCount = 1;

    if (delYears > 2)
    {
        useMajorIncrementUnits = atuYears;
        useMajorIncrementCount = delYears / 5;

        if (useMajorIncrementCount > 2)
        {
            useMinorIncrementUnits = atuYears;
            useMinorIncrementCount = useMajorIncrementCount / 5;
        }
        else
        {
            useMinorIncrementUnits = atuMonths;
            useMinorIncrementCount = 3;
        }
    }
    else
    {
        double delDays = endTime.GetExcelTime(DC_DateTime::tuDays)
                        - startTime.GetExcelTime(DC_DateTime::tuDays);

        if (delDays > 120.0)
        {
            double delMonths = delDays / 30.0; // approx
            useMajorIncrementUnits = atuMonths;
            useMajorIncrementCount = int(delMonths) / 5;

            if (useMajorIncrementCount > 1)
            {
                useMinorIncrementUnits = atuMonths;
            }
            else
            {
                useMinorIncrementUnits = atuDays;
                useMinorIncrementCount = 7;
            }
        }
        else if (delDays > 30.0)
        {
            useMajorIncrementUnits = atuMonths;
            useMinorIncrementUnits = atuDays;
            useMinorIncrementCount = 5;
        }
        else if (delDays > 10.0)
        {
            useMajorIncrementUnits = atuDays;
            useMajorIncrementCount = 5;
            useMinorIncrementUnits = atuDays;
        }
        else if (delDays > 1.0)
        {
            useMajorIncrementUnits = atuDays;
            useMinorIncrementUnits = atuHours;
            useMinorIncrementCount = 12;
        }
        else
        {
            double delHours = delDays * 24.0;
            if (delHours > 10.0)
            {
                useMajorIncrementUnits = atuHours;
                useMajorIncrementCount = 2;
                useMinorIncrementUnits = atuMinutes;
                useMinorIncrementCount = 30;
            }
            else if (delHours > 5.0)
            {
                useMajorIncrementUnits = atuHours;
                useMinorIncrementUnits = atuMinutes;
                useMinorIncrementCount = 15;
            }
            else if (delHours > 1.0)
            {
                useMajorIncrementUnits = atuMinutes;
                useMajorIncrementCount = 30;
                useMinorIncrementUnits = atuMinutes;
                useMinorIncrementCount = 5;
            }
            else
            {
                double delMinutes = delHours * 60.0;
                if (delMinutes > 15.0)
                {
                    useMajorIncrementUnits = atuMinutes;
                    useMajorIncrementCount = 10;
                    useMinorIncrementUnits = atuMinutes;
                    useMinorIncrementCount = 2;
                }
                else if (delMinutes > 5.0)
                {
                    useMajorIncrementUnits = atuMinutes;
                    useMajorIncrementCount = 2;
                    useMinorIncrementUnits = atuSeconds;
                    useMinorIncrementCount = 30;
                }
                else if (delMinutes > 1.0)
                {
                    useMajorIncrementUnits = atuMinutes;
                    useMinorIncrementUnits = atuSeconds;
                    useMinorIncrementCount = 15;
                }
                else
                {
                    useMajorIncrementUnits = atuSeconds;
                    useMajorIncrementCount = 5;
                    useMinorIncrementUnits = atuSeconds;
                }
            }
        }
    }

    if (useMajorIncrementCount == 0)
        useMajorIncrementCount = 1;

    if (useMinorIncrementCount == 0)
        useMinorIncrementCount = 1;
}

void PFO_TimeAxesGL::DrawPlotObject()
{
    if (!AxesSetup())
    {
        SetObjErrMsg("Axes limits out of reasonable time range");
        return;
    }

    if (!InitDrawObject())
        return;

    if (autoIncrements)
        SetAutoIncrements();
    else
    {
        useMinorIncrementUnits = minorIncrementUnits;
        useMinorIncrementCount = minorIncrementCount;
        useMajorIncrementUnits = majorIncrementUnits;
        useMajorIncrementCount = majorIncrementCount;
    }

    PD_2DXYPS*  currXY = dynamic_cast<PD_2DXYPS*>(assocPlot);
    if (currXY == 0)
        GenAppInternalError("PFO_TimeAxesGL::DrawPlotObject");

    currXY->SetIncrFont();
    const PSC_Font& xIncrFont = currXY->xAxis.incrFont;
    const PC_2DPlotAnno& plotAnnotation = currXY->plotAnnotation;


    OGL2DBase& currPlot = static_cast<OGL2DBase&>(*objectBase);

    currPlot.ClearClipPlanes();

    Point2D axesOffComp, labelOffComp;
    bool dummy;
    currPlot.GetAxesOffsets(axesOffComp, labelOffComp, dummy);

    Point2D majorTicLength = currPlot.GetPixelComponents(plotAnnotation.majorTicLength);
    Point2D minorTicLength = currPlot.GetPixelComponents(plotAnnotation.minorTicLength);
    Point2D majorTicOffset = currPlot.GetPixelComponents(plotAnnotation.majorTicOffset);
    Point2D minorTicOffset = currPlot.GetPixelComponents(plotAnnotation.minorTicOffset);

    double convYaxisMin, convYaxisMax;
    currXY->yAxis.GetAxisLimits(convYaxisMin, convYaxisMax);
    convYaxisMin = currXY->TransformY(convYaxisMin);
    convYaxisMax = currXY->TransformY(convYaxisMax);

    if (plotAnnotation.drawAxesLines)
    {
        currPlot.AxesDrawSetup();
        currPlot.HardCopyBlockStart(12);         // 2 axes + 2 caps per axes * 2

        currXY->DrawXAxes(convYaxisMin, -axesOffComp.pY);
        if (plotAnnotation.framePlot)
            currXY->DrawXAxes(convYaxisMax, axesOffComp.pY);

        if (plotAnnotation.axesOffset > 0)
        {
            // slight kluge -- tic routines require untransformed X&Ys
            double xmin, xmax;
            currXY->xAxis.GetAxisLimits(xmin, xmax);

            // draw caps on axes
            currXY->DrawXTic(xmin, convYaxisMin, -axesOffComp.pY, axesOffComp.pY, 0.0);
            currXY->DrawXTic(xmax, convYaxisMin, -axesOffComp.pY, axesOffComp.pY, 0.0);

            if (plotAnnotation.framePlot)
            {
                currXY->DrawXTic(xmin, convYaxisMax, axesOffComp.pY, -axesOffComp.pY, 0.0);
                currXY->DrawXTic(xmax, convYaxisMax, axesOffComp.pY, -axesOffComp.pY, 0.0);
            }
        }
        currPlot.HardCopyBlockEnd();
    }

    // minor incs
    double axmaxOffset = 0.0;
    int nincs = CountIncrements(useMinorIncrementUnits, useMinorIncrementCount);

    if ((nincs > 0) && (currXY->xAxis.axisMinorIncs != PC_Axes::aitNone))
    {
        if (currXY->xAxis.axisMinorIncs == PC_Axes::aitGrid)
        {
            currPlot.MinorGridDrawSetup();
            currPlot.HardCopyBlockStart(nincs  * 2);
            if (plotAnnotation.framePlot)
                axmaxOffset = axesOffComp.pY;
        }
        else
        {
            currPlot.MinorTicDrawSetup();
            currPlot.HardCopyBlockStart(nincs  *  4);
        }

        DC_DateTime nextInc = SetIncStart(useMinorIncrementUnits, useMinorIncrementCount);
        int nincChecked = 0;
        while (true)
        {
            double nextVal = GetIncVal(nextInc);
            nincChecked++;
            if ((nextVal > xend) || (nincChecked > 500))
                break;
            if (nextVal >= xstart)
            {
                if (currXY->xAxis.axisMinorIncs == PC_Axes::aitGrid)
                    currXY->DrawXGrid(nextVal, -axesOffComp.pY, axmaxOffset, minorTicOffset.pY);
                else
                {
                    currXY->DrawXTic(nextVal, convYaxisMin, -axesOffComp.pY, minorTicLength.pY, majorTicOffset.pY);
                    if (plotAnnotation.framePlot)
                        currXY->DrawXTic(nextVal, convYaxisMax, axesOffComp.pY, -minorTicLength.pY, -majorTicOffset.pY);
                }
            }
            GetNextInc(nextInc, useMinorIncrementUnits, useMinorIncrementCount);
        }
        currPlot.HardCopyBlockEnd();
    }

    nincs = CountIncrements(useMajorIncrementUnits, useMajorIncrementCount);
    if ((nincs > 0) && (currXY->xAxis.axisMajorIncs != PC_Axes::aitNone))
    {
        if (currXY->xAxis.axisMajorIncs == PC_Axes::aitGrid)
        {
            currPlot.MajorGridDrawSetup();
            currPlot.HardCopyBlockStart(nincs  * 2);
            if (plotAnnotation.framePlot)
                axmaxOffset = axesOffComp.pY;
        }
        else
        {
            currPlot.MajorTicDrawSetup();
            currPlot.HardCopyBlockStart(nincs  *  4);
        }

        DC_DateTime nextInc = SetIncStart(useMajorIncrementUnits, useMajorIncrementCount);
        int nincChecked = 0;
        while (true)
        {
            double nextVal = GetIncVal(nextInc);
            nincChecked++;
            if ((nextVal > xend) || (nincChecked > 500))
                break;
            if (nextVal >= xstart)
            {
                if (currXY->xAxis.axisMajorIncs == PC_Axes::aitGrid)
                    currXY->DrawXGrid(nextVal, -axesOffComp.pY, axmaxOffset, majorTicOffset.pY);
                else
                {
                    currXY->DrawXTic(nextVal, convYaxisMin, -axesOffComp.pY, majorTicLength.pY, majorTicOffset.pY);
                    if (plotAnnotation.framePlot)
                        currXY->DrawXTic(nextVal, convYaxisMax, axesOffComp.pY, -majorTicLength.pY, -majorTicOffset.pY);
                }
            }
            GetNextInc(nextInc, useMajorIncrementUnits, useMajorIncrementCount);
        }
        currPlot.HardCopyBlockEnd();
    }


    // increment labels

    double maxHeight = 0.0;
    double xaxYpos = convYaxisMin - axesOffComp.pY - labelOffComp.pY - majorTicOffset.pY;

    GL_Base::SetColor(plotAnnotation.axesDataPen);

    HorizontalTextAlignment halign;
    VerticalTextAlignment valign;
    if (xIncrFont.fontRotation == PSC_Font::fr_Horiz)
    {
        halign = hta_Center;
        valign = vta_Top;
    }
    else
    {
        valign = vta_Center;
        if (xIncrFont.fontRotation == PSC_Font::fr_VertLeft)
            halign = hta_Right;
        else
            halign = hta_Left;
    }

    ExtendedLabelData labInfo;

    DC_DateTime nextInc = SetIncStart(useMajorIncrementUnits, useMajorIncrementCount);
    int nincChecked = 0;
    while (true)
    {
        double nextVal = GetIncVal(nextInc);
        nincChecked++;
        if ((nextVal > xend) || (nincChecked > 500))
            break;

        if (nextVal >= xstart)
        {
            char labStr[120];
            switch (incrementLabelType) {
                case iltDate : {
                    nextInc.DateToString(labStr, 120, dateFormat);
                    break;
                }
                case iltTime : {
                    nextInc.TimeToString(labStr, 120, timeFormat);
                    break;
                }
                case iltTimeDate : {
                    char timeStr[80];
                    nextInc.TimeToString(timeStr, 80, timeFormat);
                    char dateStr[80];
                    nextInc.DateToString(dateStr, 80, dateFormat);
                    MakeString(labStr, timeStr, dateTimeDelimStr, dateStr, 120);
                    break;
                }
                case iltDateTime : {
                    char timeStr[80];
                    nextInc.TimeToString(timeStr, 80, timeFormat);
                    char dateStr[80];
                    nextInc.DateToString(dateStr, 80, dateFormat);
                    MakeString(labStr, dateStr, dateTimeDelimStr, timeStr, 120);
                    break;
                }
            }

            if (xIncrFont.fontRotation == PSC_Font::fr_Horiz)
            {
                if (currPlot.GetExtendedStringSizeInfo(xIncrFont, labStr, labInfo))
                {
                    double totHeight = labInfo.height + labInfo.aboveTop;
                    if ((fabs(totHeight) > fabs(maxHeight)))
                        maxHeight = totHeight;

                }
            }
            else
            {
                double w, h;
                if (currPlot.GetStringSizeInfo(xIncrFont, labStr, w, h))
                {
                    if (fabs(w) > fabs(maxHeight))
                        maxHeight = w;
                }
            }
            currPlot.PrintString(xIncrFont, labStr,
                                    halign, valign,
                                    currXY->TransformX(nextVal), xaxYpos);
        }
        GetNextInc(nextInc, useMajorIncrementUnits, useMajorIncrementCount);
    }

    if (IsNotNull(axesLabel))
    {
        double convXaxisMin, convXaxisMax;
        currXY->xAxis.GetAxisLimits(convXaxisMin, convXaxisMax);
        convXaxisMin = currXY->TransformX(convXaxisMin);
        convXaxisMax = currXY->TransformX(convXaxisMax);

        double xPos = (convXaxisMin + convXaxisMax) / 2.0;
        double yPos = xaxYpos - maxHeight - labelOffComp.pY;
        currPlot.PrintString(plotAnnotation.labelFont, axesLabel, hta_Center, vta_Top, xPos, yPos);
    }

    currPlot.SetClipPlanes();

    CloseDrawObject();
}

void PFO_TimeAxesGL::ReportCursorData(const Coord3D& cursorPos)
{
    if ((!doPlot) || (!cursorReport->showReport) || StatusNotOK())
    {
        //  unmanage if set up
        cursorReport->ReportTitleSetup(GetID());
        return;
    }

    if (!cursorReport->reportInitialized)
    {
        cursorReport->ReportInit("Date:");
        cursorReport->ReportTitleSetup(GetID());
        cursorReport->ReportLabelSetup(0, "Date:");
        cursorReport->ReportLabelSetup(1, "Time:");
    }

    DC_DateTime cursorTime;
    startTime.SetExcelTime(cursorPos.cX + zeroTime, currAxesUnits);

    char outStr[80];
    startTime.DateToString(outStr, 80);
    cursorReport->ReportUpdate(0, outStr);
    startTime.TimeToString(outStr, 80);
    cursorReport->ReportUpdate(1, outStr);
}

