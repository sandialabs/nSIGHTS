///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ExtractSASteps.cpp
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
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <saDFO/DFO_ExtractSASteps.h>

DFO_ExtractSASteps :: DFO_ExtractSASteps() :
    FuncObjC("Extract SA Table Steps")
{
    extractByIndexes = true;
    minIndex = 1;
    maxIndex = 999;
    indexIncrement = 1;
    minValue = 0.0;
    maxValue = 1.0E+07;
    InitCommon();
}

DFO_ExtractSASteps::DFO_ExtractSASteps(const DFO_ExtractSASteps& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ExtractSASteps::InitCommon()
{
    AddInPort(inputTableObjRef, typeid(DO_SATable));
    AddOutPort(outputTableDO);
    outputTableDO.saTable = &outputTableDC;
    DoStatusChk();
}

DFO_ExtractSASteps& DFO_ExtractSASteps::operator= (const DFO_ExtractSASteps& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ExtractSASteps::LocalCopy(const DFO_ExtractSASteps& a)
{
    inputTableObjRef = a.inputTableObjRef;
    extractByIndexes = a.extractByIndexes;
    minIndex = a.minIndex;
    maxIndex = a.maxIndex;
    indexIncrement = a.indexIncrement;
    minValue = a.minValue;
    maxValue = a.maxValue;
}

void  DFO_ExtractSASteps:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (!CheckInputFO(inputTableObjRef, "Input transfer table"))
        return;

    inputTableDC = static_cast<DO_SATable*>(GetInPortData(inputTableObjRef))->GetSATablePtr();


    if (extractByIndexes)
    {
        if (minIndex > maxIndex)
            SetObjErrMsg("minimum index must be less than maximum index");

        if (minIndex > inputTableDC->GetNStep())
            SetObjErrMsg("minimum index greater than n input table steps");
    }
    else
    {
        if (minValue > maxValue )
            SetObjErrMsg("minimum value must be less than maximum value");

        if (maxValue < inputTableDC->stepValues[0])
            SetObjErrMsg("maximum value less than input first step value");

        if (minValue > inputTableDC->stepValues.LastIndex())
            SetObjErrMsg("minimum value greater than input last step value");
    }
}

void DFO_ExtractSASteps::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    SC_IntArray selSteps;
    selSteps.Alloc(inputTableDC->GetNStep());
    if (extractByIndexes)
    {
        int maxInSteps = inputTableDC->GetNStep();
        int maxOutIndex = IMin(maxInSteps, maxIndex);
        maxOutIndex--; // to C Index

        int outIndex = minIndex;
        outIndex--;  // to C index

        while (outIndex <= maxOutIndex)
        {
            selSteps += outIndex;
            outIndex += indexIncrement;
        }
    }
    else
    {
        for (int i = 0; i < inputTableDC->GetNStep(); i++)
            if (Limit::WithinOneLimit(minValue, maxValue, inputTableDC->stepValues[i]))
                selSteps += i;
    }

    if (selSteps.IsEmpty())
    {
        SetObjErrMsg("no steps in limits ??");
        return;
    }

    int nvector = inputTableDC->GetNVector();
    int nvar = inputTableDC->GetNVariable();

    if (!outputTableDC.Alloc(selSteps.Size(), nvar, nvector))
    {
        SetObjErrMsg("table allocation ??");
        return;
    }

    outputTableDC.SetID("Selected data");

    CopyString(outputTableDC.originalFile, inputTableDC->originalFile, stdFileStrLen);
    outputTableDC.processingSteps = inputTableDC->processingSteps;
    outputTableDC.processingSteps += "Step Selection";

    char outStr[80], noutStr[40], ninStr[40];
    IntToString(selSteps.Size(), noutStr, 40);
    IntToString(inputTableDC->GetNStep(), ninStr, 40);
    MakeString(outStr, " -- Selected ", noutStr, " of ", ninStr, " steps", 80);
    outputTableDC.processingSteps += outStr;

    outputTableDC.variableNames = inputTableDC->variableNames;
    outputTableDC.variableIndexes = inputTableDC->variableIndexes;

    for (int i = 0; i < selSteps.Size(); i++)
    {
        outputTableDC.stepValues[i] = inputTableDC->stepValues[selSteps[i]];
        outputTableDC.stepIndexes[i] = inputTableDC->stepIndexes[selSteps[i]];
    }
    for (int i = 0; i < selSteps.Size(); i++)
    {
        int selStep = selSteps[i];
        for (int j = 0; j < nvar; j++)
            outputTableDC.saData[i][j] = inputTableDC->saData[selStep][j];
    }

    if (objIDisDefault)
    {
        char newID[80];
        MakeString(newID, "Step(", inputTableDC->GetID(), ")", 80);
        outputTableDC.SetID(newID);
    }
    else
        outputTableDC.SetID(GetID());

}


