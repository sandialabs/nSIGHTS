///////////////////////////////////////////////////////////////////////////////////
//
// PSC_ContourSpec.h
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
//      define contouring of data
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PSC_CONTOURSPEC_H
#define PSC_CONTOURSPEC_H

#include <genPlotClass/C_PlotObj.h>

#include <genClass/DO_ContourSpec.h>
#include <genClass/DC_ContourSpec.h>

#include <genPlotClass/PSC_SeriesSpec.h>
#include <genPlotClass/PSC_LineSpec.h>
#include <genPlotClass/PSC_LegendOverride.h>
#include <genPlotClass/DO_SeriesLegendData.h>

class PSC_ContourSpec {
    public:
        enum            {maxContourIncs = 1000};

        //  input objects
        FuncObjRef          dataObjRef;           // data to plot, type unknown add in derived class

        FuncObjRef          specMasterObjRef;    // contour spec
        bool                specIsMaster;        // gets range limit from input

        bool                calcIncVal;
        bool                doLogInc;
        double              startVal;
        double              endVal;
        double              incVal;
        int                 nIncs;

        PSC_LineSpec        contourLineSpec;

        DC_ContourSpecArray  contourSpec;        // contour intervals, thick, and color

        PSC_LegendOverride   legendGC;

        bool                overrideMasterLineSpec;

    private:
        DO_SeriesLegendData legendOutputDO;
        DO_ContourSpec      contourSpecDO;


    public:
                            PSC_ContourSpec();
                            PSC_ContourSpec(const PSC_ContourSpec& a);
                            ~PSC_ContourSpec();

        PSC_ContourSpec&     operator= (const PSC_ContourSpec& a);

        void                DoStatusChk(PlotObjC&   inPO);

        void                InitCommon(FuncObjC& inFO);     // common to normal and copy constructor in derived class


    private:
        void            LocalCopy(const PSC_ContourSpec& a);  // common to copy constructor and = operator
};
#endif // !PSC_CONTOURSPEC_H

