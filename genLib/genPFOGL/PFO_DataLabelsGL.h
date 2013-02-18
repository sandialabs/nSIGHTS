///////////////////////////////////////////////////////////////////////////////////
//
// PFO_DataLabelsGL.h
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

#ifndef PFO_DATALABELSPS_H
#define PFO_DATALABELSPS_H

#include <genClass/SC_StringArray.h>
#include <genPlotClassPS/OGL_Base.h>

#include <genPFOGL/OGLBaseObj.h>
#include <genPFO/PFO_DataLabels.h>

class PFO_DataLabelsGL : public PFO_DataLabels, protected OGLBaseObj {
    private:
        SC_StringArray  labelDescText;
        SC_StringArray  labelValText;
        SC_StringArray  labelUnitText;
        double          maxLabHeight;
        double          lineMult;

    public:
                        PFO_DataLabelsGL(PlotDefC& assPlt);
                        PFO_DataLabelsGL(const PFO_DataLabelsGL& a, PlotDefC& assPlt);
                        ~PFO_DataLabelsGL();

        PFO_DataLabelsGL& operator= (const PFO_DataLabelsGL& a);

        //  do draw
        void            DrawPlotObject();

    private:
        void            InitCommon();
        bool            CheckTextSize(const char               inLab[],
                                            ExtendedLabelData& labData,
                                            ExtendedLabelData& maxLabData,
                                            double&            currWidth,
                                            double&            maxWidth);


};

#endif // !PFO_DATALABELSPS_H

