///////////////////////////////////////////////////////////////////////////////////
//
// PC_PSOutData.cpp
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
#include <genPlotClass/PC_PSOutData.h>

PC_PSOutData :: PC_PSOutData()
{
    SetDefault();
}

PC_PSOutData :: PC_PSOutData(const PC_PSOutData& a)
{
    LocalCopy(a);
}

PC_PSOutData& PC_PSOutData::operator= (const PC_PSOutData& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void PC_PSOutData::LocalCopy(const PC_PSOutData& a)
{
    CopyString(outputFile, a.outputFile, stdFileStrLen);
    outputFormat        = a.outputFormat    ;
    outputIsPortrait    = a.outputIsPortrait;

    pageWidth           = a.pageWidth       ;
    pageHeight          = a.pageHeight      ;
    rightMargin         = a.rightMargin     ;
    leftMargin          = a.leftMargin      ;
    topMargin           = a.topMargin       ;
    bottomMargin        = a.bottomMargin    ;
    lineWidthMultiplier = a.lineWidthMultiplier;
    gammaCorrection     = a.gammaCorrection;
    zBufferMultiplier   = a.zBufferMultiplier;
    textLengthMultiplier = a.textLengthMultiplier;
    arialIsHelvetica     = a.arialIsHelvetica;
    nextPSIncrement      = a.nextPSIncrement;
}

void PC_PSOutData::SetDefault()
{
    CopyString(outputFile, "app.ps", stdFileStrLen);
    outputFormat        = of_PS;
    outputIsPortrait    = true;

    pageWidth           = 8.5;
    pageHeight          = 11.0;
    rightMargin         = 0.5;
    leftMargin          = 0.5;
    topMargin           = 0.5;
    bottomMargin        = 0.5;
    lineWidthMultiplier = 0.40;
    gammaCorrection     = 1.0;
    zBufferMultiplier   = 2.0;
    textLengthMultiplier = 1.0;
    arialIsHelvetica    = true;
    nextPSIncrement     = 0;
}

