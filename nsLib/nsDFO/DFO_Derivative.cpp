///////////////////////////////////////////////////////////////////////////////////
//
// DFO_Derivative.cpp
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
//      Calculate derivative of input data
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <nsClass/DO_Derivative.h>
#include <nsClass/DO_TimeProcess.h>

#include <nsDFO/DFO_Derivative.h>


DFO_Derivative :: DFO_Derivative() :
    FuncObjC("Derivative"),
    windowPctSpanMS("Window % span"),
    windowValSpanMS("Window value span")
{
    isMaster = true;
    windowPctSpanMS.enteredRealValue = derivativeDC.windowPctSpan;
    windowValSpanMS.enteredRealValue = derivativeDC.windowValSpan;
    InitCommon();
}

DFO_Derivative::DFO_Derivative(const DFO_Derivative& a) :
    FuncObjC(a),
    windowPctSpanMS(a.windowPctSpanMS),
    windowValSpanMS(a.windowValSpanMS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_Derivative :: ~DFO_Derivative()
{
}

void DFO_Derivative::InitCommon()
{
    AddInPort(inputTPObjRef, typeid(DO_XYData));
    AddMSInPort(masterObjRef, typeid(DO_Derivative), isMaster);

    AddInPort(timeProcessObjRef, typeid(DO_TimeProcess));
    useDerivativeCheckRef = GetLastInPortCheckRef();

    windowPctSpanMS.AddPort(*this);
    windowPctSpanCheckRef = GetLastInPortCheckRef();

    windowValSpanMS.AddPort(*this);
    windowValSpanCheckRef = GetLastInPortCheckRef();

    AddOutPort(derivativeDO);
    derivativeDO.derivative = &derivativeDC;

    AddOutPort(outputTPDO);
    outputTPDO.xyData = &outputTPDC;

    DoStatusChk();
}

DFO_Derivative& DFO_Derivative::operator= (const DFO_Derivative& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}

void  DFO_Derivative::LocalCopy(const DFO_Derivative& a)
{
    inputTPObjRef   = a.inputTPObjRef;
    isMaster        = a.isMaster;
    masterObjRef    = a.masterObjRef;

    derivativeDC        = a.derivativeDC;
    timeProcessObjRef   = a.timeProcessObjRef;
    windowPctSpanMS     = a.windowPctSpanMS;
    windowValSpanMS     = a.windowValSpanMS;
}


void DFO_Derivative::SetDefault()
{
}

void  DFO_Derivative:: DoStatusChk()
{
    // set flags for data to use
    *useDerivativeCheckRef = isMaster && derivativeDC.useSuperTime;

    *windowPctSpanCheckRef = isMaster &&
                                 ((derivativeDC.derivativeCalcType == dctLogPctWindow) ||
                                  (derivativeDC.derivativeCalcType == dctLinPctWindow));
    *windowValSpanCheckRef = isMaster &&
                                 ((derivativeDC.derivativeCalcType == dctLogValWindow) ||
                                  (derivativeDC.derivativeCalcType == dctLinValWindow));

    FuncObjC::DoStatusChk();
    outputTPDC.SetID("derivative");

    if (CheckInputFO(inputTPObjRef, "Input P(t)"))
        tpDataDC = static_cast<DO_XYData*>(GetInPortData(inputTPObjRef))->xyData;

    derivativeDO.timeProcess = 0;
    if ((!isMaster) && (CheckInputFO(masterObjRef, "Master spec")))
    {
        DO_Derivative*  inputDO = static_cast<DO_Derivative*>(GetInPortData(masterObjRef));
        derivativeDC = *(inputDO->derivative);
        windowPctSpanMS.enteredRealValue = derivativeDC.windowPctSpan;
        windowValSpanMS.enteredRealValue = derivativeDC.windowValSpan;
        derivativeDO.timeProcess = inputDO->timeProcess;
    }

    if (isMaster)
    {
        //  get data
        if (derivativeDC.useSuperTime && CheckInputFO(timeProcessObjRef, "Input time process"))
            derivativeDO.timeProcess = static_cast<DO_TimeProcess*>(GetInPortData(timeProcessObjRef))->timeProcess;

        if (*windowPctSpanCheckRef)
        {
            windowPctSpanMS.DoCheck("Window % lin/log span", *this);
            derivativeDC.windowPctSpan = windowPctSpanMS.GetLimitedValue(1.0, 85.0);
        }

        if (*windowValSpanCheckRef)
        {
            windowValSpanMS.DoCheck("Window val lin/log span", *this);
            derivativeDC.windowValSpan = windowValSpanMS.GetLimitedValue(0.001, 1000.0);
        }
    }
}

void DFO_Derivative:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    // create time process if master
    SC_SetupErr errData;
    if (isMaster)
    {
        //  doesn't actually do anything yet ....
        if (!derivativeDC.CreateDerivative(errData))
        {
            SetObjErrMsg(errData.GetErrorText());
            return;
        }
    }

    outputTPDC.SetEmpty();

    bool derivOK;
    if (derivativeDC.useSuperTime)
        derivOK = derivativeDC.GetDerivatives(*tpDataDC, *derivativeDO.timeProcess, outputTPDC, errData);
    else
        derivOK = derivativeDC.GetDerivatives(*tpDataDC, outputTPDC, errData);

    if (derivOK)
        outputTPDC.SetID(GetID());
/*  else
    {
        char debugMsg[120]; 
        MakeString(debugMsg, "DFO_Derivative::", errData.GetErrorText(), "::", "empty after processing", 120);
        SetObjErrMsg(debugMsg);
    }
*/

}

