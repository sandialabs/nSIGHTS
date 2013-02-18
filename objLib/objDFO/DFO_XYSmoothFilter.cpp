///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYSmoothFilter.cpp
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
//      applies smoothing and FFT based filtering operations to a data set.
//
///////////////////////////////////////////////////////////////////////////////////



#include <genClass/U_Real.h>
#include <genClass/SC_DoubleArray.h>
#include <objDFO/DFO_XYSmoothFilter.h>

DFO_XYSmoothFilter::DFO_XYSmoothFilter() :
    DFO_XYBase("Smooth/Filter")
{
    smoothFilterOp =  sfMedian;
    ptsInHalfWindow = 2;
    InitCommon();
}

DFO_XYSmoothFilter::DFO_XYSmoothFilter(const DFO_XYSmoothFilter& a) :
    DFO_XYBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYSmoothFilter :: ~DFO_XYSmoothFilter ()
{
}

void DFO_XYSmoothFilter::InitCommon()
{
    DoStatusChk();
}

DFO_XYSmoothFilter& DFO_XYSmoothFilter::operator= (const DFO_XYSmoothFilter& a)
{
    if (&a != this)
    {
        DFO_XYBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYSmoothFilter::LocalCopy(const DFO_XYSmoothFilter& a)
{
    smoothFilterOp = a.smoothFilterOp;
    ptsInHalfWindow = a.ptsInHalfWindow;
}

void  DFO_XYSmoothFilter:: DoStatusChk()
{
    outputXY.SetID("filtered");
    DFO_XYBase::DoStatusChk();
    if (StatusOK() && (inputXY->Size() < 2))
    {
        SetObjErrMsg("insufficient data in input");
    }
}


void DFO_XYSmoothFilter:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    // basic copy & clean
    outputXY.CreateFrom(inputXY->xData, inputXY->yData);
    if (outputXY.Size() < 2)
    {
        SetObjErrMsg("insufficient data after cleaning");
        return;
    }

    SC_DoubleArray& x = outputXY.xData;
    SC_DoubleArray& y = outputXY.yData;
    switch (smoothFilterOp)
    {
        case sfMedian: {
            do_median_smooth(y, outputXY.Size(), ptsInHalfWindow);
            break;
        }
        case sfMean: {
            do_mean_smooth(y, outputXY.Size(), ptsInHalfWindow);
            break;
        }
    }

    CalcFinish();
}

bool DFO_XYSmoothFilter::do_mean_smooth(SC_DoubleArray& data, int n, int n_win)
{
    //this smoother is a central window -- the median smoother appears
    //to be backward looking?

    int j, k, win_start, win_end, count;
    double tot;

    SC_DoubleArray temp_data;  // zero based arrays
    temp_data.AllocAndSetSize(n);

    for(j=0;j<n;j++)
    {
        if (j <= n_win)
            win_start = 0;
        else
            win_start = j - n_win;

        if ((j + n_win) >= n)
            win_end = n - 1;
        else
            win_end = j + n_win;

        tot = 0;
        count = 0;
        for(k=win_start;k<=win_end;k++) {
            tot += data[k];
            count++;
        }
        tot /= count;
        temp_data[j] = tot;

    }
    for(j=0;j<n;j++)
        data[j] = temp_data[j];

    return true;
}

bool DFO_XYSmoothFilter::do_median_smooth(SC_DoubleArray& data, int n, int n_win)
{
    int j, k, win_start, n_copy;
    SC_DoubleArray win, temp_data;  // zero based arrays

    n_copy = n_win + n_win + 1;
    win.AllocAndSetSize(n_copy);

    temp_data.AllocAndSetSize(n);

    // brute force
    for(j=0;j<n;j++)
    {
        if (j <= n_win)
            win_start = 1;
        else
        {
            win_start = j - n_win;
            if ((win_start + n_copy) >= n)
                win_start = n - n_copy + 1;
        }
        win_start = win_start - 1;
        for(k=0;k<n_copy;k++)
            win[k] = data[win_start + k];
        win.Sort(true);
        temp_data[j] = win[n_win + 1];
    }
    for(j=0;j<n;j++)
        data[j] = temp_data[j];

    return true;
}



