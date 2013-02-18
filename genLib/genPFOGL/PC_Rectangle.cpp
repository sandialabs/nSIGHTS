///////////////////////////////////////////////////////////////////////////////////
//
// PC_Rectangle.cpp
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

#include <genPFOGL/PC_Rectangle.h>


double PC_Rectangle::dX() {return fabs(lr.pX - ul.pX);}
double PC_Rectangle::dY() {return fabs(lr.pY - ul.pY);}

void  PC_Rectangle::AddToBoundingBox(const Point2D& inPt)
{
    if ( inPt.PointIsNull())
        return;

    if (ul.PointIsNull())
    {
        ul = inPt;
        lr = inPt;
        return;
    }

    if (inPt.pX < ul.pX)
        ul.pX = inPt.pX;
    else
        if (inPt.pX > lr.pX)
            lr.pX = inPt.pX;

    if (inPt.pY > ul.pY)
        ul.pY = inPt.pY;
    else
        if (inPt.pY < lr.pY)
            lr.pY = inPt.pY;
}

void  PC_Rectangle::AddToBoundingBox(const PC_Rectangle& inRect)
{
    AddToBoundingBox(inRect.ul);
    AddToBoundingBox(inRect.lr);
}

void  PC_Rectangle::IncreaseBoundingBox(const double& pctIncrease)
{
    double dx = (lr.pX - ul.pX) * pctIncrease / 100.0;
    double dd = (ul.pY - lr.pY) * pctIncrease / 100.0;
    if (dx < dd)
        dd = dx;
    ul.pX -= dd;
    lr.pX += dd;
    ul.pY += dd;
    lr.pY -= dd;
}


void PC_Rectangle::IncreaseBoundingBox(const double& xIncrease,
                                        const double& yIncrease)
{
    ul.pX -= xIncrease;
    lr.pX += xIncrease;
    ul.pY += yIncrease;
    lr.pY -= yIncrease;
}

