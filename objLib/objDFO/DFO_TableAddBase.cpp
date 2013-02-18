///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableAddBase.cpp
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

#include <objDFO/DFO_TableAddBase.h>

DFO_TableAddBase::TableInputData::TableInputData() :
    doTable(false)
{
}
DFO_TableAddBase::TableInputData::TableInputData(const TableInputData& a)
{
    LocalCopy(a);
}
DFO_TableAddBase::TableInputData& DFO_TableAddBase::TableInputData::operator= (const TableInputData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void DFO_TableAddBase::TableInputData::LocalCopy(const TableInputData& a)
{
    doTable     = a.doTable;
    tableObjRef = a.tableObjRef;
}

DFO_TableAddBase :: DFO_TableAddBase(const char* objID, const std::type_info& tblType, DO_TableData& outTblDO, DC_TableData& outTblDC)
    : FuncObjC(objID), tableType(tblType), outputTableDO(outTblDO), outputTableDC(outTblDC), inputTableDC(32)
{
    skipXColumn     = true;
    xcolumnIndex    = 0;

    InitCommon();
}

DFO_TableAddBase::DFO_TableAddBase(const DFO_TableAddBase& a, DO_TableData& outTblDO, DC_TableData& outTblDC)
    : FuncObjC(a), tableType(a.tableType), outputTableDO(outTblDO), outputTableDC(outTblDC), inputTableDC(32)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableAddBase :: ~DFO_TableAddBase ()
{
}

void DFO_TableAddBase::InitCommon()
{
    AddOutPort(outputTableDO);

    for (int i = 0; i < maxInputTable; i++)
        AddUseFlagInPort(tablesToAdd[i].tableObjRef, tableType, tablesToAdd[i].doTable);

    tablesToAdd[0].doTable = true;

    DoStatusChk();
}

DFO_TableAddBase& DFO_TableAddBase::operator= (const DFO_TableAddBase& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_TableAddBase::LocalCopy(const DFO_TableAddBase& a)
{
    skipXColumn     = a.skipXColumn;
    xcolumnIndex    = a.xcolumnIndex;

    for (int i = 0; i < maxInputTable; i++)
        tablesToAdd[i] = a.tablesToAdd[i];
}


void  DFO_TableAddBase:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    // check input objects
    inputTableDC.SetEmpty();

    ncols = -1;
    nrows = -1;
    for (int i = 0; i < maxInputTable; i++)
        if (tablesToAdd[i].doTable)
        {
            if (!CheckInputFO(tablesToAdd[i].tableObjRef, "input table"))
                return;

            const DC_TableData* nextTbl = static_cast<DO_TableData*>(GetInPortData(tablesToAdd[i].tableObjRef))->tableData;
            if (ncols < 0)
            {
                ncols = nextTbl->GetNCols();
                nrows = nextTbl->GetNRows();
            }

            if ((ncols != nextTbl->GetNCols()) || (nrows != nextTbl->GetNRows()))
            {
                SetObjErrMsg("number of cols/rows mismatch -- cannot add");
                return;
            }

            inputTableDC += nextTbl;
        }

    if (inputTableDC.IsEmpty())
    {
        SetObjErrMsg("no tables to sum ??");
        outputTableDC.ClearData();
        return;
    }

    if (skipXColumn && (xcolumnIndex >= ncols))
        SetObjErrMsg("X column is out of range ??");

    if (StatusNotOK())
    {
        outputTableDO.tableData = 0;
        outputTableDC.ClearData();
    }
}


void DFO_TableAddBase::CalcOutput(FOcalcType  calcType)
{
    outputTableDO.tableData = &outputTableDC;

    SC_DoubleMatrix& outTbl = outputTableDC.dataTable;

    if (skipXColumn)
    {
        const SC_DoubleMatrix& currTbl = inputTableDC[0]->dataTable;
        outTbl[xcolumnIndex] = currTbl[xcolumnIndex];
    }


    for (int i = 0; i < inputTableDC.Size(); i++)
    {
        const SC_DoubleMatrix& currTbl = inputTableDC[i]->dataTable;

        for (int j = 0; j < ncols; j++)
        {
            if ((!skipXColumn) || (j != xcolumnIndex))
                for (int k = 0; k < nrows; k++)
                    outTbl[j][k] += currTbl[j][k];
        }
    }

    outputTableDC.SetID(GetID());

}


