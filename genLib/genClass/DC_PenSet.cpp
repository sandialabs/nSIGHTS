///////////////////////////////////////////////////////////////////////////////////
//
// DC_PenSet.cpp
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
// DESCRIPTION: Platform independent and application independent base classes and
//              support classes.
//              Data Classes (DC) that define basic types of data used within the
//              framework: Defines pen set consisting of 24 separate colors
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genClass/DC_PenSet.h>

DC_PenSet :: DC_PenSet()
{
    SetDefault();
}

DC_PenSet :: DC_PenSet(const DC_PenSet& a)
{
    LocalCopy(a);
}

DC_PenSet&   DC_PenSet::operator= (const DC_PenSet& a)
{
    if (this != &a)
        LocalCopy(a);
    return *this;
}


void DC_PenSet::LocalCopy(const DC_PenSet& a)
{
    for ( int i = 0; i < maxPenSet; i++)
        {
            penColor[i] = a.penColor[i];
            CopyString(penLabel[i], a.penLabel[i], maxPenLabelLen);
        }
}


SC_ColorSpec DC_PenSet :: GetColor(int penIndx) const
{
    penIndx = penIndx < maxPenSet ? penIndx : 0;
    return (penColor[penIndx]);
}


char* DC_PenSet :: GetLabel(int penIndx)
{
    penIndx = penIndx < maxPenSet ? penIndx : 0;
    return (penLabel[penIndx]);
}


void DC_PenSet::SetDefault()
{

    char* colorLab[maxPenSet];

    penColor[0]  = SC_ColorSpec(1.0, 1.0, 1.0);   colorLab[0]  = "White";
    penColor[1]  = SC_ColorSpec(1.0, 0.0, 0.0);   colorLab[1]  = "Red";
    penColor[2]  = SC_ColorSpec(0.0, 1.0, 0.0);   colorLab[2]  = "Green";
    penColor[3]  = SC_ColorSpec(0.0, 0.0, 1.0);   colorLab[3]  = "Blue";
    penColor[4]  = SC_ColorSpec(1.0, 0.0, 1.0);   colorLab[4]  = "Magenta";
    penColor[5]  = SC_ColorSpec(0.0, 1.0, 1.0);   colorLab[5]  = "Cyan";
    penColor[6]  = SC_ColorSpec(0.0, 0.0, 0.0);   colorLab[6]  = "Black";
    penColor[7]  = SC_ColorSpec(1.0, 1.0, 0.0);   colorLab[7]  = "Yellow";
    penColor[8]  = SC_ColorSpec(1.0, 0.5, 0.1);   colorLab[8]  = "Orange";
    penColor[9]  = SC_ColorSpec(0.8, 0.8, 0.8);   colorLab[9]  = "Grey 8";
    penColor[10] = SC_ColorSpec(0.5, 0.5, 0.5);   colorLab[10] = "Grey 5";
    penColor[11] = SC_ColorSpec(0.2, 0.2, 0.2);   colorLab[11] = "Grey 2";
    penColor[12] = SC_ColorSpec(1.0, 0.25, 0.6);  colorLab[12] = "Purple";
    penColor[13] = SC_ColorSpec(1.0, 0.60, 0.6);  colorLab[13] = "Pink";
    penColor[14] = SC_ColorSpec(0.5, 0.40, 0.4);  colorLab[14] = "Brown";
    penColor[15] = SC_ColorSpec(0.7, 0.80, 0.9);  colorLab[15] = "PowderBlue";
    penColor[16] = SC_ColorSpec(0.0, 0.5,  0.9);  colorLab[16] = "SkyBlue";
    penColor[17] = SC_ColorSpec(0.2, 0.7,  0.8);  colorLab[17] = "Turquoise";
    penColor[18] = SC_ColorSpec(0.5, 0.8,  0.2);  colorLab[18] = "Chartreuse";
    penColor[19] = SC_ColorSpec(0.5, 0.8,  0.7);  colorLab[19] = "Mint";
    penColor[20] = SC_ColorSpec(1.0, 1.0,  0.5);  colorLab[20] = "LightYellow";
    penColor[21] = SC_ColorSpec(0.9, 0.7,  0.0);  colorLab[21] = "Gold";
    penColor[22] = SC_ColorSpec(0.6, 0.4,  0.6);  colorLab[22] = "Violet";
    penColor[23] = SC_ColorSpec(0.5, 0.25, 0.3);  colorLab[23] = "Grape";

    for (int i = 0; i < DC_PenSet::maxPenSet; i++)
        CopyString(penLabel[i], colorLab[i], maxPenLabelLen);
}

