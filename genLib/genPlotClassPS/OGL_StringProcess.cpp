///////////////////////////////////////////////////////////////////////////////////
//
// OGL_StringProcess.cpp
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
//      Platform specific implementation of genPlotClass codes
//  adds Windows and OpenGL specific code.  Includes
//  operating system independent implementation of
//  2D and 3D OpenGL functionality (platform specific
//  in this case refers to the OpenGL renderer).
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genClass/SC_IntArray.h>

#include <genPlotClassPS/OGL_Font.h>
#include <genPlotClassPS/OGL_3DBase.h>

#include <genPlotClassPS/OGL_StringProcess.h>

namespace OGLStringProcessing {

double  CharacterInfo::subSuperSizeRatio  = 0.6;
double  CharacterInfo::superStartOffset   = 0.60;
double  CharacterInfo::subStartOffset     = 0.30;


CharacterInfo::CharacterInfo()
{
    characterType = ct_NotSet;
    isSymbol    = false;
    cWidth      = 0.0;
    cWidthInc   = 0.0;
    cHeight     = 0.0;
    cDescent    = 0.0;
    cOffset     = 0.0;
    cVal        = 0;
    xVal        = 0.0;
    yVal        = 0.0;
    listIndex   = 0;
}

void CharacterInfo::Set2DCharacterInfo(FontWithMetricsArray& inFonts,
                                       int                   baseIndex,
                                       int                   baseSymbolIndex,
                                       int                   subSupIndex,
                                       int                   subSupSymbolIndex)
{
    if (IsNullChar())
        return;

    int width, widthInc, ascent, descent;

    if (IsSymbol())
    {
        baseIndex = baseSymbolIndex;
        subSupIndex = subSupSymbolIndex;
    }

    OGL2DFont& baseFont = *(static_cast<OGL2DFont*>(inFonts[baseIndex].oglFont));

    if (characterType == ct_BaseChar)
    {
        baseFont.GetCharSizeInfo(unsigned char (cVal), width, widthInc, ascent, descent);
        listIndex = baseFont.GetListBase() + cVal;
        fontIndex = baseIndex;
    }
    else
    {
        OGL2DFont& subsupFont = *(static_cast<OGL2DFont*>(inFonts[subSupIndex].oglFont));
        subsupFont.GetCharSizeInfo(unsigned char (cVal), width, widthInc, ascent, descent);
        listIndex = subsupFont.GetListBase()+ cVal;
        if (characterType == ct_SuperChar)
            cOffset = double(baseFont.GetLineSize()) * superStartOffset;
        else
            cOffset = -double(baseFont.GetLineSize()) * subStartOffset;
        fontIndex = subSupIndex;
    }

    cWidth    = double(width);
    cWidthInc = double(widthInc);
    cHeight   = double(ascent);
    cDescent  = double(descent);
}

void CharacterInfo::Set3DCharacterInfo(FontWithMetricsArray& inFonts,
                                       int                   baseIndex,
                                       int                   baseSymbolIndex,
                                       int                   subSupIndex,
                                       int                   subSupSymbolIndex)
{
    if (IsNullChar())
        return;

    if (IsSymbol())
    {
        baseIndex = baseSymbolIndex;
        subSupIndex = subSupSymbolIndex;
    }

    OGL3DFont& baseFont = *(static_cast<OGL3DFont*>(inFonts[baseIndex].oglFont));

    if (characterType == ct_BaseChar)
    {
        baseFont.GetCharSizeInfo(unsigned char (cVal), cWidth, cWidthInc, cHeight, cDescent);
        listIndex = baseFont.GetListBase() + cVal;
        fontIndex = baseIndex;
    }
    else
    {
        OGL3DFont& subsupFont = *(static_cast<OGL3DFont*>(inFonts[subSupIndex].oglFont));
        subsupFont.GetCharSizeInfo(unsigned char (cVal), cWidth, cWidthInc, cHeight, cDescent);
        listIndex = subsupFont.GetListBase()+ cVal;
        if (characterType == ct_SuperChar)
            cOffset = baseFont.GetLineSize() * superStartOffset;
        else
            cOffset = -baseFont.GetLineSize() * subStartOffset;
        fontIndex = subSupIndex;
    }
}


void CharacterInfo::SetForXY(const double& xperWidthUnit,
                             const double& yperHeightUnit,
                                   bool    adjustForSuper)
{
    cWidth    *= xperWidthUnit;
    cWidthInc *= xperWidthUnit;
    cHeight   *= yperHeightUnit;
    cDescent  *= yperHeightUnit;
    cOffset   *= yperHeightUnit;

    if (adjustForSuper && IsSubSuper())
    {
        cWidth    *= subSuperSizeRatio;
        cWidthInc *= subSuperSizeRatio;
        cHeight   *= subSuperSizeRatio;
        cDescent  *= subSuperSizeRatio;
    }
}

bool CharacterInfo::IsTokenChar(unsigned char inChar)
{
    return (characterType == ct_NotSet) && (cVal == int (inChar));
}

int CharacterInfoArray::GetTokenPos(const char* token,
                                      int   startPos)
{
    int ntok = StringLength(token);
    int maxPos = Size() - ntok + 1;
    if (startPos > maxPos)
        return -1;

    for (int i = startPos; i < maxPos; i++)
    {
        if ((*this)[i].IsTokenChar(token[0]))
        {
            bool tokOK = true;
            for (int j = 1; j < ntok; j++)
                if (!(*this)[i + j].IsTokenChar(token[j]))
                {
                    tokOK = false;
                    break;
                }

            if (tokOK)
            {
                SetCharType(i, i + ntok, ct_Token);
                return i + ntok;
            }
        }
    }

    return -1;
}

void CharacterInfoArray::SetCharType(int           startPos,
                                     int           endPos,
                                     CharacterType charType)
{
    for (int i = startPos; i < endPos; i++)
    {
        CharacterInfo& currChar = (*this)[i];
        if ((currChar.characterType == ct_NotSet) || (currChar.characterType == ct_BaseChar))
            currChar.characterType = charType;
    }
}


FontWithMetrics::FontWithMetrics()
{
    lineSize = 1.0;
    descenderSize = 0.5;
}

FontWithMetrics::FontWithMetrics(const PSC_Font& a) : PSC_Font(a)
{
    lineSize = 1.0;
    descenderSize = 0.5;
}


void FontWithMetrics::SetForXY(const double& yperHeightUnit)
{
    lineSize *= yperHeightUnit;
    descenderSize *= yperHeightUnit;
}

bool FontWithMetrics::operator==(const FontWithMetrics& a)
{
    return PSC_Font::operator==(a);
}
bool FontWithMetrics::operator==(const PSC_Font& a)
{
    return PSC_Font::operator==(a);
}

#ifdef _GLPRHARDCOPY_

void FontWithMetrics::SetForHardCopy(const double& glpFontSizeScale)
{
    int glpFontFamily, glpFontFace;
    switch (fontFamily)
    {
        case PSC_Font::ff_Helvetica : {
                glpFontFamily = GLP_FONTFAMILY_HELVETICA; break; }
        case PSC_Font::ff_Times : {
                glpFontFamily = GLP_FONTFAMILY_TIMES; break; }
        case PSC_Font::ff_Courier : {
                glpFontFamily = GLP_FONTFAMILY_COURIER; break; }
        case PSC_Font::ff_Symbol : {
                glpFontFamily = GLP_FONTFAMILY_SYMBOL; break; }
    }

    if (slantRegular && weightMedium)
        glpFontFace = GLP_FONTFACE_REGULAR;
    else if (slantRegular)
        glpFontFace = GLP_FONTFACE_BOLD;
    else if (weightMedium)
        glpFontFace = GLP_FONTFACE_ITALIC;
    else
        glpFontFace = GLP_FONTFACE_BOLDITALIC;

    // glpFontScale is pts per pixel
    double actualPix = double(static_cast<OGL2DFont*>(oglFont)->GetFontSize());

    GLfloat pointSize = float(actualPix * glpFontSizeScale);

    glpSetFont(glpFontFamily, glpFontFace, pointSize);
}

#endif



bool FontWithMetricsArray::Get2DFontIndex(const PSC_Font& newFont,
                                               int&      fontIndex)
{
    for ( int i = 0; i < Size(); i++)
        if ((*this)[i] == newFont)
        {
            fontIndex = i;
            return true;
        }

    // font not setup yet - get ogl
    OGL2DFont* nextFont = OGL2DFont::SetupFont(newFont);
    if (nextFont == 0)
        return false;

    // add to list
    (*this) += newFont;

    // set members
    LastIndex().oglFont = nextFont;
    LastIndex().lineSize = double(nextFont->GetLineSize());

    int wid, widInc, asc, desc;
    nextFont->GetCharSizeInfo('y', wid, widInc, asc, desc);
    LastIndex().descenderSize = double(desc);

    fontIndex = UpperBound();

    return true;
}


bool FontWithMetricsArray::Get3DFontIndex(const PSC_Font& newFont,
                                               int&      fontIndex)
{
    for ( int i = 0; i < Size(); i++)
        if ((*this)[i] == newFont)
        {
            fontIndex = i;
            return true;
        }

    // font not setup yet - get ogl
    OGL3DFont* nextFont = OGL3DFont::SetupFont(newFont);
    if (nextFont == 0)
        return false;

    // add to list
    (*this) += newFont;

    // set members
    LastIndex().oglFont = nextFont;
    LastIndex().lineSize = nextFont->GetLineSize();

    double wid, widInc, asc;
    nextFont->GetCharSizeInfo('y', wid, widInc, asc, LastIndex().descenderSize);

    fontIndex = UpperBound();

    return true;
}



void FontWithMetricsArray::SetForXY(const double& yperHeightUnit)
{
    for (int i = 0; i < Size(); i++)
        (*this)[i].SetForXY(yperHeightUnit);
}



ProcessedString::ProcessedString(double& lsMult) : lineSpacingMultiplier(lsMult)
{
    fontWithMetrics.SetResizable();
}

void ProcessedString::ProcessPair(const char*           startTok,
                                  const char*           endTok,
                                        CharacterType   betweenTok)
{
    int endSize = StringLength(endTok);
    int startVal = charInfo.GetTokenPos(startTok, 0);
    while (startVal >= 0)
    {
        int endVal = charInfo.GetTokenPos(endTok, startVal);

        // no close till end, assume pair is until end
        if (endVal < 0)
        {
            charInfo.SetCharType(startVal, charInfo.Size(), betweenTok);
            return;
        }

        charInfo.SetCharType(startVal, endVal - endSize, betweenTok);
        startVal = charInfo.GetTokenPos(startTok, endVal);
    }
}

void ProcessedString::ProcessSingle(const char*     snglTok,
                                    CharacterType   atTok)
{
    int tokPos = charInfo.GetTokenPos(snglTok, 0);
    while (tokPos >= 0)
    {
        // set last char of tok to atTok
        charInfo[tokPos - 1].characterType = atTok;

        tokPos = charInfo.GetTokenPos(snglTok, tokPos);
    }
}

void ProcessedString::ProcessMetrics(const char*    metricTok,
                                           double&  metricVal,
                                           int      metricMin,
                                           int      metricMax)
{
    //  only 1 per input accepted
    int metPos = charInfo.GetTokenPos(metricTok, 0);
    int intVal;
    if ((metPos > 0) && ProcessXXX(metPos, intVal, metricMin, metricMax))
        metricVal = double(intVal) / 100.0;
}


void ProcessedString::ProcessString(const char*  inStr)
{
    int strLen = StringLength(inStr);
    if (strLen == 0)
        return;

    // initialize with chars
    charInfo.AllocAndSetSize(strLen);
    int i;
    for (i = 0; i < strLen; i++)
        charInfo[i].cVal = int(static_cast<unsigned char>(inStr[i]));

    // set special chars
    int charPos = charInfo.GetTokenPos("\\C=", 0);
    while (charPos >= 0)
    {
        int charVal;
        if (ProcessXXX(charPos, charVal, 0, 255))
        {
            // set last char of tok to atTok
            charInfo[charPos].characterType = ct_BaseChar;
            charInfo[charPos].cVal = charVal;
        }
        charPos = charInfo.GetTokenPos("\\C=", charPos + 1);
    }

    // set symbols
    charPos = charInfo.GetTokenPos("\\SYM", 0);
    while (charPos >= 0)
    {
        int charVal;
        if (ProcessXXX(charPos, charVal, 0, 255))
        {
            // set last char of tok to atTok
            charInfo[charPos].characterType = ct_BaseChar;
            charInfo[charPos].isSymbol = true;
            charInfo[charPos].cVal = charVal;
        }
        charPos = charInfo.GetTokenPos("\\SYM", charPos + 1);
    }


    // look for token pairs
    ProcessPair("<^", "^>", ct_SuperChar);  // superscripts
    ProcessPair("<_", "_>", ct_SubChar);    // subscripts

    // new lines
    ProcessSingle("\\n", ct_NewLine);

    //  spacing metrics
    //  line spacing
    ProcessMetrics("\\LS", lineSpacingMultiplier, 20, 900);
    ProcessMetrics("\\SSS", CharacterInfo::subSuperSizeRatio, 30, 100);
    ProcessMetrics("\\SPO", CharacterInfo::superStartOffset, 50, 100);
    ProcessMetrics("\\SBO", CharacterInfo::subStartOffset, 0, 50);


    //  everything thats still not set must be a base char
    for(i = 0; i < strLen; i++)
        if (charInfo[i].characterType == ct_NotSet)
            charInfo[i].characterType = ct_BaseChar;

}

bool ProcessedString::Process2DString(const PSC_Font&   baseFont,  // returns false only if fonts can't be set up
                                      const char*       inStr)
{
    // initialize the base font
    int baseIndex;
    if (!fontWithMetrics.Get2DFontIndex(baseFont, baseIndex))
        return false;

    ProcessString(inStr);
    if (charInfo.IsEmpty())
        return true;

    int baseSymIndex = baseIndex;
    if (ContainsBaseSymbol())
    {
        PSC_Font symFont = baseFont;
        symFont.fontFamily = PSC_Font::ff_Symbol;
        if (!fontWithMetrics.Get2DFontIndex(symFont, baseSymIndex))
            return false;
    }


    int subSupIndex = baseIndex;
    int subSupSymIndex = baseIndex;
    if (ContainsSubSuper())
    {
        PSC_Font subFont = baseFont;
        subFont.fontSize *= CharacterInfo::subSuperSizeRatio;

        if (!fontWithMetrics.Get2DFontIndex(subFont, subSupIndex))
            return false;

        if (ContainsSubSuperSymbol())
        {
            PSC_Font symFont = subFont;
            symFont.fontFamily = PSC_Font::ff_Symbol;
            if (!fontWithMetrics.Get2DFontIndex(symFont, subSupSymIndex))
                return false;
        }
    }

    for (int i = 0; i < charInfo.Size(); i++)
        charInfo[i].Set2DCharacterInfo(fontWithMetrics, baseIndex, baseSymIndex, subSupIndex, subSupSymIndex);

    return true;
}

bool ProcessedString::Process3DString(const PSC_Font&   baseFont,  // returns false only if fonts can't be set up
                                      const char*       inStr)
{
    // initialize the base font
    int baseIndex;
    if (!fontWithMetrics.Get3DFontIndex(baseFont, baseIndex))
        return false;

    ProcessString(inStr);
    if (charInfo.IsEmpty())
        return true;

    int baseSymIndex = baseIndex;
    if (ContainsBaseSymbol())
    {
        PSC_Font symFont = baseFont;
        symFont.fontFamily = PSC_Font::ff_Symbol;
        if (!fontWithMetrics.Get3DFontIndex(symFont, baseSymIndex))
            return false;
    }

    int subSupIndex = baseIndex;
    int subSupSymIndex = baseIndex;
    if (ContainsSubSuper())
    {
        PSC_Font subFont = baseFont;
        subFont.fontSize *= CharacterInfo::subSuperSizeRatio;

        if (!fontWithMetrics.Get3DFontIndex(subFont, subSupIndex))
            return false;

        if (ContainsSubSuperSymbol())
        {
            PSC_Font symFont = subFont;
            symFont.fontFamily = PSC_Font::ff_Symbol;
            if (!fontWithMetrics.Get2DFontIndex(symFont, subSupSymIndex))
                return false;
        }
    }

    for (int i = 0; i < charInfo.Size(); i++)
        charInfo[i].Set3DCharacterInfo(fontWithMetrics, baseIndex, baseSymIndex, subSupIndex, subSupSymIndex);

    return true;
}



void  ProcessedString::ProcessLine(int             lineStart,
                                   int             lineEnd,
                                   const double&   baseLineY)
{
    double currX = 0.0;
    double lastBaseCharX = 0.0;         // start point for sub supers

    bool lastWasSub = false;
    bool lastWasSuper = false;

    for (int i = lineStart; i < lineEnd; i++)
    {
        CharacterInfo& currChar = charInfo[i];
        if (!currChar.IsPrintable())
            continue;

        // set base y
        currChar.yVal = baseLineY;

        // subscripts
        if (currChar.characterType == ct_SubChar)
        {
            lastWasSuper = false;
            if (!lastWasSub)
                currX = lastBaseCharX;
            lastWasSub = true;

            currChar.yVal += currChar.cOffset;
        }
        else if (currChar.characterType == ct_SuperChar)
        {
            lastWasSub = false;
            if (!lastWasSuper)
                currX = lastBaseCharX;
            lastWasSuper = true;

            currChar.yVal += currChar.cOffset;
        }

        currChar.xVal = currX;
        currX += currChar.cWidthInc;

        if (currChar.characterType == ct_BaseChar)
        {
            lastWasSub = false;
            lastWasSuper = false;
            lastBaseCharX = currX;
        }
    }
}

bool ProcessedString::ProcessXXX(int      xxxStart,
                                 int&     xxxVal,
                                 int      minVal,
                                 int      maxVal)
{
    int maxPos = charInfo.Size() - 2;
    if (xxxStart > maxPos)
        return false;

    char temp[4];
    bool leadZero = true;
    for (int i = 0; i < 3; i++)
    {
        temp[i] = static_cast<unsigned char>(charInfo[xxxStart + i].cVal);
        leadZero = leadZero && (temp[i] == '0');
        if (leadZero)
            temp[i] = ' ';
    }
    temp[3] = '\0';
    TrimLeft(temp);

    // all 0?
    bool retVal;
    if (IsNull(temp))
    {
        xxxVal = 0;
        retVal = (minVal <= 0) && (maxVal >= 0);
    }
    else
        retVal = IsValidInt(temp, xxxVal) && (xxxVal >= minVal) && (xxxVal <= maxVal);
    if (retVal)
    {
        charInfo[xxxStart].characterType = ct_Token;
        charInfo[xxxStart + 1].characterType = ct_Token;
        charInfo[xxxStart + 2].characterType = ct_Token;
    }

    return retVal;
}


void ProcessedString::SetBasicXY(const double& xPerPixel,
                                 const double& yPerPixel,
                                 const double& textLengthMultiplier,
                                       bool    adjustForSuper)

{
    if (charInfo.IsEmpty())
        return;

    double xperWidthUnit, yperHeightUnit;

    if (fontWithMetrics[0].IsHorizontal())
    {
        xperWidthUnit = xPerPixel * textLengthMultiplier;
        yperHeightUnit = yPerPixel;
    }
    else
    {
        xperWidthUnit = yPerPixel * textLengthMultiplier;
        yperHeightUnit = xPerPixel;
    }

    // set font metrics
    fontWithMetrics.SetForXY(yperHeightUnit);

    // break out line positions and process for XY
    int nLines = GetnLines();
    lineStart.AllocAndSetSize(nLines + 1);
    lineStart[0] = 0;
    nLines = 1;
    int i;
    for(i = 0; i < charInfo.Size(); i++)
    {
        charInfo[i].SetForXY(xperWidthUnit, yperHeightUnit, adjustForSuper);
        // slight kluge as index 0 is already set as line start, but is safe as can't be set because token is two chars long
        if (charInfo[i].characterType == ct_NewLine)
            lineStart[nLines++] = i;
    }

    // and the end for ease of use
    lineStart[nLines] = charInfo.Size();

    double yStart = 0.0;
    for (i = 0; i < nLines; i++)
    {
        ProcessLine(lineStart[i], lineStart[i + 1], yStart);
        yStart -=  fontWithMetrics[0].lineSize * lineSpacingMultiplier;
    }
}

//  final step (2D) -- set actual XY relative to start XY and accounting for rotation
void ProcessedString::SetFinalXY(const double&                  x0,
                                 const double&                  y0,
                                       HorizontalTextAlignment  inStringAlign)
{
    if (charInfo.IsEmpty())
        return;

    // line adjust if necessary
    if ((inStringAlign != hta_Left) && (lineStart.Size() > 2))
    {
        double maxX, maxY;
        GetSizeInfo(maxX, maxY);
        int nLines = lineStart.Size() - 1;
        for (int i = 0; i < nLines; i++)
        {
            double currLen = GetLineLen(lineStart[i], lineStart[i + 1]);
            if (fabs(currLen) < fabs(maxX))
            {
                double dX = maxX - currLen;
                if (inStringAlign == hta_Center)
                    dX /= 2.0;
                AdjustLineX(lineStart[i], lineStart[i + 1], dX);
            }
        }
    }


    FontWithMetrics&  baseFont = fontWithMetrics[0];

    // raster Y is 0 at baseline -- y0 is 0 at linesize

    for(int i = 0; i < charInfo.Size(); i++)
    {
        CharacterInfo& currChar = charInfo[i];
        if ( baseFont.IsHorizontal())
        {
            currChar.xVal += x0;
            currChar.yVal += y0 - baseFont.lineSize;
        }
        else
        {
            // swap X and Y
            double temp = currChar.xVal;
            currChar.xVal = currChar.yVal;
            currChar.yVal = temp;

            if (baseFont.fontRotation == PSC_Font::fr_VertLeft)
            {
                currChar.xVal = x0 - currChar.xVal + baseFont.lineSize;
                currChar.yVal += y0;
            }
            else
            {
                currChar.xVal += x0 - baseFont.lineSize;
                currChar.yVal = y0 - currChar.yVal;
            }
        }
    }
}

void ProcessedString::Print2D(const double& rasterZ)
{
    for (int i = 0; i < charInfo.Size(); i++)
    {
        CharacterInfo& currChar = charInfo[i];
        if (currChar.IsPrintable())
        {
            glRasterPos3d(currChar.xVal, currChar.yVal, rasterZ);
            glCallList(currChar.listIndex);
        }
    }
}

void ProcessedString::Print3D(OGL3DBase& baseObj,
                              bool       shouldBeLit)
{
    // save everything -- necessary otherwise lighting gets screwed up
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    //  set lighting
    if (shouldBeLit)
        glEnable(GL_LIGHTING);

    //  set modes and culling
    glPolygonMode(GL_FRONT, GL_FILL);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // just estimates for now
    int nPoly = charInfo.Size() * 250;
    if (fontWithMetrics[0].fontThickness > 0.0)
            nPoly *= 5;

    baseObj.HardCopyBlockStart(nPoly);
    for (int i = 0; i < charInfo.Size(); i++)
    {
        CharacterInfo& currChar = charInfo[i];
        if (currChar.IsPrintable())
        {
            glPushMatrix();
                glTranslated(currChar.xVal, currChar.yVal, 0.0);

                if (currChar.IsSubSuper())
                    glScaled(CharacterInfo::subSuperSizeRatio, CharacterInfo::subSuperSizeRatio, CharacterInfo::subSuperSizeRatio);

                glCallList(currChar.listIndex);
            glPopMatrix();
        }
    }
    baseObj.HardCopyBlockEnd();

    glPopAttrib();

}


#ifdef _GLPRHARDCOPY_

void ProcessedString::HardCopy2D(const double& glpFontSizeScale,
                                 const double& rasterZ)
{
    if (charInfo.IsEmpty())
        return;

    int lastFont = -1;

    char charStr[2];
    charStr[1] = '\0';

    GLfloat rotAng;
    switch (fontWithMetrics[0].fontRotation) {
        case PSC_Font::fr_Horiz : {
            rotAng = 0.0;
            break;
        }
        case PSC_Font::fr_VertLeft : {
            rotAng = 90.0;
            break;
        }
        case PSC_Font::fr_VertRight : {
            rotAng = -90.0;
            break;
        }
    }

    for (int i = 0; i < charInfo.Size(); i++)
    {
        CharacterInfo& currChar = charInfo[i];

        if (currChar.IsPrintable())
        {

            if (currChar.fontIndex != lastFont)
            {
                lastFont = currChar.fontIndex;
                fontWithMetrics[lastFont].SetForHardCopy(glpFontSizeScale);
            }

            glRasterPos3d(currChar.xVal, currChar.yVal, rasterZ);
            // special cases for glpr bug
            charStr[0] = unsigned char(currChar.cVal);
            if (charStr[0] == '(')
                glpText("\\(", rotAng);
            else if (charStr[0] == ')')
                glpText("\\)", rotAng);
            else
                glpText(charStr, rotAng);
        }
    }
}

#endif

void ProcessedString::GetSizeInfo(double&   blockWidth,       // max width of all lines
                                  double&   blockHeight)
{
    blockWidth = 0.0;
    double baseLineSize = fontWithMetrics[0].lineSize;
    blockHeight = baseLineSize;

    for(int i = 0; i < charInfo.Size(); i++)
    {
        CharacterInfo& currChar = charInfo[i];
        if (currChar.IsPrintable())
        {
            double nextWidth = currChar.xVal + currChar.cWidth;
            if (fabs(nextWidth) > fabs(blockWidth))
                blockWidth = nextWidth;
        }
        else if (currChar.characterType == ct_NewLine)
        {
            blockHeight += baseLineSize * lineSpacingMultiplier;
        }
    }
}

void ProcessedString::GetSizeDeviation(double& overTop,
                                       double& belowBase)
{
    overTop = 0.0;
    belowBase = 0.0;

    bool firstLine = true;
    for(int i = 0; i < charInfo.Size(); i++)
    {
        CharacterInfo& currChar = charInfo[i];
        if (currChar.IsPrintable())
        {
            if (firstLine)
            {
                double charHeight = currChar.yVal + currChar.cHeight;
                if (fabs(charHeight) > fabs(overTop))
                    overTop = charHeight;
            }

            double charDrop  = currChar.cOffset - currChar.cDescent;
            if (fabs(charDrop) < fabs(belowBase))
                belowBase = charDrop;
        }
        else if (currChar.characterType == ct_NewLine)
        {
            firstLine = false;
            belowBase = 0.0;
        }
    }

    if (fabs(overTop) > 0.0)
        overTop -= fontWithMetrics[0].lineSize;

    belowBase = -belowBase;

}


int ProcessedString::GetnLines()
{
    int nLines = 1;
    for(int i = 0; i < charInfo.Size(); i++)
        if (charInfo[i].characterType == ct_NewLine)
            nLines++;
    return nLines;
}

bool ProcessedString::ContainsSubSuper()
{
    for(int i = 0; i < charInfo.Size(); i++)
        if (charInfo[i].IsSubSuper())
            return true;

    return false;
}

bool ProcessedString::ContainsBaseSymbol()
{
   for(int i = 0; i < charInfo.Size(); i++)
        if (charInfo[i].IsSymbol() &&(!charInfo[i].IsSubSuper()))
            return true;
    return false;
}

bool ProcessedString::ContainsSubSuperSymbol()
{
    for(int i = 0; i < charInfo.Size(); i++)
        if (charInfo[i].IsSymbol() && charInfo[i].IsSubSuper())
            return true;

    return false;
}


bool ProcessedString::IsPrintable()
{
    for(int i = 0; i < charInfo.Size(); i++)
        if (charInfo[i].IsPrintable())
            return true;

    return false;
}


double ProcessedString::GetLineLen(int lineStart, int lineEnd)
{
    double lineWidth = 0.0;

    for(int i = lineStart; i < lineEnd; i++)
    {
        CharacterInfo& currChar = charInfo[i];
        if (currChar.IsPrintable())
        {
            double nextWidth = currChar.xVal + currChar.cWidth;
            if (fabs(nextWidth) > fabs(lineWidth))
                lineWidth = nextWidth;
        }
    }
    return lineWidth;
}


void ProcessedString::AdjustLineX(int             lineStart,
                                  int             lineEnd,
                                  const double&   xIncrement)
{
    for(int i = lineStart; i < lineEnd; i++)
        charInfo[i].xVal += xIncrement;
}

};   // end NS


