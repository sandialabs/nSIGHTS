///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TransformSATable.cpp
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

#include <saDFO/DFO_TransformSATable.h>

DFO_TransformSATable::TransformDesc::TransformDesc()
{
    doTransform = false;
    SetToNull(transformID);
    SetToNull(transformDesc);
}

DFO_TransformSATable::TransformDesc::TransformDesc(const TransformDesc& a)
{
    LocalCopy(a);
}

DFO_TransformSATable::TransformDesc&  DFO_TransformSATable::TransformDesc::operator= (const TransformDesc& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void DFO_TransformSATable::TransformDesc::LocalCopy(const TransformDesc& a)
{
    doTransform = a.doTransform;
    CopyString(transformID, a.transformID, transformIDLen);
    CopyString(transformDesc, a.transformDesc, transformDescLen);
}


DFO_TransformSATable :: DFO_TransformSATable() :
    FuncObjC("Transform SA Table")
{
    dualInputTables = true;
    InitCommon();
}

DFO_TransformSATable::DFO_TransformSATable(const DFO_TransformSATable& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_TransformSATable::InitCommon()
{
    activeTransforms.Alloc(maxTransform);

    AddInPort(table1ObjRef, typeid(DO_SATable));
    AddUseFlagInPort(table2ObjRef, typeid(DO_SATable), dualInputTables);

    AddOutPort(outputTable1DO);
    outputTable1DO.saTable = &outputTable1DC;
    AddOutPort(outputTable2DO);
    outputTable2DO.saTable = &outputTable2DC;
    DoStatusChk();
}

DFO_TransformSATable& DFO_TransformSATable::operator= (const DFO_TransformSATable& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TransformSATable::LocalCopy(const DFO_TransformSATable& a)
{
    dualInputTables = a.dualInputTables;
    table1ObjRef = a.table1ObjRef;
    table2ObjRef = a.table2ObjRef;
    for (int i = 0; i < maxTransform; i++)
        transformDesc[i] = a.transformDesc[i];
}


void  DFO_TransformSATable:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (!CheckInputFO(table1ObjRef, "Input transfer table 1"))
        return;
    if (dualInputTables && (!CheckInputFO(table1ObjRef, "Input transfer table 2")))
        return;

    table1DC = static_cast<DO_SATable*>(GetInPortData(table1ObjRef))->GetSATablePtr();
    if (dualInputTables)
    {
        if (table1DC->GetNStep() > 1)
            SetObjErrMsg("IV table can only have one step for IV/DV transforms");
        table2DC = static_cast<DO_SATable*>(GetInPortData(table2ObjRef))->GetSATablePtr();
        if (table1DC->GetNVector() != table2DC->GetNVector())
            SetObjErrMsg("IV and DV table must have the same number of observations");
    }

    activeTransforms.SetEmpty();
    for (int i = 0; i < maxTransform; i++)
        if (transformDesc[i].doTransform)
            activeTransforms += transformDesc[i];

    if (activeTransforms.IsEmpty())
        SetObjErrMsg("no transforms set");

}

void DFO_TransformSATable::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;
    int ninput = table1DC->GetNVariable();
    int nvec = table1DC->GetNVector();
    int nstep1 = table1DC->GetNStep();
    int ndata = nvec * nstep1;

    int nstep2 = 0;
    if (dualInputTables)
    {
        ninput += table2DC->GetNVariable();
        nstep2 = table2DC->GetNStep();
        ndata = nstep2 * nvec;
    }

    // for testing
    SetVectorSize(-1);

    ninput += activeTransforms.Size();
    inputVars.Alloc(ninput);
    for (int i = 0; i < table1DC->GetNVariable(); i++)
        SetInputVariable(table1DC->GetVariableName(i), 0);

    if (dualInputTables)
    {
        for (int i = 0; i < table2DC->GetNVariable(); i++)
            SetInputVariable(table2DC->GetVariableName(i), 1);
    }


    int ntransform = activeTransforms.Size();
    SC_SetupErr err;

    for (int i = 0; i < ntransform; i++)
    {
        const TransformDesc& currTransform = activeTransforms[i];
        const char* tranID = currTransform.transformID;

        char errStr[120];
        if (!CheckInputVariables(err))
        {
            MakeString(errStr, "TrID:", tranID, " :: ", err.GetErrorText(), 120);
            SetObjErrMsg(errStr);
            return;
        }

        ClearForNext();

        char eqnStr[255];
        MakeString(eqnStr, tranID, " = ", currTransform.transformDesc, 255);
        SetInputVariable(tranID);

        if (!ParseEquation(eqnStr, err))
        {
            MakeString(errStr, "TrID:Parse", tranID, " :: ", err.GetErrorText(), 120);
            SetObjErrMsg(errStr);
            return;
        }

        inputVars.LastIndex().inputClass = GetTranVarClass();
    }


    // passed all checks - should evaluate
    SetVectorSize(ndata);
    inputVars.SetEmpty();

    SC_DoubleMatrix tempVar;
    tempVar.MatrixAllocAndSetSize(ninput, ndata);
    int nextVar = 0;

    // copy variables and steps to single var
    for (int i = 0; i < table1DC->GetNVariable(); i++)
    {
        SC_DoubleArray& nextData = tempVar[nextVar++];
        nextData.SetEmpty();
        if (dualInputTables)
        {
            // mult copies of IV step 0
            for (int j = 0; j < nstep2; j++)
                nextData += table1DC->saData[0][i];
        }
        else
        {
            // string together steps
            for (int j = 0; j < nstep1; j++)
                nextData += table1DC->saData[j][i];
        }
        SetInputVariable(table1DC->GetVariableName(i), nextData, 0);
    }

    if (dualInputTables)
    {
        for (int i = 0; i < table2DC->GetNVariable(); i++)
        {
            SC_DoubleArray& nextData = tempVar[nextVar++];
            nextData.SetEmpty();
            for (int j = 0; j < nstep2; j++)
                nextData += table2DC->saData[j][i];
            SetInputVariable(table2DC->GetVariableName(i), nextData, 1);
        }
    }

    // evaluate functions - repeat loop
    int trStart = inputVars.Size();

    for (int i = 0; i < ntransform; i++)
    {
        const TransformDesc& currTransform = activeTransforms[i];
        const char* tranID = currTransform.transformID;

        char errStr[120];
        if (!CheckInputVariables(err))
        {
            MakeString(errStr, "??TrID:", tranID, " :: ", err.GetErrorText(), 120);
            SetObjErrMsg(errStr);
            return;
        }

        ClearForNext();

        char eqnStr[255];
        MakeString(eqnStr, tranID, " = ", currTransform.transformDesc, 255);
        SetInputVariable(tranID, tempVar[nextVar]);

        if (!ParseEquation(eqnStr, err))
        {
            MakeString(errStr, "??TrID:", tranID, " :: ", err.GetErrorText(), 120);
            SetObjErrMsg(errStr);
            return;
        }

        if (!Evaluate(tempVar[nextVar], true, err))
        {
            MakeString(errStr, "TrID:Eval::", tranID, " :: ", err.GetErrorText(), 120);
            SetObjErrMsg(errStr);
            return;
        }

        inputVars.LastIndex().inputClass = GetTranVarClass();

        nextVar++;
    }

    // collect and copy to output tables
    int n1 = 0;
    int n2 = 0;
    for (int i = 0; i < inputVars.Size(); i++)
    {
        if (inputVars[i].inputClass == 0)
            n1++;
        else
            n2++;
    }

    // IV or table 1
    outputTable1DC.Alloc(nstep1, n1, nvec);
    outputTable1DC.CopyBasicInfo(*table1DC);

    if (dualInputTables)
    {
        AddToTable(outputTable1DC, trStart, 0, "IVTR");

        // DV or table 2
        outputTable2DC.Alloc(nstep2, n2, nvec);
        outputTable2DC.CopyBasicInfo(*table2DC);
        AddToTable(outputTable2DC, trStart, 1, "DVTR");
    }
    else
    {
        AddToTable(outputTable1DC, trStart, 0, "TR");
        outputTable2DC.ClearData();
        outputTable2DC.SetID("no DV");
    }
}

int DFO_TransformSATable::GetTranVarClass()
{
    if (!dualInputTables)
        return 0;

    int tranVarClass = 0;
    for (int j = 0; j < inputVars.UpperBound(); j++)
    {
        Variable& currVar = inputVars[j];
        if (currVar.inputUsed)
        {
            if (currVar.inputClass == 1)
                tranVarClass = 1;
            currVar.inputUsed = false; // clear for next
        }
    }
    return tranVarClass;
}


void DFO_TransformSATable::AddToTable(DC_SATable& outTable,
                                            int trStart,
                                            int varClass,
                                            const char* idExt)
{
    int n = 0;
    int nvec = outTable.GetNVector();
    int nstep = outTable.GetNStep();
    bool trAdded = false;
    for (int i = 0; i < inputVars.Size(); i++)
    {
        Variable& currVar = inputVars[i];
        if (currVar.inputClass != varClass)
            continue;

        outTable.variableNames.SetString(currVar.varID, n);
        int trNum = 0;
        const SC_DoubleArray& trValue = *currVar.varData;
        for (int j = 0; j < nstep; j++)
        {
            SC_DoubleArray& stData = outTable.saData[j][n];
            for (int k = 0; k < nvec; k++)
                stData[k] = trValue[trNum++];
        }
        n++;

        if (i >= trStart)
        {
            trAdded = true;
            int trNum = i - trStart;
            char descStr[DC_SATable::procStepLen];
            MakeString(descStr, "Added ",
                        activeTransforms[trNum].transformID, " = ",
                        activeTransforms[trNum].transformDesc, DC_SATable::procStepLen);
            outTable.processingSteps += descStr;
        }
    }

    outTable.SetInitialIndexes();

    if (trAdded)
    {
        char newID[80];
        if (objIDisDefault)
            MakeString(newID, idExt, "(", outTable.GetID(), ")", 80);
        else
            if (dualInputTables)
                MakeString(newID, idExt, "(", GetID(), ")", 80);
            else
                CopyString(newID, GetID(), 80);

        outTable.SetID(newID);
    }

}

