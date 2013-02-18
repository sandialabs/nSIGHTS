///////////////////////////////////////////////////////////////////////////////////
//
// GL_Extrusion.h
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

#ifndef GL_EXTRUSION_H
#define GL_EXTRUSION_H

#include <genClass/SC_ColorSpec.h>
#include <genClass/SC_CoordArray.h>
#include <genClass/SC_PointArray.h>

#include <genPlotClass/PSC_EdgeSpec.h>
#include <genPlotClass/PSC_ExtrusionSpec.h>

#include <genPFOGL/GL_Polygon.h>


class GL_Extrusion : public GL_Polygon {
    private:
        double              extrudeScale;
        SC_CoordArray       coordsToExtrude;

        const double&       facetPolyOffset;
        const SC_ColorSpec& facetColor;
        const PSC_EdgeSpec&  edgeData;

    public:
                    GL_Extrusion(const double& polyOff,
                                 const SC_ColorSpec& inColor,
                                 const PSC_EdgeSpec&  edgeIn) : facetPolyOffset(polyOff), facetColor(inColor), edgeData(edgeIn) {}


        void        DrawExtrusion(const SC_CoordArray&           extrudeCoords,
                                  const PSC_TubeExtrusionSpec&   extrusionData,
                                        int                      startIndx = 0);

        void        DrawExtrusion(const SC_CoordArray&            extrudeCoords,
                                  const PSC_PolygonExtrusionSpec& extrusionData,
                                        int                       startIndx = 0);

    private:
        void        ProcessAndExtrude(const PSC_ExtrusionSpec&   extrusionData,
                                      const Coord3D&             scaleMultiplier, // for axes scalin
                                      const SC_PointArray&       contour,        // 2D contour
                                      const SC_PointArray&       contourNormal);  // 2D contour normals


        //  adapted from gle library
        //  by Linas Vepstas 1991
        void        UpSanityCheck (      Coord3D&  up,      // up vector for contour
                                   const SC_CoordArray& pointArray);


        void        ExtrusionAngleJoin (const SC_PointArray& contour,        // 2D contour
                                        const SC_PointArray& contourNormal,  // 2D contour normals
                                        const Coord3D&       up,             // up vector for contour
                                        const SC_CoordArray& extrudeCoords,  // path to extrude
                                              bool           capEnds);

        void        DrawEndCap(const SC_CoordArray& endCap,
                               const Coord3D&       endNormal);

};



#endif // !GL_EXTRUSION_H

