///////////////////////////////////////////////////////////////////////////////////
//
// PD_2DXY.h
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
//      derived from PD_2D - defines 2D plot with lin/log axes
//
//      defines basic 2D plot
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PD_2DXY_H
#define PD_2DXY_H

#include <genClass/C_Common.h>
#include <genClass/C_Graphic.h>

#include <genPlotClass/PC_Axes.h>

#include <genPlotClass/PD_2D.h>


class PD_2DXY : public PD_2D {
    public:
        LinLogAxis1D    xAxis;
        LinLogAxis1D    yAxis;
        LinLogAxis1D    setxAxis;       // only used if axesHaveBeenSet is true
        LinLogAxis1D    setyAxis;

    public:
                        PD_2DXY();
                        PD_2DXY(const PD_2DXY& a);
                        ~PD_2DXY();

        PD_2DXY&        operator= (const PD_2DXY& a);

        void            ResetView();
        PC_ViewOpRec    GetPlotViewOps();

        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

        void            SetViewFromAxes();    // sets view from current axes limits
        void            SetAxesAutoLimits();  // axes limits from plot objects if auto
        bool            SetAxesFromExtents();  // axes limits from plot objects, returns true if not default
        void            SetAxesFromView();  // sets axes to manual, limits to current
        void            RestoreSetAxes();   // returns to last settings from setAxes

        //  user to pixel conversion from PD_2D
        bool            UserXYtoPixelXY(const Point2D& userPt,
                                        int&     userPixX,
                                        int&     userPixY);

        bool            PixelXYtoUserXY(int      userPixX,
                                        int      userPixY,
                                        Point2D& userPt);

        virtual void    GetAxesTypes(bool& xIsLinear,
                                     bool& yIsLinear,
                                     bool& zIsLinear);

        void            SetIncrFont();  //establishes separate increment
                                        //fonts for x and y axes

    protected:
        void            FullCopy(const PD_2DXY& a);

    private:
        void            InitCommon();  // common to normal and copy constructor
        void            LocalCopy(const PD_2DXY& a);
};


#endif // !PD_2DXY_H

