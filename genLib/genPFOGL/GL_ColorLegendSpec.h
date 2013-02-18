///////////////////////////////////////////////////////////////////////////////////
//
// GL_ColorLegendSpec.h
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

#ifndef GL_COLORLEGENDSPEC_H
#define GL_COLORLEGENDSPEC_H

#include <genClass/C_Graphic.h>

#include <genClass/SC_StringArray.h>
#include <genClass/SC_DoubleArray.h>
#include <genPFOGL/PC_Rectangle.h>

#include <genPlotClass/PSC_ColorLegendSpec.h>
#include <genPlotClass/PSC_AnnoSpec.h>

class GL_ColorLegendSpec : public PSC_ColorLegendSpec, PSC_AnnoSpec  {

    // slightly klugy approach -- base class is copied from inSpec in constructor
    // simplifies notation

    public:
        PC_Rectangle        fullFrame;    //outside frame
        PC_Rectangle        colorBarRect;   //color bar

        SC_StringArray      incLabels;
        SC_DoubleArray      incTVal;        // 0 - 1 (L - R) (B - T)

    private:
        OGLBase&            objectBase;
        PSC_Font            labelFont;
        double              mainLabelPos;
        double              incLabelPos;
        double              incXtra;
        double              aspectCorrection;

    public:
                                GL_ColorLegendSpec(PSC_ColorLegendSpec& inSpec, AnnoObjC& inObj, OGLBase&  inBase);
                                ~GL_ColorLegendSpec();

        void                    CalculateLegendPosition();
        void                    DrawLegendBase();
        void                    DrawColorBar();
        void                    DrawMainLabel();
        void                    DrawIncLabels();

    private:
        // no copy
                                GL_ColorLegendSpec(const GL_ColorLegendSpec& a);
                                GL_ColorLegendSpec&     operator= (const GL_ColorLegendSpec& a) {return *this;}


};

#endif // !GL_COLORLEGENDSPEC_H

