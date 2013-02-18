///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableColScaleTransform.cpp
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
//      Creates an output table that is the same as the input table except a
//  scale/transform operation has been applied to the data in a single column
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <objDFO/DFO_TableColScaleTransform.h>

DFO_TableColScaleTransform :: DFO_TableColScaleTransform() :
    DFO_ScaleTransform("Sc/Tran Table Column")
{
    CopyString(outputColID, "S/TCol", idLen);
    operateOnExisting = true;
    columnsSel = col_single;

    InitCommon();
}

DFO_TableColScaleTransform::DFO_TableColScaleTransform(const DFO_TableColScaleTransform& a) :
    DFO_ScaleTransform(a), DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableColScaleTransform :: ~DFO_TableColScaleTransform ()
{
}

void DFO_TableColScaleTransform::InitCommon()
{
    DSC_TableColSel::AddPort(*this);
    AddOutPort(outputTableDO);
    DoStatusChk();
}

DFO_TableColScaleTransform& DFO_TableColScaleTransform::operator= (const DFO_TableColScaleTransform& a)
{
    if (&a != this)
    {
        DFO_ScaleTransform::operator=(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableColScaleTransform::LocalCopy(const DFO_TableColScaleTransform& a)
{
    thresholdOp             = a.thresholdOp;
    operateOnExisting       = a.operateOnExisting;
    columnsSel              = a.columnsSel;
    CopyString(outputColID,  a.outputColID, idLen);

}

void  DFO_TableColScaleTransform::DoStatusChk()
{
    DFO_ScaleTransform::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);

    if (StatusNotOK())
    {
        outputTableDO.tableData = 0;
        outputTableDC.ClearData();
    }
}

void DFO_TableColScaleTransform::DoScaleTransform(SC_DoubleArray& stData)
{
    for (int i = 0; i < stData.Size(); i++)
    {
        DSC_ScaleTransform::DoScaleTransform(stData[i]);
        stData[i] = thresholdOp.ApplyThresholds(stData[i]);
        if (processNulls && RealIsNull(stData[i]))
            stData[i] = setNullValue;
    }
}


void DFO_TableColScaleTransform:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int nrow = inputData->GetNRows();
    int ncol = inputData->GetNCols();
    if ((ncol == 0) || (nrow == 0))
    {
        SetObjErrMsg("No input table cols or rows");
        outputTableDO.tableData = 0;
        outputTableDC.ClearData();
        return;
    }

    if ((columnsSel == col_single) && (!operateOnExisting))
        ncol++;

    if (!outputTableDC.Alloc(ncol, nrow, inputData->columnDesc.GetStringLen()))
    {
        SetObjErrMsg("Table allocation error??");
        return;
    }

    outputTableDO.tableData = &outputTableDC;

    outputTableDC.rowDesc = inputData->rowDesc;
    for (int i = 0; i < inputData->GetNCols(); i++)
    {
        outputTableDC.SetTitle(i, inputData->columnDesc[i]);
        outputTableDC.dataTable[i] = inputData->dataTable[i];
    }
    outputTableDC.dataTable.SetSize(ncol);

    if (columnsSel == col_single)
    {
        SC_DoubleArray* outputCol = &outputTableDC.dataTable[selectedCol];
        if (!operateOnExisting)
        {
            outputTableDC.dataTable.LastIndex() = *outputCol;
            outputCol = &outputTableDC.dataTable.LastIndex();
            outputTableDC.columnDesc += outputColID;
        }
        else
            outputTableDC.SetTitle(selectedCol, outputColID);

        DoScaleTransform(*outputCol);
    }
    else
    {
        for (int i = 0; i < ncol; i++)
        {
            if ((columnsSel == col_skip) && (selectedCol == i))
                continue;
            SC_DoubleArray* outputCol = &outputTableDC.dataTable[i];
            DoScaleTransform(*outputCol);
        }
    }

    outputTableDC.SetID("S/T table");

}

