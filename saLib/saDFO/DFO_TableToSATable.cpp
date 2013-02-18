///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableToSATable.cpp
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
#include <genClass/SC_RealFormat.h>

#include <saDFO/DFO_TableToSATable.h>

DFO_TableToSATable :: DFO_TableToSATable() :
    FuncObjC("Table2SATable")
{
    rowData = DC_SATable::saVector;
    colData = DC_SATable::saVariable;
    CopyString(variableName, "Var", DC_SATable::varNameLen);
    stepVal = 0.0;
    colRow0IsObsIndex = true;
    stepValueIsIDs = false;
    InitCommon();
}

DFO_TableToSATable::DFO_TableToSATable(const DFO_TableToSATable& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_TableToSATable::InitCommon()
{
    AddInPort(inputTableObjRef, typeid(DO_TableData));
    AddOutPort(outputSATableDO);
    outputSATableDO.saTable = &outputSATableDC;
    DoStatusChk();
}

DFO_TableToSATable& DFO_TableToSATable::operator= (const DFO_TableToSATable& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableToSATable::LocalCopy(const DFO_TableToSATable& a)
{
    inputTableObjRef = a.inputTableObjRef;
    rowData = a.rowData;
    colData = a.colData;
    CopyString(variableName, a.variableName, DC_SATable::varNameLen);
    stepVal = a.stepVal;
    colRow0IsObsIndex = a.colRow0IsObsIndex;
    stepValueIsIDs = a.stepValueIsIDs;
}

void  DFO_TableToSATable:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (!CheckInputFO(inputTableObjRef, "Input table"))
        return;


    if (rowData == colData)
        SetObjErrMsg("row and column data must be different");

    if ((rowData != DC_SATable::saVector) && (colData != DC_SATable::saVector))
        SetObjErrMsg("one of row/column must be obs");

    const DC_TableData& inputTableDC = static_cast<DO_TableData*>(GetInPortData(inputTableObjRef))->GetTable();
    ncol = inputTableDC.GetNCols();
    if ((rowData == DC_SATable::saVector) && colRow0IsObsIndex)
        ncol--;
    if ((rowData == DC_SATable::saSteps) && (!stepValueIsIDs))
        ncol--;

    nrow = inputTableDC.GetNRows();
    if ((colData == DC_SATable::saVector) && colRow0IsObsIndex)
        nrow--;
    if ((colData == DC_SATable::saSteps) && (!stepValueIsIDs))// first row must be step values
        nrow--;

    if ((nrow < 1) || (ncol < 1))
        SetObjErrMsg("table must have at least 1 data row/col");

    if (((rowData == DC_SATable::saVector) && (nrow < 2)) ||
        ((colData == DC_SATable::saVector) && (ncol < 2)))
        SetObjErrMsg("table must have > 1 observation/vector");
}

void DFO_TableToSATable::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    const DC_TableData& inputTableDC = static_cast<DO_TableData*>(GetInPortData(inputTableObjRef))->GetTable();
    for (int i = 0; i < inputTableDC.GetNCols(); i++)
        if (!inputTableDC.dataTable[i].AllDataOK())
        {
            SetObjErrMsg("input table may not contain nulls");
            return;
        }

    int nstep = 1;
    int nvar = 1;
    int nobs = 1;

    switch (rowData) {
        case DC_SATable::saVariable : {
            nvar = nrow;
            break;
        }
        case DC_SATable::saVector : {
            nobs = nrow;
            break;
        }
        case DC_SATable::saSteps : {
            nstep = nrow;
            break;
        }
    }
    switch (colData) {
        case DC_SATable::saVariable : {
            nvar = ncol;
            break;
        }
        case DC_SATable::saVector : {
            nobs = ncol;
            break;
        }
        case DC_SATable::saSteps : {
            nstep = ncol;
            break;
        }
    }

    if (!outputSATableDC.Alloc(nstep, nvar, nobs))
    {
        SetObjErrMsg("table allocation error");
        return;
    }

    outputSATableDC.processingSteps += "Created by Table to SA Table";
    outputSATableDC.SetInitialIndexes();

    // set up variables
    outputSATableDC.variableNames.SetEmpty();
    SC_StringArray varTemp;

    if (colData == DC_SATable::saVariable)
    {
        varTemp = inputTableDC.columnDesc;
    }
    else if (rowData == DC_SATable::saVariable)
    {
        varTemp = inputTableDC.rowDesc;
    }
    else
    {
        outputSATableDC.variableNames.SetString(variableName, 0);
    }
    if (outputSATableDC.variableNames.IsEmpty())
    {
        if (varTemp.IsNotEmpty() && colRow0IsObsIndex)
            varTemp.DeleteItem(0);
        if (varTemp.Size() != nvar)
        {
            outputSATableDC.variableNames.SetIndexed("Var",1);
        }
        else
            outputSATableDC.variableNames = varTemp;
    }

    const SC_DoubleMatrix& inTbl = inputTableDC.dataTable;
    // set up steps
    if (colData == DC_SATable::saSteps)
    {
        int stCol = 0;
        if (colRow0IsObsIndex)
            stCol = 1;

        const SC_StringArray& stepID = inputTableDC.columnDesc;
        for (int i = 0; i < nstep; i++)
        {
            double& currStep = outputSATableDC.stepValues[i];
            if (stepValueIsIDs)
            {
                if ((stepID.UpperBound() < stCol) ||
                    (!IsValidReal(stepID[stCol], currStep)))
                    currStep = double(i);
            }
            else
                currStep = inTbl[stCol][0];
            stCol++;
        }
    }
    else if (rowData == DC_SATable::saSteps)
    {
        int stRow = 0;
        if (colRow0IsObsIndex)
            stRow = 1;

        const SC_StringArray& stepID = inputTableDC.rowDesc;
        for (int i = 0; i < nstep; i++)
        {
            double& currStep = outputSATableDC.stepValues[i];
            if (stepValueIsIDs)
            {
                if ((stepID.UpperBound() < stRow) ||
                    (!IsValidReal(stepID[stRow], currStep)))
                    currStep = double(i);
            }
            else
                currStep = inTbl[0][stRow];
            stRow++;
        }
    }
    else
    {
        outputSATableDC.stepValues[0] = stepVal;
    }


    // and data
    SC_DoubleCube& outCube = outputSATableDC.saData;

    switch (rowData) {
        case DC_SATable::saVariable : {
            // columns must be obs
            int stRow = 0;
            if (colRow0IsObsIndex)
                stRow = 1;
            for (int i = 0; i < nobs; i++)
            {
                const SC_DoubleArray& inCol = inTbl[i];
                for (int j = 0; j < nvar; j++)
                    outCube[0][j][i] = inCol[j + stRow];
            }
            break;
        }
        case DC_SATable::saVector : {
            int stCol = 0;
            if (colRow0IsObsIndex)
                stCol = 1;
            if (colData == DC_SATable::saVariable)
            {
                // cols are vars
                for (int i = 0; i < nvar; i++)
                {
                    const SC_DoubleArray& inCol = inTbl[stCol + i];
                    for (int j = 0; j < nobs; j++)
                        outCube[0][i][j] = inCol[j];
                }
            }
            else
            {
                // cols are steps
                int stRow = 1;
                if (stepValueIsIDs)
                    stRow = 0;

                for (int i = 0; i < nstep; i++)
                {
                    const SC_DoubleArray& inCol = inTbl[stCol + i];
                    for (int j = 0; j < nobs; j++)
                        outCube[i][0][j] = inCol[j + stRow];
                }
            }
            break;
        }
        case DC_SATable::saSteps : {
            // cols are obs
            int stRow = 0;
            if (colRow0IsObsIndex)
                stRow = 1;
            int stCol = 1;
            if (stepValueIsIDs)
                stCol = 0;
            for (int i = 0; i < nobs; i++)
            {
                const SC_DoubleArray& inCol = inTbl[stCol + i];
                for (int j = 0; j < nstep; j++)
                    outCube[j][0][i] = inCol[j + stRow];
            }
            break;
        }
    }


    if (objIDisDefault)
    {
        char titleStr[80];
        MakeString(titleStr, "Cnvert(", inputTableDC.GetID(), ")", 80);
        outputSATableDC.SetID(titleStr);
    }
    else
        outputSATableDC.SetID(GetID());


}

