///////////////////////////////////////////////////////////////////////////////////
//
// GL_StippleSpec.cpp
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

#include <genPFOGL/GL_StippleSpec.h>



void  GL_StippleSpec::SetStippleScale(int symSize)
{
    if (defAssocGL == 0)
        GenAppInternalError("GL_StippleSpec::SetStippleScale");
    stippleScale = defAssocGL->GetIndependentScale(symSize);

    if (assoc2DGL == 0)
        GenAppInternalError("GL_StippleSpec::SetStippleScale-3D??");
    stippleScale.cZ = 1.0;
}

void  GL_StippleSpec::SetAnnoStippleScale(int symSize)
{
    stippleScale = defAssocGL->GetIndependentScale(symSize);
    stippleScale.cZ = 1.0;
}


void  GL_StippleSpec::SetProperties(const PSC_StippleSpec&  inSpec)
{
    currType = inSpec.stippleType;
    SetLineType(plt_Solid);
    SetLine(inSpec.stippleLineThickness);
    SetStippleScale(inSpec.stippleSize);
    SetColor(inSpec.stipplePen);
}

void  GL_StippleSpec::DrawStipple(const Point2D& llPoint,           // in plot coords
                                  const Point2D& urPoint)
{
    int dummy = 0;
    DrawStipple(llPoint, urPoint, dummy);
}

void  GL_StippleSpec::DrawStipple(const Point2D& llPoint,           // in plot coords
                                  const Point2D& urPoint,
                                          int&     nvertex)
{
    if (llPoint.PointIsNull() || urPoint.PointIsNull())
        return;

    Coord3D tranLL(llPoint);
    Coord3D tranUR(urPoint);
    if (!(TransformAndOffset(tranLL) && TransformAndOffset(tranUR)))
        return;

    double startX = tranLL.cX;  // DMin(tranLL.cX, tranUR.cX);
    double endX = tranUR.cX;    //DMax(tranLL.cX, tranUR.cX);
    double startY = tranLL.cY;  //DMin(tranLL.cY, tranUR.cY);
    double endY = tranUR.cY;    //DMax(tranLL.cY, tranUR.cY);

    const SC_Line2DArray& currSpec = stippleSpecData.GetStippleSpec(currType).stippleLines;
    double maxStipX = 0.0;
    double maxStipY = 0.0;
    for (int i = 0; i < currSpec.Size(); i++)
    {
        const Point2D& stPt = currSpec[i].stPt;
        const Point2D& endPt = currSpec[i].endPt;
        maxStipX = DMax(maxStipX, stPt.pX, endPt.pX);
        maxStipY = DMax(maxStipY, stPt.pY, endPt.pY);
    }

    SC_PointArray clipRegion;
    clipRegion.AllocAndSetSize(4);
    clipRegion[0] = Point2D(startX, startY);
    clipRegion[1] = Point2D(startX, endY);
    clipRegion[2] = Point2D(endX, endY);
    clipRegion[3] = Point2D(endX, startY);

    double zVal = assoc2DGL->GetZVal();

    Line2D plotLine;

    bool yreverse = (stippleScale.cY < 0.0);
    bool xreverse = (stippleScale.cX < 0.0);

/*  if (stippleScale.cY < 0.0)
    {
        if (startY > endY)
            Swap(startY, endY);
        stippleScale.cY = fabs(stippleScale.cY);
    }


    if (stippleScale.cX < 0.0)
    {
        if (startX > endX)
            Swap(startX, endX);
        stippleScale.cX = fabs(stippleScale.cX);
    }*/

    double currY = startY;
    if (yreverse)
        currY = endY;

    while ((yreverse && (currY > startY)) || ((!yreverse) && (currY < endY)))
    {
        double currX =  startX;
        if (xreverse)
            currX = endX;
        while ((xreverse && (currX > startX)) || ((!xreverse) && (currX < endX)))
        {
            for (int i = 0; i < currSpec.Size(); i++)
            {
                const Point2D& stPt = currSpec[i].stPt;
                const Point2D& endPt = currSpec[i].endPt;
                if (stPt.SamePoint(endPt))
                {
                    Point2D plotPoint(currX + stPt.pX * stippleScale.cX, currY + stPt.pY * stippleScale.cY);
                    if (clipRegion.PointInPoly(plotPoint))
                    {
                        glBegin(GL_POINTS);
                            glVertex3d(GLdouble(plotPoint.pX), GLdouble(plotPoint.pY), GLdouble(zVal));
                        glEnd();
                        nvertex ++;
                    }
                }
                else
                {

                    Line2D nextLine(Point2D(currX + stPt.pX * stippleScale.cX, currY + stPt.pY * stippleScale.cY),
                                    Point2D(currX + endPt.pX * stippleScale.cX, currY + endPt.pY * stippleScale.cY));
                    if (clipRegion.IntersectLine(nextLine, plotLine, false, false))
                    {
                        glBegin(GL_LINES);
                            glVertex3d(GLdouble(plotLine.stPt.pX), GLdouble(plotLine.stPt.pY), GLdouble(zVal));
                            glVertex3d(GLdouble(plotLine.endPt.pX), GLdouble(plotLine.endPt.pY), GLdouble(zVal));
                        glEnd();
                        nvertex += 2;
                    }
                }
            }
            currX += stippleScale.cX * maxStipX;
        }
        currY += stippleScale.cY * maxStipY;
    }

}

