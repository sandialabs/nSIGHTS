///////////////////////////////////////////////////////////////////////////////////
//
// C_Graphic.cpp
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

#include <genClass/C_Graphic.h>
#include <genClass/U_Str.h>

namespace genGraphicTypes {


    bool PlotSymbolOps::filledSymbols[] = {false, false, false, false, false, false,
                                           true, true, true, true};

    PlotSymbol PlotSymbolOps::outlinesForFilled[] = {ps_X, ps_X, ps_X, ps_X, ps_X, ps_X,
                                                     ps_Square, ps_Circle, ps_Triangle, ps_Diamond};

    bool        PlotSymbolOps::SymbolIsFilled(PlotSymbol inSym) {return filledSymbols[int(inSym)];}
    PlotSymbol  PlotSymbolOps::OutlineSymbol(PlotSymbol inSym) {return outlinesForFilled[int(inSym)];}

#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before any objects in app
    const LineTypeArray lineTypeData;

    LineTypeSpec::LineTypeSpec()
    {
        linePattern = 0xFFFF;
        lineFactor  = 1;
        CopyString(patName, "Solid", maxPatNameLen);
    }


    LineTypeSpec::LineTypeSpec(unsigned short lPat, int lFac, const char* pName)
        : linePattern(lPat), lineFactor(lFac)
    {
        CopyString(patName, pName, maxPatNameLen);
    }

    LineTypeSpec::LineTypeSpec(const LineTypeSpec& a)
        : linePattern(a.linePattern), lineFactor(a.lineFactor)
    {
        CopyString(patName, a.patName, maxPatNameLen);
    }

    LineTypeSpec&  LineTypeSpec::operator= (const LineTypeSpec& a)
    {
        if (&a != this)
            {
                linePattern = a.linePattern;
                lineFactor  = a.lineFactor;
                CopyString(patName, a.patName, maxPatNameLen);
            }
        return *this;
    }

    LineTypeArray::LineTypeArray()
    {
        SetResizable(16);
        (*this) += LineTypeSpec(0xFFFF, 1, "XXXXXXX");
        (*this) += LineTypeSpec(0x00FF, 4, "a Dash");
        (*this) += LineTypeSpec(0x00FF, 3, "b Dash");
        (*this) += LineTypeSpec(0x00FF, 2, "c Dash");
        (*this) += LineTypeSpec(0x00FF, 1, "d Dash");
        (*this) += LineTypeSpec(0x0F0F, 1, "e Dash");
        (*this) += LineTypeSpec(0x3333, 1, "f  Dot");
        (*this) += LineTypeSpec(0x0FFF, 4, "a Mix");
        (*this) += LineTypeSpec(0x0FFF, 3, "b Mix");
        (*this) += LineTypeSpec(0x0FFF, 2, "c Mix");
        (*this) += LineTypeSpec(0x0FFF, 1, "d Mix");
        (*this) += LineTypeSpec(0x3CFF, 4, "a Phan");
        (*this) += LineTypeSpec(0x3CFF, 3, "b Phan");
        (*this) += LineTypeSpec(0x3CFF, 2, "c Phan");
        (*this) += LineTypeSpec(0x3CFF, 1, "d Phan");
        (*this) += LineTypeSpec(0xCCCF, 4, "a Phan2");
        (*this) += LineTypeSpec(0xCCCF, 3, "a Phan2");
        (*this) += LineTypeSpec(0xCCCF, 2, "a Phan2");
        (*this) += LineTypeSpec(0xCCCF, 1, "a Phan2");
    }

    void LineTypeArray::GetPatternNames(SC_ConstStringArray& patNames) const
    {
        patNames.Alloc(MaxLineType());
        for (int i = 0; i < MaxLineType(); i++)
            patNames += GetLineTypeSpec(i).patName;
    }

    const LineTypeSpec& LineTypeArray::GetLineTypeSpec(PlotLineType plType) const
    {
        if (plType <= 0)
            return (*this)[0];

        if (plType >= nlistData)
            return LastIndex();

        return (*this)[plType];
    }


    const StippleSpecArray stippleSpecData;

    StippleSpec::StippleSpec()
    {
        stippleLines.SetResizable(16);
        CopyString(patName, "Rect", maxPatNameLen);
    }


    StippleSpec::StippleSpec(const char* pName)
    {
        stippleLines.SetResizable(16);
        CopyString(patName, pName, maxPatNameLen);
    }

    StippleSpec::StippleSpec(const StippleSpec& a)
        : stippleLines(a.stippleLines)
    {
        CopyString(patName, a.patName, maxPatNameLen);
    }

    StippleSpec&  StippleSpec::operator= (const StippleSpec& a)
    {
        if (&a != this)
            {
                stippleLines = a.stippleLines;
                CopyString(patName, a.patName, maxPatNameLen);
            }
        return *this;
    }

    StippleSpecArray::StippleSpecArray()
    {
        SetResizable(16);
        int currStipIndex = 0;
        (*this) += StippleSpec("Rect");
        {
            SC_Line2DArray& currStipp = tListData[currStipIndex++].stippleLines;
            currStipp += Line2D(Point2D(0.0, 0.5), Point2D(1.0, 0.5));
            currStipp += Line2D(Point2D(0.5, 0.0), Point2D(0.5, 1.0));
        }
        (*this) += StippleSpec("Cross");
        {
            SC_Line2DArray& currStipp = tListData[currStipIndex++].stippleLines;
            currStipp += Line2D(Point2D(0.25, 0.5), Point2D(0.75, 0.5));
            currStipp += Line2D(Point2D(0.5, 0.25), Point2D(0.5, 0.750));
        }
        (*this) += StippleSpec("Subway");
        {
            SC_Line2DArray& currStipp = tListData[currStipIndex++].stippleLines;
            currStipp += Line2D(Point2D(0.0, 0.0), Point2D(1.0, 0.0));
            currStipp += Line2D(Point2D(0.0, 0.0), Point2D(0.0, 0.5));
            currStipp += Line2D(Point2D(0.0, 0.5), Point2D(1.0, 0.5));
            currStipp += Line2D(Point2D(0.5, 0.5), Point2D(0.5, 1.0));
        }

        (*this) += StippleSpec("Dolostone");
        {
            SC_Line2DArray& currStipp = tListData[currStipIndex++].stippleLines;
            currStipp += Line2D(Point2D(0.0, 0.0),  Point2D(1.5, 0.0));
            currStipp += Line2D(Point2D(0.0, 0.25), Point2D(1.5, 0.25));
            currStipp += Line2D(Point2D(0.0, 0.50), Point2D(1.5, 0.50));
            currStipp += Line2D(Point2D(0.0, 0.75), Point2D(1.5, 0.75));

            currStipp += Line2D(Point2D(0.0, 0.0),  Point2D(0.25, 0.25));
            currStipp += Line2D(Point2D(1.0, 0.25), Point2D(1.25, 0.50));
            currStipp += Line2D(Point2D(0.5, 0.75), Point2D(0.75, 1.0));
        }

        (*this) += StippleSpec("Dolostone-Shale");
        {
            SC_Line2DArray& currStipp = tListData[currStipIndex++].stippleLines;
            currStipp += Line2D(Point2D(0.0, 0.0),  Point2D(1.5, 0.0));
            currStipp += Line2D(Point2D(0.0, 0.25), Point2D(1.5, 0.25));
            currStipp += Line2D(Point2D(0.0, 0.50), Point2D(1.5, 0.50));
            currStipp += Line2D(Point2D(0.0, 0.75), Point2D(1.5, 0.75));

            currStipp += Line2D(Point2D(0.0, 0.0),  Point2D(0.25, 0.25));
            currStipp += Line2D(Point2D(1.0, 0.25), Point2D(1.25, 0.50));
            currStipp += Line2D(Point2D(0.5, 0.75), Point2D(0.75, 1.0));

            currStipp += Line2D(Point2D(0.00, 0.125), Point2D(0.20, 0.125));
            currStipp += Line2D(Point2D(0.30, 0.125), Point2D(0.50, 0.125));
            currStipp += Line2D(Point2D(0.60, 0.125), Point2D(0.80, 0.125));
            currStipp += Line2D(Point2D(0.90, 0.125), Point2D(1.10, 0.125));
            currStipp += Line2D(Point2D(1.20, 0.125), Point2D(1.400, 0.125));

            currStipp += Line2D(Point2D(0.00, 0.375), Point2D(0.20, 0.375));
            currStipp += Line2D(Point2D(0.30, 0.375), Point2D(0.50, 0.375));
            currStipp += Line2D(Point2D(0.60, 0.375), Point2D(0.80, 0.375));
            currStipp += Line2D(Point2D(0.90, 0.375), Point2D(1.10, 0.375));
            currStipp += Line2D(Point2D(1.20, 0.375), Point2D(1.400, 0.375));

            currStipp += Line2D(Point2D(0.00, 0.625), Point2D(0.20, 0.625));
            currStipp += Line2D(Point2D(0.30, 0.625), Point2D(0.50, 0.625));
            currStipp += Line2D(Point2D(0.60, 0.625), Point2D(0.80, 0.625));
            currStipp += Line2D(Point2D(0.90, 0.625), Point2D(1.10, 0.625));
            currStipp += Line2D(Point2D(1.20, 0.625), Point2D(1.400, 0.625));

            currStipp += Line2D(Point2D(0.00, 0.875), Point2D(0.20, 0.875));
            currStipp += Line2D(Point2D(0.30, 0.875), Point2D(0.50, 0.875));
            currStipp += Line2D(Point2D(0.60, 0.875), Point2D(0.80, 0.875));
            currStipp += Line2D(Point2D(0.90, 0.875), Point2D(1.10, 0.875));
            currStipp += Line2D(Point2D(1.20, 0.875), Point2D(1.400, 0.875));

        }

        (*this) += StippleSpec("Evaporite");
        {
            SC_Line2DArray& currStipp = tListData[currStipIndex++].stippleLines;

            currStipp += Line2D(Point2D(0.0, 0.75), Point2D(0.25, 1.0));
            currStipp += Line2D(Point2D(0.0, 0.50), Point2D(0.50, 1.0));
            currStipp += Line2D(Point2D(0.0, 0.25), Point2D(0.75, 1.0));
            currStipp += Line2D(Point2D(0.0, 0.0),  Point2D(1.0, 1.0));
            currStipp += Line2D(Point2D(0.25, 0.0), Point2D(1.0, 0.75));
            currStipp += Line2D(Point2D(0.50, 0.0), Point2D(1.0, 0.50));
            currStipp += Line2D(Point2D(0.75, 0.0), Point2D(1.0, 0.25));

            currStipp += Line2D(Point2D(0.0, 0.25), Point2D(0.25, 0.0));
            currStipp += Line2D(Point2D(0.0, 0.50), Point2D(0.50, 0.0));
            currStipp += Line2D(Point2D(0.0, 0.75), Point2D(0.75, 0.0));
            currStipp += Line2D(Point2D(0.0, 1.0),  Point2D(1.0, 0.0));
            currStipp += Line2D(Point2D(0.25, 1.0), Point2D(1.0, 0.25));
            currStipp += Line2D(Point2D(0.50, 1.0), Point2D(1.0, 0.50));
            currStipp += Line2D(Point2D(0.75, 1.0), Point2D(1.0, 0.75));
        }

        (*this) += StippleSpec("Shale");
        {
            SC_Line2DArray& currStipp = tListData[currStipIndex++].stippleLines;
            currStipp += Line2D(Point2D(0.0, 0.0),  Point2D(1.5, 0.0));
            currStipp += Line2D(Point2D(0.0, 0.25), Point2D(1.5, 0.25));
            currStipp += Line2D(Point2D(0.0, 0.50), Point2D(1.5, 0.50));
            currStipp += Line2D(Point2D(0.0, 0.75), Point2D(1.5, 0.75));

            currStipp += Line2D(Point2D(0.00, 0.125), Point2D(0.20, 0.125));
            currStipp += Line2D(Point2D(0.30, 0.125), Point2D(0.50, 0.125));
            currStipp += Line2D(Point2D(0.60, 0.125), Point2D(0.80, 0.125));
            currStipp += Line2D(Point2D(0.90, 0.125), Point2D(1.10, 0.125));
            currStipp += Line2D(Point2D(1.20, 0.125), Point2D(1.400, 0.125));

            currStipp += Line2D(Point2D(0.00, 0.375), Point2D(0.20, 0.375));
            currStipp += Line2D(Point2D(0.30, 0.375), Point2D(0.50, 0.375));
            currStipp += Line2D(Point2D(0.60, 0.375), Point2D(0.80, 0.375));
            currStipp += Line2D(Point2D(0.90, 0.375), Point2D(1.10, 0.375));
            currStipp += Line2D(Point2D(1.20, 0.375), Point2D(1.400, 0.375));

            currStipp += Line2D(Point2D(0.00, 0.625), Point2D(0.20, 0.625));
            currStipp += Line2D(Point2D(0.30, 0.625), Point2D(0.50, 0.625));
            currStipp += Line2D(Point2D(0.60, 0.625), Point2D(0.80, 0.625));
            currStipp += Line2D(Point2D(0.90, 0.625), Point2D(1.10, 0.625));
            currStipp += Line2D(Point2D(1.20, 0.625), Point2D(1.400, 0.625));

            currStipp += Line2D(Point2D(0.00, 0.875), Point2D(0.20, 0.875));
            currStipp += Line2D(Point2D(0.30, 0.875), Point2D(0.50, 0.875));
            currStipp += Line2D(Point2D(0.60, 0.875), Point2D(0.80, 0.875));
            currStipp += Line2D(Point2D(0.90, 0.875), Point2D(1.10, 0.875));
            currStipp += Line2D(Point2D(1.20, 0.875), Point2D(1.400, 0.875));
        }

        (*this) += StippleSpec("Limestone");
        {
            SC_Line2DArray& currStipp = tListData[currStipIndex++].stippleLines;
            currStipp += Line2D(Point2D(0.0, 0.0),  Point2D(1.5, 0.0));
            currStipp += Line2D(Point2D(0.0, 0.35), Point2D(1.5, 0.35));
            currStipp += Line2D(Point2D(0.0, 0.70), Point2D(1.5, 0.70));
            currStipp += Line2D(Point2D(0.0, 1.00), Point2D(1.5, 1.00));

            currStipp += Line2D(Point2D(0.25, 0.0), Point2D(0.25, 0.35));
            currStipp += Line2D(Point2D(1.00, 0.0), Point2D(1.00, 0.35));

            currStipp += Line2D(Point2D(0.75, 0.35), Point2D(0.75, 0.70));
            currStipp += Line2D(Point2D(1.5, 0.35), Point2D(1.5, 0.70));

            currStipp += Line2D(Point2D(0.25, 0.70), Point2D(0.25, 1.00));
            currStipp += Line2D(Point2D(1.00, 0.70), Point2D(1.00, 1.00));

            currStipp += Line2D(Point2D(0.75, 1.00), Point2D(0.75, 1.30));
            currStipp += Line2D(Point2D(1.5, 1.00),  Point2D(1.5, 1.30));
        }


        (*this) += StippleSpec("Sand");
        {
            SC_Line2DArray& currStipp = tListData[currStipIndex++].stippleLines;
            currStipp += Line2D(Point2D(0.,           0.),           Point2D(0.,           0.));
            currStipp += Line2D(Point2D(0.91591744,   0.684482275),  Point2D(0.91591744,   0.684482275));
            currStipp += Line2D(Point2D(0.17996864,   0.913518907),  Point2D(0.17996864,   0.913518907));
            currStipp += Line2D(Point2D(0.854486066,  0.926145039),  Point2D(0.854486066,  0.926145039));
            currStipp += Line2D(Point2D(0.024691874,  0.10567273),   Point2D(0.024691874,  0.10567273 ));
            currStipp += Line2D(Point2D(0.879670928,  0.865120265),  Point2D(0.879670928,  0.865120265));
            currStipp += Line2D(Point2D(0.9676475,    0.588002978),  Point2D(0.9676475,    0.588002978));
            currStipp += Line2D(Point2D(0.712531227,  0.941353273),  Point2D(0.712531227,  0.941353273));
            currStipp += Line2D(Point2D(0.092885973,  0.946385623),  Point2D(0.092885973,  0.946385623));
            currStipp += Line2D(Point2D(0.350691871,  0.401825458),  Point2D(0.350691871,  0.401825458));
            currStipp += Line2D(Point2D(0.82698922,   0.972032729),  Point2D(0.82698922,   0.972032729));
            currStipp += Line2D(Point2D(0.156379761,  0.639800406),  Point2D(0.156379761,  0.639800406));
            currStipp += Line2D(Point2D(0.99878013,   0.354085638),  Point2D(0.99878013,   0.354085638));
            currStipp += Line2D(Point2D(0.583445904,  0.060320249),  Point2D(0.583445904,  0.060320249));
            currStipp += Line2D(Point2D(0.026180856,  0.244517713),  Point2D(0.026180856,  0.244517713));
            currStipp += Line2D(Point2D(0.126549971,  0.997960819),  Point2D(0.126549971,  0.997960819));
            currStipp += Line2D(Point2D(0.059924298,  0.216928984),  Point2D(0.059924298,  0.216928984));
            currStipp += Line2D(Point2D(0.912912602,  0.723842936),  Point2D(0.912912602,  0.723842936));
            currStipp += Line2D(Point2D(0.434433458,  0.697368579),  Point2D(0.434433458,  0.697368579));
            currStipp += Line2D(Point2D(1.,           1.),           Point2D(1.,           1.));
        }


    }

    void StippleSpecArray::GetStippleNames(SC_ConstStringArray& patNames) const
    {
        patNames.Alloc(MaxStippleSpec());
        for (int i = 0; i < MaxStippleSpec(); i++)
            patNames += GetStippleSpec(i).patName;
    }

    const StippleSpec&  StippleSpecArray::GetStippleSpec(StippleSpecType ssType) const
    {
        if (ssType <= 0)
            return (*this)[0];

        if (ssType >= nlistData)
            return LastIndex();

        return (*this)[ssType];
    }


};

