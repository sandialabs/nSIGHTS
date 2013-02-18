///////////////////////////////////////////////////////////////////////////////////
//
// GL_Base.h
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

#ifndef GL_BASE_H
#define GL_BASE_H

#include <GL/GL.h>
#include <gl/GLU.h>

#include <genClass/C_Common.h>
#include <genClass/SC_ColorSpec.h>
#include <genClass/SC_CoordArray.h>

#include <genPlotClassPS/OGL_Base.h>
#include <genPlotClassPS/OGL_2DBase.h>
#include <genPlotClassPS/OGL_3DBase.h>

// forward refs
class PlotDefC;

class GL_Base {
    protected:
        static PlotDefC*   defAssocPlot;   // associated plot definition - used mostly for pen extraction
        static OGLBase*    defAssocGL;     // base OpenGL object - required for hardcopy
        static OGL2DBase*  assoc2DGL;      // 2D object used to get z val for 2D plotting
        static OGL3DBase*  assoc3DGL;      // 3D object used to get lighting info
        static const double polyOffsetEps;  // min fabs(polyOff) before set to 0.0 -- currently 1.0E-09
        static double       polyOffsetBias; // Z depth specific polyOffset bias for OpenGL 1.0
                                            // default is 1.0E-7 which is OK for 24 bit buffer on SGI
                                            // can be set from mView command line with -polybias

        static Coord3D      offsetCoord;     // for hard copy offsets
        static Coord3D      normalCoords[3];   // for lighting normal calcs on poly faces

        static SC_ColorSpec  currColor;     // used for VRML shape

        static  bool        axesReversed;   // used to trigger more intensive normal calcs

    public:
                        GL_Base() {}

        static void     SetDefaultObjectPlotDef(PlotDefC*    assPlot);
        static void     SetDefaultObjectOGLBase(OGLBase*     assGL);

        static void     SetColor(const SC_ColorSpec&  color);
        static void     SetColor(int penNumber);  // uses plotDef pen

        static void     GetColor(SC_ColorSpec&  color);
        static void     GetClearColor(SC_ColorSpec&  color);

        static void     SetDataLayer(int layer);
        static void     SetDataLayerOffset(int layer);

        static void     SetPolyOffsetBias(const double& polyBias);  // sets static member & prints val to std out

        static void     GetAnnoPixelXY(const Point2D&   annoXY,
                                             int&       pixelX,
                                             int&       pixelY);

        static void     SetOffsetCoord(const Coord3D& objOffsetCoord);

                        // were in GL_Polygon - moved when GL_Polygon
                        // internal structure changed
        static void     SetPolygonAsFilled(const double& polyOffset);
        static void     SetPolygonAsLine(        int     lineWidth,
                                        const double& polyOffset);



        static void     Disable3Dlighting();  // if 3D with lighting enabled, disable it
        static void     Restore3Dlighting();  // if 3D with lighting enabled, enable it

#ifdef _GLPRHARDCOPY_
        static void     StdHardCopyBlockStart(int nvertex);
        static void     Std2DHardCopyBlockStart(int nvertex);
        static void     HardCopyBlockEnd();
#else
        static void     StdHardCopyBlockStart(int nvertex) {};
        static void     Std2DHardCopyBlockStart(int nvertex) {};
        static void     HardCopyBlockEnd() {};
#endif

    protected:
        static bool     TransformAndOffset(Coord3D& untranCoord);

        static void     CalcNormals();
        static void     CalcNormals(Coord3D& normVector);

        // VRML output support
        static void     VMRLwriteShapePolyHeader(bool polyIsFilled);
        static void     VMRLwritePolyCoords(const SC_CoordArray& polyPoints);
        static void     VMRLwriteColor();


};


#endif // !GL_BASE_H

