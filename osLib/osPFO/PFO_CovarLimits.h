///////////////////////////////////////////////////////////////////////////////////
//
// PFO_CovarLimits.h
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

#ifndef PFO_COVARLIMITS_H
#define PFO_COVARLIMITS_H

#include <genClass/C_Common.h>

#include <genClass/DO_XYData.h>

#include <objClass/DO_XYDataArray.h>

#include <osClass/DC_CovarArray.h>

#include <genPlotClass/PSC_ExtrusionSpec.h>
#include <genPlotClass/PSC_LegendOverride.h>

#include <genPlotClass/DO_SeriesLegendData.h>

#include <genPlotClass/C_PlotObj.h>


class PFO_CovarLimits : public PlotObjC  {

    public:
        enum    CovarPlotType       {cptSingle, cptDual, cptTriple};  // triple only for 3D
        enum    CovarPlotFormat     {cpfLine, cpfFill, cpfTube};      // 2D - single - line only
                                                                      //    - dual   - line/fill
                                                                      // 3D - single - line/tube
                                                                      // 3D - dual   - all
    protected:
        //  input objects
        FuncObjRef                  covarArrayObjRef;

        // properties
        int                         xDataIndx;
        int                         yDataIndx;
        int                         zDataIndx;

        CovarPlotType               covarPlotType;
        CovarPlotFormat             covarPlotFormat;
        DC_Covar::ConfidenceLimits  cLimits;


        int                         covarPen;      // for all
        int                         covarLineThk;  // for line
        PSC_TubeExtrusionSpec        tubeData;      // for tube

        //  dual ellipse desc
        int                         ndualPts;

        //  triple ellipse desc
        int                         ntripleSlice;
        int                         ntripleMeridian;

        PSC_LegendOverride           legendOverride;

        // plotting data
        DC_CovarArray*              covarArrayDC;
        DC_CovarHeader*             covarHeaderDC;


    private:
        // output objects
        DO_SeriesLegendData         seriesLegendData;    // output for legend

        DC_XYData                   covarXYDC; // 2D output of ellipse/cross points
        DO_XYData                   covarXYDO; // 2D output of ellipse/cross points

        DC_XYDataArray              covarXYArrayDC; // 2D array output of ellipse
        DO_XYDataArray              covarXYArrayDO;

    public:
                        PFO_CovarLimits(PlotDefC& assPlt);
                        PFO_CovarLimits(const PFO_CovarLimits& a, PlotDefC& assPlt);
                        ~PFO_CovarLimits();

        PFO_CovarLimits& operator= (const PFO_CovarLimits& a);

                        //  FuncObjC virtuals
        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

                        //  PlotObjC  draw virtuals in derived PFO_CovarLimitsGL
        Limit3D         GetPlotObjLimits();

    protected:
        bool            Get2DSingle(int indx,
                                    SC_PointArray& st,
                                    SC_PointArray& end);

        bool            Get2DDual(int indx,
                                    SC_PointArray& ellipse);

        bool            Get3DSingle(int indx,
                                    SC_CoordArray& st,
                                    SC_CoordArray& end);

        bool            Get3DDual(int            indx,
                                  Plane3D        dualPlane,
                                  SC_CoordArray& ell);

        bool            Get3DTriple(int indx,
                                     SC_CoordMatrix& ellipse);


    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_CovarLimits& a);  // common to copy constructor and = operator
};

#endif // !PFO_COVARLIMITS_H

