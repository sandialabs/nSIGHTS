///////////////////////////////////////////////////////////////////////////////////
//
// GL_Line.h
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

#ifndef GL_LINE_H
#define GL_LINE_H

#include <genClass/C_Graphic.h>
#include <genClass/SC_PointArray.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_CoordArray.h>
#include <genClass/SC_LineArray.h>

#include <genPlotClass/PSC_LineSpec.h>

#include <genPFOGL/GL_Base.h>


class GL_Line : public GL_Base {
    protected:
        Coord3D     tranSt;
        Coord3D     tranEnd;

    private:
        static PlotLineType currLineType;
        static int          currLineThick;

        // for pixel processed lines
        int             vertexCount;
        SC_DoubleArray  pixelX;
        SC_DoubleArray  pixelY;
        double          pixelZ;

        //
        bool            lineIsMitred;
        Point2D         lastPolyEnd;
        Point2D         lastPolyPoints[4];
        Point2D         currPolyPoints[4];

        SC_PointArray   lastPoly;

    public:
                    GL_Line();

        static void SetLine(int lineWidth);
        static void SetLineType(PlotLineType lineType);
        static void SetLineProperties(const PSC_LineSpec&  inSpec);

        //  basic line draw - no hardcopy
        void        DrawLine(const Coord3D& stPoint,
                             const Coord3D& endPoint);

                    // for annotation -- no transform, includes hard
        void        DrawAnnoLine(const Point2D& stPoint,
                                 const Point2D& endPoint);

        //  multiple line draw - includes hard copy
        void        DrawLines(const SC_Line3DArray& lineData);

        void        DrawLines(const SC_CoordArray& stCoords,
                              const SC_CoordArray& endCoords);

        void        DrawLines(const SC_PointArray& stPoints,
                              const SC_PointArray& endPoints);

        // sigs below have correct line type processing for hardcopy
        // and have mitring support
        void        SetLineMitreing(bool doMitre);

        void        DrawLines(const SC_PointArray& linePoints);
        void        DrawLines(const SC_CoordArray& lineCoords,
                                    int             startIndx = 0);
        void        DrawLines(const SC_DoubleArray& xData,
                              const SC_DoubleArray& yData,
                                    int             startIndx = 0);

    protected:
        bool        SetupDraw(const Coord3D& stPoint,
                              const Coord3D& endPoint);

        void        ProcessPixelLines();
        void        DoPixelLines(bool drawLines);

        void        DrawPixelSeg(const double& stX, const double& stY,
                                 const double& endX, const double& endY,
                                 bool  drawSeg);



};



#endif // !GL_LINE_H

