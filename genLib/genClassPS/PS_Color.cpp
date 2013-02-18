///////////////////////////////////////////////////////////////////////////////////
//
// PS_Color.cpp
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
// DESCRIPTION: Platform specific but application independent code.
//              maps SC_ColorSpec to Windows API COLORREF
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClassPS/PS_Color.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int nColorEnum = 10;
static COLORREF  stdColorVect[nColorEnum];
static StdColors stdColorMatch[nColorEnum];

static void AddColor(COLORREF       newColor,
                     StdColors      eMatch,
                     int&           indx)
{
    stdColorVect[indx] = newColor;
    stdColorMatch[indx] = eMatch;
    indx++;
}


void MviewColorSetup()
{
    int nextColor = 0;

    // system colors
    AddColor(::GetSysColor(COLOR_MENU),     sc_Background, nextColor);
    AddColor(::GetSysColor(COLOR_BTNFACE),  sc_Foreground, nextColor);

    // defined colors
    AddColor(RGB(255,   0,      0),         sc_Red, nextColor);
    AddColor(RGB(0,     255,    0),         sc_Green, nextColor);
    AddColor(RGB(0,     0,      255),       sc_Blue, nextColor);
    AddColor(RGB(0,     255,    255),       sc_Yellow, nextColor);
    AddColor(RGB(0,     0,      0),         sc_Black, nextColor);
    AddColor(RGB(255,   255,    255),       sc_White, nextColor);
    AddColor(RGB(0,     0,      64),        sc_LightBlue, nextColor);
    AddColor(RGB(64,    0,      0),         sc_Pink, nextColor);
}

COLORREF GetStdColor(StdColors colorToGet)
//  gets the Motif pixel for the std color
{
    for ( int i = 0; i < nColorEnum; i++)
        if (stdColorMatch[i] == colorToGet)
            return stdColorVect[i];
    return stdColorVect[0];
}


COLORREF  ConvertSC_ColorSpec(const SC_ColorSpec&  mvColor)
{
    int red   = int(mvColor.RH * 255.0);
    int green = int(mvColor.GS * 255.0);
    int blue  = int(mvColor.BV * 255.0);
    return RGB(red, green, blue);
}

