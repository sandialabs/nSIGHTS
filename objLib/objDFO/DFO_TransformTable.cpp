///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TransformTable.cpp
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

#include <genClass/SC_BoolArray.h>
#include <objDFO/DFO_TransformTable.h>



DFO_TransformTable::TransformDesc::TransformDesc()
{
    doTransform = false;
    SetToNull(transformID);
    SetToNull(transformDesc);
}

DFO_TransformTable::TransformDesc::TransformDesc(const TransformDesc& a)
{
    LocalCopy(a);
}

DFO_TransformTable::TransformDesc&  DFO_TransformTable::TransformDesc::operator= (const TransformDesc& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void DFO_TransformTable::TransformDesc::LocalCopy(const TransformDesc& a)
{
    doTransform = a.doTransform;
    CopyString(transformID, a.transformID, transformIDLen);
    CopyString(transformDesc, a.transformDesc, transformDescLen);
}


DFO_TransformTable :: DFO_TransformTable() :
    FuncObjC("Transform Table")
{
    columnDescAreVarID = true;
    outputTableData = otd_All;
    InitCommon();
}

DFO_TransformTable::DFO_TransformTable(const DFO_TransformTable& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_TransformTable::InitCommon()
{
    activeTransforms.Alloc(maxTransform);

    AddInPort(inputTableObjRef, typeid(DO_TableData));

    AddOutPort(outputTableDO);
    outputTableDO.tableData = &outputTableDC;
    DoStatusChk();
}

DFO_TransformTable& DFO_TransformTable::operator= (const DFO_TransformTable& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TransformTable::LocalCopy(const DFO_TransformTable& a)
{
    columnDescAreVarID = a.columnDescAreVarID;
    outputTableData = a.outputTableData;
    inputTableObjRef = a.inputTableObjRef;
    for (int i = 0; i < maxTransform; i++)
        transformDesc[i] = a.transformDesc[i];
}


void  DFO_TransformTable:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (!CheckInputFO(inputTableObjRef, "Input table"))
        return;

    inputTableDC = static_cast<DO_TableData*>(GetInPortData(inputTableObjRef))->tableData;

    if (columnDescAreVarID)
    {
        int ncol = inputTableDC->GetNCols();
        inputVars.Alloc(ncol);
        for (int i = 0; i < ncol; i++)
            SetInputVariable(inputTableDC->columnDesc[i]);

        SC_SetupErr err;
        if (!CheckInputVariables(err))
            SetObjErrMsg(err.GetErrorText());
    }

    activeTransforms.SetEmpty();
    for (int i = 0; i < maxTransform; i++)
        if (transformDesc[i].doTransform)
            activeTransforms += transformDesc[i];

    if (activeTransforms.IsEmpty())
        SetObjErrMsg("no transforms set");

}

void DFO_TransformTable::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int ncol = inputTableDC->GetNCols();
    int ndata = inputTableDC->GetNRows();
    int ntransform = activeTransforms.Size();
    int ninput = ncol + ntransform;

    // for testing
    SetVectorSize(-1);

    inputVars.Alloc(ninput);

    SC_StringArray varIDs = inputTableDC->columnDesc;
    if (!columnDescAreVarID)
    {
        char intStr[40];
        for (int i = 0; i < ncol; i++)
        {
            IntToString(i + 1, intStr, 40);
            MakeString(varIDs[i], "C", intStr, varIDs.GetStringLen());
        }
    }

    for (int i = 0; i < ncol; i++)
        SetInputVariable(varIDs[i]);

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
    }

    int noutCol;
    SC_BoolArray copyInput(ncol, false);
    switch (outputTableData) {
        case otd_All : {
            copyInput.FillToAlloc(true);
            noutCol = ninput;
            break;
        }
        case otUsedAndTransform : {
            noutCol = ntransform;
            for (int i = 0; i < ncol; i++)
                if (inputVars[i].inputUsed)
                {
                    copyInput[i] = true;
                    noutCol++;
                }
            break;
        }
        case otd_TransformOnly : {
            noutCol = ntransform;
            break;
        }
    }


    if (!outputTableDC.Alloc(noutCol, ndata))
    {
        SetObjErrMsg("output table allocation ??");
        return;
    }

    // passed all checks - should evaluate
    SetVectorSize(ndata);
    inputVars.SetEmpty();

    int nextOutCol = 0;

    // set up variables and copy input to output if necessary
    for (int i = 0; i < ncol; i++)
    {
        const SC_DoubleArray& nextCol = inputTableDC->dataTable[i];
        SetInputVariable(varIDs[i], nextCol);
        if (copyInput[i])
        {
            outputTableDC.SetTitle(nextOutCol, inputTableDC->columnDesc[i]);
            outputTableDC.dataTable[nextOutCol] = nextCol;
            nextOutCol++;
        }
    }


    // evaluate functions - repeat loop
    int trStart = ncol;

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
        SC_DoubleArray& results = outputTableDC.dataTable[nextOutCol];
        results.SetSize(ndata);
        SetInputVariable(tranID, results);

        if (!ParseEquation(eqnStr, err))
        {
            MakeString(errStr, "??TrID:", tranID, " :: ", err.GetErrorText(), 120);
            SetObjErrMsg(errStr);
            return;
        }


        if (!Evaluate(results, false, err))
        {
            MakeString(errStr, "TrID:Eval::", tranID, " :: ", err.GetErrorText(), 120);
            SetObjErrMsg(errStr);
            return;
        }

        outputTableDC.SetTitle(nextOutCol, tranID);
        nextOutCol++;
    }

    outputTableDC.dataTable.MatrixSetSize(nextOutCol, ndata);
    outputTableDC.rowDesc = inputTableDC->rowDesc;

    if (objIDisDefault)
    {
        char newID[80];
        MakeString(newID, "TR(", inputTableDC->GetID(), ")", 80);
        outputTableDC.SetID(newID);
    }
    else
        outputTableDC.SetID(GetID());
}


