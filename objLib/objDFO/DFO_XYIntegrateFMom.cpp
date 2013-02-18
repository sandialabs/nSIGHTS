///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYIntegrateFMom.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <genClass/SC_DoubleArray.h>
#include <objDFO/DFO_XYIntegrateFMom.h>

DFO_XYIntegrateFMom :: DFO_XYIntegrateFMom() : DFO_XYBase("First Moment")
{
    InitCommon();
}

DFO_XYIntegrateFMom::DFO_XYIntegrateFMom(const DFO_XYIntegrateFMom& a) : DFO_XYBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYIntegrateFMom :: ~DFO_XYIntegrateFMom ()
{
}

void DFO_XYIntegrateFMom::InitCommon()
{
    AddOutPort(integrateValDO);
    integrateValDO.Reset("Integrate total");
    AddOutPort(fmValDO);
    fmValDO.Reset("First Moment");
    AddOutPort(mrtValDO);
    mrtValDO.Reset("Mean Residence Time");

    DoStatusChk();
}

DFO_XYIntegrateFMom& DFO_XYIntegrateFMom::operator= (const DFO_XYIntegrateFMom& a)
{
    if (&a != this)
    {
        DFO_XYBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYIntegrateFMom::LocalCopy(const DFO_XYIntegrateFMom& a)
{
}

void  DFO_XYIntegrateFMom:: DoStatusChk()
{
    outputXY.SetID("FM Analysis");

    DFO_XYBase::DoStatusChk();
    if (StatusOK() && (inputXY->Size() < 2))
        SetObjErrMsg("insufficient data in input");

}


void DFO_XYIntegrateFMom:: CalcOutput(FOcalcType  calcType)
{
    integrateValDO.SetValueLabel(0.0);
    fmValDO.SetValueLabel(0.0);
    mrtValDO.SetValueLabel(0.0);
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

    double totArea = 0;
    double totFM = 0;
    double lastY = y[0];
    bool alwaysPos = true;  //might want to flip this switch sometime in the future

    for ( int i = 1; i < outputXY.Size(); i++)
    {
        double currY = y[i];
        double dx = fabs(x[i] - x[i - 1]);
        double avgX = (x[i] + x[i-1])/2;
        // check for zero crossing
        double area;
        double areaFM;
        if (((currY >= 0.0) && (lastY >= 0.0)) || ((currY < 0.0) && (lastY < 0.0)))
        {
            if(alwaysPos)
                currY = fabs(currY);
            area  = (lastY + currY) / 2.0 * dx;
            areaFM = area * avgX;
        }
        else
        {   
            if(alwaysPos)
                currY = fabs(currY);
            double dx0 = dx/(fabs(currY)+fabs(lastY))*fabs(lastY); //distance from x[i-1] to crossing point
            double dx1 = dx - dx0;
            double avgx0 = x[i]+(dx0*0.5);
            double avgx1 = x[i]+dx0+(dx1*0.5);
            area = 0.5 * (lastY * dx0 + currY * dx1); //triangles
            areaFM = 0.5 * (lastY * dx0 * avgx0 + currY * dx1 * avgx1);
        }

        totArea += area;
        totFM += areaFM;

        lastY = currY;
    }

    integrateValDO.InitLabelAndValue(totArea);
    fmValDO.InitLabelAndValue(totFM);
    mrtValDO.InitLabelAndValue(totFM/totArea);

    CalcFinish();
}

