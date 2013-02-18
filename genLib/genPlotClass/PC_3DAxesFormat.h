///////////////////////////////////////////////////////////////////////////////////
//
// PC_3DAxesFormat.h
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
//      defines axes/increment layout for 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PC_AXESFORMAT_H
#define PC_AXESFORMAT_H

#include <genClass/C_Common.h>
#include <genClass/C_Graphic.h>
#include <genPlotClass/PC_Axes.h>

class PC_AxesFormat {

    public:
            PC_Axes::AxesIncType    axesMajorInc;
            PC_Axes::AxesIncType    axesMinorInc;

            enum AxesLinePos        {alpNone, alpNeg, alpPos, alpBoth};
            AxesLinePos             axesLinePos;

            enum AxesTicPos         {atpNeg, atpPos, atpBoth};
            AxesTicPos              axesTicPos;

    public:
                                PC_AxesFormat();
                                PC_AxesFormat(const PC_AxesFormat& a);
            PC_AxesFormat&      operator= (const PC_AxesFormat& a);

            void                SetWorkingCopy(PC_AxesFormat& wCopy,
                                               bool           doNeg) const;  // sets plp and plt to both if to be plotted

    private:
            void                LocalCopy(const PC_AxesFormat& a);
};



class PC_3DAxesFormat {

    public:
            enum                {naxes3DPlanes = 6};
                                                  // viewing from Az -30, El 30
            enum Axes3DPlanes   {a3pXYp = 0, a3pXYn = 1,  // XY+ - top plane, XY- - bottom plane,
                                 a3pXZp = 2, a3pXZn = 3,  // XZ+ - far plane, XZ- - near plane,
                                 a3pYZp = 4, a3pYZn = 5};  // YZ+ - right plane, YZ- -left plane,

            PC_AxesFormat       axesUFormat[naxes3DPlanes];
            PC_AxesFormat       axesVFormat[naxes3DPlanes];

            bool                autoFormat;

                                // pens
            int                 axesLinePen;
            int                 majorPen;
            int                 minorPen;

                                //  line widths (pixels)
            int                 axesLineWidth;
            int                 majorTicWidth;
            int                 minorTicWidth;
            int                 majorGridWidth;
            int                 minorGridWidth;

                                // grid line types - added Mar 06
            PlotLineType        majorGridLineType;
            PlotLineType        minorGridLineType;

                                //  miscellania
            int                 axesOffset;
            int                 majorTicLength;
            int                 minorTicLength;

    public:
                                PC_3DAxesFormat();
                                PC_3DAxesFormat(const PC_3DAxesFormat& a);
            PC_3DAxesFormat&    operator= (const PC_3DAxesFormat& a);

            void                SetDefault();
            void                Clear();

            static Plane3D      GetPlane(Axes3DPlanes axIn);
    private:
            void                LocalCopy(const PC_3DAxesFormat& a);

};


#endif // !PC_AXESFORMAT_H

