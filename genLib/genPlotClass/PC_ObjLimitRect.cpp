///////////////////////////////////////////////////////////////////////////////////
//
// PC_ObjLimitRect.cpp
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
//      Platform independent base classes for plot definitions and plot functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genPlotClass/PC_ObjLimitRect.h>


PC_ObjLimitRect::PC_ObjLimitRect(const PC_ObjLimitRect& a)
{
    LocalCopy(a);
}

PC_ObjLimitRect::PC_ObjLimitRect()
{
    ulXpos  = -1;
    ulYpos  = -1;
    lrXpos  = -1;
    lrYpos  = -1;
}

bool PC_ObjLimitRect::InRect(int xpos, int ypos) const
{
    return (xpos <= lrXpos) && (xpos >= ulXpos) &&
           (ypos <= lrYpos) && (ypos >= ulYpos);
}

bool PC_ObjLimitRect::NotNull() const
{
    return (ulXpos >= 0) && (ulYpos >= 0) && ((lrXpos - ulXpos) > 0) && ((lrYpos - ulYpos) > 0) ;
}

void  PC_ObjLimitRect::GetWidthHeight(int& width, int& height) const
{
    width  = lrXpos - ulXpos;
    height = lrYpos - ulYpos;
}

int PC_ObjLimitRect::DistFromCenter(int xpos, int ypos) const
{
    int midX = (ulXpos + lrXpos) / 2;
    int midY = (ulYpos + lrYpos) / 2;
    return (xpos - midX) * (xpos - midX)  + (ypos - midY) * (ypos - midY);
}

PC_ObjLimitRect&  PC_ObjLimitRect::operator= (const PC_ObjLimitRect& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void  PC_ObjLimitRect::LocalCopy(const PC_ObjLimitRect& a)
{
    ulXpos  = a.ulXpos;
    ulYpos  = a.ulYpos;
    lrXpos  = a.lrXpos;
    lrYpos  = a.lrYpos;
}

void PC_ObjLimitRect::Adjust(int uOffset, int vOffset)
{
    ulXpos  += uOffset;
    ulYpos  += vOffset;
    lrXpos  += uOffset;;
    lrYpos  += vOffset;
}

