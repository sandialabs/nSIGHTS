///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TimeAxes.h
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
//      Basic plotting objects used by all applications, primarily concerned with plot annotation:
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_TIMEAXES_H
#define PFO_TIMEAXES_H

#include <genClass/C_Graphic.h>

#include <genClass/DC_DateTime.h>
#include <genClass/DO_DateTimeSpec.h>

#include <genPlotClass/C_PlotObj.h>


class PFO_TimeAxes : public PlotObjC  {
    public:
        enum                        {maxLabelLen = 60};
        enum                        {dateTimeStrLen = 20};

        enum    AxesTimeUnits       {atuYears, atuQuarters, atuMonths, atuDays, atuHours, atuMinutes, atuSeconds};
        enum    IncrementLabelType  {iltDate, iltTime, iltTimeDate, iltDateTime};

    protected:

        //  axes time mapping
        bool                        timeSpecIsMaster;           // spec is master
        FuncObjRef                  timeSpecMasterObjRef;       // master spec
        DC_DateTimeSpec             dateTimeSpec;

        char                        arbitraryTimeStr[dateTimeStrLen];
        char                        arbitraryDateStr[dateTimeStrLen];

        bool                        autoIncrements;
        AxesTimeUnits               minorIncrementUnits;
        int                         minorIncrementCount;

        AxesTimeUnits               majorIncrementUnits;
        int                         majorIncrementCount;

        IncrementLabelType          incrementLabelType;
        DC_DateFormat               dateFormat;
        DC_DateTime::TimeFormat     timeFormat;
        char                        dateTimeDelimStr[dateTimeStrLen];


        char                        axesLabel[maxLabelLen];

        DC_DateTimeSpec             currDateTimeSpec;
    public:
                        PFO_TimeAxes(PlotDefC& assPlt);
                        PFO_TimeAxes(const PFO_TimeAxes& a, PlotDefC& assPlt);
                        ~PFO_TimeAxes();

        PFO_TimeAxes&   operator= (const PFO_TimeAxes& a);

                        //  FuncObjC virtuals
        virtual void    DoStatusChk();
        virtual void    CalcOutput(FOcalcType  calcType);

                        //  PlotObjC  draw virtuals in derived PFO_TimeAxesGL
        virtual bool    ContainsLinesOnly();


    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_TimeAxes& a);  // common to copy constructor and = operator
};

#endif // !PFO_TIMEAXES_H

