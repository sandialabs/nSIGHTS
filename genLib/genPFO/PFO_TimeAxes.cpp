///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TimeAxes.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <genPFO/PFO_TimeAxes.h>


PFO_TimeAxes :: PFO_TimeAxes(PlotDefC& assPlt)
    : PlotObjC("Time Axis", assPlt)
{
    timeSpecIsMaster = true;

    CopyString(arbitraryTimeStr, "00:00:00", dateTimeStrLen);
    CopyString(arbitraryDateStr, "01/01/2000", dateTimeStrLen);

    autoIncrements = true;

    minorIncrementUnits = atuDays;
    minorIncrementCount = 7;

    majorIncrementUnits = atuMonths;
    majorIncrementCount = 1;

    incrementLabelType = iltDate;

    timeFormat = DC_DateTime::tfHHMMSS24;
    CopyString(dateTimeDelimStr, "\\n", dateTimeStrLen);

    CopyString(axesLabel, "Time [days]", maxLabelLen);

    InitCommon();
}

PFO_TimeAxes::PFO_TimeAxes(const PFO_TimeAxes& a, PlotDefC& assPlt) :
    PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_TimeAxes :: ~PFO_TimeAxes ()
{
}

void PFO_TimeAxes::InitCommon()
{
    reportOK        = true;
    usePolyOffset   = true;

    AddMSInPort(timeSpecMasterObjRef, typeid(DO_DateTimeSpec), timeSpecIsMaster);


    DoStatusChk();
}

PFO_TimeAxes& PFO_TimeAxes::operator= (const PFO_TimeAxes& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

bool PFO_TimeAxes::ContainsLinesOnly()
{
    return true;
}


void  PFO_TimeAxes::LocalCopy(const PFO_TimeAxes& a)
{
    timeSpecIsMaster        = a.timeSpecIsMaster;
    timeSpecMasterObjRef    = a.timeSpecMasterObjRef;

    dateTimeSpec = a.dateTimeSpec;

    CopyString(arbitraryTimeStr, a.arbitraryTimeStr, dateTimeStrLen);
    CopyString(arbitraryDateStr, a.arbitraryDateStr, dateTimeStrLen);

    autoIncrements = a.autoIncrements;

    minorIncrementUnits = a.minorIncrementUnits;
    minorIncrementCount = a.minorIncrementCount;

    majorIncrementUnits = a.majorIncrementUnits;
    majorIncrementCount = a.majorIncrementCount;

    incrementLabelType = a.incrementLabelType;

    dateFormat = a.dateFormat;
    timeFormat = a.timeFormat;
    CopyString(dateTimeDelimStr, a.dateTimeDelimStr, dateTimeStrLen);

    CopyString(axesLabel, a.axesLabel, maxLabelLen);
}


void  PFO_TimeAxes:: DoStatusChk()
{
    // base class check
    PlotObjC::DoStatusChk();

    if ((!timeSpecIsMaster) && (CheckInputFO(timeSpecMasterObjRef, "Time/Date master spec")))
    {
        DO_DateTimeSpec*  inputDO = static_cast<DO_DateTimeSpec*>(GetInPortData(timeSpecMasterObjRef));
        currDateTimeSpec = *(inputDO->dateTimeSpec);
    }

    if (StatusNotOK())
        return;

    if (timeSpecIsMaster)
    {
        if (dateTimeSpec.timeSpecType == DC_DateTimeSpec::tstArbitraryZero)
        {
            DC_DateTimeConv temp;
            if (!temp.StringToDate(arbitraryDateStr))
                SetObjErrMsg("zero date is bad (must be MM/DD/YY)");
            if (!temp.StringToTime(arbitraryTimeStr))
                SetObjErrMsg("zero time is bad (must be HH:MM:SS)");
            dateTimeSpec.arbitraryZeroDT = temp;
        }
        currDateTimeSpec = dateTimeSpec;
    }

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);
    if (!xLin)
        SetObjErrMsg("X axes must be linear");


    Limit3D currLimits = GetCurrentViewLimits();
    double xstart = currLimits.minLim.cX;
    double xend = currLimits.maxLim.cX;

    if (xend <= xstart)
        SetObjErrMsg("X axes must be ascending");

}

void PFO_TimeAxes:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

