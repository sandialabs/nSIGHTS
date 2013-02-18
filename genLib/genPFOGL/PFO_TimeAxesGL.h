///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TimeAxesGL.h
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

#ifndef PFO_TimeAxesGL_H
#define PFO_TimeAxesGL_H

#include <genPFOGL/OGLBaseObj.h>

#include <genPFO/PFO_TimeAxes.h>


class PFO_TimeAxesGL : public PFO_TimeAxes, private OGLBaseObj  {
    private:
        DC_DateTime::TimeUnits  currAxesUnits;
        double                  zeroTime;
        double                  xstart;
        double                  xend;
        DC_DateTime             startTime;
        DC_DateTime             endTime;

        AxesTimeUnits           useMinorIncrementUnits;
        int                     useMinorIncrementCount;
        AxesTimeUnits           useMajorIncrementUnits;
        int                     useMajorIncrementCount;


    public:
                        PFO_TimeAxesGL(PlotDefC& assPlt);
                        PFO_TimeAxesGL(const PFO_TimeAxesGL& a, PlotDefC& assPlt);
                        ~PFO_TimeAxesGL();

        PFO_TimeAxesGL& operator= (const PFO_TimeAxesGL& a);
        //  do draw
        virtual void    DrawPlotObject();
        virtual void    ReportCursorData(const Coord3D& cursorPos);

    private:
        void            InitCommon(PlotDefC& assPlt);

        bool            AxesSetup();
        void            DoMinorIncrements();
        int             CountIncrements(AxesTimeUnits incrementType,
                                        int           incrementCount);

        DC_DateTime     SetIncStart(AxesTimeUnits incType,
                                    int           incrementCount);
        void            GetNextInc(DC_DateTime& nextInc,
                                   AxesTimeUnits incType,
                                   int           incCount);

        double          GetIncVal(const DC_DateTime& nextInc);

        void            SetAutoIncrements();
};

#endif // !PFO_TimeAxesGL_H

