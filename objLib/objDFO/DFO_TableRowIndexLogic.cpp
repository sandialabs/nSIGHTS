///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableRowIndexLogic.cpp
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
//      Supports combining extraction operations from several tables
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <objDFO/DFO_TableRowIndexLogic.h>

DFO_TableRowIndexLogic :: DFO_TableRowIndexLogic()
    : FuncObjC("Table Row Index Logic"), outputIndexDO(false, true)
{
    logicOp      = ot_AND;
    InitCommon();
}

DFO_TableRowIndexLogic::DFO_TableRowIndexLogic(const DFO_TableRowIndexLogic& a)
    : FuncObjC(a), outputIndexDO(false, true)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableRowIndexLogic :: ~DFO_TableRowIndexLogic ()
{
}

void DFO_TableRowIndexLogic::InitCommon()
{
    AddInPort(dataInputAObjRef, typeid(DO_Indx));
    AddInPort(dataInputBObjRef, typeid(DO_Indx));
    AddOutPort(outputIndexDO);
    outputIndexDO.Reset("Logic results");

    DoStatusChk();
}

DFO_TableRowIndexLogic& DFO_TableRowIndexLogic::operator= (const DFO_TableRowIndexLogic& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableRowIndexLogic::LocalCopy(const DFO_TableRowIndexLogic& a)
{
    dataInputAObjRef = a.dataInputAObjRef;
    dataInputBObjRef = a.dataInputBObjRef;
    logicOp          = a.logicOp;
}

void  DFO_TableRowIndexLogic:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    inputDataA = 0;
    inputDataB = 0;

    CheckInputFO(dataInputAObjRef, "input data A source");
    CheckInputFO(dataInputBObjRef, "input data B source");
    if (StatusNotOK())
        return;

    DO_Indx* tmpADO = static_cast<DO_Indx*>(GetInPortData(dataInputAObjRef));
    if ((!tmpADO->IsMultiple()) || (!tmpADO->MultipleOK()))
        SetObjErrMsg("Input A must have multiple indexes");

    DO_Indx* tmpBDO = static_cast<DO_Indx*>(GetInPortData(dataInputBObjRef));
    if ((!tmpBDO->IsMultiple()) || (!tmpBDO->MultipleOK()))
        SetObjErrMsg("Input B must have multiple indexes");

    if (StatusOK())
    {
        inputDataA = &tmpADO->indxArray;
        inputDataB = &tmpBDO->indxArray;

        if (inputDataA->IsEmpty() || inputDataB->IsEmpty())
            SetObjErrMsg("A or B has no selections");
    }
}


void DFO_TableRowIndexLogic:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    outputIndexDO.Reset("Logic results");


    int minA, maxA, minB, maxB;

    inputDataA->GetMinMax(minA, maxA);
    inputDataB->GetMinMax(minB, maxB);
    if ((minA < 0) || (minB < 0))
    {
        SetObjErrMsg("minimum index < 0 ??");
        return;
    }

    int maxIndex =  (maxA > maxB) ? maxA : maxB;
    maxIndex++;

    SC_IntArray& outputData = outputIndexDO.indxArray;
    outputData.AllocAndFill(maxIndex, 0);

    int i;
    for (i = 0; i < inputDataA->Size(); i++)
        outputData[(*inputDataA)[i]] = 1;

    for (i = 0; i < inputDataB->Size(); i++)
        outputData[(*inputDataB)[i]]++;

    int okCount = 0;
    for (i = 0; i < maxIndex; i++)
    {
        bool ok = false;
        switch (logicOp)
        {
            case ot_AND : {ok = (outputData[i] == 2); break;}
            case ot_OR  : {ok = (outputData[i] > 0); break;}
            case ot_XOR : {ok = (outputData[i] == 1); break;}
        }

        if (ok)
            outputData[okCount++] = i;
    }
    outputData.SetSize(okCount);


    if (outputData.IsEmpty())
    {
        SetObjErrMsg("No data passed operation");
        outputData.DeAlloc();
    }
}

