///////////////////////////////////////////////////////////////////////////////////
//
// GL_Symbol.h
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

#ifndef GL_SYMBOL_H
#define GL_SYMBOL_H

#include <genClass/C_Graphic.h>
#include <genClass/SC_ColorSpec.h>
#include <genClass/SC_PointArray.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_CoordArray.h>

#include <genPlotClass/PSC_SeriesSpec.h>
#include <genPlotClass/PSC_SymbolSpec.h>
#include <genPlotClass/PSC_3DSeriesSpec.h>

#include <genPlotClass/PSC_EdgeSpec.h>

#include <genPFOGL/GL_Line.h>


class GL_Symbol : public GL_Line {
    private:
        Coord3D             symScale;


#ifdef _GLPRHARDCOPY_
        //  for glpr hard copy support
        enum        {nsymbols = 10};
        static int  symbol2DVertexCount[nsymbols];
        static int  symbol3DVertexCount[nsymbols];
        static int  symbol3DPolygonCount[nsymbols];

        static bool         doingOverlay;
        static int          overlayEdgePen;
        static SC_ColorSpec overlaySymbolColor;
#endif
        static SC_CoordArray    sphereCoords;
        static SC_IntArray      sphereLineIndexes;
        static SC_IntArray      spherePolyIndexes;

    public:
        void        SetSymScale(int  symSize);
                    // 3D
        void        SetSymProperties(      PlotSymbol   plotSymbol,  // checks if filled, sets poly settings approprioately
                                           int          pWidth,      // line w if outline
                                     const double&      lineOffset,   // poly offset if poly
                                     const double&      polyOffset);  // poly offset if poly

        void        SetSymProperties(const PSC_SymbolSpec&  inSpec);

                    // 2D
        void        SetSymProperties(PlotSymbol   plotSymbol,  // checks if filled, sets poly settings approprioately
                                     int          pWidth);      // line w if outline

                    // 2D & 3D  - no hard copy
        void        DrawSymbol(const Coord3D& symCoord,
                                     PlotSymbol plotSymbol);
                    // 2D & 3D - hard copy
        void        DrawSymbols(const SC_CoordArray& symCoords,
                                      PlotSymbol plotSymbol);

                    // 2D & 3D - hard copy with special 3D edge processing support
                    // 2 sigs
        void        DrawSymbols(const SC_CoordArray& symCoords,
                                const PSC_EdgeSpec&   edgeData,
                                      int            plotLayer,
                                const double&        polyOffset,
                                      int            symbolPen,
                                      PlotSymbol     plotSymbol,
                                      int            startIndx = 0);

        void        DrawSymbols(const SC_CoordArray& symCoords,
                                const PSC_EdgeSpec&   edgeData,
                                      int            plotLayer,
                                const double&        polyOffset,
                                const SC_ColorSpec&  symbolColor,
                                      PlotSymbol     plotSymbol,
                                      int            startIndx = 0);

                    // 2D only - no hard copy
        void        DrawSymbol(const Point2D& symCoord,
                                     PlotSymbol plotSymbol);
                    // 2D only - hard copy
        void        DrawSymbols(const SC_PointArray& symCoords,
                                      PlotSymbol plotSymbol);

                    // 2D series - includes hard copy
        void        DrawSeriesSpec(const PSC_SeriesSpec& seriesData,
                                   const SC_PointArray& seriesPoints);

        void        DrawSeriesSpec(const PSC_SeriesSpec&  seriesData,
                                   const SC_DoubleArray& xData,
                                   const SC_DoubleArray& yData,
                                         int             startIndx = 0);

                    //  2D & 3D - includes hard copy - if 3D and edges then edges are done with hard copy
        void        DrawSeriesSpec(const PSC_SeriesSpec& seriesData,
                                   const PSC_EdgeSpec&   edgeData,
                                   const double&        lineOffset,
                                         int            plotLayer,
                                   const double&        polyOffset,
                                   const SC_CoordArray& seriesCoords);

                    // 3D only
        void        DrawSeriesSpec(const PSC_3DSeriesSpec&  seriesData,
                                   const SC_CoordArray&     seriesCoords,
                                         const double&      polyOffset,
                                                int         startIndx = 0);

                    // Anno  - includes hard copy
        void        DrawAnnoSeriesSpec(const PSC_SeriesSpec& seriesData,
                                       const SC_PointArray& seriesPoints);

    private:
        void        SetAnnoSymScale(int  symSize);

        void        Draw3DSymbol(PlotSymbol plotSymbol);

        void        Draw2DSymbol(PlotSymbol plotSymbol);

        void        Draw3DSymLines(const Coord3D ptArray[],
                                   const int     indxArray[],
                                         int     nlines);

        void        Draw3DSymFaces(const Coord3D ptArray[],
                                   const int     indxArray[],
                                         int     nface);

        void        Draw2DLineSym(const Point2D ptArray[],
                                        int     npts);

        void        Draw2DSolidSym(const Point2D ptArray[],
                                         int     npts);

        static void BuildSphere();

};


#endif // !GL_SYMBOL_H

