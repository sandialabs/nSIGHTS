///////////////////////////////////////////////////////////////////////////////////
//
// PFO_CovarLimMatrixGL.cpp
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
//      Implements code to draw osPFO functional objects on a plot.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClass/U_Real.h>

#include <genPFOGL/GL_Line.h>
#include <genPFOGL/GL_Symbol.h>

#include <osPFOGL/PFO_CovarLimMatrixGL.h>


PFO_CovarLimMatrixGL :: PFO_CovarLimMatrixGL(PlotDefC& assPlt) :
                            PFO_CovarLimMatrix(assPlt),
                            OGLBaseObj(this)
{
}
PFO_CovarLimMatrixGL::PFO_CovarLimMatrixGL(const PFO_CovarLimMatrixGL& a, PlotDefC& assPlt) :
                            PFO_CovarLimMatrix(a, assPlt),
                            OGLBaseObj(this)
{
}


PFO_CovarLimMatrixGL::~PFO_CovarLimMatrixGL()
{
}

PFO_CovarLimMatrixGL& PFO_CovarLimMatrixGL::operator= (const PFO_CovarLimMatrixGL& a)
{
    if (&a != this)
        PFO_CovarLimMatrix::FullCopy(a);
    return *this;
}

/*int  PFO_CovarLimMatrixGL :: GetColorIndx(double aDiag, double bDiag)
{
    if (colorAttribute == caIndx)
    {
        nextPen += dxPen;
    }
    else
    {
        double tempDiag;
        if (colorAttribute == caMainDiag)
            tempDiag = aDiag;
        else //if (colorAttribute == caOffDiag)
            tempDiag = bDiag;

        nextPen = ((tempDiag - minDiag) / dxPen) * maxPen;
    }
    return int(nextPen);
} */

void  PFO_CovarLimMatrixGL :: DrawPoly(SC_PointArray& ellipsePts, int color)
{
    SC_PointArray polyPoints(4);

    if (plotWithColorMap)
        covarPoly.SetColor(colorMap->GetColor(color));

    int maxpt = ellipsePts.UpperBound();
    int nhalf = ellipsePts.Size() / 2;
    int npoly = nhalf - 1;

#ifdef _GLPRHARDCOPY_
    if (DoingHardCopy())
    {
        int glpNum = 4 * npoly;  //4 points in each polygon
        glpBegin(GLP_3D_COLOR, glpNum);
        objectBase->SetHardCopyParameters();
    }
#endif

    for (int i = 0; i < npoly; i++)
    {
        polyPoints.SetSize(3);
        polyPoints[0] = ellipsePts[i];
        polyPoints[1] = ellipsePts[i + 1];
        if (i == 0)
        {
            polyPoints[2] = ellipsePts[maxpt - 1];
        }
        else if (i == (npoly - 1))
        {
            polyPoints[2] = ellipsePts[maxpt - i];
        }
        else
        {
            polyPoints.SetSize(4);
            polyPoints[2] = ellipsePts[maxpt - i - 1];
            polyPoints[3] = ellipsePts[maxpt - i];
        }

        covarPoly.DrawPolygon(polyPoints);
    }

    GL_Base::HardCopyBlockEnd();
}

void  PFO_CovarLimMatrixGL :: DrawBadX(const int i, const int j)
{
    if (plotBadCov)
    {
        double badPercent = 0.8 * percent;
        GL_Line badCovLine;
        badCovLine.SetColor(badCovXPen);
        badCovLine.SetLine(badCovXThk);

        SC_PointArray stBadCovX(2);  //two start points per X
        SC_PointArray endBadCovX(2); //two end points per X

        stBadCovX[0].pX = i - badPercent * 0.5;
        stBadCovX[0].pY = j - badPercent * 0.5;

        endBadCovX[0].pX = i + badPercent * 0.5;
        endBadCovX[0].pY = j + badPercent * 0.5;

        stBadCovX[1].pX = i - badPercent * 0.5;
        stBadCovX[1].pY = j + badPercent * 0.5;

        endBadCovX[1].pX = i + badPercent * 0.5;
        endBadCovX[1].pY = j - badPercent * 0.5;

        if ((i == j) || ((i != j) && IncUpper()))
            badCovLine.DrawLines(stBadCovX, endBadCovX);

        if ((i != j)  && IncLower())
        {
            double temp;
            temp = stBadCovX[0].pX;
            stBadCovX[0].pX = stBadCovX[0].pY;
            stBadCovX[0].pY = temp;

            temp = stBadCovX[1].pX;
            stBadCovX[1].pX = stBadCovX[1].pY;
            stBadCovX[1].pY = temp;

            temp = endBadCovX[0].pX;
            endBadCovX[0].pX = endBadCovX[0].pY;
            endBadCovX[0].pY = temp;

            temp = endBadCovX[1].pX;
            endBadCovX[1].pX = endBadCovX[1].pY;
            endBadCovX[1].pY = temp;

            badCovLine.DrawLines(stBadCovX, endBadCovX);
        }
    }
}

void  PFO_CovarLimMatrixGL :: DrawEllipse(SC_PointArray& ell,
                                          SC_PointArray& oppEll,
                                          double         aDiag,
                                          double         bDiag)
{
    if (covarEllipseType  == cetFill)
    {
        int penVal = covarPen;
        if (plotWithColorMap)
            penVal = symbolColors[colorIndx];
        DrawPoly(ell, penVal);
        if (!oppEll.IsEmpty())
            DrawPoly(oppEll, penVal);
    }
    else
    {
        GL_Line covarLine;
        if (plotWithColorMap)
            covarLine.SetColor(colorMap->GetColor(symbolColors[colorIndx]));
        else
            covarLine.SetColor(covarPen);
        covarLine.SetLine(covarLineThk);

        if (!ell.IsEmpty())
            covarLine.DrawLines(ell);
        if (!oppEll.IsEmpty())
            covarLine.DrawLines(oppEll);
    }
}

void  PFO_CovarLimMatrixGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    if (covarEllipseType  == cetFill)
    {
        GL_Polygon::SetColor(covarPen);
        covarPoly.SetPolygonFilled(polyOffset);
    }

    GL_Symbol currSym;
    currSym.SetSymProperties(bestEstSymbol);
    SC_PointArray  bestPoints(2);

    if (plotWithColorMap)
        rangeLimit.MapDataToRange(colorData, symbolColors, clipToRange, colorMap->ncolorMap);

    /*
    dxPen = 0;
    if (plotWithColorMap)
    {
        maxPen = colorMap->ncolorMap - 1;
        if (colorAttribute == caIndx)
        {
            if (maxCovar > 1)
                dxPen = maxPen / (maxCovar - 1.0);  //here dx is color map increment
        }
        else
            dxPen = maxDiag - minDiag;  //here dx is value range for map
    } */

    double minSingle, maxSingle;

    SC_PointArray ell, oppEll;
    oppEll.Alloc(ndualPts);

    for (int i = 0; i < maxParam; i++)
    {
        for (int j = i; j < maxParam; j++)
        {
            //if (colorAttribute == caIndx)
            //  nextPen = -dxPen;   //reset color map to below zero since incremented before used

            for (int k = 0; k < covarArrayDC->Size(); k++)
            {
                bool plotOK = true;
                if (plotWithColorMap)
                {
                    if (ColorAttributeIsDiag())
                    {
                        colorIndx = colorIndxMap[i][j][k];
                    }
                    else
                        colorIndx = k;
                    plotOK = symbolColors[colorIndx] >= 0;
                }

                double aDiag = 0.0;
                double bDiag = 0.0;

                if ((i == j) && IncDiag())
                {
                    if (Get2DSingle(i, k, minSingle, maxSingle))
                    {
                        double radius = (Normalize(maxSingle) - Normalize(minSingle)) / 2.0;
                        double center = Normalize(minSingle) + radius;
                        aDiag = (maxSingle - minSingle) / 2.0;
                        bDiag = aDiag;

                        if (plotEllipse)
                        {
                            int nhalf = ndualPts / 2;
                            int npts = nhalf + nhalf;
                            ell.AllocAndSetSize(npts);

                            double shift = double(i);
                            if (!centerEllipse)
                                shift += center - 0.5;
                            double inc = 2.0 * radius / double(nhalf - 1);

                            for (int ii = 0; ii < nhalf; ii++)
                            {
                                double x = -radius + double(ii) * inc;
                                double y;
                                if ((ii == 0) || (ii == (nhalf - 1)))
                                    y = 0.0;
                                else
                                    y = sqrt(-Sqr(x) + Sqr(radius));

                                ell[ii].pX = shift + x;
                                ell[ii].pY = shift + y;

                                ell[npts - ii - 1].pX = shift + x;
                                ell[npts - ii - 1].pY = shift - y;
                            }

                            oppEll.SetEmpty();
                            if (plotOK)
                                DrawEllipse(ell, oppEll, aDiag, bDiag);
                        }
                    }
                    else
                        DrawBadX(i, j);
                }
                else if ((i != j) && (matrixPlotOption !=mpoDiag))
                {
                    if (Get2DDual(i, j, k, ell, aDiag, bDiag))
                    {
                        if (plotEllipse)
                        {
                            for (int ii = 0; ii < ell.Size(); ii++)
                            {
                                ell[ii].pX = Normalize(ell[ii].pX) + i - 0.5 + Shift(i, j, k, true);
                                ell[ii].pY = Normalize(ell[ii].pY) + j - 0.5 + Shift(i, j, k, false);
                                oppEll[ii].pX = ell[ii].pY;
                                oppEll[ii].pY = ell[ii].pX;
                            }

                            if (IncLower())
                                oppEll.SetSize(ndualPts);
                            else
                                oppEll.SetEmpty();

                            if (IncUpper())
                                ell.SetSize(ndualPts);
                            else
                                ell.SetEmpty();

                            if (plotOK)
                                DrawEllipse(ell, oppEll, aDiag, bDiag);
                        }//if plotEllipse
                    }//if get dual ellipse pts
                    else //bad covariance
                        DrawBadX(i, j);
                }

                /*outside plotBestEst to ensure increment of color
                //even if bad cov
                int penVal;
                if (drawEll)
                    penVal = int(nextPen);
                else
                    penVal = GetColorIndx(aDiag, bDiag); */

                if (plotBestEst)
                {
                    if (plotWithColorMap)
                        currSym.SetColor(colorMap->GetColor(symbolColors[colorIndx]));
                    else
                        currSym.SetColor(covarPen);

                    DC_Covar& curr = (*covarArrayDC)[k];

                    double xVal, yVal;
                    if (plotEllipse && (!centerEllipse))
                    {
                        xVal = Normalize(curr.covarFitVals[i]) + i - 0.5;
                        yVal = Normalize(curr.covarFitVals[j]) + j - 0.5;
                    }
                    else
                    {
                        xVal = Reduce(curr.covarFitVals[i]) + i - 0.5;
                        yVal = Reduce(curr.covarFitVals[j]) + j - 0.5;
                    }

                    bestPoints.SetEmpty();
                    if (((i == j) && IncDiag()) || ((i != j) && IncUpper()))
                        bestPoints[0] = Point2D(xVal, yVal);

                    if ((i != j) && IncLower())
                    {
                        bestPoints[1] = Point2D(yVal, xVal);
                        bestPoints.SetSize(2);
                    }
                    else
                        bestPoints.SetSize(1);

                    if (plotOK)
                        currSym.DrawSymbols(bestPoints, bestEstSymbol.symbolType);

                }//if plotBestEst
            }//covar loop

            if (plotAreaLim)
            {
                GL_Line plotAreaLine;
                plotAreaLine.SetColor(plotAreaLimPen);
                plotAreaLine.SetLine(plotAreaLimThk);

                SC_PointArray squareSt(4);
                SC_PointArray squareEnd(4);

                double zeroVal, oneVal;
                if (plotEllipse && (!centerEllipse))
                {
                    zeroVal = Normalize(0.0);
                    oneVal = Normalize(1.0);
                }
                else
                {
                    zeroVal = Reduce(0.0);
                    oneVal = Reduce(1.0);
                }

                double shiftX = i - 0.5;
                double shiftY = j - 0.5;

                squareSt[0].pX = zeroVal + shiftX;
                squareSt[0].pY = zeroVal + shiftY;

                squareEnd[0].pX = zeroVal + shiftX;
                squareEnd[0].pY = oneVal + shiftY;

                squareEnd[1].pX = oneVal + shiftX;
                squareEnd[1].pY = oneVal + shiftY;

                squareEnd[2].pX = oneVal + shiftX;
                squareEnd[2].pY = zeroVal + shiftY;

                squareEnd[3].pX = zeroVal + shiftX;
                squareEnd[3].pY = zeroVal + shiftY;

                for (int jj = 1; jj <= 3; jj++)
                {
                    squareSt[jj].pX = squareEnd[jj - 1].pX;
                    squareSt[jj].pY = squareEnd[jj - 1].pY;
                }

                if (((i == j) && IncDiag()) || ((i != j) && IncUpper()))
                    plotAreaLine.DrawLines(squareSt, squareEnd);

                //opposite squares
                if ((i != j) && IncLower())
                {
                    double temp;
                    for (int jj = 0; jj <= 3; jj++)
                    {
                        temp = squareSt[jj].pX;
                        squareSt[jj].pX = squareSt[jj].pY;
                        squareSt[jj].pY = temp;

                        temp = squareEnd[jj].pX;
                        squareEnd[jj].pX = squareEnd[jj].pY;
                        squareEnd[jj].pY = temp;

                    }

                    plotAreaLine.DrawLines(squareSt, squareEnd);
                }
            }
        }//y parameter loop
    }//x parameter loop

    CloseDrawObject();
}

