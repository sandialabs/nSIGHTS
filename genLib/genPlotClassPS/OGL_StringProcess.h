///////////////////////////////////////////////////////////////////////////////////
//
// OGL_StringProcess.h
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

#ifndef OGL_STRINGPROCESS_H
#define OGL_STRINGPROCESS_H

#include <genClass/C_Graphic.h>

#include <genClass/T_SC_Array.h>
#include <genClass/SC_IntArray.h>

#include <genPlotClass/PSC_Font.h>

class OGLFont;
class OGL3DBase;

namespace OGLStringProcessing {

    enum  CharacterType   {ct_NotSet,           // just for initialization -- error if found after processing
                           ct_Token,            // part of a token -- skipped when printing
                           ct_BaseChar,         // character on string base line
                           ct_SuperChar,        // part of a superscript
                           ct_SubChar,          // part of a subscript
                           ct_NewLine};         // a new line

    class FontWithMetricsArray;

    class CharacterInfo {

        public:
            CharacterType   characterType;
                                            // units of cXX are pixels(2D) or notional units (3D)
                                            // independent of rotation
            bool            isSymbol;       // ignore current fot type - render as symbol
            double          cWidth;         // character width
            double          cWidthInc;      // increment width independent of rotation
            double          cHeight;        //  height of top of char above baseline
            double          cDescent;       //  distance of bottom of char below baseline
            double          cOffset;        // start distance above/below baseline for sub/super
            int             cVal;           // actual value

            double          xVal;           // start pos of char after all processing complete
            double          yVal;

            int             listIndex;       // OpenGL display list index for char

            int             fontIndex;      //  index of font in associated font index array


            // these will probably end up in project settings
            static double           subSuperSizeRatio;
            static double           superStartOffset;
            static double           subStartOffset;

        public:
                            CharacterInfo();

                            ~CharacterInfo() {};

            inline bool     IsNullChar()    {return (characterType == ct_Token) || (characterType == ct_NewLine);}
            inline bool     IsSubSuper()    {return (characterType == ct_SuperChar) || (characterType == ct_SubChar);}
            inline bool     IsSymbol()      {return isSymbol;}
            inline bool     IsPrintable()   {return (characterType == ct_BaseChar) ||
                                                  (characterType == ct_SuperChar) || (characterType == ct_SubChar);}

            void            Set2DCharacterInfo(FontWithMetricsArray& inFonts,
                                               int                   baseIndex,
                                               int                   baseSymbolIndex,
                                               int                   subSupIndex,
                                               int                   subSupSymbolIndex);

            void            Set3DCharacterInfo(FontWithMetricsArray& inFonts,
                                               int                   baseIndex,
                                               int                   baseSymbolIndex,
                                               int                   subSupIndex,
                                               int                   subSupSymbolIndex);

            bool            IsTokenChar(unsigned char inChar);

            void            SetForXY(const double& xperWidthUnit,        //
                                     const double& yperHeightUnit,
                                           bool    adjustForSuper);   // because 3D fonts are all same size


        private:
                            // no copying required
                            CharacterInfo(const CharacterInfo& a) {};

    };


    class CharacterInfoArray : public T_SC_Array<CharacterInfo> {
        public:
                int     GetTokenPos(const char* token,
                                          int   startPos);

                void    SetCharType(int           startPos,
                                    int           endPos,
                                    CharacterType charType);

    };


    //  basic metrics for each font type in processed string
    class FontWithMetrics : public PSC_Font {
        public:
            double      lineSize;
            double      descenderSize;
            OGLFont*    oglFont;

        public:
                    FontWithMetrics();
                    FontWithMetrics(const PSC_Font& a);

                    ~FontWithMetrics() {};

            bool    operator==(const FontWithMetrics& a);
            bool    operator==(const PSC_Font& a);

            void    SetForXY(const double& yperHeightUnit);

#ifdef _GLPRHARDCOPY_
            void    SetForHardCopy(const double& glpFontSizeScale);
#endif


    };

    class FontWithMetricsArray : public T_SC_Array<FontWithMetrics> {
        public:
                bool    Get2DFontIndex(const PSC_Font& newFont,
                                             int&      fontIndex);
                bool    Get3DFontIndex(const PSC_Font& newFont,
                                             int&      fontIndex);

                void    SetForXY(const double& yperHeightUnit);

    };


    class  ProcessedString {
        public:
            CharacterInfoArray      charInfo;
            FontWithMetricsArray    fontWithMetrics;
            double&                 lineSpacingMultiplier;


        private:
            SC_IntArray             lineStart;

        public:
                        ProcessedString(double& lsMult);
                        ~ProcessedString() {};

            //  first step -- process tokens, set fonts, get character metrics
            bool        Process2DString(const PSC_Font&   baseFont,  // returns false only if fonts can't be set up
                                        const char*       inStr);

            bool        Process3DString(const PSC_Font&   baseFont,  // returns false only if fonts can't be set up
                                        const char*       inStr);

            //  next step -- set XY for horizontal relative to 0,0 base pos
            void        SetBasicXY(const double& xPerPixel,
                                   const double& yPerPixel,
                                   const double& textLengthMultiplier,
                                         bool    adjustForSuper);

            //  final step (2D) -- set actual XY relative to start XY and accounting for rotation
            void        SetFinalXY(const double&                    x0,   // X coord of top left corner
                                   const double&                    y0,
                                         HorizontalTextAlignment    inStringAlign);  // Y coord of top left corner

            // actual output routines
            void        Print2D(const double& rasterZ);
            void        Print3D(OGL3DBase& baseObj,
                                bool       shouldBeLit);


#ifdef _GLPRHARDCOPY_
            void        HardCopy2D(const double& glpFontSizeScale,
                                   const double& rasterZ);
#endif

            // block information -- must be past SetBasicXY()
            void        GetSizeInfo(double&   blockWidth,       // max width of all lines
                                    double&   blockHeight);     // lineSize + (n lines - 1) * line size * lineSpacingMultiplier


            void        GetSizeDeviation(double& overTop,      // account for superscript or all aeiougj
                                         double& belowBase);   // > 0 if descenders in last line

            double      GetLineSize() {return fontWithMetrics[0].lineSize;}

            int         GetnLines();

            bool        ContainsSubSuper();

            bool        ContainsBaseSymbol();

            bool        ContainsSubSuperSymbol();

            inline bool IsSingleLine() {return GetnLines() == 1;}

            bool        IsPrintable();

        private:
            void        ProcessPair(const char*     startTok,
                                    const char*     endTok,
                                    CharacterType   betweenTok);

            void        ProcessSingle(const char*     snglTok,
                                      CharacterType   atTok);

            void        ProcessString(const char*     inStr);

            void        ProcessLine(int             lineStart,
                                    int             lineEnd,
                                    const double&   baseLineY);

                        // for line size, super/sub ratio, offset
            void        ProcessMetrics(const char*    metricTok,
                                             double&  metricVal,
                                             int      metricMin,
                                             int      metricMax);

            bool        ProcessXXX(int      xxxStart,
                                   int&     xxxVal,
                                   int      minVal,
                                   int      maxVal);

            double      GetLineLen(int lineStart,
                                   int lineEnd);

            void        AdjustLineX(int             lineStart,
                                    int             lineEnd,
                                    const double&   xIncrement);

   };



};   // end NS


using namespace OGLStringProcessing;



#endif // !OGL_STRINGPROCESS_H

