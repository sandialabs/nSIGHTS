///////////////////////////////////////////////////////////////////////////////////
//
// GL_Arrow.cpp
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

#include <genPFOGL/GL_Arrow.h>


const double GL_Arrow::arrowEps = 1.0E-05;


void  GL_Arrow::SetArrowProperties(const PSC_ArrowSpec& inSpec)
{
    if (defAssocGL == 0)
        GenAppInternalError("GL_Arrow::SetArrowProperties");


    drawHead = inSpec.plotHeads;
    bodyThick = inSpec.lineThk;
    headSize = inSpec.headSize;
    arrowScale = defAssocGL->GetIndependentScale(headSize);
    perPixelScale = defAssocGL->GetIndependentScale(1);

    GL_Line::SetLine(bodyThick);
    if (inSpec.fillHeads)
    {
        SetPolygonAsFilled(0.0);
        headMode = GL_POLYGON;
    }
    else
    {
        SetPolygonAsLine(bodyThick, 0.0);
        headMode = GL_LINE_LOOP;
    }

    if (assoc2DGL == 0)
    {
        //  completely arbitary -- sym size 4 is v small, 35 is large
        static const double arrScFactor = 300.0;

        arrowScale.cX /= arrScFactor;
        arrowScale.cY /= arrScFactor;
        arrowScale.cZ /= arrScFactor;
    }

}
bool GL_Arrow::CalcPlaneRotation(const double& dx,
                                 const double& dy,
                                       double& rotAng)
{
    double len = sqrt(dx * dx + dy * dy);
    if (len < arrowEps)
    {
        rotAng = 0.0;
        return false;
    }

    double sinR = dy / len;
    double cosR = dx / len;
    rotAng = Degrees(atan2(sinR, cosR));
    return true;
}


void GL_Arrow::DrawArrow(const Coord3D& stPoint,
                         const Coord3D& endPoint,
                               Plane3D  arrowPlane)
{
    if (stPoint == endPoint)
        return;

    //now drawing line within arrow code to have correct normals
    //DrawLine(stPoint, endPoint);
    if (!SetupDraw(stPoint, endPoint) || (tranSt.SameCoord(tranEnd)))
        return;

    if (!drawHead)
        return;

    // get direction components
    double dx = (tranEnd.cX - tranSt.cX) / perPixelScale.cX;
    double dy = (tranEnd.cY - tranSt.cY) / perPixelScale.cY;
    double dz = (tranEnd.cZ - tranSt.cZ) / perPixelScale.cZ;
    double rP;
    bool planeOK;

    // calc primary rotation and check for 0 length
    // also set coordinates of arrow head for normal calculations
    switch (arrowPlane)
    {
        case p3D_XY : {
                planeOK = CalcPlaneRotation(dx, dy, rP);
                normalCoords[1] = Coord3D(-1.0, 0.25, 0.0);
                normalCoords[2] = Coord3D(-1.0, -0.25, 0.0);
                break;
        }
        case p3D_XZ : {
                planeOK = CalcPlaneRotation(dx, dz, rP);
                normalCoords[1] = Coord3D(-1.0, 0.0, 0.25);
                normalCoords[2] = Coord3D(-1.0, 0.0, -0.25);
                break;
        }
        case p3D_YZ : {
                planeOK = CalcPlaneRotation(dy, dz, rP);
                normalCoords[1] = Coord3D(0.0, -1.0, 0.25);
                normalCoords[2] = Coord3D(0.0, -1.0, -0.25);
                break;
        }
    }

    // head of arrow, required for normal calcs
    normalCoords[0] = Coord3D(0.0);

    // need to set normals for drawing of line
    Coord3D normVector;
    CalcNormals(normVector);
    glNormal3d(GLdouble(normVector.cX), GLdouble(normVector.cY), GLdouble(normVector.cZ));

    //draw line
    glBegin(GL_LINES);
        glVertex3d(GLdouble(tranSt.cX), GLdouble(tranSt.cY), GLdouble(tranSt.cZ));
        glVertex3d(GLdouble(tranEnd.cX), GLdouble(tranEnd.cY), GLdouble(tranEnd.cZ));
    glEnd();

    //if plane is not okay, still want line, but arrow head will not be drawn
    if (!planeOK)
        return;

    //now prepare to draw arrow head
    glPushMatrix();
    glTranslated(tranEnd.cX, tranEnd.cY, tranEnd.cZ);
    glScaled(arrowScale.cX, arrowScale.cY, arrowScale.cZ);

    // rotate plane
    switch (arrowPlane)
    {
        case p3D_XY : {
            //  rotation in plane
            glRotated(rP, 0.0, 0.0, 1.0);
            if (fabs(dz) > arrowEps)
            {
                double dxy = sqrt(dx * dx + dy * dy);
                double rZ;
                CalcPlaneRotation(dxy, dz, rZ);
                glRotated(rZ, 0.0, -1.0, 0.0);
            }
            break;
        }
        case p3D_XZ : {
            glRotated(rP, 0.0, -1.0, 0.0);
            if (fabs(dy) > arrowEps)
            {
                double dxz = sqrt(dx * dx + dz * dz);
                double rY;
                CalcPlaneRotation(dxz, dy, rY);  // must be true
                glRotated(rY, 0.0, 0.0, 1.0);
            }
            break;
        }
        case p3D_YZ : {
            glRotated(rP, 1.0, 0.0, 0.0);
            if (fabs(dx) > arrowEps)
            {
                double dyz = sqrt(dy * dy + dz * dz);
                double rZ;
                CalcPlaneRotation(dyz, dx, rZ);  // must be true
                glRotated(rZ, 0.0, 0.0, -1.0);
            }
            break;
        }
    }


    //reset normals after glScale
    glNormal3d(GLdouble(normVector.cX), GLdouble(normVector.cY), GLdouble(normVector.cZ));

    glBegin(headMode);
        glVertex3d(normalCoords[0].cX, normalCoords[0].cY, normalCoords[0].cZ);
        glVertex3d(normalCoords[1].cX, normalCoords[1].cY, normalCoords[1].cZ);
        glVertex3d(normalCoords[2].cX, normalCoords[2].cY, normalCoords[2].cZ);
    glEnd();

    glPopMatrix();
}

void GL_Arrow::DrawArrow(const Point2D& stPoint,
                         const Point2D& endPoint)
{
    if (stPoint == endPoint)
        return;

    if (assoc2DGL == 0)
        GenAppInternalError("GL_Arrow::Draw2DArrow");

    DrawLine(Coord3D(stPoint, 0.0), Coord3D(endPoint, 0.0));

    if (!drawHead)
        return;

    double dx = (tranEnd.cX - tranSt.cX) / perPixelScale.cX;
    double dy = (tranEnd.cY - tranSt.cY) / perPixelScale.cY;
    double r;
    if (!CalcPlaneRotation(dx, dy, r))
        return;

    glPushMatrix();
    glTranslated(tranEnd.cX, tranEnd.cY, tranEnd.cZ);
    glScaled(arrowScale.cX, arrowScale.cY, 0.0);
    glRotated(r, 0.0, 0.0, 1.0);

    glBegin(headMode);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(-1.0, -0.25, 0.0);
        glVertex3d(-1.0, 0.25, 0.0);
    glEnd();

    glPopMatrix();
}


void GL_Arrow::DrawAnnoArrow(const Point2D& stPoint,
                         const Point2D& endPoint)
{
    if (stPoint == endPoint)
        GenAppInternalError("GL_Arrow::DrawAnnoArrow");

    Std2DHardCopyBlockStart(5);

    glBegin(GL_LINE_STRIP);
        glVertex3d(GLdouble(stPoint.pX), GLdouble(stPoint.pY), 0.0);
        glVertex3d(GLdouble(endPoint.pX), GLdouble(endPoint.pY), 0.0);
    glEnd();

    if (!drawHead)
    {
        HardCopyBlockEnd();
        return;
    }

    double dx = (endPoint.pX - stPoint.pX) / perPixelScale.cX;
    double dy = (endPoint.pY - stPoint.pY) / perPixelScale.cY;
    double r;
    if (!CalcPlaneRotation(dx, dy, r))
    {
        HardCopyBlockEnd();
        return;
    }

    glPushMatrix();
    glTranslated(endPoint.pX, endPoint.pY, 0.0);

    arrowScale = defAssocGL->GetIndependentScale(double(headSize) * 1.25);
    glScaled(arrowScale.cX, arrowScale.cY, 1.0);
    glRotated(r, 0.0, 0.0, 1.0);

    glBegin(headMode);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(-1.0, -0.25, 0.0);
        glVertex3d(-1.0, 0.25, 0.0);
    glEnd();

    glPopMatrix();

    HardCopyBlockEnd();

}

