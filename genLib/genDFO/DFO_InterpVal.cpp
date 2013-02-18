///////////////////////////////////////////////////////////////////////////////////
//
// DFO_InterpVal.cpp
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
// DESCRIPTION: Basic functional objects (all derived from FuncObjC)
//              used by all applications, primarily concerned with
//              defining/operating on pen sets and colour maps: Also
//              includes base classes for common operations with
//              different application specific implementations:
//              Interpolates a Y value from XY input given an X value.
//              Various options are available (linear, log, nearest, etc)
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genDFO/DFO_InterpVal.h>


DFO_InterpVal :: DFO_InterpVal(const char* inID) :
    FuncObjC(inID),
    interpXValueMS("Interp X value")
{
    interpMethod    = imLinear;
    logX            = false;
    logY            = false;

    InitCommon();
}

DFO_InterpVal::DFO_InterpVal(const DFO_InterpVal& a) :
    FuncObjC(a),
    interpXValueMS(a.interpXValueMS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_InterpVal :: ~DFO_InterpVal ()
{
}

void DFO_InterpVal::InitCommon()
{
    interpXValueMS.AddPort(*this);
    AddOutPort(outputRealDO);
}

DFO_InterpVal& DFO_InterpVal::operator= (const DFO_InterpVal& a)
{
    if (&a != this)
        {
            FuncObjC::operator=(a);
            interpXValueMS = a.interpXValueMS;
            LocalCopy(a);
        }
    return *this;
}


void  DFO_InterpVal::LocalCopy(const DFO_InterpVal& a)
{
    interpMethod    = a.interpMethod;
    logX            = a.logX;
    logY            = a.logY;
}

void  DFO_InterpVal:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    outputRealDO.Reset("Interpolated");
    interpXValueMS.DoCheck("interp X value master", *this);
    interpX = interpXValueMS.GetCurrentValue();
    interpXValueMS.InitLabelAndValue(interpX);
    if (logX && (!IsPositive(interpX)))
        SetObjErrMsg("interp X must be +ve");
}


void DFO_InterpVal:: DoInterpVal(const SC_DoubleArray& xData,
                                 const SC_DoubleArray& yData)

{
    if (xData.IsEmpty())
        {
            SetObjErrMsg("no X data ");
            return;
        }

    if (xData.Size() != yData.Size())
        {
            SetObjErrMsg("X/Y data size mismatch ?");
            return;
        }

    if (interpX <= xData[0])
        {
            outputRealDO.InitLabelAndValue(yData[0]);
            return;
        }
    if (interpX >= xData.LastIndex())
        {
            outputRealDO.InitLabelAndValue(yData.LastIndex());
            return;
        }

    if (logX)
        interpX = Lgt(interpX);

    for (int i = 0; i < xData.UpperBound(); i++)
        {
            double x1 = xData[i];
            double x2 = xData[i + 1];
            if (logX)
                {
                    if (!(IsPositive(x1) && IsPositive(x2)))
                        continue;
                    x1 = Lgt(x1);
                    x2 = Lgt(x2);
                }

            if (interpX < x1)
                {
                    SetObjErrMsg("X data non-ascending");
                    return;
                }

            if (interpX >= x2)
                continue;

            double y1 = yData[i];
            double y2 = yData[i + 1];

            double yout;
            switch (interpMethod) {
            case imLinear : {
                if (logY)
                    {
                        if (!(IsPositive(y1) && IsPositive(y2)))
                            {
                                SetObjErrMsg("Y not +ve in X span");
                                return;
                            }
                        y1 = Lgt(y1);
                        y2 = Lgt(y2);
                    }

                yout = (y2 - y1) / (x2 - x1) * (interpX - x1) + y1;
                if (logY)
                    yout = InvLgt(yout);
                break;
            }
            case imPrevious : {
                yout = y1;
                break;
            }
            case imNext : {
                yout = y2;
                break;
            }
            case imClosest : {
                if (fabs(x1 - interpX) < fabs(x2 - interpX))
                    yout = y1;
                else
                    yout = y2;
                break;
            }
            }
            outputRealDO.InitLabelAndValue(yout);
            return;
        }

    SetObjErrMsg("X span not found ??");
}

