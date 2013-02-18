///////////////////////////////////////////////////////////////////////////////////
//
// GL_Line.cpp
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

#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>

#include <genPFOGL/GL_Line.h>


PlotLineType GL_Line::currLineType = 0;
int GL_Line::currLineThick = 1;

GL_Line::GL_Line()
:   lineIsMitred(false), lastPoly(lastPolyPoints, 4)
{
    lastPoly.SetSize(4);
}

void  GL_Line::SetLine(int lineWidth)
{
    if (defAssocGL == 0)
        GenAppInternalError("GL_Line::SetLine");
    defAssocGL->SetLine(lineWidth);
    currLineThick = lineWidth;
}

void GL_Line::SetLineType(PlotLineType lineType)
{
    if (defAssocGL == 0)
        GenAppInternalError("GL_Line::SetLineType");
    defAssocGL->SetLineType(lineType);
    currLineType = lineType;
}


void GL_Line::SetLineProperties(const PSC_LineSpec&  inSpec)
{
    GL_Base::SetColor(inSpec.linePen);
    GL_Line::SetLine(inSpec.lineThickness);
    GL_Line::SetLineType(inSpec.lineType);
}


bool GL_Line::SetupDraw(const Coord3D& stPoint,
                        const Coord3D& endPoint)
{
    tranSt = stPoint;
    tranEnd = endPoint;
    bool retVal = TransformAndOffset(tranSt);
    return TransformAndOffset(tranEnd) && retVal;
}

void GL_Line::DrawLine(const Coord3D& stPoint,
                       const Coord3D& endPoint)
{
    if (defAssocGL == 0)
        GenAppInternalError("GL_Line::DrawLine");

    if (!SetupDraw(stPoint, endPoint) || (tranSt.SameCoord(tranEnd)))
        return;

    //reset normals?  based on xy
    //this seems to eliminate the dark line issue
    glNormal3d(GLdouble(0.0), GLdouble(0.0), GLdouble(1.0));

    glBegin(GL_LINES);
        glVertex3d(GLdouble(tranSt.cX), GLdouble(tranSt.cY), GLdouble(tranSt.cZ));
        glVertex3d(GLdouble(tranEnd.cX), GLdouble(tranEnd.cY), GLdouble(tranEnd.cZ));
    glEnd();

}


void GL_Line::DrawAnnoLine(const Point2D& stPoint,
                           const Point2D& endPoint)
{
    Std2DHardCopyBlockStart(2);

    glBegin(GL_LINES);
        glVertex2d(GLdouble(stPoint.pX), GLdouble(stPoint.pY));
        glVertex2d(GLdouble(endPoint.pX), GLdouble(endPoint.pY));
    glEnd();

    HardCopyBlockEnd();
}

void GL_Line::DrawLines(const SC_Line3DArray& lineData)
{
    StdHardCopyBlockStart(lineData.Size() * 2);

    for (int i = 0; i < lineData.Size(); i++)
        DrawLine(lineData[i].stPt, lineData[i].endPt);

    HardCopyBlockEnd();
}

void  GL_Line::DrawLines(const SC_CoordArray& stCoords,
                         const SC_CoordArray& endCoords)
{
    StdHardCopyBlockStart(stCoords.Size() * 2);

    for (int i = 0; i < stCoords.Size(); i++)
        DrawLine(stCoords[i], endCoords[i]);

    HardCopyBlockEnd();
}

void  GL_Line::DrawLines(const SC_PointArray& stPoints,
                         const SC_PointArray& endPoints)
{
    if (assoc2DGL == 0)
        GenAppInternalError("GL_Line::DrawLines2");

    StdHardCopyBlockStart(stPoints.Size() * 2);

    for (int i = 0; i < stPoints.Size(); i++)
        DrawLine(Coord3D(stPoints[i], 0.0), Coord3D(endPoints[i], 0.0));

    HardCopyBlockEnd();
}


void GL_Line::SetLineMitreing(bool doMitre)
{
    lineIsMitred = doMitre;
}

void  GL_Line::DrawLines(const SC_CoordArray& lineCoords,
                               int            startIndx)
{
    if ((assoc2DGL != 0) && ((currLineType > 0) || lineIsMitred))
    {
        assoc2DGL->SetPixelScaling();

        int npts = lineCoords.Size() - startIndx;
        pixelX.Alloc(npts);
        pixelY.Alloc(npts);
        for (int i = 0; i < npts; i++)
        {
            Coord3D tempPoint(lineCoords[i + startIndx]);
            if (tempPoint.CoordIsNull() || (!assoc2DGL->GetPixelXY(tempPoint)))
            {
                pixelX[i] = nullReal;
            }
            else
            {
                pixelX[i] = tempPoint.cX;
                pixelY[i] = tempPoint.cY;
                pixelZ = tempPoint.cZ;
            }
        }
        pixelX.SetSize(npts);
        ProcessPixelLines();
        return;
    }

    StdHardCopyBlockStart(lineCoords.Size());

    int okCount = 0;
    for (int i = startIndx; i < lineCoords.Size(); i++)
    {
         Coord3D tempPoint(lineCoords[i]);

         if (tempPoint.CoordIsNull() || (!TransformAndOffset(tempPoint)))
         {
             if (okCount > 0)
                 glEnd();
             okCount = 0;
             continue;
         }

         //reset normals?  based on xy
        //this seems to eliminate the dark line issue
        glNormal3d(GLdouble(0.0), GLdouble(0.0), GLdouble(1.0));

         if (okCount == 0)
             glBegin(GL_LINE_STRIP);
         okCount++;
         glVertex3d(GLdouble(tempPoint.cX), GLdouble(tempPoint.cY), GLdouble(tempPoint.cZ));

    }

    if (okCount > 0)
        glEnd();

    HardCopyBlockEnd();

}



void  GL_Line::DrawLines(const SC_PointArray& linePoints)
{
    if (assoc2DGL == 0)
        GenAppInternalError("GL_Line::DrawLines1");

    if (lineIsMitred || (currLineType > 0))
    {
        assoc2DGL->SetPixelScaling();

        int npts = linePoints.Size();
        pixelX.Alloc(npts);
        pixelY.Alloc(npts);
        for (int i = 0; i < npts; i++)
        {
            Coord3D tempPoint(linePoints[i], 0.0);
            if (tempPoint.CoordIsNull() || (!assoc2DGL->GetPixelXY(tempPoint)))
            {
                pixelX[i] = nullReal;
            }
            else
            {
                pixelX[i] = tempPoint.cX;
                pixelY[i] = tempPoint.cY;
                pixelZ = tempPoint.cZ;
            }
        }
        pixelX.SetSize(npts);

        ProcessPixelLines();
        return;
    }

    StdHardCopyBlockStart(linePoints.Size());
    int okCount = 0;
    for (int i = 0; i < linePoints.Size(); i++)
    {
         Coord3D tempPoint(linePoints[i], 0.0);
         if (linePoints[i].PointIsNull() || (!defAssocGL->TransformForAxes(tempPoint)))
         {
             if (okCount > 0)
                 glEnd();
             okCount = 0;
             continue;
         }

         //reset normals?  based on xy
        //this seems to eliminate the dark line issue
        glNormal3d(GLdouble(0.0), GLdouble(0.0), GLdouble(1.0));

         if (okCount == 0)
             glBegin(GL_LINE_STRIP);
         okCount++;
         glVertex3d(GLdouble(tempPoint.cX), GLdouble(tempPoint.cY), GLdouble(tempPoint.cZ));
    }

    if (okCount > 0)
        glEnd();

    HardCopyBlockEnd();
}


void  GL_Line::DrawLines(const SC_DoubleArray& xData,
                         const SC_DoubleArray& yData,
                               int             startIndx)
{
    if (assoc2DGL == 0)
        GenAppInternalError("GL_Line::DrawLines3");

    if (lineIsMitred || (currLineType > 0))
    {
        assoc2DGL->SetPixelScaling();

        int npts = xData.Size() - startIndx;
        pixelX.Alloc(npts);
        pixelY.Alloc(npts);
        for (int i = 0; i < npts; i++)
        {
            Coord3D tempPoint(xData[i + startIndx], yData[i + startIndx], 0.0);
            if (tempPoint.CoordIsNull() || (!assoc2DGL->GetPixelXY(tempPoint)))
            {
                pixelX[i] = nullReal;
            }
            else
            {
                pixelX[i] = tempPoint.cX;
                pixelY[i] = tempPoint.cY;
                pixelZ = tempPoint.cZ;
            }
        }
        pixelX.SetSize(npts);

        ProcessPixelLines();
        return;
    }


    StdHardCopyBlockStart(xData.Size());

    int okCount = 0;
    for (int i = startIndx; i < xData.Size(); i++)
    {
         Coord3D tempPoint(xData[i], yData[i], 0.0);
         if (RealIsNull(xData[i]) || RealIsNull(yData[i]) || (!defAssocGL->TransformForAxes(tempPoint)))
         {
             if (okCount > 0)
                 glEnd();
             okCount = 0;
             continue;
         }

         //reset normals?  based on xy
        //this seems to eliminate the dark line issue
        glNormal3d(GLdouble(0.0), GLdouble(0.0), GLdouble(1.0));

         if (okCount == 0)
             glBegin(GL_LINE_STRIP);
         okCount++;

         glVertex3d(GLdouble(tempPoint.cX), GLdouble(tempPoint.cY), GLdouble(tempPoint.cZ));
    }

    if (okCount > 0)
        glEnd();

    HardCopyBlockEnd();
}


void GL_Line::ProcessPixelLines()
{
    defAssocGL->SetLineSolid();
    if (lineIsMitred)
    {
//      SetPolygonAsLine(1, 0.0);

        SetPolygonAsFilled(0.0);
    }

    vertexCount = 0;
    if (defAssocGL->DoingHardCopy())
        DoPixelLines(false);

    Std2DHardCopyBlockStart(vertexCount);

    DoPixelLines(true);

    HardCopyBlockEnd();

    glPopMatrix();
}

void GL_Line::DoPixelLines(bool drawLines)
{

    unsigned short basePattern = lineTypeData.GetLineTypeSpec(currLineType).linePattern;
    int factor = lineTypeData.GetLineTypeSpec(currLineType).lineFactor;

    unsigned short pattern = basePattern;
    double  linePos = 0.0;
    bool    penOn = false;
    int     bitPos = 0;

    double lastX, lastY, lineStartX, lineStartY;
    bool segOK = false;
    for (int i = 0; i < pixelX.Size(); i++)
    {
         if (RealIsNull(pixelX[i]))
         {
             // reset to start new line
             segOK = false;
             penOn = false;
             pattern = basePattern;
             linePos = 0.0;
             bitPos = 0;
             continue;
         }

        double nextX = pixelX[i];
        double nextY = pixelY[i];
        if (segOK)
        {
            double dX = nextX - lastX;
            double dY = nextY - lastY;

            double segLen = sqrt(Sqr(dX) + Sqr(dY));
            if (segLen < 0.1)  // ignore sub-sub-pixel segs
                continue;

            while (linePos < segLen) {

                double numLoops = (segLen - linePos)/double(factor);
                if (numLoops > 1.0E+4)
                {
                    GenAppWarningMsg("GL_Line", "Line too large to draw with current pattern");
                    return;
                }

                if (pattern & 0x0001)
                {
                    if (!penOn)
                    {
                        penOn = true;
                        lineStartX = lastX + linePos / segLen * dX;
                        lineStartY = lastY + linePos / segLen * dY;
                    }
                }
                else
                {
                    if (penOn)
                    {
                        penOn = false;
                        double lineEndX = lastX + linePos / segLen * dX;
                        double lineEndY = lastY + linePos / segLen * dY;
                        DrawPixelSeg(lineStartX, lineStartY, lineEndX, lineEndY, drawLines);
                    }
                }

                linePos += double(factor);
                bitPos++;
                if (bitPos == 16)
                {
                    pattern = basePattern;
                    bitPos = 0;
                }
                else
                    pattern = (pattern >> 1);
            }

            linePos -= segLen;

            if (penOn)
            {
                DrawPixelSeg(lineStartX, lineStartY, nextX, nextY, drawLines);
                lineStartX = nextX;
                lineStartY = nextY;
            }
        }

        lastX = nextX;
        lastY = nextY;
        segOK = true;
    }
}


void GL_Line::DrawPixelSeg(const double& stX, const double& stY,
                           const double& endX, const double& endY,
                           bool  drawSeg)
{
    if (lineIsMitred)
    {
        // line equation
        // x = x1 + f t
        // y = y1 + g t
        double f  = endX - stX;
        double g  = endY - stY;

        double shiftDistance = double(currLineThick) / 2.0;

        double dx, dy;

        // vertical line
        if (fabs(f) < stdEps)
        {
            dy = 0.0;
            if (g < 0.0)
            {
                dx = - shiftDistance;
            }
            else
            {
                dx = shiftDistance;
            }
        }
        // horizontal line
        else if (fabs(g) < stdEps)
        {
            dx = 0.0;
            if (f < 0.0)
            {
                dy = shiftDistance;
            }
            else
            {
                dy = -shiftDistance;
            }
        }
        else
        {
            double c2 = g * g / (f * f);
            double z2 = shiftDistance * shiftDistance;

            double xterm = sqrt(c2 * z2 / (1.0 + c2));

            dx = xterm;
            dy = f * xterm / g;

            if (g < 0.0)
            {
                dx = -dx;
            }
            else
            {
                dy = -dy;
            }
        }

        currPolyPoints[0].pX = stX + dx;
        currPolyPoints[1].pX = endX + dx;
        currPolyPoints[2].pX = endX - dx;
        currPolyPoints[3].pX = stX - dx;

        currPolyPoints[0].pY = stY + dy;
        currPolyPoints[1].pY = endY + dy;
        currPolyPoints[2].pY = endY - dy;
        currPolyPoints[3].pY = stY - dy;

        if (drawSeg)
        {
            //reset normals?  based on xy
            //this seems to eliminate the dark line issue
            glNormal3d(GLdouble(0.0), GLdouble(0.0), GLdouble(1.0));

            glBegin(GL_POLYGON);
                for(int i = 0; i <  4; i++)
                {
                    glVertex3d(GLdouble(currPolyPoints[i].pX), GLdouble(currPolyPoints[i].pY), GLdouble(pixelZ));
                }
            glEnd();
        }
        vertexCount += 4;

        // do we need to mitre ?
        Point2D currPolyStart(stX, stY);
        if (currPolyStart.SamePoint(lastPolyEnd, 0.01))
        {
            if (lastPoly.PointInPoly(currPolyPoints[0]))
            {
                if (drawSeg)
                {
                    //reset normals?  based on xy
                    //this seems to eliminate the dark line issue
                    glNormal3d(GLdouble(0.0), GLdouble(0.0), GLdouble(1.0));

                    glBegin(GL_POLYGON);
                        glVertex3d(GLdouble(currPolyPoints[3].pX), GLdouble(currPolyPoints[3].pY), GLdouble(pixelZ));
                        glVertex3d(GLdouble(lastPolyPoints[2].pX), GLdouble(lastPolyPoints[2].pY), GLdouble(pixelZ));
                        glVertex3d(GLdouble(stX), GLdouble(stY), GLdouble(pixelZ));
                    glEnd();
                }
                vertexCount += 3;
            }
            else if (lastPoly.PointInPoly(currPolyPoints[3]))
            {
                if (drawSeg)
                {
                    //reset normals?  based on xy
                    //this seems to eliminate the dark line issue
                    glNormal3d(GLdouble(0.0), GLdouble(0.0), GLdouble(1.0));

                    glBegin(GL_POLYGON);
                        glVertex3d(GLdouble(currPolyPoints[0].pX), GLdouble(currPolyPoints[0].pY), GLdouble(pixelZ));
                        glVertex3d(GLdouble(lastPolyPoints[1].pX), GLdouble(lastPolyPoints[1].pY), GLdouble(pixelZ));
                        glVertex3d(GLdouble(stX), GLdouble(stY), GLdouble(pixelZ));
                    glEnd();
                }
                vertexCount += 3;
            }
            else
            {
                // pathological - ignore
            }

        }

        for (int i = 0; i < 4; i++)
            lastPolyPoints[i] = currPolyPoints[i];
        lastPolyEnd = Point2D(endX, endY);
    }
    else
    {
        if (drawSeg)
        {
            //reset normals?  based on xy
            //this seems to eliminate the dark line issue
            glNormal3d(GLdouble(0.0), GLdouble(0.0), GLdouble(1.0));

            glBegin(GL_LINES);
                glVertex3d(GLdouble(stX), GLdouble(stY), GLdouble(pixelZ));
                glVertex3d(GLdouble(endX), GLdouble(endY), GLdouble(pixelZ));
            glEnd();
        }
        vertexCount += 2;
    }
}



