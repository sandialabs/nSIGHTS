///////////////////////////////////////////////////////////////////////////////////
//
// PD_3DXYZ.h
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
//      derived from PD_3D - defines 3D plot with lin/log axes
//
//      defines basic 3D plot
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PD_3DXYZ_H
#define PD_3DXYZ_H

#include <genClass/C_Common.h>
#include <genClass/C_Graphic.h>

#include <genPlotClass/PC_Axes.h>

#include <genPlotClass/PD_3D.h>


class PD_3DXYZ :  public PD_3D {
    public:
        LinLogAxis1D    xAxis;
        LinLogAxis1D    yAxis;
        LinLogAxis1D    zAxis;

        LinLogAxis1D    setxAxis;       // only used if axesHaveBeenSet is true
        LinLogAxis1D    setyAxis;
        LinLogAxis1D    setzAxis;

        // affects relative length of XYZ axes
        // assume x axis is of normalized length 1.0
        double          xyaxesLengthRatio;  // y axes length = xyaxesLengthRatio
        double          xzaxesLengthRatio;  // z axes length = xzaxesLengthRatio

    public:
                        PD_3DXYZ();
                        PD_3DXYZ(const PD_3DXYZ& a);
        virtual         ~PD_3DXYZ();

        PD_3DXYZ&   operator= (const PD_3DXYZ& a);

        void            ResetView();
        PC_ViewOpRec    GetPlotViewOps();
        Coord3D         GetNewViewCenter();

        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

        // axes setting virtuals
        void            SetViewFromAxes();    // sets view from current axes limits
        void            SetAxesAutoLimits();  // axes limits from plot objects if auto
        bool            SetAxesFromExtents();  // axes limits from plot objects, returns true if not default
        void            SetAxesFromView();  // sets axes to manual, limits to current
        void            RestoreSetAxes();   // returns to last settings from setAxes

        virtual void    GetAxesTypes(bool& xIsLinear,
                                     bool& yIsLinear,
                                     bool& zIsLinear);
    protected:
        void            FullCopy(const PD_3DXYZ& a);

    private:
        void            LocalCopy(const PD_3DXYZ& a);


};


#endif // !PD_3DXYZ_H

