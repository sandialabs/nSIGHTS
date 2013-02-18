///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadColorMapCF.cpp
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
// DESCRIPTION: Basic functional objects (all derived from FuncObjC)
//              used by all applications, primarily concerned with
//              defining/operating on pen sets and colour maps:
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_ConvFile.h>
#include <genClass/U_Str.h>

#include <genClass/C_ConvFile.h>

#include <genDFO/DFO_ReadColorMap.h>


// ------------------ derived class for processing

class ColorMapFile :  public ConvFileC {

public:
    DC_ColorMap&    colorMapData;

public:
    ColorMapFile(char*          inputFile,
                 DC_ColorMap&   cm);

    ~ColorMapFile(){};

    void            ReadmViewColorMap();
    void            ReadColorDouble(double& colorVal);
    void            ReadPVwaveColorMap();
    void            ReadColorInt(double& colorVal);  // reads pvWave int converst to mView double

};

ColorMapFile :: ColorMapFile(char*          inputFile,
                             DC_ColorMap&   cm)
    : ConvFileC(inputFile), colorMapData(cm)
{
}

void ColorMapFile ::ReadColorDouble(double& colorVal)
{
    colorVal = CFgetNextDouble();
    if ((colorVal < 0.0) || (colorVal > 1.0))
        CFtextError("color value must be >= 0.0 and <= 1.0");
}

void ColorMapFile :: ReadmViewColorMap()
{
    if (!CFreadText())
        CFtextError("File empty");
    int nCM = CFgetNextInt();

    if ((nCM < 2) || (nCM > DC_ColorMap::maxCMcolors))
        CFtextError("# must be > 1 and <= 255");

    bool isHSV = false;
    char tempStr[80];
    if (CFgetNextStr(tempStr, 80))
        isHSV = (tempStr[0] == 'H');

    for ( int i = 0; i < nCM; i++)
        {
            if (!CFreadText())
                CFtextError("Unexpected EOF");
            ReadColorDouble(colorMapData.colorMapColors[i].RH);
            ReadColorDouble(colorMapData.colorMapColors[i].GS);
            ReadColorDouble(colorMapData.colorMapColors[i].BV);

            if (isHSV)
                colorMapData.colorMapColors[i].ToRGB();
        }
    colorMapData.ncolorMap = nCM;
}

void ColorMapFile ::ReadColorInt(double& colorVal)
{
    int colorInt = CFgetNextInt();
    if ((colorInt < 0) || (colorInt > 255))
        CFtextError("PVWave color value must be >= 0 and <= 255");
    colorVal = double(colorInt) / 255.0;
}

void ColorMapFile :: ReadPVwaveColorMap()
{
    int nCM = 0;
    while ((!CFeof()) && CFreadText())
        {
            if (nCM == DC_ColorMap::maxCMcolors)
                CFtextError("too many colors - max 256");
            ReadColorInt(colorMapData.colorMapColors[nCM].RH);
            ReadColorInt(colorMapData.colorMapColors[nCM].GS);
            ReadColorInt(colorMapData.colorMapColors[nCM].BV);
            nCM++;
        }
    colorMapData.ncolorMap = nCM;
}


bool DFO_ReadColorMap::ReadColorMap()
{
    ColorMapFile cmFile(colorMapFname, colorMap);
    cmFile.CFsetMsgStuff("ReadColorMap", "ColorMapFile", 10);
    try
        {
            if (usePVwaveFormat)
                cmFile.ReadPVwaveColorMap();
            else
                cmFile.ReadmViewColorMap();
        }
    catch (ConvFileC::CFReadError re) {
        SetObjErrMsg(re.errMsg);
        return false;
    }

    return true;
}

