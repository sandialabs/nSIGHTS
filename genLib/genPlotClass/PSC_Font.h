///////////////////////////////////////////////////////////////////////////////////
//
// PSC_Font.h
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

#ifndef PSC_FONT_H
#define PSC_FONT_H

class PSC_Font {

    public:
        enum  FontFamily   {ff_Helvetica, ff_Times, ff_Courier, ff_Symbol};
        enum  FontRotation {fr_Horiz, fr_VertLeft, fr_VertRight};

        FontFamily          fontFamily;
        bool                slantRegular;  // if false then italic
        bool                weightMedium;  // if false then bold
        double              fontSize;      // default 10 -- changed to real to support fractional fonts
                                           // for subscript/superscripts
        FontRotation        fontRotation;  // initial support for 2D rotated fonts

        double              fontThickness; // for 3D fonts - extrusion parameter

    public:

                            PSC_Font();
                            PSC_Font(const PSC_Font& a);
        virtual             ~PSC_Font();

        PSC_Font&            operator=(const PSC_Font& a);
        bool                operator==(const PSC_Font& a);

        inline bool         IsHorizontal()  const {return fontRotation == fr_Horiz;}
        inline bool         IsVertical()    const {return fontRotation != fr_Horiz;}

    protected:
        void                LocalCopy(const PSC_Font& a);

};


#endif // !PSC_FONT_H

