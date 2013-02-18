///////////////////////////////////////////////////////////////////////////////////
//
// PFO_CovarLimitsGL.h
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
//      Plots confidence limits of 2D or 3D plots.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_COVARLIMITSGL_H
#define PFO_COVARLIMITSGL_H

#include <genPFOGL/GL_Base.h>
#include <genPFOGL/GL_Polygon.h>

#include <genPFOGL/OGLBaseObj.h>
#include <osPFO/PFO_CovarLimits.h>


class PFO_CovarLimitsGL : public PFO_CovarLimits, private OGLBaseObj  {
    private:
        GL_Polygon  covarPoly;

    public:
                        PFO_CovarLimitsGL(PlotDefC& assPlt);
                        PFO_CovarLimitsGL(const PFO_CovarLimitsGL& a, PlotDefC& assPlt);
                        ~PFO_CovarLimitsGL();

        PFO_CovarLimitsGL& operator= (const PFO_CovarLimitsGL& a);
        //  do draw
        void            DrawPlotObject();
    private:
        void            Draw2D();
        void            Draw3D();
        void            Draw3DEllipse(const SC_CoordArray& ell);
        void            Draw3DEllipsoid(const SC_CoordMatrix& ell);


};

#endif // !PFO_COVARLIMITSGL_H

