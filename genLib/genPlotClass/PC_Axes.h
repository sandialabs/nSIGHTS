///////////////////////////////////////////////////////////////////////////////////
//
// PC_Axes.h
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
//      basic axes definition and derived class for Lin/Log axes.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PC_AXES_H
#define PC_AXES_H

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_RealFormat.h>

#include <genPlotClass/PSC_Font.h>


class PC_Axes {
    protected:

            static const double rAxesEps;


    public:
            enum        {maxIncAlloc = 100};
            enum AxesIncType {aitNone, aitTic, aitGrid};

            bool        axesAreLinear;

            bool        autoScaleAxes;      // set axes limits based on plot data

            AxesIncType axisMajorIncs;      // labelled increments
            bool        autoMajorIncs;      //  set based on range

            AxesIncType axisMinorIncs;      // unlabelled increments
            bool        autoMinorIncs;      // set based on major size

            bool        startAtFirst;       // if not auto incs

    public:
                        PC_Axes();
                        PC_Axes(const PC_Axes& a);

            PC_Axes&    operator=(const PC_Axes& a);

            bool        StartIsMajor(const double&  stVal,
                                     const double&  majIncVal);  // returns true if
                                                                 // stVal is multiple of inc
            void        MakeDifferent(double& axStart,  // fixes auto adjust
                                      double& axEnd,    // where start and end are the same
                                      bool    logAxes);

    protected:
        void            FullCopy(const PC_Axes& a);
        void            SetLinearMajorIncs(const double& axMin,
                                           const double& axMax,
                                           const double& incVal,
                                                 SC_DoubleArray& incVals);

        void            SetLinearMinorIncs(const double& majorIncVal,
                                                 int     nMinorPerMajor,
                                                 SC_DoubleArray& incVals);

                        // misc routines translated from pascal GPlib
        void            LinFirstInc(const double&   axStVal,
                                    const double&   axEndVal,
                                    const double&   axIncVal,
                                    double&         firstInc);

        static bool     InRange(const double&   axStVal,
                                const double&   axEndVal,
                                bool            reversed,
                                const double&   tVal);

        static void     SetIncVal(const double& setVal,
                                  bool          reversed,
                                  double&       axIncVal);

        bool            AutoLinIncs(const double&   axStVal,
                                    const double&   axEndVal,
                                    double&         firstInc,
                                    double&         axIncVal);

        static void     LinAdjust(const double& firstInc,
                                  const double& incVal,
                                        double& minVal,
                                        double& maxVal);

        static void     AutoLogIncs(const double&   axStVal,
                                    const double&   axEndVal,
                                    double&         firstInc,
                                    double&         axMultVal);

        static void     AutoLogAdjust(double& minVal,
                                      double& maxVal);


    private:
        void            LocalCopy(const PC_Axes& a);

};


class LinLogAxis1D : public PC_Axes {
    public:
            double          axisLinMin;           //  manual range
            double          axisLinMax;
            double          axisLogMin;
            double          axisLogMax;

            double          majorIncVal;        //  for linear manual only
            int             nminorPerMajor;     // for linear manual only

            SC_RealFormat   axisIncFormat;      // format for axis labels
            SC_RealFormat   cursorReportFormat; // format for cursor reporting

            PSC_Font        incrFont;   //font rotation only

    public:
                            LinLogAxis1D();
                            LinLogAxis1D(const LinLogAxis1D& a);

            LinLogAxis1D&  operator= (const LinLogAxis1D& a);

                            ~LinLogAxis1D() {}

            //  auto adjust
            void            AdjustAxisLimits();
                                        // if autoScaleAxes then adjust axes limits
                                        // for best presentation

            bool            SetMajorIncs();
                                        // returns true if autoMajor and origin and
                                        // first increment pos are the same
                                        // will be the same

            void            SetMinorIncs();     // resets minor if autoMinorincs


            void            SetAxisLimits(const double& minVal,
                                          const double& maxVal);

            void            GetAxisLimits(double& minVal,
                                          double& maxVal);


            void            GetAxesMajorIncs(SC_DoubleArray& incVals);
            void            GetAxesMinorIncs(SC_DoubleArray& incVals);

            //  to help with pixel->user conversion
            bool            InAxisLimits(const double& inVal);
            double          AxisSpanValue(const double& userVal);  // ratio on axis
            double          AxisUserValue(const double& spanVal);

    protected:
            void            FullCopy(const LinLogAxis1D& a);
    private:
            void            LocalCopy(const LinLogAxis1D& a);
};


#endif // !PC_AXES_H

