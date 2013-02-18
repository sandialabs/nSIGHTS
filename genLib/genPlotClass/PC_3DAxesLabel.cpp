///////////////////////////////////////////////////////////////////////////////////
//
// PC_3DAxesLabel.cpp
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

#include <genClass/U_Str.h>

#include <genPlotClass/PC_3DAxesLabel.h>

PC_3DAxesLabel :: PC_3DAxesLabel()
{
    SetDefault();
}

PC_3DAxesLabel :: PC_3DAxesLabel(const PC_3DAxesLabel& a)
{
    LocalCopy(a);
}

PC_3DAxesLabel& PC_3DAxesLabel::operator= (const PC_3DAxesLabel& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void PC_3DAxesLabel::LocalCopy(const PC_3DAxesLabel& a)
{
    plotAxesLabel       = a.plotAxesLabel       ;
    plotIncrementLabel  = a.plotIncrementLabel  ;
    autoPositionLabels  = a.autoPositionLabels  ;

    labelFont           = a.labelFont           ;
    incrementFont       = a.incrementFont       ;

    xyLabIsVert         = a.xyLabIsVert         ;
    xLabYPosIsMax       = a.xLabYPosIsMax       ;
    yLabXPosIsMax       = a.yLabXPosIsMax       ;

    zLabOrientation     = a.zLabOrientation     ;
    zLabYPosIsMax       = a.zLabYPosIsMax       ;
    zLabXPosIsMax       = a.zLabXPosIsMax       ;

    //  labels
    CopyString(xaxesLabel, xaxesLabel, maxLabelLen);
    CopyString(yaxesLabel, yaxesLabel, maxLabelLen);
    CopyString(zaxesLabel, zaxesLabel, maxLabelLen);
}


void PC_3DAxesLabel::SetDefault()
{
    plotAxesLabel       = false;
    plotIncrementLabel  = true;
    autoPositionLabels  = true;

    //  labels
    xaxesLabel[0]   =   '\0';
    yaxesLabel[0]   =   '\0';
    zaxesLabel[0]   =   '\0';

    //  fonts
    labelFont.fontSize = 14;
    incrementFont.fontSize = 12;

    //  XY label format
    xyLabIsVert = true;
    xLabYPosIsMax = false;
    yLabXPosIsMax = true;

    //  Z label format
    zLabOrientation = zloYZn;
    zLabYPosIsMax = false;
    zLabXPosIsMax = false;
}


