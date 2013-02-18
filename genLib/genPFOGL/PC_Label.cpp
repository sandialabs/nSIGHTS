///////////////////////////////////////////////////////////////////////////////////
//
// PC_Label.cpp
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

#include <genClass/U_Str.h>

#include <genPFOGL/PC_Label.h>


PC_Label::PC_Label(OGL2DBase& inPlot)
    : currPlot(inPlot)
{
};


void  PC_Label::OffsetLabelPos(const PSC_Font&               labelFont,
                                        int                     offset,
                                        HorizontalTextAlignment halign,
                                        VerticalTextAlignment   valign)
{
    Point2D labOffset = currPlot.GetPixelComponents(offset);
    switch (labelFont.fontRotation)
    {
        case PSC_Font::fr_Horiz : {
            switch (valign)
            {
                case vta_Top    : {labelPos.pY -= labOffset.pY; break;}
                case vta_Center : {break;}
                case vta_Bottom : {labelPos.pY += labOffset.pY; break;}
            }

            switch (halign)
            {
                case hta_Left   : {labelPos.pX += labOffset.pX; break;}
                case hta_Center : {break;}
                case hta_Right  : {labelPos.pX -= labOffset.pX; break;}
            }
            break;
        }
        case PSC_Font::fr_VertLeft : {
            switch (valign)
            {
                case vta_Top    : {labelPos.pX += labOffset.pX; break;}
                case vta_Center : {break;}
                case vta_Bottom : {labelPos.pX -= labOffset.pX; break;}
            }

            switch (halign)
            {
                case hta_Left   : {labelPos.pY += labOffset.pY; break;}
                case hta_Center : {break;}
                case hta_Right  : {labelPos.pY -= labOffset.pY; break;}
            }
            break;
        }

        case PSC_Font::fr_VertRight : {
            switch (valign)
            {
                case vta_Top    : {labelPos.pX -= labOffset.pX; break;}
                case vta_Center : {break;}
                case vta_Bottom : {labelPos.pX += labOffset.pX; break;}
            }

            switch (halign)
            {
                case hta_Left   : {labelPos.pY -= labOffset.pY; break;}
                case hta_Center : {break;}
                case hta_Right  : {labelPos.pY += labOffset.pY; break;}
            }
            break;
        }

    }
}

void PC_Label::PrintUnclippedLabel(const PSC_Font&               labelFont,
                                   const char*                     labelText,
                                         HorizontalTextAlignment halign,
                                         VerticalTextAlignment   valign)
{
    currPlot.ClearClipPlanes();
    currPlot.PrintString(labelFont, labelText, halign, valign, labelPos.pX, labelPos.pY);
    currPlot.SetClipPlanes();
}

void PC_Label::PrintClippedLabel(const PSC_Font&               labelFont,
                                 const char*                     labelText,
                                       HorizontalTextAlignment halign,
                                       VerticalTextAlignment   valign)
{
    currPlot.PrintString(labelFont, labelText, halign, valign, labelPos.pX, labelPos.pY);
}



