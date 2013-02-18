///////////////////////////////////////////////////////////////////////////////////
//
// GL_Polygon.cpp
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

#include <iostream>

#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genPFOGL/GL_Line.h>

#include <genPFOGL/GL_Polygon.h>



SC_DoubleArray  GL_Polygon::tessCombine;
// kluge static for tesselator
// might be an MT problem

GL_Polygon::GL_Polygon()
{
    polygonIsFilled = true;
    polyTesselator = 0;
    useTesselator = false;
}

GL_Polygon::~GL_Polygon()
{

}



void  GL_Polygon::SetPolygonFilled(const double& polyOffset)
{
    polygonIsFilled = true;
    SetPolygonAsFilled(polyOffset);
}

void  GL_Polygon::SetPolygonLine(int lineWidth,
                                 const double& polyOffset)
{
    polygonIsFilled = false;
    SetPolygonAsLine(lineWidth, polyOffset);

}

// GLU tesselator stuff
// glu tesselator callbacks
void TESSCALL GL_Polygon::BeginPolyCB(GLenum which)
{
    glBegin(which);
}

void TESSCALL GL_Polygon::EndPolyCB()
{
    glEnd();
}

void TESSCALL GL_Polygon::PolyVertexCB(void* vertexData)
{
    Coord3D& tmpCoord = *static_cast<Coord3D*>(vertexData);
    glVertex3d(GLdouble(tmpCoord.cX), GLdouble(tmpCoord.cY), GLdouble(tmpCoord.cZ));
}

void TESSCALL GL_Polygon::PolyCombineCB(GLdouble        coords[3],
                                       GLdouble*        vertex_data[4],
                                       GLfloat          weight[4],
                                       GLdouble**       outData)
{
    tessCombine[0] = coords[0];
    tessCombine[1] = coords[1];
    tessCombine[2] = coords[2];

/*
    we just passed in garbage -- no need to do anything with it
    for (int i = 3; i < 7; i++)
    {
        tessCombine[i] = weight[0] * vertex_data[0][i] +
                        weight[1] * vertex_data[1][i] +
                        weight[2] * vertex_data[2][i] +
                        weight[3] * vertex_data[3][i];
    }
*/

    *outData = tessCombine.tListData;
}

void TESSCALL GL_Polygon::PolyErrorCB(GLenum errorCode)
{
    const GLubyte *estring;
    estring = gluErrorString(errorCode);
    using namespace std;
    cout << "Tesselation error: " << (char*) estring << endl;
    GenAppInternalError("GL_Polygon::PolyErrorCB");
}


void GL_Polygon::InitTesselator()
{
    useTesselator = true;
    if (polyTesselator != 0)
        return;

    polyTesselator = gluNewTess();
    if (polyTesselator == 0)
        GenAppInternalError("GL_Polygon::InitTesselator");

    // basic callbacks
#ifdef MESA
    gluTessCallback(polyTesselator, (GLenum) GLU_VERTEX, (GLvoid(*) ()) &glVertex3dv);
    gluTessCallback(polyTesselator, (GLenum) GLU_BEGIN, (GLvoid(*) ()) &BeginPolyCB);
    gluTessCallback(polyTesselator, (GLenum) GLU_END, (GLvoid(*) ()) &EndPolyCB);
    gluTessCallback(polyTesselator, (GLenum) GLU_ERROR, (GLvoid(*) ()) &PolyErrorCB);
#else
#ifndef _WIN32
    gluTessCallback(polyTesselator, GLU_TESS_VERTEX, (GLvoid(*) ()) &glVertex3dv);
    gluTessCallback(polyTesselator, GLU_TESS_BEGIN, (GLvoid(*) ()) &BeginPolyCB);
    gluTessCallback(polyTesselator, GLU_TESS_END, (GLvoid(*) ()) &EndPolyCB);
    gluTessCallback(polyTesselator, GLU_TESS_ERROR, (GLvoid(*) ()) &PolyErrorCB);
#else
    gluTessCallback(polyTesselator, GLU_TESS_VERTEX, (void (__stdcall *)(void)) &PolyVertexCB);
    gluTessCallback(polyTesselator, GLU_TESS_BEGIN, (void (__stdcall *)(void)) &BeginPolyCB);
    gluTessCallback(polyTesselator, GLU_TESS_END, (void (__stdcall *)(void)) &EndPolyCB);
    gluTessCallback(polyTesselator, GLU_TESS_ERROR, (void (__stdcall *)(void)) &PolyErrorCB);
    gluTessCallback(polyTesselator, GLU_TESS_COMBINE, (void (__stdcall *)(void)) &PolyCombineCB);
#endif
    // our polys are defined clockwise
    gluTessProperty(polyTesselator, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
#endif

    // assume max poly never exeeds 500
    tessVertexes.Alloc(500);
    // for combine
    tessCombine.Alloc(8);
}

void GL_Polygon::InitTesselator(bool useTess)
{
    if (useTess)
        InitTesselator();
}

void GL_Polygon::ClearTesselator()
{
    useTesselator = false;
}

void GL_Polygon::DrawPolygon(const SC_CoordArray& polyPoints,
                                    const Coord3D&       polyNormal)
{
    if (defAssocGL == 0)
        GenAppInternalError("GL_Polygon::DrawPolygon3D");

    Coord3D tmpCoord;
    if (useTesselator && (polyPoints.Size() > 3) && polygonIsFilled)
    {
#ifdef MESA
        gluBeginPolygon(polyTesselator);
#else
        gluTessBeginPolygon(polyTesselator, NULL);
        gluTessBeginContour(polyTesselator);
#endif
        // make sure tess vertexes is big enough
        if (polyPoints.Size() > tessVertexes.AllocSize())
            tessVertexes.Alloc(polyPoints.Size());

        gluTessNormal(polyTesselator, GLdouble(polyNormal.cX), GLdouble(polyNormal.cY), GLdouble(polyNormal.cZ));
        glNormal3d(GLdouble(polyNormal.cX), GLdouble(polyNormal.cY), GLdouble(polyNormal.cZ));

        // backward loop is kluge - our polys are defined clockwise
        for(int i = polyPoints.UpperBound(); i >= 0; i--)
        {
            tmpCoord = polyPoints[i];
            if (TransformAndOffset(tmpCoord))
            {
                tessVertexes[i] = tmpCoord;
                gluTessVertex(polyTesselator, (GLdouble*) &tessVertexes.tListData[i], (GLdouble*) &tessVertexes.tListData[i]);
            }
        }
#ifdef MESA
        gluEndPolygon(polyTesselator);
#else
        gluTessEndContour(polyTesselator);
        gluTessEndPolygon(polyTesselator);
#endif
        return;
    }


    glNormal3d(GLdouble(polyNormal.cX), GLdouble(polyNormal.cY), GLdouble(polyNormal.cZ));
    glBegin(GL_POLYGON);
        for(int i = 0; i <  polyPoints.Size(); i++)
        {
            tmpCoord = polyPoints[i];
            TransformAndOffset(tmpCoord);
            glVertex3d(GLdouble(tmpCoord.cX), GLdouble(tmpCoord.cY), GLdouble(tmpCoord.cZ));
        }
    glEnd();
}



void GL_Polygon::DrawPolygon(const SC_CoordArray& polyPoints)
{
    if (defAssocGL == 0)
        GenAppInternalError("GL_Polygon::DrawPolygon3D");

    Coord3D tmpCoord;
    if (useTesselator && (polyPoints.Size() > 3) && polygonIsFilled)
    {
#ifdef MESA
        gluBeginPolygon(polyTesselator);
#else
        gluTessBeginPolygon(polyTesselator, NULL);
        gluTessBeginContour(polyTesselator);
#endif
        // make sure tess vertexes is big enough
        if (polyPoints.Size() > tessVertexes.AllocSize())
            tessVertexes.Alloc(polyPoints.Size());

        if (SetPolyNormals(polyPoints))
        {
            // again reversed -- so swap normal
            Coord3D temp = normalCoords[0];
            normalCoords[0] = normalCoords[2];
            normalCoords[2] = temp;

            Coord3D normVector;
            CalcNormals(normVector);
            gluTessNormal(polyTesselator, GLdouble(normVector.cX), GLdouble(normVector.cY), GLdouble(normVector.cZ));
            glNormal3d(GLdouble(normVector.cX), GLdouble(normVector.cY), GLdouble(normVector.cZ));
        }



        // backward loop is kluge - our polys are defined clockwise
        for(int i = polyPoints.Size() - 1; i >= 0; i--)
        {
            tmpCoord = polyPoints[i];
            if (TransformAndOffset(tmpCoord))
            {
                tessVertexes[i] = tmpCoord;
                gluTessVertex(polyTesselator, (GLdouble*) &tessVertexes.tListData[i], (GLdouble*) &tessVertexes.tListData[i]);
            }
        }
#ifdef MESA
        gluEndPolygon(polyTesselator);
#else
        gluTessEndContour(polyTesselator);
        gluTessEndPolygon(polyTesselator);
#endif
        return;
    }

    if ((polyPoints.Size() > 2) && SetPolyNormals(polyPoints))
        CalcNormals();

    glBegin(GL_POLYGON);
        int i;
        for(i = 0; i <  polyPoints.Size(); i++)
        {
            tmpCoord = polyPoints[i];
            TransformAndOffset(tmpCoord);
            glVertex3d(GLdouble(tmpCoord.cX), GLdouble(tmpCoord.cY), GLdouble(tmpCoord.cZ));
        }
    glEnd();

    if (!defAssocGL->DoingVRMLOutput())
        return;

    const Limit3D& currLim  = defAssocGL->GetCurrentView().viewLimits;

    bool okFound = false;
    for(i = 0; i <  polyPoints.Size(); i++)
        if (currLim.WithinLimits(polyPoints[i]))
        {
            okFound = true;
            break;
        }

    if (!okFound)
        return;

    VMRLwriteShapePolyHeader(polygonIsFilled);

    VMRLwritePolyCoords(polyPoints);

    TxtFileC& currOut = defAssocGL->GetVRMLOutputFile();

    currOut.WriteText("]");
    currOut.WriteText("        }");
    currOut.WriteSolidText("        coordIndex [");

    for(i = 0; i <  polyPoints.Size(); i++)
    {
        currOut.WriteInt(i);
        currOut.WriteSolidText(",");
    }

    if (!polygonIsFilled)
        currOut.WriteText("0, ");

    currOut.WriteText("-1 ]");

    VMRLwriteColor();

    currOut.WriteText("        colorIndex [ 0 ]");
    currOut.WriteText("        colorPerVertex FALSE");
    if (polygonIsFilled)
        currOut.WriteText("        solid FALSE");

    currOut.WriteText("   }");
    currOut.WriteText("}");

}


bool GL_Polygon::SetPolyNormals(const SC_CoordArray& polyPoints)
{
    if ((assoc3DGL != 0) && assoc3DGL->DoLightingCalcs())
    {
        normalCoords[0] = polyPoints[0];
        if (polyPoints.Size() < 5)
        {
            normalCoords[1] = polyPoints[1];
            normalCoords[2] = polyPoints[2];
        }
        else
        {
            int index = polyPoints.Size()/4;
            normalCoords[1] = polyPoints[index];
            normalCoords[2] = polyPoints[index + index];
        }
        return (TransformAndOffset(normalCoords[0])) &&
            (TransformAndOffset(normalCoords[1])) &&
            (TransformAndOffset(normalCoords[2]));
    }
    return false;
}


void GL_Polygon::DrawPolygon(const SC_PointArray& polyPoints)
{
    if (assoc2DGL == 0)
        GenAppInternalError("GL_Polygon::DrawPolygon2D");

    double zVal = assoc2DGL->GetZVal();
    Point2D tmpPoint;

    if (useTesselator && (polyPoints.Size() > 3) && polygonIsFilled)
    {
#ifdef MESA
        gluBeginPolygon(polyTesselator);
#else
        gluTessBeginPolygon(polyTesselator, NULL);
            gluTessBeginContour(polyTesselator);
#endif
                // backward loop is kluge - our polys are defined clockwise
                for(int i = polyPoints.Size() - 1; i >= 0; i--)
                {
                    tmpPoint = polyPoints[i];
                    if (assoc2DGL->TransformForXY(tmpPoint))
                    {
                        tessVertexes[i].cX = tmpPoint.pX;
                        tessVertexes[i].cY = tmpPoint.pY;
                        tessVertexes[i].cZ = zVal;
                        gluTessVertex(polyTesselator, (GLdouble*) &tessVertexes.tListData[i], (GLdouble*) &tessVertexes.tListData[i]);
                    }
                }
#ifdef MESA
        gluEndPolygon(polyTesselator);
#else
            gluTessEndContour(polyTesselator);
        gluTessEndPolygon(polyTesselator);
#endif
        return;
    }

    glBegin(GL_POLYGON);
        for(int i = 0; i <  polyPoints.Size(); i++)
        {
            tmpPoint = polyPoints[i];
            if (assoc2DGL->TransformForXY(tmpPoint))
                glVertex3d(GLdouble(tmpPoint.pX), GLdouble(tmpPoint.pY), GLdouble(zVal));
        }
    glEnd();

}


void GL_Polygon::DrawPolygon(const SC_CoordArray& coords,
                             const SC_IntArray& indexes)
{
    if (indexes.IsEmpty())
    {
        DrawPolygon(coords);
        return;
    }

    polyVertexes.Alloc(coords.Size());
    int nextVertex = 0;
    for (int i = 0; i < indexes.Size(); i++)
    {
        if (indexes[i] < 0)
        {
            if (nextVertex == 0)
                continue;
            polyVertexes.SetSize(nextVertex);
            DrawPolygon(polyVertexes);
            nextVertex = 0;
        }
        else
            polyVertexes[nextVertex++] = coords[indexes[i]];
    }

    if (nextVertex > 0)
    {
        polyVertexes.SetSize(nextVertex);
        DrawPolygon(polyVertexes);
    }
}


void GL_Polygon::DrawPolygon(const SC_CoordArray& topSide,
                             const SC_CoordArray& bottomSide)
{
    DrawPolygon(topSide);
    DrawPolygon(bottomSide);

    Coord3D sideBuffer[4];
    SC_CoordArray sides(sideBuffer, 4);
    for (int i = 0; i <  topSide.Size(); i++)
    {
        sides[0] = topSide[i];
        if (i == 0)
        {
            sides[1] = topSide.LastIndex();
            sides[2] = bottomSide.LastIndex();
        }
        else
        {
            sides[1] = topSide[i - 1];
            sides[2] = bottomSide[i - 1];
        }
        sides[3] = bottomSide[i];
        DrawPolygon(sides);
    }
}


void  GL_Polygon::DrawRectangle(const PC_Rectangle& rectData,
                                      bool           opaque,
                                      bool           frame,
                                      int            frameThk)
{
    if (opaque)
    {
        SC_ColorSpec prevColor;
        GL_Base::GetColor(prevColor);

        SC_ColorSpec clrColor;
        GL_Base::GetClearColor(clrColor);
        GL_Base::SetColor(clrColor);

        SetPolygonFilled(0.0);
        DoDrawRectangle(rectData, GL_POLYGON);

        GL_Base::SetColor(prevColor);
    }

    if (frame)
    {
        GL_Line::SetLineType(plt_Solid);
        SetPolygonLine(frameThk, 0.0);
        DoDrawRectangle(rectData, GL_LINE_LOOP);
    }
}

void GL_Polygon::DrawRectangle(const PC_Rectangle& rectData)
{
    DoDrawRectangle(rectData, GL_LINE_LOOP);
}

void  GL_Polygon::DrawFilledRectangle(const PC_Rectangle& rectData)
{
    DoDrawRectangle(rectData, GL_POLYGON);
}

void GL_Polygon::DoDrawRectangle(const PC_Rectangle& rectData,
                                       GLenum        drawMode)

{
    if (defAssocGL == 0)
        GenAppInternalError("GL_Polygon::DoDrawRectangle");

    Std2DHardCopyBlockStart(4);

    glBegin(drawMode);
        glVertex2d(GLdouble(rectData.ul.pX), GLdouble(rectData.ul.pY));
        glVertex2d(GLdouble(rectData.lr.pX), GLdouble(rectData.ul.pY));
        glVertex2d(GLdouble(rectData.lr.pX), GLdouble(rectData.lr.pY));
        glVertex2d(GLdouble(rectData.ul.pX), GLdouble(rectData.lr.pY));
    glEnd();

    HardCopyBlockEnd();
}


