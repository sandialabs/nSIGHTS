///////////////////////////////////////////////////////////////////////////////////
//
// PD_2DXYPS.h
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
//      merges OGL_2DXY with PD_2DXY.  Implements axes drawing routines.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PD_2DXYPS_H
#define PD_2DXYPS_H

#include <genPlotClass/PD_2DXY.h>
#include <genPlotClassPS/OGL_2DXY.h>


class PD_2DXYPS : public PD_2DXY, public OGL2DXY {
    private:
        double          convXaxisMin;
        double          convXaxisMax;
        double          convYaxisMin;
        double          convYaxisMax;

    public:
                        PD_2DXYPS();
                        PD_2DXYPS(const PD_2DXYPS& a);

                        ~PD_2DXYPS();

        PD_2DXYPS&      operator= (const PD_2DXYPS& a);

                        // PlotDefC virtuals
        void            DrawPlotAxes();
        void            ResetSize();
        OGLBase*        GetOGLBase();



        void            InitRenderer();        // binds context, clears screen
        void            SetupDataView();       // sets axes limits and view rotations/translations
        void            StartLineSection();    // all poly objects drawn, start line only objects
        void            EndLineSection();
        void            SetupAnnoView();       // changes to 0-100 system for annotation
        void            CloseRenderer();       // flushes

        virtual void    SetClipPlanes();        // OGL Base virtual

        // for use by supplementary axes drawing routines
        void            DrawXAxes(const double& yVal,
                                  const double& yaxOffset);

        void            DrawXTic(const double& xVal,
                                 const double& yVal,
                                 const double& yaxOffset,
                                 const double& ticLen,
                                 const double& ticOffset);

        void            DrawXGrid(const double& xVal,
                                  const double& yminOffset,
                                  const double& ymaxOffset,
                                  const double& ticOffset);

        void            DrawYAxes(const double& xVal,
                                  const double& xaxOffset);

        void            DrawYTic(const double& xVal,
                                 const double& yVal,
                                 const double& xaxOffset,
                                 const double& ticLen,
                                 const double& ticOffset);

        void            DrawYGrid(const double& yVal,
                                  const double& xminOffset,
                                  const double& xmaxOffset,
                                  const double& ticOffset);

        void            SetAxesLocals();

    protected:
        void            FullCopy(const PD_2DXYPS& a);

    private:
        void            InitCommon();  // common to normal and copy constructor
        void            LocalCopy(const PD_2DXYPS& a);

        static double   GetMinorIncVal(const SC_DoubleArray& majorVals,
                                       const SC_DoubleArray& minorVals,
                                             int             majorIndex,
                                             int             minorIndex,
                                             bool            isLog,
                                             bool            isReversed);


};


#endif // !PD_2DXYPS_H

