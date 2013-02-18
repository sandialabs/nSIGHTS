///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectVarStep.cpp
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

#include <saDFO/DFO_SelectVarStep.h>


DFO_SelectVarStep :: DFO_SelectVarStep() :
    FuncObjC("Select Variables and Steps"),
    variableIndexMS(true, true),
    stepIndexMS(true, true)
{
    InitCommon();
}

DFO_SelectVarStep::DFO_SelectVarStep(const DFO_SelectVarStep& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_SelectVarStep :: ~DFO_SelectVarStep ()
{
}

void DFO_SelectVarStep::InitCommon()
{
    AddInPort(inputTableObjRef, typeid(DO_SATable));
    stepIndexMS.AddPort(*this);
    variableIndexMS.AddPort(*this);
    AddOutPort(outputTableDO);
    outputTableDO.saTable = &outputTableDC;
    AddOutPort(selectedVarDO);
    selectedVarDO.Reset("Selected variables");
    DoStatusChk();
}


DFO_SelectVarStep& DFO_SelectVarStep::operator= (const DFO_SelectVarStep& a)
{
    if (&a != this)
    {
        FuncObjC::operator= (a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectVarStep::LocalCopy(const DFO_SelectVarStep& a)
{
    inputTableObjRef = a.inputTableObjRef;
    stepIndexMS = a.stepIndexMS;
    variableIndexMS = a.variableIndexMS;
}


void DFO_SelectVarStep::DoStatusChk()
{
    FuncObjC::DoStatusChk();

    CheckInputFO(inputTableObjRef, "Input SA table");

    variableIndexMS.Reset("Variables");
    stepIndexMS.Reset("Step");

    variableIndexMS.DoCheck("Variables", *this);
    stepIndexMS.DoCheck("Steps", *this);

}


void DFO_SelectVarStep:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
    {
        selectedVarDO.Reset("Selected variables");
        return;
    }

    const DC_SATable&   inputTableDC =
        static_cast<DO_SATable*>(GetInPortData(inputTableObjRef))->GetSATableRef();


    SC_IntArray selVars;
    variableIndexMS.GetIndexArray(selVars, inputTableDC.GetNVariable());

    SC_IntArray selSteps;
    stepIndexMS.GetIndexArray(selSteps, inputTableDC.GetNStep());

    if (selSteps.IsEmpty() || selVars.IsEmpty())
    {
        SetObjErrMsg("no steps or variables selected ??");
        return;
    }

    int nvector = inputTableDC.GetNVector();

    if (!outputTableDC.Alloc(selSteps.Size(), selVars.Size(), nvector))
    {
        SetObjErrMsg("table allocation ??");
        return;
    }

    outputTableDC.SetID("Selected data");

    CopyString(outputTableDC.originalFile, inputTableDC.originalFile, stdFileStrLen);
    outputTableDC.processingSteps = inputTableDC.processingSteps;
    outputTableDC.processingSteps += "Variable/Step Selection";

    char outStr[80], noutStr[40], ninStr[40];
    IntToString(selVars.Size(), noutStr, 40);
    IntToString(inputTableDC.GetNVariable(), ninStr, 40);
    MakeString(outStr, " -- Selected ", noutStr, " of ", ninStr, " variables", 80);
    outputTableDC.processingSteps += outStr;

    IntToString(selSteps.Size(), noutStr, 40);
    IntToString(inputTableDC.GetNStep(), ninStr, 40);
    MakeString(outStr, " -- Selected ", noutStr, " of ", ninStr, " steps", 80);
    outputTableDC.processingSteps += outStr;

    selectedVarDO.dataCoords.Alloc(selVars.Size());
    selectedVarDO.dataLabels.Alloc(selVars.Size());
    for (int i = 0; i < selVars.Size(); i++)
    {
        const char* nextVar = inputTableDC.GetVariableName(selVars[i]);
        outputTableDC.variableNames.SetString(nextVar, i);
        outputTableDC.variableIndexes[i] = inputTableDC.variableIndexes[selVars[i]];

        selectedVarDO.dataCoords += Coord3D(double(i + 1));
        selectedVarDO.dataLabels += nextVar;
    }
    for (int i = 0; i < selSteps.Size(); i++)
    {
        outputTableDC.stepValues[i] = inputTableDC.stepValues[selSteps[i]];
        outputTableDC.stepIndexes[i] = inputTableDC.stepIndexes[selSteps[i]];
    }
    for (int i = 0; i < selSteps.Size(); i++)
    {
        int selStep = selSteps[i];
        for (int j = 0; j < selVars.Size(); j++)
            outputTableDC.saData[i][j] = inputTableDC.saData[selStep][selVars[j]];
    }

    if (objIDisDefault)
    {
        char newID[80];
        MakeString(newID, "Sel(", inputTableDC.GetID(), ")", 80);
        outputTableDC.SetID(newID);
    }
    else
        outputTableDC.SetID(GetID());

}

