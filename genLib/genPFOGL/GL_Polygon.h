///////////////////////////////////////////////////////////////////////////////////
//
// GL_Polygon.h
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

#ifndef GL_POLYGON_H
#define GL_POLYGON_H

#include <genClass/C_Graphic.h>

#include <genClass/SC_PointArray.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_CoordArray.h>

#include <genPFOGL/PC_Rectangle.h>
#include <genPFOGL/GL_Base.h>


class GL_Polygon : public GL_Base {

    private:

#define TESSCALL
#ifdef MESA
        GLUtriangulatorObj*  polyTesselator;
#else
        GLUtesselator*  polyTesselator;
#ifdef _WIN32
#undef  TESSCALL
#define TESSCALL __stdcall
#endif
#endif
        bool            useTesselator;
        bool            polygonIsFilled;
        SC_CoordArray   tessVertexes;
        static SC_DoubleArray   tessCombine;

        SC_CoordArray   polyVertexes;

    public:
                        GL_Polygon();
                        ~GL_Polygon();

        void            SetPolygonFilled(const double& polyOffset);
        void            SetPolygonLine(      int     lineWidth,
                                   const double& polyOffset);

        void            InitTesselator();
        void            InitTesselator(bool useTess);
        void            ClearTesselator();

        //  basic polygon draw - no hardcopy
        void        DrawPolygon(const SC_CoordArray& polyPoints);
        void        DrawPolygon(const SC_CoordArray& polyPoints,
                                const Coord3D&       polyNormal); // specified normal
        void        DrawPolygon(const SC_CoordArray& topSide,
                                const SC_CoordArray& bottomSide);
        void        DrawPolygon(const SC_PointArray& polyPoints); // 2D
        void        DrawPolygon(const SC_CoordArray& coords,
                                const SC_IntArray& indexes);

        //  used mostly by Anno routines for legend box frames - includes hardcopy
        void        DrawRectangle(const PC_Rectangle& rectData,
                                        bool           opaque,
                                        bool           frame,
                                        int            frameThk);
        void        DrawFilledRectangle(const PC_Rectangle& rectData);
        void        DrawRectangle(const PC_Rectangle& rectData);


    private:
        void        DoDrawRectangle(const PC_Rectangle& rectData,
                                          GLenum        drawMode);

        // glu tesselator callbacks
        static void TESSCALL BeginPolyCB(GLenum which);
        static void TESSCALL EndPolyCB();
        static void TESSCALL PolyErrorCB(GLenum errorCode);
        static void TESSCALL PolyVertexCB(void* vertexData);
        static void TESSCALL PolyCombineCB(GLdouble     coords[3],
                                           GLdouble*    vertex_data[4],
                                           GLfloat      weight[4],
                                           GLdouble**   outData);

        bool        SetPolyNormals(const SC_CoordArray& polyPoints);


};


#endif
