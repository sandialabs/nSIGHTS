///////////////////////////////////////////////////////////////////////////////////
//
// DC_ColorMap.cpp
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
//              framework: arrays of colors.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/DC_ColorMap.h>

DC_ColorMap :: DC_ColorMap()
{
    ncolorMap = 50;
}

DC_ColorMap :: DC_ColorMap(const DC_ColorMap& a)
{
    LocalCopy(a);
}

DC_ColorMap :: ~DC_ColorMap ()
{
}


DC_ColorMap&  DC_ColorMap::operator=(const DC_ColorMap& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void DC_ColorMap::LocalCopy(const DC_ColorMap& a)
{
    ncolorMap = a.ncolorMap;
    for (int i = 0; i < ncolorMap; i++)
        colorMapColors[i] = a.colorMapColors[i];
}




bool DC_ColorMap::AddColorMaps(const DC_ColorMap& map1,
                               const DC_ColorMap& map2)
{
    if ((map1.ncolorMap + map2.ncolorMap) > maxCMcolors)
        return false;

    ncolorMap = 0;
    int i;
    for ( i = 0; i < map1.ncolorMap; i++)
        colorMapColors[ncolorMap++] = map1.colorMapColors[i];
    for ( i = 0; i < map2.ncolorMap; i++)
        colorMapColors[ncolorMap++] = map2.colorMapColors[i];

    return true;
}

void DC_ColorMap::MakeLinearRGBMap(const SC_ColorSpec& startMap,
                                   const SC_ColorSpec& endMap)
{
    double nShad = ncolorMap;
    double delR =  (endMap.RH - startMap.RH) / nShad;
    double delG =  (endMap.GS - startMap.GS) / nShad;
    double delB =  (endMap.BV - startMap.BV) / nShad;

    SC_ColorSpec loopRGB(startMap);
    for (int i = 0; i < ncolorMap; i++)
        {
            colorMapColors[i] = loopRGB;

            loopRGB.RH += delR;
            loopRGB.GS += delG;
            loopRGB.BV += delB;
        }
}

void DC_ColorMap::MakeLinearHSVMap(const SC_ColorSpec& startMap,
                                   const SC_ColorSpec& endMap)
{
    double nShad = ncolorMap;
    double delH =  (endMap.RH - startMap.RH) / nShad;
    double delS =  (endMap.GS - startMap.GS) / nShad;
    double delV =  (endMap.BV - startMap.BV) / nShad;

    SC_ColorSpec loopHSV(startMap);
    for (int i = 0; i < ncolorMap; i++)
        {
            colorMapColors[i] = loopHSV;
            colorMapColors[i].ToRGB();

            loopHSV.RH += delH;
            loopHSV.GS += delS;
            loopHSV.BV += delV;
        }
}


SC_ColorSpec DC_ColorMap :: GetColor(int colorIndx) const
{
    colorIndx = colorIndx < ncolorMap ? colorIndx : 0;

    return (colorMapColors[colorIndx]);
}


bool DC_ColorMap::AllSameColor() const
{
    for (int i = 1; i < ncolorMap; i++)
        if (colorMapColors[0] != colorMapColors[i])
            return false;

    return true;
}

