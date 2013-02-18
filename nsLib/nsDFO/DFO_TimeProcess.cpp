///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TimeProcess.cpp
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

#include <genClass/U_Str.h>
#include <nsClass/DO_TimeProcess.h>

#include <nsDFO/DFO_TimeProcess.h>


DFO_TimeProcess :: DFO_TimeProcess() :
    FuncObjC("CreateTimeProcess"),
    constantTMS("Horner/Argawal t")
{
    constantTMS.enteredRealValue = timeProcessDC.hornerTval;
    isMaster = true;

    InitCommon();
}

DFO_TimeProcess::DFO_TimeProcess(const DFO_TimeProcess& a) :
    FuncObjC(a),
    constantTMS(a.constantTMS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TimeProcess :: ~DFO_TimeProcess()
{
}

void DFO_TimeProcess::InitCommon()
{
    AddInPort(inputTPObjRef, typeid(DO_XYData));
    AddMSInPort(masterObjRef, typeid(DO_TimeProcess), isMaster);

    constantTMS.AddPort(*this);
    constantTCheckRef = GetLastInPortCheckRef();

    AddInPort(inputTQObjRef, typeid(DO_XYData));
    inputTQCheckRef = GetLastInPortCheckRef();

    AddOutPort(timeProcessDO);
    timeProcessDO.timeProcess = &timeProcessDC;

    AddOutPort(outputTPDO);
    outputTPDO.xyData = &outputTPDC;
    outputTPDC.SetID("processed input");

    AddOutPort(outputTQDO);
    outputTQDO.xyData = &timeProcessDC.tqi;
    timeProcessDC.tqi.SetID("super ti/qi data");

    DoStatusChk();
}

DFO_TimeProcess& DFO_TimeProcess::operator= (const DFO_TimeProcess& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}

void  DFO_TimeProcess::LocalCopy(const DFO_TimeProcess& a)
{
    inputTPObjRef   = a.inputTPObjRef;
    isMaster        = a.isMaster;
    masterObjRef    = a.masterObjRef;

    timeProcessDC   = a.timeProcessDC;
    constantTMS     = a.constantTMS;
    inputTQObjRef   = a.inputTQObjRef;
}


void DFO_TimeProcess::SetDefault()
{
}

void  DFO_TimeProcess:: DoStatusChk()
{
    // set flags for data to use
    *constantTCheckRef = isMaster&&
                         ((timeProcessDC.timeProcessType == TimeProcess::tpHorner) ||
                          (timeProcessDC.timeProcessType == TimeProcess::tpArgawal));

    *inputTQCheckRef   = isMaster &&
                         ((timeProcessDC.timeProcessType == TimeProcess::tpHornerSuper) ||
                          (timeProcessDC.timeProcessType == TimeProcess::tpBourdetSuper));

    FuncObjC::DoStatusChk();

    if (CheckInputFO(inputTPObjRef, "Input P(t)"))
        tpDataDC = static_cast<DO_XYData*>(GetInPortData(inputTPObjRef))->xyData;

    if ((*inputTQCheckRef) && (CheckInputFO(inputTQObjRef, "Superposition Q(t)")))
        tqDataDC = static_cast<DO_XYData*>(GetInPortData(inputTQObjRef))->xyData;

    if ((!isMaster) && (CheckInputFO(masterObjRef, "Master spec")))
    {
        timeProcessDC = *(static_cast<DO_TimeProcess*>(GetInPortData(masterObjRef))->timeProcess);
        constantTMS.enteredRealValue = timeProcessDC.hornerTval;
    }

    if (*constantTCheckRef)
    {
        constantTMS.DoCheck("Horner/Argawal t", *this);
        timeProcessDC.hornerTval = constantTMS.GetCurrentValue();
        timeProcessDC.argawalTpVal = timeProcessDC.hornerTval;
    }

    SC_SetupErr errData;
    if (!timeProcessDC.TimeProcessSetupOK(errData))
        SetObjErrMsg(errData.GetErrorText());
}

void DFO_TimeProcess:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;

    // create time process if master
    if (isMaster)
    {
        SC_SetupErr errData;
        if (*inputTQCheckRef)
            timeProcessDC.CreateTimeProcess(*tqDataDC, errData);
        else
            timeProcessDC.CreateTimeProcess(errData);

        if (errData.ErrorFound())
        {
            SetObjErrMsg(errData.GetErrorText());
            return;
        }
    }

    if (!timeProcessDC.GetProcessedTime(*tpDataDC, outputTPDC))
        SetObjErrMsg("empty after processing");
}

