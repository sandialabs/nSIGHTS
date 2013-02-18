///////////////////////////////////////////////////////////////////////////////////
//
// C_Graphic.h
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
// DESCRIPTION: Platform independent and application independent base claseess and
//              support classes.
//              Defines line types and symbol specifications.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_GRAPHIC_H
#define C_GRAPHIC_H

#include <genClass/C_Common.h>
#include <genClass/SC_StringArray.h>
#include <genClass/SC_LineArray.h>
#include <genClass/T_SC_Array.h>

namespace genGraphicTypes {

  enum  PlotSymbol {ps_X, ps_Plus, ps_Square, ps_Circle, ps_Triangle, ps_Diamond,
                    ps_FSquare, ps_FCircle, ps_FTriangle, ps_FDiamond};

  enum  PlotSymLine {psl_LineOnly, psl_SymbolOnly, psl_SymbolAndLine};

  //      modified for new generic line type support - Mar 06 - JDA
  //      enum  PlotLineType{plt_Solid, plt_Dashed, plt_DoubleDashed};
  typedef int PlotLineType;  // index into global lineTypeData
  enum    {plt_Solid = 0};   // solid line is index 0

  typedef int StippleSpecType;  // index into global stippleSpecData
  enum    {pss_Rect = 0};       // basic rectangle index 0

  enum  HorizontalTextAlignment {hta_Left, hta_Center, hta_Right};

  enum  VerticalTextAlignment {vta_Top, vta_Center, vta_Bottom};

  class PlotSymbolOps {
  private:
    enum               {nsymbols = 10};
    static bool        filledSymbols[nsymbols];
    static PlotSymbol  outlinesForFilled[nsymbols];
  public:
    static bool        SymbolIsFilled(PlotSymbol inSym);
    static PlotSymbol  OutlineSymbol(PlotSymbol inSym);
  };

  // modified for new generic line type support - Mar 06 - JDA
  class LineTypeSpec {
  public:
    unsigned short          linePattern;   // see glLineStipple
    int                     lineFactor;
    enum                    {maxPatNameLen = 40};
    char                    patName[maxPatNameLen];

  public:
    LineTypeSpec();
    LineTypeSpec(unsigned short lPat, int lFac, const char* pName);
    LineTypeSpec(const LineTypeSpec& a);

    ~LineTypeSpec(){};

    LineTypeSpec&       operator= (const LineTypeSpec& a);
  };


  // somewhat of a kluge - ment for const access only
  // all line types defined on construction;
  // index 0 is assumed to be solid line

  class LineTypeArray : T_SC_Array <LineTypeSpec> {

  public:
    LineTypeArray();

    int MaxLineType() const {return Size();}

    const LineTypeSpec& GetLineTypeSpec(PlotLineType plType) const;
    void                GetPatternNames(SC_ConstStringArray& patNames) const;

  };

  extern const LineTypeArray lineTypeData;

  // stipple support - FB364 - Jan 10
  class StippleSpec {
  public:
    SC_Line2DArray  stippleLines; // defined in 0-1 coord space
    enum                    {maxPatNameLen = 40};
    char                    patName[maxPatNameLen];

  public:
    StippleSpec();
    StippleSpec(const char* pName);
    StippleSpec(const StippleSpec& a);

    ~StippleSpec(){};

    StippleSpec&   operator= (const StippleSpec& a);
  };


  class StippleSpecArray : T_SC_Array <StippleSpec> {

  public:
    StippleSpecArray();

    int MaxStippleSpec() const {return Size();}

    const StippleSpec&  GetStippleSpec(StippleSpecType ssType) const;
    void                GetStippleNames(SC_ConstStringArray& patNames) const;

  };

  extern const StippleSpecArray stippleSpecData;



};

using namespace genGraphicTypes;


#endif
