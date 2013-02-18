///////////////////////////////////////////////////////////////////////////////////
//
// PFO_SequenceGridLines.cpp
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
//      plot grid lines and labels at sequence intervals
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <nsClass/DO_SequenceTimes.h>

#include <nsPFO/PFO_SequenceGridLines.h>


PFO_SequenceGridLines :: PFO_SequenceGridLines(PlotDefC& assPlt)
    :PlotObjC("SequenceGridLines", assPlt)
{
    showEndLabel = true;
    labelFormat.labelType = PSC_GridLineLabelFormat::ltText;
    labelFormat.labelPosition = 80.0;
    InitCommon();
}

PFO_SequenceGridLines::PFO_SequenceGridLines(const PFO_SequenceGridLines& a, PlotDefC& assPlt)
    :PlotObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_SequenceGridLines::~PFO_SequenceGridLines ()
{
}

void PFO_SequenceGridLines::InitCommon()
{
    reportOK = false;
    AddInPort(sequenceTimesObjRef, typeid(DO_SequenceTimes));
    DoStatusChk();
}

PFO_SequenceGridLines& PFO_SequenceGridLines::operator= (const PFO_SequenceGridLines& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_SequenceGridLines::LocalCopy(const PFO_SequenceGridLines& a)
{
    sequenceTimesObjRef = a.sequenceTimesObjRef;
    gridLineSpec        = a.gridLineSpec;
    labelFormat         = a.labelFormat;
    labelFont           = a.labelFont;
    showEndLabel        = a.showEndLabel;
    timeFormat          = a.timeFormat;
}


void  PFO_SequenceGridLines:: DoStatusChk()
{
    sequenceTimesDC = 0;

    // base class check
    PlotObjC::DoStatusChk();

        // check input objects
    CheckInputFO(sequenceTimesObjRef, "Sequence times");
    if (StatusNotOK())
        return;

    sequenceTimesDC = static_cast<DO_SequenceTimes*>(GetInPortData(sequenceTimesObjRef))->seqTimes;
}


void PFO_SequenceGridLines:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
}

Limit3D PFO_SequenceGridLines::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK() || (! doPlot))
        return currLimits;

    bool xLin, yLin;
    GetAxesTypes(xLin, yLin);

    SC_DoubleArray& seqTimes =  sequenceTimesDC->sequenceStartTimes;

    if (xLin || (seqTimes[0] > stdEps))
        currLimits.minLim.cX = seqTimes[0];

    if (xLin || (seqTimes.LastIndex() > stdEps))
        currLimits.maxLim.cX = seqTimes.LastIndex();

    return currLimits;

}

