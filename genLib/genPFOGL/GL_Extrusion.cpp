///////////////////////////////////////////////////////////////////////////////////
//
// GL_Extrusion.cpp
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

#include <genPlotClassPS/OGL_MatrixOps.h>
#include <genPFOGL/GL_Line.h>

#include <genPFOGL/GL_Extrusion.h>

void GL_Extrusion::UpSanityCheck (      Coord3D&  up,      // up vector for contour
                                  const SC_CoordArray& pointArray)
{

    // now, right off the bat, we should make sure that the up vector
    // is in fact perpendicular to the polyline direction
    Coord3D diff = pointArray[1] - pointArray[0];
    double len   = diff.Magnitude();

    if (fabs(len) < stdEps)
    {
        GenAppWarningMsg("GL_Extrusion" , "initial segment zero length");

        // loop till we find something that ain't of zero length */
        for (int i= 1; i < pointArray.Size() - 2; i++)
        {
            diff = pointArray[i + 1] - pointArray[i];
            len  = diff.Magnitude();
            if (fabs(len) > stdEps)
                break;
        }
   }

   // normalize diff to be of unit length
   diff.Normalize();

   // we want to take only perpendicular component of up w.r.t. the initial segment
   up.Perpendicular(diff);

   len = up.Magnitude();
   if (fabs(len) < stdEps)
   {
        GenAppWarningMsg("GL_Extrusion" , "contour up vector parallel to tubing direction");

        // do like the error message says ...
        up = diff;
   }

}

void  GL_Extrusion::DrawEndCap(const SC_CoordArray& endCap,
                               const Coord3D&       endNormal)
{
    glBegin(GL_POLYGON);
        glNormal3dv((GLdouble *) (&endNormal));
        for (int k = 0; k < endCap.Size(); k++)
            glVertex3dv((GLdouble *) (&endCap[k]));
    glEnd();

    if (edgeData.plotEdges  && (!defAssocGL->DoingHardCopy()))
    {
        SetColor(edgeData.edgePen);
        SetPolygonLine(edgeData.edgeThk, edgeData.edgePolyOffset);

        glBegin(GL_LINE_LOOP);
            glNormal3dv((GLdouble *) (&endNormal));
            for (int k = 0; k < endCap.Size(); k++)
                glVertex3dv((GLdouble *) (&endCap[k]));

        glEnd();

        SetColor(facetColor);
        SetPolygonFilled(facetPolyOffset);
    }
}




void  GL_Extrusion::ExtrusionAngleJoin (const SC_PointArray& contour,        // 2D contour
                                        const SC_PointArray& contourNormal,  // 2D contour normals
                                        const Coord3D&       up,             // up vector for contour
                                        const SC_CoordArray& extrudeCoords,  // path to extrude
                                              bool           capEnds)
{
    // copy to local
    Coord3D yup(up);

    UpSanityCheck(yup, extrudeCoords);

    // the origin is at the origin
    Coord3D origin(0.0);
    // and neg_z is at neg z
    Coord3D neg_z(0.0, 0.0, 1.0);

    // ignore all segments of zero length
    int i = 1;
    int inext = i;
    Coord3D diff;
    double len = 0.0;
    extrudeCoords.NextNonDegeneratePoint(inext, len, diff);
    double len_seg = len;   // store for later use

    // get the bisecting plane */
    Coord3D bi_0;
    bi_0.BisectingPlane(extrudeCoords[0], extrudeCoords[1], extrudeCoords[inext]);

    // reflect the up vector in the bisecting plane
    yup.Reflect(bi_0);


    // may as well get the normals set up now
    SC_CoordArray normals(contour.Size());
    for (int j = 0; j < contour.Size(); j++)
    {
        normals[j].cX = contourNormal[j].pX;
        normals[j].cY = contourNormal[j].pY;
        normals[j].cZ = 0.0;
     }

    Coord3D bi_1;
    bool firstEnd = true;
    Matrix4 view;

    //  coordinates for each segment
    SC_CoordArray frontLoop(contour.Size());
    SC_CoordArray backLoop(contour.Size());

    SetColor(facetColor);
    SetPolygonFilled(facetPolyOffset);


    // draw tubing, not doing the first segment
    while (inext < extrudeCoords.Size()-1)
    {

        int inextnext = inext;
        // ignore all segments of zero length
        extrudeCoords.NextNonDegeneratePoint(inextnext, len, diff);

        // get the next bisecting plane
        bi_1.BisectingPlane(extrudeCoords[i], extrudeCoords[inext], extrudeCoords[inextnext]);

        // save current view matrix
        glPushMatrix();

        // rotate so that z-axis points down v2-v1 axis,
        // and so that origen is at v1
        view.Uviewpoint(extrudeCoords[i], extrudeCoords[inext], yup);
        view.GLMultMatrix();

        // rotate the bisecting planes into the local coordinate system
        Coord3D bisector_0 = view.VectorDotProduct(bi_0);
        Coord3D bisector_1 = view.VectorDotProduct(bi_1);

        neg_z.cZ = -len_seg;

        // calculate the tube points
        for (int j=0; j < contour.Size(); j++)
        {
            /* Next, we want to define segments. We find the endpoints of
             * the segments by intersecting the contour with the bisecting
             * plane.
             */

            Coord3D end_point_0(contour[j], 0.0);
            Coord3D end_point_1(contour[j], -len_seg);

            /* The two end-points define a line.  Intersect this line
             * against the clipping plane defined by the PREVIOUS
             * tube segment.  */
            frontLoop[j].PlaneIntersect(origin,       // point on intersecting plane
                                   bisector_0,   // normal vector to plane
                                   end_point_0,  // point on line
                                   end_point_1); // another point on the line


            /* The two end-points define a line.  Intersect this line
             * against the clipping plane defined by the NEXT
             * tube segment.  */

            backLoop[j].PlaneIntersect(neg_z,       // point on intersecting plane
                                  bisector_1,   // normal vector to plane
                                  end_point_0,  // point on line
                                  end_point_1); // another point on the line

        }


        // if end caps are required, draw them. But don't draw any but the very first and last caps
        if (capEnds)
        {
            if (firstEnd)
            {
                if (bisector_0.cZ < 0.0)
                    bisector_0 *= -1.0;

                DrawEndCap(frontLoop, bisector_0);
                firstEnd = false;
            }
            if (inext == extrudeCoords.Size()-2)
            {
                if (bisector_1.cZ > 0.0)
                    bisector_1 *= -1.0;
                DrawEndCap(backLoop, bisector_1);
            }
        }

#ifdef _GLPRHARDCOPY_

        //  hard copy uses different plotting for edges
        if (defAssocGL->DoingHardCopy())
        {

                for (int k = 0; k < contour.Size(); k++)
                {
                    if (edgeData.plotEdges)
                    {
                        SetColor(edgeData.edgePen);
                        glpBeginGroup(GLP_OVERLAY_GROUP);
                        SetColor(facetColor);
                        SetPolygonFilled(facetPolyOffset);
                    }

                    glBegin(GL_POLYGON);
                        glNormal3dv((GLdouble *) (&normals[k]));

                        glVertex3dv((GLdouble *) (&frontLoop[k]));
                        glVertex3dv((GLdouble *) (&backLoop[k]));
                        int backIndx = (k + 1) % contour.Size();

                        glVertex3dv((GLdouble *) (&backLoop[backIndx]));
                        glVertex3dv((GLdouble *) (&frontLoop[backIndx]));
                    glEnd();

                    if (edgeData.plotEdges)
                        glpEndGroup();
                }
        }
        else
        {
#endif

            // draw the segments
            if (! assoc3DGL->DoLightingCalcs())
            {
                glBegin(GL_QUAD_STRIP);

                    for (int k = 0; k < contour.Size(); k++)
                    {
                        glVertex3dv((GLdouble *) (&frontLoop[k]));
                        glVertex3dv((GLdouble *) (&backLoop[k]));
                    }
                    // close loop
                    glVertex3dv((GLdouble *) (&frontLoop[0]));
                    glVertex3dv((GLdouble *) (&backLoop[0]));

                glEnd();
            }
            else
            {
                glBegin(GL_QUADS);

                    for (int k = 0; k < contour.Size(); k++)
                    {
                        glNormal3dv((GLdouble *) (&normals[k]));

                        glVertex3dv((GLdouble *) (&frontLoop[k]));
                        glVertex3dv((GLdouble *) (&backLoop[k]));
                        int backIndx = (k + 1) % contour.Size();

                        glVertex3dv((GLdouble *) (&backLoop[backIndx]));
                        glVertex3dv((GLdouble *) (&frontLoop[backIndx]));
                    }

                glEnd();
            }

            //  draw the edges
            if (edgeData.plotEdges  && (!defAssocGL->DoingHardCopy()))
            {
                SetColor(edgeData.edgePen);
                SetPolygonLine(edgeData.edgeThk, edgeData.edgePolyOffset);

                glBegin(GL_LINES);
                    for (int k = 0; k < contour.Size(); k++)
                    {
                        glNormal3dv((GLdouble *) (&normals[k]));
                        glVertex3dv((GLdouble *) (&frontLoop[k]));
                        glVertex3dv((GLdouble *) (&backLoop[k]));
                    }
                glEnd();

                SetColor(facetColor);
                SetPolygonFilled(facetPolyOffset);
            }
#ifdef _GLPRHARDCOPY_
        }
#endif

        // pop this matrix, do the next set
        glPopMatrix();

        // bump everything to the next vertex
        len_seg = len;
        i = inext;
        inext = inextnext;
        bi_0 = bi_1;

        // reflect the up vector in the bisecting plane
        yup.Reflect(bi_0);
    }
}

void  GL_Extrusion::ProcessAndExtrude(const PSC_ExtrusionSpec&   extrusionData,
                                      const Coord3D&             scaleMultiplier,
                                      const SC_PointArray&      contour,
                                      const SC_PointArray&      contourNormal)
{
    int i;
    int nOK = 0;
    for (i = 0; i < coordsToExtrude.Size(); i++)
    {
        Coord3D nextCoord(coordsToExtrude[i]);
        if (TransformAndOffset(nextCoord))
            coordsToExtrude[nOK++] = nextCoord;
    }
    coordsToExtrude.SetSize(nOK);
    if (coordsToExtrude.Size() < 3)
        return;

    bool capEnds = extrusionData.capEnds;
    int ntube = contour.Size();

    Line3D startLine, endLine;
    startLine.stPt = coordsToExtrude[1];
    endLine.stPt = coordsToExtrude.LastIndex();
    double tVal = 0.5;
    // check for closed loops

    if (startLine.stPt.SameCoord(endLine.stPt))
    {
        startLine.endPt = coordsToExtrude[coordsToExtrude.Size() - 2];
        endLine.endPt = coordsToExtrude[2];
        capEnds = false;
    }
    else
    {
        startLine.endPt = coordsToExtrude[2];
        endLine.endPt = coordsToExtrude[coordsToExtrude.Size() - 2];
        tVal = -0.5;
    }

    coordsToExtrude[0] = startLine.PointOnLine(tVal);
    coordsToExtrude += endLine.PointOnLine(tVal);


    // avoid degenerate vectors
    // first, find a non-zero length segment
    int inext = 0;
    double len = 0.0;
    Coord3D v21;
    coordsToExtrude.NextNonDegeneratePoint(inext, len, v21);
    if (inext == coordsToExtrude.Size())
    {
        coordsToExtrude.SetEmpty();
        return;
    }

    // next, check to see if this segment lies along x-axis
    Coord3D up;
    if ((fabs(v21.cX) < stdEps) && (fabs(v21.cZ) < stdEps))
    {
        up = 1.0;
    }
    else
    {
        up = 0.0;
        up.cY = 1.0;
    }

#ifdef _GLPRHARDCOPY_
    if (defAssocGL->DoingHardCopy())
    {
        int npoly = (coordsToExtrude.Size() + 2) * ntube;
        int nvertex = npoly * 4 + ntube * 2;
        if (edgeData.plotEdges)
        {
            nvertex += npoly * 2;
            GL_Line::SetLine(edgeData.edgeThk);
        }
        StdHardCopyBlockStart(nvertex);
    }
#endif

    if (extrusionData.adjustForScaling)
    {

        for (i = 0; i < coordsToExtrude.Size(); i++)
            coordsToExtrude[i] *= scaleMultiplier;

        // save current view matrix
        glPushMatrix();
        // scale by multiplier
        glScaled(1.0 / scaleMultiplier.cX, 1.0 / scaleMultiplier.cY, 1.0 / scaleMultiplier.cZ);
    }


    ExtrusionAngleJoin (contour, contourNormal, up, coordsToExtrude, capEnds);

    if (extrusionData.adjustForScaling)
        glPopMatrix();

    HardCopyBlockEnd();


    coordsToExtrude.SetEmpty();
}



void  GL_Extrusion::DrawExtrusion(const SC_CoordArray&          extrudeCoords,
                                  const PSC_TubeExtrusionSpec&  extrusionData,
                                        int                     startIndx)
{
    if (assoc3DGL == 0)
        GenAppInternalError("GL_Extrusion::DrawExtrusion");


    if (extrusionData.usePixelSizing)
    {
        static const double exScFactor = 450.0; // arbitrary but seems to be OK
        extrudeScale = double(extrusionData.extrusionPixelSize) /
                        (exScFactor * defAssocGL->GetCurrentView().scale);
        // big time kluge
        const_cast<PSC_TubeExtrusionSpec&>(extrusionData).adjustForScaling = true;
    }
    else
    {
        extrudeScale = extrusionData.extrusionRealSize;
    }
    Coord3D scaleMultiplier = assoc3DGL->GetExtrusionScale(extrusionData.usePixelSizing);

    // calculate contours for regular polygon
    int ntube = extrusionData.nextrusionPoints;
    SC_PointArray contour(ntube);
    SC_PointArray normals(ntube);
    double radius = extrudeScale;
    double radPerSide = Radians(360.0 / double(ntube));
    double s = sin (radPerSide);
    double c = cos (radPerSide);

    normals[0].pX = 1.0;
    normals[0].pY = 0.0;
    contour[0].pX = radius;
    contour[0].pY = 0.0;
    // calculate norm using recursion relations */
    int i;
    for (i = 1; i < ntube; i++) {
        normals[i].pX = normals[i-1].pX * c - normals[i-1].pY * s;
        normals[i].pY = normals[i-1].pX * s + normals[i-1].pY * c;
        contour[i].pX = radius * normals[i].pX;
        contour[i].pY = radius * normals[i].pY;
    }

    coordsToExtrude.Alloc(extrudeCoords.Size() + 2);

    for (i = startIndx; i < extrudeCoords.Size(); i++)
    {
        if (extrudeCoords[i].CoordIsNull())
            ProcessAndExtrude(extrusionData, scaleMultiplier, contour, normals);
        else
        {
            if (coordsToExtrude.IsEmpty())
                coordsToExtrude += extrudeCoords[i];
            coordsToExtrude += extrudeCoords[i];
        }
    }

    ProcessAndExtrude(extrusionData, scaleMultiplier, contour, normals);

}

void  GL_Extrusion::DrawExtrusion(const SC_CoordArray&           extrudeCoords,
                                  const PSC_PolygonExtrusionSpec& extrusionData,
                                        int                       startIndx)
{
    if (assoc3DGL == 0)
        GenAppInternalError("GL_Extrusion::DrawExtrusion");

    // temporary
    SC_PointArray contour(extrusionData.extrusionShape.Size()), normals(extrusionData.extrusionShape.Size());
    Coord3D orthCoord(0.0, 0.0, 1.0), normCoord, nextCoord;

    coordsToExtrude.Alloc(extrudeCoords.Size() + 2);

    Coord3D scaleMultiplier = assoc3DGL->GetExtrusionScale(false);


    int nextSeg = 0;
    while(extrusionData.extrusionShape.GetNextSegment(nextSeg, contour))
    {
        if (contour.Size() < 3)
            continue;

        // swap X & Y (gle uses different convention ???
        contour.Transpose();

        //  y to -y  -- gle seems to be +ve down
        int npoly = contour.Size();
        int i;
        for (i = 0; i < npoly; i++)
            contour[i].pY *= -1.0;

        // calculate normals for arbitrary polygon  -- klugy approach
        for (i = 0; i < npoly; i++) {
            Coord3D currCoord(contour[i], 0.0);
            if (i < (npoly - 1))
                nextCoord = contour[i + 1];
            else
                nextCoord = contour[0];
            nextCoord.cZ = 0.0;

            normCoord = CrossProduct(currCoord - nextCoord, orthCoord);
            normCoord.cZ = 0.0;
            normCoord  *= -1.0;
            normCoord.Normalize();
            normals[i] = Point2D(normCoord);
        }

        coordsToExtrude.SetEmpty();
        for (i = startIndx; i < extrudeCoords.Size(); i++)
        {
            if (extrudeCoords[i].CoordIsNull())
                ProcessAndExtrude(extrusionData, scaleMultiplier, contour, normals);
            else
            {
                if (coordsToExtrude.IsEmpty())
                    coordsToExtrude += extrudeCoords[i];
                coordsToExtrude += extrudeCoords[i];
            }
        }

        ProcessAndExtrude(extrusionData, scaleMultiplier, contour, normals);
    }


}

