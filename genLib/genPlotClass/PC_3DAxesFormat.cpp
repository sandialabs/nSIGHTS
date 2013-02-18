///////////////////////////////////////////////////////////////////////////////////
//
// PC_3DAxesFormat.cpp
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
//      defines axes/increment layout for 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genPlotClass/PC_3DAxesFormat.h>

PC_AxesFormat :: PC_AxesFormat()
{
    axesMajorInc = PC_Axes::aitNone;
    axesMinorInc = PC_Axes::aitNone;
    axesLinePos  = alpNone;
    axesTicPos   = atpNeg;
}

PC_AxesFormat :: PC_AxesFormat(const PC_AxesFormat& a)
{
    LocalCopy(a);
}

PC_AxesFormat& PC_AxesFormat::operator= (const PC_AxesFormat& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void PC_AxesFormat::SetWorkingCopy(PC_AxesFormat& wCopy,
                                   bool           doNeg) const
{
    wCopy = (*this);
    if (doNeg)
    {
        if (axesLinePos == alpNeg)
            wCopy.axesLinePos = alpBoth;
        if (axesTicPos == atpNeg)
            wCopy.axesTicPos = atpBoth;
    }
    else
    {
        if (axesMajorInc == PC_Axes::aitGrid)
            wCopy.axesMajorInc = PC_Axes::aitNone;
        if (axesMinorInc == PC_Axes::aitGrid)
            wCopy.axesMinorInc = PC_Axes::aitNone;
        if (axesLinePos == alpPos)
            wCopy.axesLinePos = alpBoth;
        if (axesTicPos == atpPos)
            wCopy.axesTicPos = atpBoth;
    }
}

void PC_AxesFormat::LocalCopy(const PC_AxesFormat& a)
{
    axesMajorInc =  a.axesMajorInc;
    axesMinorInc =  a.axesMinorInc;
    axesLinePos  =  a.axesLinePos;
    axesTicPos   =  a.axesTicPos;
}


PC_3DAxesFormat :: PC_3DAxesFormat()
{
    SetDefault();
}

PC_3DAxesFormat :: PC_3DAxesFormat(const PC_3DAxesFormat& a)
{
    LocalCopy(a);
}

PC_3DAxesFormat& PC_3DAxesFormat::operator= (const PC_3DAxesFormat& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void PC_3DAxesFormat::LocalCopy(const PC_3DAxesFormat& a)
{
    for (int i = 0; i < naxes3DPlanes; i++)
    {
        axesUFormat[i] = a.axesUFormat[i];
        axesVFormat[i] = a.axesVFormat[i];
    }

    autoFormat      = a.autoFormat;
    axesLinePen     = a.axesLinePen    ;
    majorPen        = a.majorPen       ;
    minorPen        = a.minorPen       ;

    axesLineWidth   = a.axesLineWidth  ;
    majorTicWidth   = a.majorTicWidth  ;
    minorTicWidth   = a.minorTicWidth  ;
    majorGridWidth  = a.majorGridWidth ;
    minorGridWidth  = a.minorGridWidth ;

    axesOffset      = a.axesOffset     ;
    majorTicLength  = a.majorTicLength ;
    minorTicLength  = a.minorTicLength ;

}
void PC_3DAxesFormat::Clear()
{
    PC_AxesFormat nullFormat;
    for ( int i = 0; i < naxes3DPlanes; i++)
    {
        axesUFormat[i] = nullFormat;
        axesVFormat[i] = nullFormat;
    }
}

void PC_3DAxesFormat::SetDefault()
{
    PC_AxesFormat nullFormat;
    nullFormat.axesLinePos = PC_AxesFormat::alpBoth;
    for ( int i = 0; i < naxes3DPlanes; i++)
    {
        axesUFormat[i] = nullFormat;
        axesVFormat[i] = nullFormat;
    }
    axesUFormat[a3pXYn].axesMajorInc = PC_Axes::aitGrid;
    axesUFormat[a3pXYn].axesMinorInc = PC_Axes::aitTic;
    axesVFormat[a3pXYn] = axesUFormat[a3pXYn];

    autoFormat      = true;

    axesLinePen     = 6;
    majorPen        = 6;
    minorPen        = 6;

    axesLineWidth   = 2;
    majorTicWidth   = 2;
    minorTicWidth   = 1;
    majorGridWidth  = 1;
    minorGridWidth  = 1;

    axesOffset      = 0;
    majorTicLength  = 15;
    minorTicLength  = 10;
}


Plane3D  PC_3DAxesFormat::GetPlane(Axes3DPlanes axIn)
{
    static const Plane3D planeMap[naxes3DPlanes] = {p3D_XY, p3D_XY,  p3D_XZ, p3D_XZ, p3D_YZ, p3D_YZ};
    return planeMap[axIn];
}

