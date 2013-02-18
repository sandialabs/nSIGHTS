///////////////////////////////////////////////////////////////////////////////////
//
// LFO_SATable.cpp
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
#include <genClass/U_Real.h>

#include <saClass/DO_SATable.h>

#include <saLFO/LFO_SATable.h>

LFO_SATable :: LFO_SATable(ListDefC& assLst)
    : ListObjC("List SATable", assLst)
{
    listFormat = lfInput;
    InitCommon();
}

LFO_SATable::LFO_SATable(const LFO_SATable& a, ListDefC& assLst) : ListObjC(a, assLst)
{
    LocalCopy(a);
    InitCommon();
}

LFO_SATable :: ~LFO_SATable ()
{
}

void LFO_SATable::InitCommon()
{
    AddInPort(tableObjRef, typeid(DO_SATable));
    DoStatusChk();
}

LFO_SATable& LFO_SATable::operator= (const LFO_SATable& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void LFO_SATable::LocalCopy(const LFO_SATable& a)
{
    tableObjRef         = a.tableObjRef;
    listFormat          = a.listFormat;
}


void LFO_SATable::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(tableObjRef, "SATable");
    if (listFormat != lfInput)
        SetObjErrMsg("only Input format available");
}

void LFO_SATable::CreateListing()
{
    InputFormatListing();
}

void LFO_SATable::InputFormatListing()
{
    const DC_SATable& saTable = static_cast<DO_SATable*>(GetInPortData(tableObjRef))->GetSATableRef();

    if (IsNotNull(saTable.originalFile) || saTable.processingSteps.IsNotEmpty())
    {
        char outStr[256];

        SubTitle("QA Info:");
        if (IsNotNull(saTable.originalFile))
        {
            MakeString(outStr, "From file: ", saTable.originalFile, 256);
            AddLine(outStr);
        }

        if (saTable.processingSteps.IsNotEmpty())
        {
            AddLine("Processing:");
            for (int i = 0; i < saTable.processingSteps.Size(); i++)
            {
                MakeString(outStr, "! ", saTable.processingSteps[i], 256);
                AddLine(outStr);
            }
        }

        AddConditionalBlank();
    }

    SubTitle("Size");
    AddStdInt("Number of steps", saTable.GetNStep());
    AddStdInt("Number of variables", saTable.GetNVariable());
    AddStdInt("Number of vectors", saTable.GetNVector());

    const int maxCol = 5;
    SC_IntArray colWidths(maxCol, 12);
    TableSetup(colWidths, 2);
    tableLeftJustify.FillToAlloc(true);

    SubTitle("Variables");
    int colNum = 0;
    for (int i = 0; i < saTable.GetNVariable(); i++)
    {
        SetTableCol(colNum++, saTable.GetVariableName(i));
        if (colNum == maxCol)
        {
            AddNextLine();
            colNum = 0;
        }
    }
    if (colNum > 0)
        AddNextLine();
    AddConditionalBlank();

    SubTitle("Steps");
    tableLeftJustify.FillToAlloc(false);
    colNum = 0;
    for (int i = 0; i < saTable.GetNStep(); i++)
    {
        SetTableReal(colNum++, saTable.stepValues[i], SC_DecFormat(3));
        if (colNum == maxCol)
        {
            AddNextLine();
            colNum = 0;
        }
    }
    if (colNum > 0)
        AddNextLine();
    AddConditionalBlank();



    SubTitle("Data");
    for (int k = 0; k < saTable.GetNVector(); k++)
    {
        for (int i = 0; i < saTable.GetNStep(); i++)
        {
            colNum = 0;
            for (int j = 0; j < saTable.GetNVariable(); j++)
            {
                SetTableReal(colNum++, saTable.saData[i][j][k], SC_SciFormat(6));
                if (colNum == maxCol)
                {
                    AddNextLine();
                    colNum = 0;
                }
            }
            if (colNum > 0)
                AddNextLine();
        }
        AddConditionalBlank();
    }

}


