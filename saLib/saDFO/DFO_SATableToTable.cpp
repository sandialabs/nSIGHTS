///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SATableToTable.cpp
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

#include <saDFO/DFO_SATableToTable.h>

DFO_SATableToTable :: DFO_SATableToTable() :
    FuncObjC("SATable2Table")
{
    rowData = DC_SATable::saVector;
    colData = DC_SATable::saVariable;
    variableIndex = 0;
    vectorIndex = 0;
    stepIndex = 0;
    InitCommon();
}

DFO_SATableToTable::DFO_SATableToTable(const DFO_SATableToTable& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SATableToTable::InitCommon()
{
    AddInPort(inputSATableObjRef, typeid(DO_SATable));
    AddOutPort(outputTableDO);
    outputTableDO.tableData = &outputTableDC;
    DoStatusChk();
}

DFO_SATableToTable& DFO_SATableToTable::operator= (const DFO_SATableToTable& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SATableToTable::LocalCopy(const DFO_SATableToTable& a)
{
    inputSATableObjRef = a.inputSATableObjRef;
    rowData = a.rowData;
    colData = a.colData;
    variableIndex = a.variableIndex;
    vectorIndex = a.vectorIndex;
    stepIndex = a.stepIndex;
}

void  DFO_SATableToTable:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (!CheckInputFO(inputSATableObjRef, "Input SA table"))
        return;

    inputSATableDC = static_cast<DO_SATable*>(GetInPortData(inputSATableObjRef))->GetSATablePtr();

    if (rowData == colData)
    {
        SetObjErrMsg("row and column data must be different");
        return;
    }

    fixedData = DC_SATable::SATableComponent(3 - int(rowData) - int(colData));
    int nfixed = GetNIndex(fixedData);

    switch (fixedData) {
        case DC_SATable::saVariable : {
            if (variableIndex >= nfixed)
                SetObjErrMsg("selected variable bad");
            break;
        }
        case DC_SATable::saVector : {
            if (vectorIndex >= nfixed)
                SetObjErrMsg("selected vector bad");
            break;
        }
        case DC_SATable::saSteps : {
            if (stepIndex >= nfixed)
                SetObjErrMsg("selected step bad");
            break;
        }
    }
}

void DFO_SATableToTable::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int nrow = GetNIndex(rowData);
    int ncol = GetNIndex(colData);
    if (rowData != DC_SATable::saVariable)
        ncol++;

    if (!outputTableDC.Alloc(ncol, nrow))
    {
        SetObjErrMsg("table allocation ??");
        return;
    }

    SC_DoubleMatrix& outTbl = outputTableDC.dataTable;
    outTbl.MatrixSetSize(ncol, nrow);
    inputSATableDC->GetTypeIDs(rowData, outputTableDC.rowDesc);
    SC_StringArray& colDesc = outputTableDC.columnDesc;

    // brute force
    const SC_DoubleCube& inCube = inputSATableDC->saData;


    colDesc.SetEmpty();

    switch (rowData) {
        case DC_SATable::saVariable : {
            if (colData == DC_SATable::saVector)
            {
                for (int i = 0; i < ncol; i++)
                {
                    SC_DoubleArray& outCol = outTbl[i];
                    for (int j = 0; j < nrow; j++)
                        outCol[j] = inCube[stepIndex][j][i];
                }
            }
            else
            {
                for (int i = 0; i < ncol; i++)
                {
                    SC_DoubleArray& outCol = outTbl[i];
                    for (int j = 0; j < nrow; j++)
                        outCol[j] = inCube[i][j][vectorIndex];
                }
            }
            break;
        }
        case DC_SATable::saVector : {

            for (int i = 0; i < nrow; i++)
                outTbl[0][i] = double(i + 1);
            colDesc += "Obs";

            if (colData == DC_SATable::saVariable)
            {
                for (int i = 1; i < ncol; i++)
                {
                    SC_DoubleArray& outCol = outTbl[i];
                    for (int j = 0; j < nrow; j++)
                        outCol[j] = inCube[stepIndex][i - 1][j];
                }
            }
            else
            {
                for (int i = 1; i < ncol; i++)
                {
                    SC_DoubleArray& outCol = outTbl[i];
                    for (int j = 0; j < nrow; j++)
                        outCol[j] = inCube[i - 1][variableIndex][j];
                }
            }
            break;
        }
        case DC_SATable::saSteps : {
            for (int i = 0; i < nrow; i++)
                outTbl[0][i] = inputSATableDC->stepValues[i];;
            colDesc += "Step";

            if (colData == DC_SATable::saVariable)
            {
                for (int i = 1; i < ncol; i++)
                {
                    SC_DoubleArray& outCol = outTbl[i];
                    for (int j = 0; j < nrow; j++)
                        outCol[j] = inCube[j][i - 1][vectorIndex];
                }
            }
            else
            {
                for (int i = 1; i < ncol; i++)
                {
                    SC_DoubleArray& outCol = outTbl[i];
                    for (int j = 0; j < nrow; j++)
                        outCol[j] = inCube[j][variableIndex][i - 1];
                }
            }
            break;
        }
    }

    SC_StringArray temp;
    inputSATableDC->GetTypeIDs(colData, temp);
    colDesc += temp;

    char titleStr[80];
    switch (fixedData) {
        case DC_SATable::saVariable : {
            MakeString(titleStr, inputSATableDC->GetVariableName(variableIndex), " table", 80);
            break;
        }
        case DC_SATable::saVector : {
            char tempStr[40];
            IntToString(vectorIndex + 1, tempStr, 40);
            MakeString(titleStr, "Obs# ", tempStr, " table", 80);
            break;
        }
        case DC_SATable::saSteps : {
            SC_RealFormat rForm;
            char tempStr[40];
            rForm.RealToString(inputSATableDC->GetStepVal(stepIndex), tempStr, 40);
            MakeString(titleStr, "Step ", tempStr, " table", 80);
            break;
        }
    }

    if (objIDisDefault)
        outputTableDC.SetID(titleStr);
    else
        outputTableDC.SetID(GetID());

}

int DFO_SATableToTable::GetNIndex(DC_SATable::SATableComponent  saTableComp)
{
    return inputSATableDC->GetNType(saTableComp);
}

