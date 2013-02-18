///////////////////////////////////////////////////////////////////////////////////
//
// GL_PixelCursorTrack.h
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

#ifndef GL_PIXELCURSORTRACK_H
#define GL_PIXELCURSORTRACK_H

#include <genClass/SC_IntArray.h>
#include <genClass/SC_PointArray.h>
#include <genClass/SC_DoubleArray.h>

#include <genPFOGL/GL_Base.h>

// doesnt use OpenGL but does use currrent base plot def
class PD_2D;

class GL_PixelCursorTrack : public GL_Base {
    public:
        enum        {cursorStrLen = 10};

        SC_IntArray     xCursor;
        SC_IntArray     yCursor;
        SC_IntArray     cursorIndx;

        PD_2D*          assoc2DPlot;

    public:
                    GL_PixelCursorTrack();
                    ~GL_PixelCursorTrack();


        void        Init();

        void        AddSeriesDataToTrack(const SC_PointArray&   seriesData,
                                         const SC_IntArray&     indexData);
        void        AddSeriesDataToTrack(const SC_DoubleArray&  xData,
                                         const SC_DoubleArray&  yData,
                                         const SC_IntArray&     indexData);
        void        AddPointDataToTrack(const Point2D&   pointData,
                                              int        indx);
        void        AddPointDataToTrack(const double&    xData,
                                        const double&    yData,
                                              int        indx);

        bool        GetClosestIndex(const double&    xData,
                                    const double&    yData,
                                          int&       indx);
};


#endif // !GL_PIXELCURSORTRACK_H

