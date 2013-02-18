///////////////////////////////////////////////////////////////////////////////////
//
// PSC_FontPS.cpp
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
//      maps PSC_Font structure to MFC CFont
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <math.h>
#include <genClass/U_Str.h>

#include <genPlotClassPS/PSC_FontPS.h>

bool PSC_FontPS::CreateCFont(CFont& wFont,
                            int     logPixelsY)
{

    LOGFONT lf;

    memset(&lf, 0, sizeof(LOGFONT));
    switch ( fontFamily)
    {
        case ff_Helvetica : {
            lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
            CopyString(lf.lfFaceName, "Arial", LF_FACESIZE);
//          CopyString(lf.lfFaceName, "Symbol", LF_FACESIZE);
            break;
        }
        case ff_Times     : {
            lf.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
            CopyString(lf.lfFaceName, "Times New Roman", LF_FACESIZE);
            break;
        }
        case ff_Courier   : {
            lf.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
            CopyString(lf.lfFaceName, "Courier New", LF_FACESIZE);
            break;
        }
        case ff_Symbol   : {
            lf.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
            CopyString(lf.lfFaceName, "Symbol", LF_FACESIZE);
            break;
        }
    }

    if (weightMedium)
        lf.lfWeight = FW_NORMAL;
    else
        lf.lfWeight = FW_BOLD;

    if (slantRegular)
        lf.lfItalic = 0;
    else
        lf.lfItalic = 1;

    lf.lfUnderline = 0;

//  lf.lfHeight = fontSize;
//  lf.lfHeight = -MulDiv(fontSize, logPixelsY, 72);
    lf.lfHeight = int(fontSize * double(logPixelsY) / 72.0);

    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS;

    lf.lfCharSet = DEFAULT_CHARSET;

    HFONT hFont = HFONT(GetStockObject(DEFAULT_GUI_FONT));
    if (hFont !=NULL ) {
        LOGFONT lfDefaultGuiFont;
        if ( ::GetObject(hFont, sizeof(lfDefaultGuiFont), &lfDefaultGuiFont) ) {
            lf.lfCharSet = lfDefaultGuiFont.lfCharSet;
        }
    }

    return wFont.CreateFontIndirect(&lf) != 0;

}



