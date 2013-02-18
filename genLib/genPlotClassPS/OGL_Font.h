///////////////////////////////////////////////////////////////////////////////////
//
// OGL_Font.h
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

#ifndef PS_OGLFONT_H
#define PS_OGLFONT_H

#include <genPlotClassPS/PSC_FontPS.h>


class OGLFont {
    protected:
        enum            {endFontList = 256};

        PSC_FontPS      fontType;       // type of font contained in class

        int             listBase;       // OpenGL display list start

    private:
        int             nLists;         // number created
        HGLRC           createContext;  // need so that when context is destroyed, font is unloaded
        CClientDC*      createDisplay;

        // linked list pointers
        OGLFont*        nextFont;
        OGLFont*        prevFont;
        static OGLFont* firstFont;

        const bool      is2D;

    public:
                        OGLFont(const PSC_Font& inFont, bool inis2D);
                        ~OGLFont();

                        // gets display list base
        int             GetListBase() {return listBase;}

        static OGLFont* GetLoadedFont(const PSC_Font& inFont, bool inis2D);

        static void     UnloadFontsForContext(HGLRC contextToUnload);

    protected:
        void            SetLocals();

};

class OGL2DFont : public OGLFont {

    private:
        GLYPHMETRICS    fontInfo[endFontList];


    public:
                        OGL2DFont(const PSC_Font& inFont);
                        ~OGL2DFont();

        int             GetLineSize();  // returns height of char 'M';
        int             GetFontSize();  // returns height of char 'M' + descent of 'g';

                        // info for a single char
        void            GetCharSizeInfo(unsigned char   inChar,
                                        int&            width,
                                        int&            widthInc,   // X increment
                                        int&            ascent, // pixels above and including baseline
                                        int&            descent);   // pixels below baseline


        //  used by OGL Base font routines to get font handle for anno routines
        static OGL2DFont* SetupFont(const PSC_Font&    inFont);  // returns a handle to OGL2DFont
                                                                 // returns NULL if context not set
                                                                 // or if font bitmap couldn't be loaded

    private:
        bool            LoadAndSetup();

};

class OGL3DFont : public OGLFont {
    private:
        GLYPHMETRICSFLOAT  fontInfo[endFontList];


    public:
                        OGL3DFont(const PSC_Font& inFont);
                        ~OGL3DFont();

        double          GetLineSize();  // returns height of char 'X';

                        // info for a single char
        void            GetCharSizeInfo(unsigned char   inChar,
                                        double&         width,
                                        double&         widthInc,   // X increment
                                        double&         ascent, // pixels above and including baseline
                                        double&         descent);   // pixels below baseline


        //  used by OGL Base font routines to get font handle for anno routines
        static OGL3DFont* SetupFont(const PSC_Font&    inFont);  // returns a handle to OGL3DFont
                                                                 // returns NULL if context not set
                                                                 // or if font bitmap couldn't be loaded

    private:
        bool            LoadAndSetup();

};



#endif // !PS_OGLFONT_H

