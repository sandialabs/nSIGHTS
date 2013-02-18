///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYFourier.cpp
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
//      Does inverse and forward FFTs on Y component of XY data set
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>
#include <objDFO/DFO_XYFourier.h>

DFO_XYFourier::DFO_XYFourier() :
    DFO_XYBase("Fourier Transform")
{
    forwardFFT = true;
    scaleInverseY = true;
    inverseXScale = 1.0;
    InitCommon();
}

DFO_XYFourier::DFO_XYFourier(const DFO_XYFourier& a) :
    DFO_XYBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYFourier :: ~DFO_XYFourier ()
{
}

void DFO_XYFourier::InitCommon()
{
    DoStatusChk();
}

DFO_XYFourier& DFO_XYFourier::operator= (const DFO_XYFourier& a)
{
    if (&a != this)
    {
        DFO_XYBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYFourier::LocalCopy(const DFO_XYFourier& a)
{
    forwardFFT = a.forwardFFT;
    scaleInverseY = a.scaleInverseY;
    inverseXScale = a.inverseXScale;
}

void  DFO_XYFourier:: DoStatusChk()
{
    if (forwardFFT)
        outputXY.SetID("forward FFT");
    else
        outputXY.SetID("inverse FFT");

    DFO_XYBase::DoStatusChk();
    if (StatusNotOK())
        return;

    int npts = inputXY->Size();
    if (npts < 8)
    {
        SetObjErrMsg("insufficient data in input");
    }
    else if ((!forwardFFT) && (!IsPower2(npts)))
    {
        SetObjErrMsg("# of input pts must be power of 2");
    }

}

bool DFO_XYFourier::IsPower2(int npts)
{
    int n2 = 2;
    while (n2 != npts)
    {
        if (n2 > npts)
            return false;
        n2 += n2;
    }

    return true;
}


void DFO_XYFourier:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    const SC_DoubleArray& inY = inputXY->yData;
    SC_DoubleArray& outY = outputXY.yData;

    outY.SetResizable(inY.Size() * 2);
    int i;
    for (i = 0; i < inY.Size(); i++)
        if (!RealIsNull(inY[i]))
            outY += inY[i];

    if (outY.Size() < 2)
    {
        SetObjErrMsg("insufficient data after cleaning");
        outputXY.DeAlloc();
        return;
    }

    if ((!forwardFFT) && (!IsPower2(outY.Size())))
    {
        SetObjErrMsg("# of pts must be power of 2 after cleaning");
        return;
    }


    if (forwardFFT)
    {
        // set size to power of 2
        int n = outY.Size();
        int nFFT = 2;
        while (nFFT < n)
            nFFT += nFFT;
        for (i = n; i < nFFT; i++ )
            outY += 0.0;
    }



    SC_DoubleArray& outX = outputXY.xData;
    outX.AllocAndSetSize(outY.Size());

    double xScale = 1.0 / (double(outY.Size()) * fabs(inputXY->xData[1] - inputXY->xData[0]));
    for (i = 0; i < outX.Size(); i++)
        outX[i] = double (i) * xScale;

    RealFFT(outY, forwardFFT);

    if (!forwardFFT) 
    {

        double inverseYScale = 1.0;
        if (scaleInverseY)
            inverseYScale = 2.0 / outY.Size();

        for (i = 0; i < outY.Size(); i++)
        {
            outY[i] *= inverseYScale;
            outX[i] *= inverseXScale;
        }
    }


    CalcFinish();
}

