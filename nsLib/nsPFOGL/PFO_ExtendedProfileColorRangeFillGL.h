///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ExtendedProfileColorRangeFillGL.h
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
//      Implements code to draw nsPFO functional objects n a plot
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_EXTENDEDPROFILECOLORRANGEFILLGL_H
#define PFO_EXTENDEDPROFILECOLORRANGEFILLGL_H

#include <genClass/C_Common.h>
#include <genClass/C_Graphic.h>

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_BoolArray.h>

#include <genPFOGL/OGLBaseObj.h>
#include <genPFOGL/GL_Polygon.h>

#include <nsPFO/PFO_ExtendedProfileColorRangeFill.h>

class PFO_ExtendedProfileColorRangeFillGL : public PFO_ExtendedProfileColorRangeFill, protected OGLBaseObj {
    private:
        int             polygonCount;
        Coord3D         edgeOffset;
        int             vertexCount;
        SC_DoubleArray  polyVals;
        SC_BoolArray    polyValid;
        SC_CoordArray   assocCoord;
        SC_CoordArray   plotCoord;
        SC_CoordArray   polyCoord;
        GL_Polygon      polyObj;

    public:
                        PFO_ExtendedProfileColorRangeFillGL(PlotDefC& assPlt);
                        PFO_ExtendedProfileColorRangeFillGL(const PFO_ExtendedProfileColorRangeFillGL& a, PlotDefC& assPlt);
                        ~PFO_ExtendedProfileColorRangeFillGL();

        PFO_ExtendedProfileColorRangeFillGL& operator= (const PFO_ExtendedProfileColorRangeFillGL& a);

        void            ReportCursorData(const Coord3D& cursorPos);
        void            DrawPlotObject();

    private:
        void            InitCommon(PlotDefC& assPlt);
        void            BuildAndDrawPoly(int colorIndx);
        void            AddInterPoint(const double& xVal, int edgeIndex);
        void            CheckInterEdges(int edgeIndex);
};

#endif //PFO_EXTENDEDPROFILECOLORRANGEFILLGL_H

