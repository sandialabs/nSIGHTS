///////////////////////////////////////////////////////////////////////////////////
//
// GL_Symbol.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genPFOGL/GL_Extrusion.h>

#include <genPFOGL/GL_Symbol.h>


SC_CoordArray   GL_Symbol::sphereCoords;
SC_IntArray     GL_Symbol::sphereLineIndexes;
SC_IntArray     GL_Symbol::spherePolyIndexes;

void  GL_Symbol::SetSymScale(int symSize)
{
    if (defAssocGL == 0)
        GenAppInternalError("GL_Symbol::SetSymScale");
    symScale = defAssocGL->GetIndependentScale(symSize);

    if (assoc2DGL == 0)
    {
//  completely arbitary -- sym size 4 is v small, 35 is large, roughly matches anno display in series legend
static const double symScFactor = 450.0;

        symScale.cX /= symScFactor;
        symScale.cY /= symScFactor;
        symScale.cZ /= symScFactor;
    }
    else
        symScale.cZ = 1.0;
}

void  GL_Symbol::SetAnnoSymScale(int symSize)
{
    symScale = defAssocGL->GetIndependentScale(symSize);
    symScale.cZ = 1.0;
}

void  GL_Symbol::SetSymProperties(      PlotSymbol   plotSymbol,  // checks if filled, sets poly settings approprioately
                                        int          pWidth,      // line w if outline
                                  const double&      lineOffset,   // poly offset if poly
                                  const double&      polyOffset)  // poly offset if poly

{
    if (PlotSymbolOps::SymbolIsFilled(plotSymbol))
        SetPolygonAsFilled(polyOffset);
    else
        SetPolygonAsLine(pWidth, lineOffset);
}

void  GL_Symbol::SetSymProperties(      PlotSymbol   plotSymbol,  // checks if filled, sets poly settings approprioately
                                        int          pWidth)      // line w if outline

{
    if (PlotSymbolOps::SymbolIsFilled(plotSymbol))
        SetPolygonAsFilled(0.0);
    else
        SetPolygonAsLine(pWidth, 0.0);
}

void  GL_Symbol::SetSymProperties(const PSC_SymbolSpec&  inSpec)
{
    SetSymProperties(inSpec.symbolType, 1);
    SetSymScale(inSpec.symbolSize);
    SetColor(inSpec.symbolPen);
}


void  GL_Symbol::DrawSymbol(const Coord3D&   symPoint,
                                  PlotSymbol plotSymbol)
{
    if (symPoint.CoordIsNull())
        return;

    Coord3D tranPoint(symPoint);
    if (!TransformAndOffset(tranPoint))
        return;

    glPushMatrix();
    glTranslated(tranPoint.cX, tranPoint.cY, tranPoint.cZ);
    glScaled(symScale.cX, symScale.cY, symScale.cZ);

    if (assoc2DGL == 0)
        Draw3DSymbol(plotSymbol);
    else
        Draw2DSymbol(plotSymbol);

    glPopMatrix();
}

/*
 * kluge - depends upon enum in c_graphics
 *
    enum  PlotSymbol {ps_X, ps_Plus,
                      ps_Square, ps_Circle, ps_Triangle, ps_Diamond,
                      ps_FSquare, ps_FCircle, ps_FTriangle, ps_FDiamond};

*/
#ifdef _GLPRHARDCOPY_

int  GL_Symbol::symbol2DVertexCount[] = {4, 4,
                                         4, 24, 3, 4,
                                         4, 24, 3, 4};

int  GL_Symbol::symbol3DVertexCount[] = {8, 6,
                                         24, 112, 12, 24,
                                         24, 112, 12, 24};

int  GL_Symbol::symbol3DPolygonCount[] = {0, 0,
                                          0, 0, 0, 0,
                                          6, 32, 4, 8};

bool         GL_Symbol::doingOverlay    = false;
int          GL_Symbol::overlayEdgePen  = 0;
SC_ColorSpec GL_Symbol::overlaySymbolColor;

#endif


void GL_Symbol::DrawSymbol(const Point2D& symCoord,
                                 PlotSymbol plotSymbol)
{
    if (assoc2DGL == 0)
        GenAppInternalError("GL_Symbol::DrawSymbol2D");

    if (RealIsNull(symCoord.pX) || RealIsNull(symCoord.pY))
        return;

    Coord3D ptCoord(symCoord.pX, symCoord.pY, assoc2DGL->GetZVal());
    DrawSymbol(ptCoord, plotSymbol);
}

void  GL_Symbol::DrawSymbols(const SC_PointArray& symCoords,
                                   PlotSymbol plotSymbol)
{
    if (assoc2DGL == 0)
        GenAppInternalError("GL_Symbol::DrawSymbols2D");

#ifdef _GLPRHARDCOPY_
    StdHardCopyBlockStart(symCoords.Size() * symbol2DVertexCount[plotSymbol]);

#endif

    for (int i = 0; i < symCoords.Size(); i++)
        DrawSymbol(symCoords[i], plotSymbol);

    HardCopyBlockEnd();
}

void GL_Symbol::DrawSymbols(const SC_CoordArray& symCoords,
                                  PlotSymbol     plotSymbol)
{
#ifdef _GLPRHARDCOPY_
    int nSymVertex;
    if (assoc2DGL == 0)
        nSymVertex = symbol3DVertexCount[plotSymbol];
    else
        nSymVertex = symbol2DVertexCount[plotSymbol];
    StdHardCopyBlockStart(symCoords.Size() * nSymVertex);

#endif

    for (int i = 0; i < symCoords.Size(); i++)
        DrawSymbol(symCoords[i], plotSymbol);

    HardCopyBlockEnd();

}

void GL_Symbol::DrawSymbols(const SC_CoordArray& symCoords,
                            const PSC_EdgeSpec&   edgeData,
                                  int            plotLayer,
                            const double&        polyOffset,
                                  int            symbolPen,
                                  PlotSymbol     plotSymbol,
                                  int             startIndx)
{
    DrawSymbols(symCoords, edgeData, plotLayer, polyOffset, defAssocPlot->defaultPenSet->GetColor(symbolPen),
                plotSymbol, startIndx);
}


// 2D & 3D - hard copy with special 3D edge processing support
void GL_Symbol::DrawSymbols(const SC_CoordArray& symCoords,
                            const PSC_EdgeSpec&   edgeData,
                                  int            plotLayer,
                            const double&        polyOffset,
                            const SC_ColorSpec&  symbolColor,
                                  PlotSymbol     plotSymbol,
                                  int             startIndx)
{
    SetColor(symbolColor);
    SetSymProperties(plotSymbol, edgeData.edgeThk, edgeData.edgePolyOffset, polyOffset);

#ifdef _GLPRHARDCOPY_
//  special processing for 3D with edges
    if ((defAssocGL->DoingHardCopy()) && (assoc3DGL != 0) &&
         edgeData.plotEdges && PlotSymbolOps::SymbolIsFilled(plotSymbol))
    {
        glpBegin(GLP_3D_COLOR, symCoords.Size() * (symbol3DVertexCount[plotSymbol]
                                                  + symbol3DPolygonCount[plotSymbol] * 2));

        GL_Line::SetLine(edgeData.edgeThk);
        defAssocGL->SetHardCopyParameters();

        doingOverlay = true;
        overlayEdgePen = edgeData.edgePen;
        overlaySymbolColor = symbolColor;

        for (int i = startIndx; i < symCoords.Size(); i++)
            DrawSymbol(symCoords[i], plotSymbol);

        doingOverlay = false;
        HardCopyBlockEnd();
        return;
    }
#endif

    // regular processing
    DrawSymbols(symCoords, plotSymbol);

    if (edgeData.plotEdges && PlotSymbolOps::SymbolIsFilled(plotSymbol))
    {
        if (assoc2DGL != 0)
            SetDataLayerOffset(plotLayer);

        SetColor(edgeData.edgePen);
        SetSymProperties(PlotSymbolOps::OutlineSymbol(plotSymbol), edgeData.edgeThk, edgeData.edgePolyOffset, polyOffset);

        DrawSymbols(symCoords, PlotSymbolOps::OutlineSymbol(plotSymbol));
    }
}



void  GL_Symbol::DrawSeriesSpec(const PSC_SeriesSpec& seriesData,
                                const SC_PointArray& seriesPoints)
{
    if (assoc2DGL == 0)
        GenAppInternalError("GL_Symbol::DrawSeriesSpec");

    SetColor(seriesData.seriesPen);
    if (seriesData.seriesPlotType != psl_SymbolOnly)
    {
        GL_Line::SetLine(seriesData.lineThickness);
        GL_Line::SetLineType(seriesData.lineType);
        SetLineMitreing(seriesData.mitreLines);
        DrawLines(seriesPoints);
    }

    if (seriesData.seriesPlotType != psl_LineOnly)
    {
        SetSymScale(seriesData.symbolSize);
        SetSymProperties(seriesData.symbolType, seriesData.lineThickness);
        DrawSymbols(seriesPoints, seriesData.symbolType);
    }
}

void  GL_Symbol::DrawSeriesSpec(const PSC_SeriesSpec&  seriesData,
                                const SC_DoubleArray& xData,
                                const SC_DoubleArray& yData,
                                      int             startIndx)
{
    if (assoc2DGL == 0)
        GenAppInternalError("GL_Symbol::DrawSeriesSpec2");

    SetColor(seriesData.seriesPen);
    if (seriesData.seriesPlotType != psl_SymbolOnly)
    {
        GL_Line::SetLine(seriesData.lineThickness);
        GL_Line::SetLineType(seriesData.lineType);
        SetLineMitreing(seriesData.mitreLines);

        DrawLines(xData, yData, startIndx);
        GL_Line::SetLineType(plt_Solid);
    }

    if (seriesData.seriesPlotType != psl_LineOnly)
    {
        SetSymScale(seriesData.symbolSize);
        SetSymProperties(seriesData.symbolType, seriesData.lineThickness);

#ifdef _GLPRHARDCOPY_

        StdHardCopyBlockStart(xData.Size() * symbol2DVertexCount[seriesData.symbolType]);

#endif
        for (int i = startIndx; i < xData.Size(); i++)
            DrawSymbol(Point2D(xData[i], yData[i]), seriesData.symbolType);

        HardCopyBlockEnd();

    }
}


void  GL_Symbol::DrawSeriesSpec(const PSC_SeriesSpec& seriesData,
                                const PSC_EdgeSpec&   edgeData,
                                const double&        lineOffset,
                                      int            plotLayer,
                                const double&        polyOffset,
                                const SC_CoordArray& seriesCoords)
{
    SetColor(seriesData.seriesPen);
    if (seriesData.seriesPlotType != psl_SymbolOnly)
    {
        GL_Line::SetLine(seriesData.lineThickness);
        GL_Line::SetLineType(seriesData.lineType);
        SetLineMitreing(seriesData.mitreLines);

        DrawLines(seriesCoords);
        GL_Line::SetLineType(plt_Solid);
    }

    if (seriesData.seriesPlotType != psl_LineOnly)
    {

        SetSymScale(seriesData.symbolSize);
        SetSymProperties(seriesData.symbolType, seriesData.lineThickness, lineOffset, polyOffset);
        DrawSymbols(seriesCoords, edgeData, plotLayer, polyOffset, seriesData.seriesPen, seriesData.symbolType);
    }
}

void GL_Symbol::DrawSeriesSpec(const PSC_3DSeriesSpec& seriesData,
                               const SC_CoordArray& seriesCoords,
                               const double&        polyOffset,
                                     int            startIndx)

{
    SetColor(seriesData.seriesPen);
    if (seriesData.seriesPlotType != psl_SymbolOnly)
        if (seriesData.plotLineAsTube)
        {
            PSC_EdgeSpec extEdge = seriesData;
            extEdge.plotEdges = seriesData.plotTubeEdges;
            SC_ColorSpec tubeColor = currColor;
            GL_Extrusion  extSpec(polyOffset, tubeColor, extEdge);
            extSpec.DrawExtrusion(seriesCoords, seriesData, startIndx);
        }
        else
        {
            GL_Line::SetLine(seriesData.lineThickness);
            GL_Line::SetLineType(seriesData.plotLineType);
            DrawLines(seriesCoords, startIndx);
            GL_Line::SetLineType(plt_Solid);
        }

    if (seriesData.seriesPlotType != psl_LineOnly)
    {
        SetSymScale(seriesData.symbolSize);
        PSC_EdgeSpec symEdge = seriesData;
        symEdge.plotEdges = seriesData.plotSymEdges;
        SetSymProperties(seriesData.symbolType, seriesData.lineThickness, symEdge.edgePolyOffset, polyOffset);

        DrawSymbols(seriesCoords, symEdge, 0, polyOffset, seriesData.seriesPen, seriesData.symbolType, startIndx);
    }
}


void  GL_Symbol::DrawAnnoSeriesSpec(const PSC_SeriesSpec& seriesData,
                                    const SC_PointArray& seriesPoints)
{
    SetColor(seriesData.seriesPen);
    if (seriesData.seriesPlotType != psl_SymbolOnly)
    {
        GL_Line::SetLine(seriesData.lineThickness);
        GL_Line::SetLineType(seriesData.lineType);

        Std2DHardCopyBlockStart(seriesPoints.Size());

        glBegin(GL_LINE_STRIP);
            for (int i = 0; i < seriesPoints.Size(); i++)
             glVertex3d(GLdouble(seriesPoints[i].pX), GLdouble(seriesPoints[i].pY), 0.0);
        glEnd();

        HardCopyBlockEnd();

        GL_Line::SetLineType(plt_Solid);
    }

    if (seriesData.seriesPlotType != psl_LineOnly)
    {
        SetAnnoSymScale(seriesData.symbolSize);

        SetSymProperties(seriesData.symbolType, seriesData.lineThickness);

#ifdef _GLPRHARDCOPY_
        Std2DHardCopyBlockStart(seriesPoints.Size() * symbol2DVertexCount[seriesData.symbolType]);

#endif

        for (int i = 0; i < seriesPoints.Size(); i++)
        {
            glPushMatrix();
            glTranslated(seriesPoints[i].pX, seriesPoints[i].pY, 0.0);
            glScaled(symScale.cX, symScale.cY, 1.0);
            Draw2DSymbol(seriesData.symbolType);
            glPopMatrix();
        }

        HardCopyBlockEnd();

    }
}


void GL_Symbol::Draw3DSymLines(const Coord3D ptArray[],
                               const int     indxArray[],
                                     int     nlines)
{
    int stIndx = 0;
    for (int i = 0; i < nlines; i++)
    {
        glBegin(GL_LINES);
            int indx = indxArray[stIndx++];
            glVertex3d(GLdouble(ptArray[indx].cX), GLdouble(ptArray[indx].cY), GLdouble(ptArray[indx].cZ));

            indx = indxArray[stIndx++];
            glVertex3d(GLdouble(ptArray[indx].cX), GLdouble(ptArray[indx].cY), GLdouble(ptArray[indx].cZ));
        glEnd();
    }
}

void GL_Symbol::Draw3DSymFaces(const Coord3D ptArray[],
                               const int     indxArray[],
                                     int     nface)
{
    int stIndx = 0;
    for (int i = 0; i < nface; i++)
    {
#ifdef _GLPRHARDCOPY_
        if (doingOverlay)
        {
            SetColor(overlayEdgePen);
            glpBeginGroup(GLP_OVERLAY_GROUP);
            SetColor(overlaySymbolColor);
        }
#endif

        if (assoc3DGL->DoLightingCalcs())
        {
            int currSt = stIndx;
            normalCoords[0] = ptArray[indxArray[currSt++]];
            normalCoords[1] = ptArray[indxArray[currSt++]];
            normalCoords[2] = ptArray[indxArray[currSt++]];
            CalcNormals();
        }

        glBegin(GL_POLYGON);
            int indx = indxArray[stIndx++];
            while (indx >= 0)
            {
                glVertex3d(GLdouble(ptArray[indx].cX), GLdouble(ptArray[indx].cY), GLdouble(ptArray[indx].cZ));
                indx = indxArray[stIndx++];
            }
        glEnd();

#ifdef _GLPRHARDCOPY_
        if (doingOverlay)
            glpEndGroup();
#endif

    }
}

void  GL_Symbol::BuildSphere()
{
    sphereCoords.SetResizable(512);
    sphereLineIndexes.SetResizable(512);
    spherePolyIndexes.SetResizable(512);

    int nmeridian = 12;
    int nslice = 9;

    sphereCoords += Coord3D(0.0, 0.0, 0.5);  // top point
    double sliceDeg = 90.0;
    double dSlice = 180.0 / double(nslice + 1);
    double dMeridian = Radians(360.0 / double(nmeridian));
    int slicestart = 0;
    int nsphereLines = 0;
    int nsphereFace = 0;
    int currPt;
    for (int i = 0; i < nslice; i++)
    {
        sliceDeg -= dSlice;
        double sliceRad = Radians(sliceDeg);
        double sliceZ = sin(sliceRad) * 0.5;

        double meridian = 0.0;
        double xylen = cos(sliceRad) * 0.5;

        for (int j = 0; j < nmeridian; j++)
        {
            sphereCoords += Coord3D(cos(meridian) * xylen, sin(meridian) * xylen, sliceZ);
            meridian += dMeridian;

            // add lines
            // meridian line
            currPt = sphereCoords.UpperBound();
            if (i == 0)
            {
                sphereLineIndexes += 0;
            }
            else
            {
                sphereLineIndexes += currPt - nmeridian;
            }
            sphereLineIndexes += currPt;
            nsphereLines++;

            // slice line
            if (j > 0)
            {
                sphereLineIndexes += currPt - 1;
                sphereLineIndexes += currPt;
                nsphereLines++;
            }

            // dont add poly on first pt
            if (j == 0)
                continue;

            // add poly
            if (i == 0)
            {
                spherePolyIndexes += 0;
                spherePolyIndexes += currPt - 1;
            }
            else
            {
                spherePolyIndexes += currPt - nmeridian;
                spherePolyIndexes += currPt - nmeridian - 1;
                spherePolyIndexes += currPt - 1;
            }
            spherePolyIndexes += currPt;
            spherePolyIndexes += -1;
            nsphereFace++;
        }

        // add last line on slice
        int meridianStart = currPt - nmeridian + 1;
        sphereLineIndexes += meridianStart;
        sphereLineIndexes += currPt;
        nsphereLines++;

        // add last poly on slice
        if (i == 0)
        {
            spherePolyIndexes += 0;
            spherePolyIndexes += currPt;
        }
        else
        {
            spherePolyIndexes += meridianStart - nmeridian;
            spherePolyIndexes += currPt - nmeridian;
            spherePolyIndexes += currPt;
        }
        spherePolyIndexes += meridianStart;
        spherePolyIndexes += -1;
        nsphereFace++;
    }

    sphereCoords += Coord3D(0.0, 0.0, -0.5);  // bottom point
    int lastPt = sphereCoords.UpperBound();
    // bottom slices
    for (int j = 1; j <= nmeridian; j++)
    {
        // meridian line
        sphereLineIndexes += lastPt;
        sphereLineIndexes += lastPt - j;
        nsphereLines++;

        spherePolyIndexes += lastPt;
        spherePolyIndexes += lastPt - j;
        spherePolyIndexes += lastPt - j - 1;
        spherePolyIndexes += -1;
        nsphereFace++;
    }

    // kluge - nlistData is convenient var
    sphereLineIndexes.SetSize(nsphereLines);
    spherePolyIndexes.SetSize(nsphereFace);

#ifdef _GLPRHARDCOPY_
    symbol3DVertexCount[ps_Circle] = nsphereLines * 2;
    symbol3DVertexCount[ps_FCircle] = nsphereLines * 2;
    symbol3DPolygonCount[ps_FCircle] = nsphereFace * 2;
#endif
}



void  GL_Symbol::Draw3DSymbol(PlotSymbol plotSymbol)
{

//    enum  PlotSymbol {ps_X, ps_Plus, ps_Square, ps_Circle, ps_Triangle, ps_Diamond,
//                      ps_FSquare, ps_FCircle, ps_FTriangle, ps_FDiamond};

    static const Coord3D xcoords[] = {Coord3D(0.5, 0.5, 0.5),    Coord3D(-0.5, 0.5, 0.5),
                                      Coord3D(-0.5, -0.5, 0.5),  Coord3D(0.5, -0.5, 0.5),
                                      Coord3D(0.5, 0.5, -0.5),   Coord3D(-0.5, 0.5, -0.5),
                                      Coord3D(-0.5, -0.5, -0.5), Coord3D(0.5, -0.5, -0.5)};

/*    static const Coord3D ccoords[] = {Coord3D(0.0, 0.0, 0.5),  // top

                                      Coord3D(0.35355,  0.0,        0.35355),
                                      Coord3D(0.25,     0.25,       0.35355),
                                      Coord3D(0.0,      0.35355,    0.35355),
                                      Coord3D(-0.25,    0.25,       0.35355),
                                      Coord3D(-0.35355, 0.0,        0.35355),
                                      Coord3D(-0.25,    -0.25,      0.35355),
                                      Coord3D(0.0,      -0.35355,   0.35355),
                                      Coord3D(0.25,     -0.25,      0.35355),  // upper row

                                      Coord3D(0.5,      0.0,        0.0),
                                      Coord3D(0.35355,  0.35355,    0.0),
                                      Coord3D(0.0,      0.5,        0.0),
                                      Coord3D(-0.35355, 0.35355,    0.0),
                                      Coord3D(-0.5,     0.0,        0.0),
                                      Coord3D(-0.35355, -0.35355,   0.0),
                                      Coord3D(0.0,      -0.5,       0.0),
                                      Coord3D(0.35355,  -0.35355,   0.0),  // middle row

                                      Coord3D(0.35355,  0.0,        -0.35355),
                                      Coord3D(0.25,     0.25,       -0.35355),
                                      Coord3D(0.0,      0.35355,    -0.35355),
                                      Coord3D(-0.25,    0.25,       -0.35355),
                                      Coord3D(-0.35355, 0.0,        -0.35355),
                                      Coord3D(-0.25,    -0.25,      -0.35355),
                                      Coord3D(0.0,      -0.35355,   -0.35355),
                                      Coord3D(0.25,     -0.25,      -0.35355),  // lower row

                                      Coord3D(0.0, 0.0, -0.5)};  // bottom
*/
    static const Coord3D tricoords[] = {Coord3D(0.0,        0.0,    0.5),  // top
                                        Coord3D(0.4330,     -0.25,  -.25),
                                        Coord3D(0.0,        0.5,    -.25),
                                        Coord3D(-0.4330,    -0.25,  -.25)};

    static const Coord3D diamcoords[] = {Coord3D(0.0,        0.0,    0.5),  // top
                                        Coord3D(0.5,         0.0,    0.0),
                                        Coord3D(0.0,         0.5,    0.0),
                                        Coord3D(-0.5,        0.0,    0.0),
                                        Coord3D(0.0,        -0.5,    0.0),
                                        Coord3D(0.0,        0.0,    -0.5)};

    switch (plotSymbol)
    {
        case ps_X : {
            static const int xindxs[] = {0, 6, 1, 7, 2, 4, 3, 5};
            Draw3DSymLines(xcoords, xindxs, 4);
            break;
        };
        case ps_Plus : {
            static const Coord3D pluscoords[] = {Coord3D(0.5, 0.0, 0.0),    Coord3D(0.0, 0.5, 0.0),
                                                 Coord3D(-0.5, 0.0, 0.0),   Coord3D(0.0, -0.5, 0.0),
                                                 Coord3D(0.0, 0.0, 0.5),   Coord3D(0.0, 0.0, -0.5)};
            static const int plusindxs[] = {0, 2, 1, 3, 4, 5};
            Draw3DSymLines(pluscoords, plusindxs, 3);
            break;
        };
        case ps_Square : {
            static const int sqindxs[] = {0, 1, 1, 2, 2, 3, 3, 0,
                                          4, 5, 5, 6, 6, 7, 7, 4,
                                          0, 4, 1, 5, 2, 6, 3, 7};
            Draw3DSymLines(xcoords, sqindxs, 12);
            break;
        };
        case ps_Circle : {

            if (sphereLineIndexes.IsEmpty())
                BuildSphere();


/*            static const int cindxs[] = {0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8,  // top vert
                                         1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 1,  // upper ring
                                         1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15, 8, 16,  // upper vert
                                         9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 9, //  middle ring
                                         9, 17, 10, 18, 11, 19, 12, 20, 13, 21, 14, 22, 15, 23, 16, 24,  // lower vert
                                         17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 17,  // lower ring
                                         17, 25, 18, 25, 19, 25, 20, 25, 21, 25, 22, 25, 23, 25, 24, 25}; //lower vert
*/
            Draw3DSymLines(sphereCoords.tListData, sphereLineIndexes.tListData, sphereLineIndexes.Size());
            break;
        };
        case ps_Triangle : {
            static const int trindxs[] = {0, 1, 0, 2, 0, 3,
                                          1, 2, 2, 3, 3, 1};
            Draw3DSymLines(tricoords, trindxs, 6);
            break;
        };

        case ps_Diamond : {
            static const int diamindxs[] = {0, 1, 0, 2, 0, 3, 0, 4,
                                            1, 2, 2, 3, 3, 4, 4, 1,
                                            1, 5, 2, 5, 3, 5, 4, 5};

            Draw3DSymLines(diamcoords, diamindxs, 12);
            break;
        };


        case ps_FSquare : {
            static const int sqface[] = {0, 1, 2, 3,  -1,  // top
                                         4, 5, 6, 7,  -1,  // bottom
                                         0, 1, 5, 4,  -1,
                                         1, 2, 6, 5,  -1,
                                         2, 3, 7, 6,  -1,
                                         3, 0, 4, 7,  -1};

            Draw3DSymFaces(xcoords, sqface, 6);
            break;
        };

        case ps_FCircle : {

            if (sphereLineIndexes.IsEmpty())
                BuildSphere();

/*            static const int circface[] = { 0,  1,  2, -1,
                                            0,  2,  3, -1,
                                            0,  3,  4, -1,
                                            0,  4,  5, -1,
                                            0,  5,  6, -1,
                                            0,  6,  7, -1,
                                            0,  7,  8, -1,
                                            0,  8,  1, -1,  // top row
                                            1,  2,  10, 9,  -1,
                                            2,  3,  11, 10,  -1,
                                            3,  4,  12, 11,  -1,
                                            4,  5,  13, 12,  -1,
                                            5,  6,  14, 13,  -1,
                                            6,  7,  15, 14,  -1,
                                            7,  8,  16, 15,  -1,
                                            8,  1,  9,  16,  -1,  // upper
                                            9,  10, 18, 17, -1,
                                            10, 11, 19, 18, -1,
                                            11, 12, 20, 19, -1,
                                            12, 13, 21, 20, -1,
                                            13, 14, 22, 21, -1,
                                            14, 15, 23, 22, -1,
                                            15, 16, 24, 23, -1,
                                            16,  9, 17, 24, -1,  // lower
                                            17, 18, 25, -1,
                                            18, 19, 25, -1,
                                            19, 20, 25, -1,
                                            20, 21, 25, -1,
                                            21, 22, 25, -1,
                                            22, 23, 25, -1,
                                            23, 24, 25, -1,
                                            24, 17, 25, -1};  // bottom
*/

            Draw3DSymFaces(sphereCoords.tListData, spherePolyIndexes.tListData, spherePolyIndexes.Size());
            break;
        };

//                      ps_FSquare, ps_FCircle, ps_FTriangle, ps_FDiamond};
        case ps_FTriangle : {
            static const int trface[] = {0, 1, 2, -1,  // top
                                         0, 2, 3, -1,
                                         0, 3, 1, -1,
                                         1, 2, 3, -1};

            Draw3DSymFaces(tricoords, trface, 4);
            break;
        };

        case ps_FDiamond : {
            static const int diface[] = {0, 1, 2, -1,  // top
                                         0, 2, 3, -1,
                                         0, 3, 4, -1,
                                         0, 4, 1, -1,
                                         5, 1, 2, -1,
                                         5, 2, 3, -1,
                                         5, 3, 4, -1,
                                         5, 4, 1, -1};

            Draw3DSymFaces(diamcoords, diface, 8);
            break;
        };
    }

}

void GL_Symbol::Draw2DLineSym(const Point2D ptArray[],
                                    int     npts)
{
    if (npts == 2)
        glBegin(GL_LINES);
    else
        glBegin(GL_LINE_LOOP);

        for (int i = 0; i < npts; i++)
            glVertex3d(GLdouble(ptArray[i].pX), GLdouble(ptArray[i].pY), GLdouble(0.0));

    glEnd();
}

void GL_Symbol::Draw2DSolidSym(const Point2D ptArray[],
                                     int     npts)
{
    glBegin(GL_POLYGON);
        for ( int i = 0; i < npts; i++)
            glVertex3d(GLdouble(ptArray[i].pX), GLdouble(ptArray[i].pY), GLdouble(0.0));
    glEnd();
}

void  GL_Symbol::Draw2DSymbol(PlotSymbol plotSymbol)
{
    static const Point2D sqcoords[] = { Point2D(-0.5, -0.5),
                                        Point2D(0.5, -0.5),
                                        Point2D(0.5, 0.5),
                                        Point2D(-0.5, 0.5) };

    static const Point2D cicoords[] = {Point2D(0.00000 ,0.50000),
                                       Point2D(0.12941 ,0.48296),
                                       Point2D(0.25000 ,0.43301),
                                       Point2D(0.35355 ,0.35355),
                                       Point2D(0.43301 ,0.25000),
                                       Point2D(0.48296 ,0.12941),
                                       Point2D(0.50000 ,0.00000),
                                       Point2D(0.48296 ,-0.12941),
                                       Point2D(0.43301 ,-0.25000),
                                       Point2D(0.35355 ,-0.35355),
                                       Point2D(0.25000 ,-0.43301),
                                       Point2D(0.12941 ,-0.48296),
                                       Point2D(0.00000 ,-0.50000),
                                       Point2D(-0.12941,-0.48296),
                                       Point2D(-0.25000,-0.43301),
                                       Point2D(-0.35355,-0.35355),
                                       Point2D(-0.43301,-0.25000),
                                       Point2D(-0.48296,-0.12941),
                                       Point2D(-0.50000,0.00000),
                                       Point2D(-0.48296,0.12941),
                                       Point2D(-0.43301,0.25000),
                                       Point2D(-0.35355,0.35355),
                                       Point2D(-0.25000,0.43301),
                                       Point2D(-0.12941,0.48296)};

    static const Point2D tricoords[] = {Point2D(-0.4330 ,-0.25),
                                        Point2D(0.4330 ,-0.25),
                                        Point2D(0.0 , 0.5)};

    static const Point2D diamcoords[] = {Point2D(0.0 ,-0.50),
                                         Point2D(0.5 , 0.0),
                                         Point2D(0.0 , 0.5),
                                         Point2D(-0.5 ,0.0)};

    switch (plotSymbol)
    {
        case ps_X : {
            static const Point2D xcoords1[] = {Point2D(-0.5, -0.5),    Point2D(0.5, 0.5)};
            static const Point2D xcoords2[] = {Point2D(-0.5, 0.5),    Point2D(0.5, -0.5)};
            Draw2DLineSym(xcoords1, 2);
            Draw2DLineSym(xcoords2, 2);
            break;
        };
        case ps_Plus : {
            static const Point2D pluscoords1[] = {Point2D(-0.5, 0.0),    Point2D(0.5, 0.0)};
            static const Point2D pluscoords2[] = {Point2D(0.0, 0.5),    Point2D(0.0, -0.5)};
            Draw2DLineSym(pluscoords1, 2);
            Draw2DLineSym(pluscoords2, 2);
            break;
        };
        case ps_Square : {
            Draw2DLineSym(sqcoords, 4);
            break;
        };
        case ps_Circle : {
            Draw2DLineSym(cicoords, 24);
            break;
        };
        case ps_Triangle : {
            Draw2DLineSym(tricoords, 3);
            break;
        };
        case ps_Diamond : {
            Draw2DLineSym(diamcoords, 4);
            break;
        };

        case ps_FSquare : {
            Draw2DSolidSym(sqcoords, 4);
            break;
        };

        case ps_FCircle : {
            Draw2DSolidSym(cicoords, 24);
            break;
        };
        case ps_FTriangle : {
            Draw2DSolidSym(tricoords, 3);
            break;
        };

        case ps_FDiamond : {
            Draw2DSolidSym(diamcoords, 4);
            break;
        };
    }
}


