///////////////////////////////////////////////////////////////////////////////////
//
// PFO_CovarLimitsGL.cpp
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
//      Plots confidence limits of 2D or 3D plots.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genPFOGL/GL_Line.h>
#include <genPFOGL/GL_Extrusion.h>

#include <osPFOGL/PFO_CovarLimitsGL.h>


PFO_CovarLimitsGL :: PFO_CovarLimitsGL(PlotDefC& assPlt) :
                            PFO_CovarLimits(assPlt),
                            OGLBaseObj(this)
{
}
PFO_CovarLimitsGL::PFO_CovarLimitsGL(const PFO_CovarLimitsGL& a, PlotDefC& assPlt) :
                            PFO_CovarLimits(a, assPlt),
                            OGLBaseObj(this)
{
}


PFO_CovarLimitsGL::~PFO_CovarLimitsGL()
{
}

PFO_CovarLimitsGL& PFO_CovarLimitsGL::operator= (const PFO_CovarLimitsGL& a)
{
    if (&a != this)
        PFO_CovarLimits::FullCopy(a);
    return *this;
}

void PFO_CovarLimitsGL::Draw2D()
{
    GL_Line::SetColor(covarPen);
    GL_Line::SetLine(covarLineThk);
    if (covarPlotType == cptSingle)
    {
        SC_PointArray st, end;
        GL_Line currLine;
        for (int i = 0; i < covarArrayDC->Size(); i++)
            if (Get2DSingle(i, st, end))
            {
                currLine.DrawLines(st, end);
            }
    }
    else
    {
        SC_PointArray ell, polyPoints;
        if (covarPlotFormat  == cpfFill)
        {
            GL_Polygon::SetColor(covarPen);
            covarPoly.SetPolygonFilled(polyOffset);
            polyPoints.Alloc(4);
        }

        for (int i = 0; i < covarArrayDC->Size(); i++)
            if (Get2DDual(i, ell))
            {
                if (covarPlotFormat  == cpfFill)
                {
                    GL_Polygon covarPoly;
                    int maxpt = ell.UpperBound();
                    int nhalf = ell.Size() / 2;
                    int npoly = nhalf - 1;

#ifdef _GLPRHARDCOPY_
                    if (DoingHardCopy())
                    {
                        int glpNum = 4 * npoly;  //4 points in each polygon
                        glpBegin(GLP_3D_COLOR, glpNum);
                        objectBase->SetHardCopyParameters();
                    }
#endif

                    for (int j = 0; j < npoly; j++)
                    {
                        polyPoints.SetSize(3);
                        polyPoints[0] = ell[j];
                        polyPoints[1] = ell[j + 1];
                        if (j == 0)
                        {
                            polyPoints[2] = ell[maxpt - 1];
                        }
                        else if (j == (npoly - 1))
                        {
                            polyPoints[2] = ell[maxpt - j];
                        }
                        else
                        {
                            polyPoints.SetSize(4);
                            polyPoints[2] = ell[maxpt - j - 1];
                            polyPoints[3] = ell[maxpt - j];
                        }
                        covarPoly.DrawPolygon(polyPoints);
                    }
                    GL_Base::HardCopyBlockEnd();
                }
                else
                {
                    GL_Line covarLine;
                    covarLine.DrawLines(ell);
                }
            }
    }
}

void PFO_CovarLimitsGL::Draw3DEllipse(const SC_CoordArray& ell)
{
    switch (covarPlotFormat)
    {
        case cpfLine : {
            GL_Line covarLine;
            covarLine.DrawLines(ell);
            break;
        }
        case cpfFill : {
            GL_Polygon::SetColor(covarPen);
            covarPoly.SetPolygonFilled(polyOffset);

            SC_CoordArray polyCoords(4);

            int maxpt = ell.UpperBound();
            int nhalf = ell.Size() / 2;
            int npoly = nhalf - 1;

#ifdef _GLPRHARDCOPY_
            if (DoingHardCopy())
            {
                int glpNum = 4 * npoly;  //4 points in each polygon
                glpBegin(GLP_3D_COLOR, glpNum);
                objectBase->SetHardCopyParameters();
            }
#endif
            for (int j = 0; j < npoly; j++)
            {
                polyCoords.SetSize(3);
                polyCoords[0] = ell[j];
                polyCoords[1] = ell[j + 1];
                if (j == 0)
                {
                    polyCoords[2] = ell[maxpt - 1];
                }
                else if (j == (npoly - 1))
                {
                    polyCoords[2] = ell[maxpt - j];
                }
                else
                {
                    polyCoords.SetSize(4);
                    polyCoords[2] = ell[maxpt - j - 1];
                    polyCoords[3] = ell[maxpt - j];
                }
                covarPoly.DrawPolygon(polyCoords);
            }
            GL_Base::HardCopyBlockEnd();

            break;
        }
        case cpfTube : {
            PSC_EdgeSpec noEdges;
            noEdges.plotEdges = false;
            GL_Extrusion tubeExt(polyOffset, assocPlot->defaultPenSet->GetColor(covarPen), noEdges);
            tubeExt.DrawExtrusion(ell, tubeData);
            break;
        }

    }
}


void PFO_CovarLimitsGL::Draw3DEllipsoid(const SC_CoordMatrix& ell)
{
    if (covarPlotFormat != cpfFill)
    {
        int i;

        // meridians
        for (i = 0; i < ell.Size(); i++)
            Draw3DEllipse(ell[i]);

        // slices
        int nslicePts = ell.Size() + 1;
        SC_CoordArray tmp(nslicePts);
        for (i = 0; i < ell[0].Size(); i++)
        {
            for (int j = 0; j < (nslicePts - 1); j++)
                tmp[j] = ell[j][i];

            tmp[nslicePts - 1] = ell[0][i];

            Draw3DEllipse(tmp);
        }
    }
    else
    {
        GL_Polygon::SetColor(covarPen);
        covarPoly.SetPolygonFilled(polyOffset);
        SC_CoordArray polyCoords(4);

        int npoly = ell[0].UpperBound();

        for (int i = 0; i < ell.Size(); i++)
        {
            int i1 = i + 1;
            if (i == ell.UpperBound())
                i1 = 0;

            for (int j = 0; j < npoly; j++)
            {
                polyCoords.SetSize(3);
                polyCoords[0] = ell[i1][j];
                polyCoords[1] = ell[i1][j + 1];
                if (j == 0)
                {
                    polyCoords[2] = ell[i][j + 1];
                }
                else if (j == (npoly - 1))
                {
                    polyCoords[2] = ell[i][j];
                }
                else
                {
                    polyCoords.SetSize(4);
                    polyCoords[2] = ell[i][j + 1];
                    polyCoords[3] = ell[i][j];
                }
                covarPoly.DrawPolygon(polyCoords);
            }
        }
    }
}


void PFO_CovarLimitsGL::Draw3D()
{
    GL_Line::SetColor(covarPen);
    GL_Line::SetLine(covarLineThk);
    switch (covarPlotType)
    {
        case cptSingle : {
            SC_CoordArray st, end;
            GL_Line currLine;
            for (int i = 0; i < covarArrayDC->Size(); i++)
            {
                if (Get3DSingle(i, st, end))
                    currLine.DrawLines(st, end);
            }
            break;
        }
        case cptDual : {
            SC_CoordArray ell;
            for (int i = 0; i < covarArrayDC->Size(); i++)
            {
                if (Get3DDual(i, p3D_XY, ell))
                    Draw3DEllipse(ell);
                if (Get3DDual(i, p3D_XZ, ell))
                    Draw3DEllipse(ell);
                if (Get3DDual(i, p3D_YZ, ell))
                    Draw3DEllipse(ell);
            }
            break;
        }
        case cptTriple : {
            SC_CoordMatrix ell;
            for (int i = 0; i < covarArrayDC->Size(); i++)
            {
                if (Get3DTriple(i, ell))
                    Draw3DEllipsoid(ell);
            }
            break;
        }

    }
}

void  PFO_CovarLimitsGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    if (is3DObject)
    {
        Draw3D();
    }
    else
    {
        Draw2D();
    }

    CloseDrawObject();
}

