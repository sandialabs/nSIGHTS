///////////////////////////////////////////////////////////////////////////////////
//
// DC_XYData.cpp
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
// DESCRIPTION: Platform independent and application independent base classes and
//              support classes.
//              Data Classes (DC) that define basic types of data used within the
//              framework: reads/writes DC_XYData in a variety of formats.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>

#include <genClass/U_Real.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/DC_XYData.h>

DC_XYData :: DC_XYData()
{
    SetToNull(dataID);
    xData.SetResizable(10);
    yData.SetResizable(10);
}

DC_XYData::DC_XYData(const DC_XYData& a)
    : xData(a.xData), yData(a.yData)
{
    CopyString(dataID, a.dataID, dataIDLen);
}

DC_XYData :: ~DC_XYData()
{
    DeAlloc();
}

DC_XYData&  DC_XYData::operator=(const DC_XYData& a)
{
    if (&a != this)
        {
            xData = a.xData;
            yData = a.yData;
            CopyString(dataID, a.dataID, dataIDLen);
        }
    return *this;
}


void DC_XYData :: DeAlloc()
{
    xData.DeAlloc();
    yData.DeAlloc();
}

bool DC_XYData::CreateFrom(const SC_DoubleArray& inX,
                           const SC_DoubleArray& inY)
{
    try
        {
            if (xData.Size() != yData.Size())
                {
                    int maxSize = xData.Size();
                    if (yData.Size() > maxSize)
                        maxSize = yData.Size();
                    if (maxSize < 10)
                        maxSize = 10;
                    xData.Alloc(maxSize);
                    yData.Alloc(maxSize);
                }

            xData = inX;
            yData = inY;
        }
    catch (SC_Array::AllocationError& )
        {
            DeAlloc();
            return false;
        }

    CleanUp();
    return ! IsEmpty();
}


bool DC_XYData::Alloc(int nRow)
{
    try
        {
            xData.Alloc(nRow);
            yData.Alloc(nRow);
        }
    catch (SC_Array::AllocationError& )
        {
            DeAlloc();
            return false;
        }

    return true;
}

void DC_XYData::SetID(const char id[])
{
    CopyString(dataID, id, dataIDLen);
}

void DC_XYData::GetID(char id[],
                      int  maxIDlen) const
{
    CopyString(id, dataID, maxIDlen);
}


void DC_XYData::CleanUp()
{
    int nOK = 0;
    for (int i = 0; i < xData.Size(); i++)
        {
            if (RealIsNull(xData[i]) || RealIsNull(yData[i]))
                continue;

            if (nOK < i)
                {
                    xData[nOK] = xData[i];
                    yData[nOK] = yData[i];
                }
            nOK++;
        }
    SetSize(nOK);
}

void DC_XYData::RemoveXDuplicates(double xEps)
{
    if (Size() < 2)
        return;

    int nOK = 0;

    // at beginning remove first
    int start = 1;
    if (RealsAreSame(xData[0], xData[1], xEps))
        {
            xData[0] = xData[1];
            yData[0] = yData[1];
            start = 2;
        }

    for (int i = start; i < xData.Size(); i++)
        if (!RealsAreSame(xData[nOK], xData[i], xEps))
            {
                nOK++;
                xData[nOK] = xData[i];
                yData[nOK] = yData[i];
            }

    nOK++;
    SetSize(nOK);
}

bool DC_XYData::SetNDX(SC_SetupErr& err,
                       SC_DoubleArray& ndxData)const
{
    int n = 0;
    for (int i = 0; i < Size(); i++)
        {
            double x = xData[i];
            double y = yData[i];
            if (RealIsNull(x) || RealIsNull(y))
                {
                    err.SetConstantError("all x and y must be non null");
                    return false;
                }

            if ((x < 0.5) || (x > 999))
                {
                    err.SetConstantError("all x values must be integers > 0 and < 1000");
                    return false;
                }

            if (y < 1.0E-5)
                {
                    err.SetConstantError("all y values must be > 1E-5");
                    return false;
                }
            n += int(x);
        }

    if (n > 10000)
        {
            err.SetConstantError("total number of x/y ndx calculated nodes points be < 10000");
            return false;
        }

    ndxData.Alloc(n);
    for (int i = 0; i < Size(); i++)
        {
            n = int(xData[i]);
            double y = yData[i];
            for (int j = 0; j < n; j++)
                ndxData += y;
        }
    return true;

}



bool DC_XYData::IsClean() const
{
    for (int i = 0; i < Size(); i++)
        if (RealIsNull(xData[i]) || RealIsNull(yData[i]))
            return false;

    return true;
}


bool DC_XYData::Integrate(DC_XYData& integratedXY) const
{
    if (Size() < 2)
        return false;

    integratedXY.CreateFrom(xData, yData);
    if (integratedXY.Size() < 2)
        return false;

    SC_DoubleArray& x = integratedXY.xData;
    SC_DoubleArray& y = integratedXY.yData;

    double lastY = y[0];
    y[0] = 0.0;
    for ( int i = 1; i < integratedXY.Size(); i++)
        {
            double currY = y[i];
            double dx = fabs(x[i] - x[i - 1]);
            // check for zero crossing
            double area;
            if (((currY >= 0.0) && (lastY >= 0.0)) || ((currY < 0.0) && (lastY < 0.0)))
                {
                    area  = (lastY + currY) / 2.0 * dx;
                }
            else
                {
                    double dx1 = - lastY * dx / (currY - lastY);
                    area = (lastY * dx1 + currY * (dx - dx1)) / 2.0;
                }

            y[i] = y[i - 1] + area;
            lastY = currY;
        }

    return true;
}

