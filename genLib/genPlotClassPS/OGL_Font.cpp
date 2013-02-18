///////////////////////////////////////////////////////////////////////////////////
//
// OGL_Font.cpp
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
//      support for bitmapped and polygon fonts using wgl API routines
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClassPS/PS_OGLBase.h>
#include <genPlotClassPS/OGL_Font.h>
#include <genPlotClassPS/OGL_Base.h>

OGLFont*  OGLFont::firstFont = 0;

// this is the character whose width metrics are used for spaces
static const char openGLBlankChar = 'n';

OGLFont::OGLFont(const PSC_Font& inFont, bool inIs2D) : fontType(inFont), is2D(inIs2D)
{
    listBase = -1;      // OpenGL display list start
    nLists   = 0;

    if (firstFont == 0)
    {
        // first member
        firstFont = this;
        prevFont = 0;
    }
    else
    {
        //  find the end of the list
        OGLFont* endList = firstFont;
        while (endList->nextFont)
            endList = endList->nextFont;
        endList->nextFont = this;
        prevFont = endList;
    }
    // new Font at end of list
    nextFont = 0;
}

OGLFont::~OGLFont()
{
   if (nextFont)
        nextFont->prevFont = prevFont;

    if (prevFont)
        prevFont->nextFont = nextFont;
    else
        firstFont = nextFont;

    if (nLists > 0)
        glDeleteLists(listBase, nLists);

}

OGLFont* OGLFont::GetLoadedFont(const PSC_Font& inFont, bool inis2D)
{
    for (OGLFont* currFont = firstFont; currFont; currFont = currFont->nextFont)
        if ((currFont->fontType == inFont) &&
            ((currFont->is2D && inis2D) || ((!currFont->is2D) && (!inis2D))))
            return currFont;

    return 0;
}


void  OGLFont::UnloadFontsForContext(HGLRC contextToUnload)

{
    OGLFont* currFont = firstFont;
    while (currFont)
    {
        OGLFont* nextFont = currFont->nextFont;
        if (currFont->createContext == contextToUnload)
            delete currFont;
        currFont = nextFont;
    }
}

void  OGLFont::SetLocals()
{
    nLists = endFontList + 1;
    listBase = glGenLists(nLists);
    if (listBase == 0)
        GenAppInternalError("OGLFont::SetLocals - out of display lists");
    createContext = PS_OGLBase::GetBaseContext();
    createDisplay = PS_OGLBase::GetBaseDisplay();
}
// *********************************************************** 2D fonts


OGL2DFont::OGL2DFont(const PSC_Font& inFont) : OGLFont(inFont, true)
{
}

OGL2DFont::~OGL2DFont()
{
}

int OGL2DFont::GetLineSize()
{

    return fontInfo[int('M')].gmBlackBoxY;
}

int OGL2DFont::GetFontSize()
{
    return fontInfo[int('M')].gmBlackBoxY + fontInfo[int('g')].gmptGlyphOrigin.y;
}

void  OGL2DFont::GetCharSizeInfo(unsigned char    inChar,
                                        int&    width,
                                        int&    widthInc,   // X increment
                                        int&    ascent, // pixels above and including baseline
                                        int&    descent)    // pixels belowbaseline
{
    GLYPHMETRICS& currInfo = fontInfo[int(inChar)];
    width    = currInfo.gmptGlyphOrigin.x + currInfo.gmBlackBoxX;
    widthInc = currInfo.gmCellIncX;
    descent  = currInfo.gmptGlyphOrigin.y;
    ascent   = currInfo.gmBlackBoxY - descent;
}



OGL2DFont* OGL2DFont::SetupFont(const PSC_Font&    inFont)  // returns a handle to OGLFont
{
    //  no fonts until context is OK
    if (PS_OGLBase::GetBaseContext() == 0)
        return 0;

    OGL2DFont* curr2DFont = static_cast<OGL2DFont*>(OGLFont::GetLoadedFont(inFont, true));
    if (curr2DFont != 0)
        return curr2DFont;

    //  alloc new font
    curr2DFont = new OGL2DFont(inFont);
    if (!curr2DFont->LoadAndSetup())
    {
        //  bad load, cleanup
        delete curr2DFont;
        return 0;
    }

    return curr2DFont;
}

bool  OGL2DFont::LoadAndSetup()
{
        //  get base DC and HDC
    CClientDC& currDC = *PS_OGLBase::GetBaseDisplay();

    //  get font structure
    CFont newFont;
    int capType;
    if (fontType.IsHorizontal())
        capType = LOGPIXELSY;
    else
        capType = LOGPIXELSX;

    if (!fontType.CreateCFont(newFont, currDC.GetDeviceCaps(capType)))
        return false;

    //  get base DC and HDC
    HDC currHDC = currDC.GetSafeHdc();
    //  select font into DC
    CFont* oldFont = currDC.SelectObject(&newFont);

    // get glyph data
    // you'd think we want no rotation transform (i.e identity matrix)
    // but for some reason this returns vertically mirrored text
    // all metrics are the same except for gmptGlyphOrigin which is what we
    // need to make further adjustments on rotated text
    // so instead we use the same matrix used for horizontal text

//  MAT2 ident = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};      // the real ident

    MAT2 horizMat = {{0, 1}, {0, 0}, {0, 0}, {0, -1}};  // the one we need
    int maxBuffer = 0;
    int i;
    for (i = 0; i < endFontList; i++)
    {
        // get metrics & required buffer size
        // kluge here because GetGlyphOutline returns an error for space (i == 32)
        // what we do is substitute metrics for letter n
        UINT currI = UINT(i);
        if (i == 32)
            currI = UINT(openGLBlankChar);

        int bufferSize = GetGlyphOutline(currHDC, currI, GGO_BITMAP, &fontInfo[i], 0, NULL, &horizMat);
        if (bufferSize < 0)
            return false;

        if (bufferSize > maxBuffer)
            maxBuffer = bufferSize;

    }

    // set up display lists
    SetLocals();

    //  make buffer
    GLubyte* buffer = new GLubyte [maxBuffer * 4];
    // for access
    PSC_Font::FontRotation currRot = fontType.fontRotation;

    //  rotation matrices for left & right
    MAT2 rotRightMat  = {{0, 0}, {0, 1}, {0, 1}, {0, 0}};
    MAT2 rotLeftMat = {{0, 0}, {0, -1}, {0, -1}, {0, 0}};

    MAT2* currMat;
    switch (currRot) {
        case PSC_Font::fr_Horiz : {
            currMat = &horizMat;
            break;
          }
        case PSC_Font::fr_VertLeft : {
            currMat = &rotLeftMat;
            break;
          }
        case PSC_Font::fr_VertRight : {
            currMat = &rotRightMat;
            break;
          }
    }

    // adjust Y step direction on rotated fonts
    bool use98 = PS_OGLBase::UseWin9XFonts();
    for (i = 0; i < endFontList; i++)
    {

        GLYPHMETRICS currMetric;

        // note: returns GDI_ERROR for i == 32
        // same kluge as prev loop
        UINT currI = UINT(i);
        bool isBlank = (i == 32);
        if (isBlank)
            currI = UINT(openGLBlankChar);

        if (GetGlyphOutline(currHDC, currI, GGO_BITMAP, &currMetric, maxBuffer, buffer, currMat) == GDI_ERROR)
        {
            // assumed blank try again with 'n'
            currI = UINT(openGLBlankChar);
            if (GetGlyphOutline(currHDC, currI, GGO_BITMAP, &currMetric, maxBuffer, buffer, currMat) == GDI_ERROR)
                GenAppInternalError("OGLFont::LoadAndSetup - GetGlyphOutline");
            isBlank = true;
        }

        GLubyte* currBuff = buffer;
        if (isBlank)
        {
            // continuing with kluge
            // pass in null buffer and 0 size so only increment is used
            currBuff = NULL;
            currMetric.gmBlackBoxX = 0;
            currMetric.gmBlackBoxY = 0;
        }


        //  fonts glyph origin needs to be adjusted dependent on rotation
        GLYPHMETRICS& infoMetric = fontInfo[i];
        switch (currRot) {
            case PSC_Font::fr_Horiz : {
                currMetric.gmptGlyphOrigin.x = -infoMetric.gmptGlyphOrigin.x;
                currMetric.gmptGlyphOrigin.y = infoMetric.gmptGlyphOrigin.y;
                break;
            }
            case PSC_Font::fr_VertLeft : {
                if (use98)
                {
                    // win98 adjustment
                    currMetric.gmCellIncY = - currMetric.gmCellIncY;
                }

                currMetric.gmptGlyphOrigin.x = infoMetric.gmBlackBoxY - infoMetric.gmptGlyphOrigin.y;
                currMetric.gmptGlyphOrigin.y = -infoMetric.gmptGlyphOrigin.x;
                break;
            }
            case PSC_Font::fr_VertRight : {
                if (use98)
                {
                    // win98 adjustment
                    currMetric.gmCellIncY = - currMetric.gmCellIncY;
                }

                currMetric.gmptGlyphOrigin.x = infoMetric.gmptGlyphOrigin.y;
                currMetric.gmptGlyphOrigin.y = infoMetric.gmBlackBoxX + infoMetric.gmptGlyphOrigin.x;
                break;
            }
        }

        // finally -- make display list
        glNewList(listBase + i, GL_COMPILE);

            glBitmap(GLsizei(currMetric.gmBlackBoxX), GLsizei(currMetric.gmBlackBoxY),
                     GLfloat(currMetric.gmptGlyphOrigin.x), GLfloat(currMetric.gmptGlyphOrigin.y),
                     GLfloat(currMetric.gmCellIncX), GLfloat(currMetric.gmCellIncY),
                     currBuff);

        glEndList();
    }

    delete [] buffer;

    // select old font
    currDC.SelectObject(oldFont);

    // done with the font
    newFont.DeleteObject();

    return true;
}

// *********************************************************** 3D fonts

OGL3DFont::OGL3DFont(const PSC_Font& inFont) : OGLFont(inFont, false)
{
}

OGL3DFont::~OGL3DFont()
{
}

double OGL3DFont::GetLineSize()
{

    return double(fontInfo[int('X')].gmfBlackBoxY);
}


void  OGL3DFont::GetCharSizeInfo(unsigned char    inChar,
                                        double&    width,
                                        double&    widthInc,   // X increment
                                        double&    ascent,     // pixels above and including baseline
                                        double&    descent)    // pixels belowbaseline
{
    GLYPHMETRICSFLOAT& currInfo = fontInfo[int(inChar)];
    width    = currInfo.gmfptGlyphOrigin.x + currInfo.gmfBlackBoxX;
    widthInc = currInfo.gmfCellIncX;

    ascent  = currInfo.gmfptGlyphOrigin.y;
    descent   = currInfo.gmfBlackBoxY - ascent;


//    descent  = currInfo.gmfptGlyphOrigin.y;
//    ascent   = currInfo.gmfBlackBoxY - descent;
}

OGL3DFont* OGL3DFont::SetupFont(const PSC_Font&    inFont)  // returns a handle to OGLFont
{
    //  no fonts until context is OK
    if (PS_OGLBase::GetBaseContext() == 0)
        return 0;

    // ignore rotation -- done in OpenGL
    PSC_Font tempFont(inFont);
    tempFont.fontRotation = PSC_Font::fr_Horiz;

    OGL3DFont* curr3DFont = static_cast<OGL3DFont*>(OGLFont::GetLoadedFont(tempFont, false));
    if (curr3DFont != 0)
        return curr3DFont;

    //  alloc new font
    curr3DFont = new OGL3DFont(inFont);
    if (!curr3DFont->LoadAndSetup())
    {
        //  bad load, cleanup
        delete curr3DFont;
        return 0;
    }

    return curr3DFont;
}


bool  OGL3DFont::LoadAndSetup()
{
        //  get base DC and HDC
    CClientDC& currDC = *PS_OGLBase::GetBaseDisplay();

    //  get font structure
    CFont newFont;
    if (!fontType.CreateCFont(newFont, currDC.GetDeviceCaps(LOGPIXELSY)))
        return false;

    //  get base DC and HDC
    HDC currHDC = currDC.GetSafeHdc();
    //  select font into DC
    CFont* oldFont = currDC.SelectObject(&newFont);

    // set up display lists
    SetLocals();

    // save all OpenGL settings - strange things may happen inside wglUseFontOutlines
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // create the bitmap display lists
    glPushMatrix();
        glLoadIdentity();

        wglUseFontOutlines(currHDC, 0, endFontList, listBase, 0.0f, float(fontType.fontThickness), WGL_FONT_POLYGONS, fontInfo);
    glPopMatrix();

    //  restore the settings
    glPopAttrib();

    // select old font
    currDC.SelectObject(oldFont);

    // done with the font
    newFont.DeleteObject();

    return true;
}













