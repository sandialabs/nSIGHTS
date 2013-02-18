///////////////////////////////////////////////////////////////////////////////////
//
// PC_2DPlotAnno.cpp
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
//      defines axes/increment layout and annotation for 2D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <genClass/U_Str.h>

#include <genPlotClass/PC_2DPlotAnno.h>

PC_2DPlotAnno :: PC_2DPlotAnno()
{
    SetDefault();
}

PC_2DPlotAnno :: PC_2DPlotAnno(const PC_2DPlotAnno& a)
{
    LocalCopy(a);
}

PC_2DPlotAnno& PC_2DPlotAnno::operator= (const PC_2DPlotAnno& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void PC_2DPlotAnno::LocalCopy(const PC_2DPlotAnno& a)
{
    plotAnno        =  a.plotAnno;

    axesDataPen     =  a.axesDataPen    ;
    gridLineDataPen =  a.gridLineDataPen;

    framePlot       =  a.framePlot      ;          // second set of axes on top and right
    labelAxes       =  a.labelAxes      ;
    labelIncrements =  a.labelIncrements;
    drawAxesLines   =  a.drawAxesLines  ;
    gridOverData    =  a.gridOverData   ;

    //  labels
    CopyString(xaxesLabel, xaxesLabel, maxLabelLen);
    CopyString(yaxesLabel, yaxesLabel, maxLabelLen);

    labelFont       =  a.labelFont     ;
    incrementFont   =  a.incrementFont ;

    axesLineWidth   =  a.axesLineWidth ;
    majorTicWidth   =  a.majorTicWidth ;
    minorTicWidth   =  a.minorTicWidth ;
    majorGridWidth  =  a.majorGridWidth;
    minorGridWidth  =  a.minorGridWidth;

    majorGridLineType = a.majorGridLineType;
    minorGridLineType = a.minorGridLineType;

    axesOffset      =  a.axesOffset    ;
    majorTicLength  =  a.majorTicLength;
    minorTicLength  =  a.minorTicLength;


}


void PC_2DPlotAnno::SetDefault()
{
    plotAnno    = paXandY;

    // pens
    axesDataPen     =   6;
    gridLineDataPen =   6;

    // format
    framePlot       =   true;          // second set of axes on top and right
    labelAxes       =   false;          //
    labelIncrements =   true;
    drawAxesLines   =   true;
    gridOverData    =   false;

    //  labels
    SetToNull(xaxesLabel);
    SetToNull(yaxesLabel);

    //  fonts
    labelFont.fontSize = 12;

    //  line widths (pixels)
    axesLineWidth   =    2;
    majorTicWidth   =    2;
    minorTicWidth   =    1;
    majorGridWidth  =    2;
    minorGridWidth  =    1;

    majorGridLineType = plt_Solid;
    minorGridLineType = plt_Solid;

    //  miscellania
    axesOffset      =    5;
    majorTicLength  =    5;
    minorTicLength  =    3;
    majorTicOffset  =    0;
    minorTicOffset  =    0;
}

bool PC_2DPlotAnno::DoAnno() const
{
    return plotAnno != paNone;
}

bool PC_2DPlotAnno::DoXAxis() const
{
    return (plotAnno == paXandY) || (plotAnno == paXOnly);
}

bool PC_2DPlotAnno::DoYAxis() const
{
    return (plotAnno == paXandY) || (plotAnno == paYOnly);
}

