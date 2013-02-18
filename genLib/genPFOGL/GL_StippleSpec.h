///////////////////////////////////////////////////////////////////////////////////
//
// GL_StippleSpec.h
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
//      Implements renderer dependent code to actually draw genPFO functional
// objects on a plot.  Is platform independent but
// relies on platform dependent code in genPlotClassPS.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef GL_STIPPLESPEC_H
#define GL_STIPPLESPEC_H

#include <genClass/C_Graphic.h>
#include <genClass/SC_ColorSpec.h>

#include <genPlotClass/PSC_StippleSpec.h>

#include <genPFOGL/GL_Line.h>


class GL_StippleSpec : public GL_Line {
    private:
        Coord3D             stippleScale;
        StippleSpecType     currType;


   public:
        void        SetStippleScale(int  stippScale);
                    // 3D

        void        SetProperties(const PSC_StippleSpec&  inSpec);

                    // 2D - no hard copy
        void        DrawStipple(const Point2D& llPoint,         // in plot coords
                                const Point2D& urPoint);

                    // 2D - hard copy
        void        DrawStipple(const Point2D& llPoint,         // in plot coords
                                const Point2D& urPoint,
                                      int&     nvertex);


    private:
        void        SetAnnoStippleScale(int  symSize);


};


#endif // !GL_STIPPLESPEC_H

