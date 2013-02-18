///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableSample.cpp
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

#include <genClass/U_Real.h>
#include <genClass/U_Str.h>
#include <genClass/DO_XYCDF.h>

#include <objDFO/DFO_TableSample.h>

DFO_TableSample :: DFO_TableSample() :
    FuncObjC("Create Sample Table")
{
    createNewTable  = true;
    nsamples        = 500;
    randomSeed      = 2347533;
    stratifySample  = true;
    randomMix       = true;

    InitCommon();
}

DFO_TableSample::DFO_TableSample(const DFO_TableSample& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableSample :: ~DFO_TableSample ()
{
}

void DFO_TableSample::InitCommon()
{
    outputTableDC.SetID("Samples");
    outputTableDO.tableData = &outputTableDC;
    AddOutPort(outputTableDO);

    AddUseFlagInPort(existingTableObjRef, typeid(DO_TableData), useExistingTable);

    AddInPort(sampVar.userCDFInputObjRef, typeid(DO_XYCDF));
    userCDFInputCheckRef = GetLastInPortCheckRef();

    AddInPort(sampVar.tableLookupObjRef, typeid(DO_TableData));
    tableLookupCheckRef = GetLastInPortCheckRef();

    SetDefaultID("Sample");

    DoStatusChk();
}

DFO_TableSample& DFO_TableSample::operator= (const DFO_TableSample& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableSample::LocalCopy(const DFO_TableSample& a)
{
    existingTableObjRef = a.existingTableObjRef;
    createNewTable      = a.createNewTable;
    nsamples            = a.nsamples;
    randomGenerator     = a.randomGenerator;
    randomSeed          = a.randomSeed;
    existingTableObjRef = a.existingTableObjRef;
    stratifySample      = a.stratifySample;
    sampVar             = a.sampVar;
    randomMix           = a.randomMix;
}


void  DFO_TableSample:: DoStatusChk()
{
    useExistingTable = ! createNewTable;
    *userCDFInputCheckRef = sampVar.IsUserCDF();
    *tableLookupCheckRef = sampVar.IsTableLookup();
    FuncObjC::DoStatusChk();

    if (useExistingTable)
    {
        if (!CheckInputFO(existingTableObjRef, "table input data source"))
            return;
        DO_SampleTable* tmpDO = static_cast<DO_SampleTable*>(GetInPortData(existingTableObjRef));

        existingTableDC = tmpDO->tableData;
        if (existingTableDC->GetNCols() < 1)
            SetObjErrMsg("table must have at least 1 column");
        if (existingTableDC->GetNRows() < 2)
            SetObjErrMsg("table must have more than 1 row");

        currRandGen = tmpDO->randomGenerator;
    }
    else
        currRandGen = &randomGenerator;

    outputTableDO.randomGenerator = currRandGen;

    SC_SetupErr sampErr;
    if (!sampVar.SampleSetupOK(sampErr, GetID()))
        SetObjErrMsg(sampErr.GetErrorText());

}

void DFO_TableSample:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int ncol = 1;
    int nrow = nsamples;
    int nchar = StringLength(GetID());
    if (useExistingTable)
    {
        ncol = existingTableDC->GetNCols() + 1;
        nrow = existingTableDC->GetNRows();
        int inputnchar = existingTableDC->GetStringLen();
        if (inputnchar > nchar)
            nchar = inputnchar;
    }

    outputTableDC.Alloc(ncol, nrow, nchar + 5);
    outputTableDC.SetSizeToAlloc();

    for (int i = 0; i < ncol - 1; i++)
    {
        outputTableDC.SetTitle(i, existingTableDC->GetTitle(i));
        existingTableDC->GetColData(i, outputTableDC.dataTable[i]);
    }
    outputTableDC.SetTitle(ncol - 1, GetID());

    for (int i = 0; i < nrow; i++)
    {
        char tempStr[80];
        CopyString(tempStr, "S", 80);
        ConcatInt(i + 1, tempStr, 4, 80);
        outputTableDC.rowDesc.SetString(tempStr, i);
    };

    if (createNewTable)
    {
        randomGenerator.SetupRandom(randomSeed);
    }

    SC_DoubleArray& outputCol = outputTableDC.GetColData(ncol - 1);
    sampVar.Sample(*currRandGen, nrow, stratifySample, outputCol);

    if (randomMix)
        currRandGen->RandomMix(outputCol);
}


