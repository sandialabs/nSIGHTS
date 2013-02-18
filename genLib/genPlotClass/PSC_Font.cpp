///////////////////////////////////////////////////////////////////////////////////
//
// PSC_Font.cpp
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
//      defines font characteristics
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genPlotClass/PSC_Font.h>

PSC_Font::PSC_Font()
{
    fontFamily       = ff_Helvetica;
    slantRegular     = true;  // if false then italic
    weightMedium     = true;  // if false then bold
    fontSize         = 8.0;   // default 8
    fontRotation     = fr_Horiz;
    fontThickness    = 0.0;
}

PSC_Font::~PSC_Font()
{

}

PSC_Font :: PSC_Font(const PSC_Font& a)
{
    LocalCopy(a);
}

PSC_Font& PSC_Font::operator= (const PSC_Font& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

bool PSC_Font::operator== (const PSC_Font& a)
{
    if (&a == this)
        return true;

    return  (fontFamily         == a.fontFamily)        &&
            (slantRegular       == a.slantRegular)      &&
            (weightMedium       == a.weightMedium)      &&
            (RealsAreSame(fontSize, a.fontSize, 0.01))  &&
            (fontRotation       == a.fontRotation)      &&
            (RealsAreSame(fontThickness, a.fontThickness, 0.01));
}


void PSC_Font::LocalCopy(const PSC_Font& a)
{
    fontFamily          = a.fontFamily  ;
    slantRegular        = a.slantRegular;
    weightMedium        = a.weightMedium;
    fontSize            = a.fontSize    ;
    fontRotation        = a.fontRotation;
    fontThickness       = a.fontThickness;
}

