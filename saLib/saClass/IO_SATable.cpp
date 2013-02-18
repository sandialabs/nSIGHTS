///////////////////////////////////////////////////////////////////////////////////
//
// IO_SATable.cpp
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

#include <sstream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <saClass/IO_SATable.h>

IO_SATable::I_SATableFile::I_SATableFile(const char* inputFile)
    : ConvFileC(inputFile, 8192)
{
}

void IO_SATable::I_SATableFile::Init()
{
    nextInc = 5;
    nextCount = 50;
}


void IO_SATable::I_SATableFile::ReadWIPPSATable(DC_SATable&    saTable)
{
    CFsetComment("!");

    CFreadNonBlank();
    int trnVersion = CFgetNextInt();
    if (trnVersion != 5)
        CFtextError("Expection version 5 transfer file");

    CFreadNonBlank();
    int nVar = CFgetNextInt();
    int nVector = CFgetNextInt();
    int nStep = CFgetNextInt();

    if (!saTable.Alloc(nStep, nVar, nVector))
        CFtextError("Could not allocate tables");

    // variable names
    CFreadNonBlank();
    for (int i = 0; i < nVar; i++)
        CFstreamText(saTable.variableNames[i], saTable.variableNames.GetStringLen());

    // special case for 1 step - if blank then 0.0
    if (nStep == 1)
    {
        if (CFreadText())
            saTable.stepValues[0] = CFstreamDouble();
        else
            saTable.stepValues[0] = 0.0;
    }
    else
    {
        // step values
        CFreadNonBlank();
        for (int i = 0; i < nStep; i++)
            saTable.stepValues[i] = CFstreamDouble();
    }

    // data
    CFreadNonBlank();
    for (int k = 0; k < nVector; k++)
        for (int i = 0; i < nStep; i++)
            for (int j = 0; j < nVar; j++)
                saTable.saData[i][j][k] = CFstreamDouble();

}

void IO_SATable::I_SATableFile::ReadGoldsimDVTable(DC_SATable&  saTable)
{
    // get realization count from first line
    CFreadNonBlank();
    int nVector = -1;  // first col is time
    char tempStr[80];
    while (CFgetNextStr(tempStr, 80))
        nVector++;

    // skip next line
    CFreadNonBlank();

    // get step count
    int nStep = 0;
    while((!CFeof()) && CFreadText())
        nStep++;

    if (!saTable.Alloc(nStep, 1, nVector))
        CFtextError("Could not allocate tables");

    // start at beginning again
    CFreset();

    // skip first two lines
    CFskipLines(2);

    // read data
    for (int i = 0; i < nStep; i++)
    {
        CFreadNonBlank();
        saTable.stepValues[i] = CFgetNextDouble();
        saTable.stepIndexes[i] = i;

        for (int j = 0; j < nVector; j++)
            saTable.saData[i][0][j] = CFgetNextDouble();
    }

}


void IO_SATable::I_SATableFile::ReadGoldsimIVTable(DC_SATable&  saTable)
{
    // skip first
    CFreadNonBlank();

    // get variable count from second line
    CFreadNonBlank();
    int nVariable = -1; // don't count "Realization" string
    char tempStr[80];
    while (CFgetNextStr(tempStr, 80))
        nVariable++;

    // get realizationcount
    int nVector = 0;
    while((!CFeof()) && CFreadText())
        nVector++;

    if (!saTable.Alloc(1, nVariable, nVector))
        CFtextError("Could not allocate tables");

    // start at beginning again
    CFreset();

    // skip first line
    CFskipLines(1);

    // get variable names from second line
    CFreadNonBlank();

    // skip "Realization" string
    CFgetNextStr(tempStr, 80);

    for (int i = 0; i < nVariable; i++)
    {
        CFgetNextStr(tempStr, 80);
        saTable.variableNames.SetString(tempStr, i);
    }

    saTable.stepValues[0] = 0.0;

    // read data
    for (int i = 0; i < nVector; i++)
    {
        CFreadNonBlank();
        if (CFgetNextInt() != (i + 1))
            CFtextError("realization index incorrect ??");

        for (int j = 0; j < nVariable; j++)
            saTable.saData[0][j][i] = CFgetNextDouble();
    }
}

IO_SATable::O_SATableFile::O_SATableFile(const char* outputFile)
    :TxtFileC(outputFile, false)
{
}

void IO_SATable::O_SATableFile::Init()
{
}

void IO_SATable::O_SATableFile::WriteWIPPSATable(const DC_SATable&  saTable)
{
    const int versionNumber = 5;
    WriteInt(versionNumber);
    WriteLine();

    char outStr[256];
    if (IsNotNull(saTable.originalFile))
    {
        MakeString(outStr, "! From file: ", saTable.originalFile, 256);
        WriteText(outStr);
    }

    if (saTable.processingSteps.IsNotEmpty())
    {
        WriteText("! Processing:");
        for (int i = 0; i < saTable.processingSteps.Size(); i++)
        {
            MakeString(outStr, "! ", saTable.processingSteps[i], 256);
            WriteText(outStr);
        }
    }

    //  sizes
    WriteInt(saTable.GetNVariable());
    WriteInt(saTable.GetNVector());
    WriteInt(saTable.GetNStep());
    WriteLine();

    // steps
    SetRealConvFormat(SC_DecFormat(3));
    for (int i = 0; i < saTable.GetNStep(); i++)
        WriteDouble(saTable.stepValues[i]);
    WriteLine();

    // variables
    int colCount = 0;
    for (int i = 0; i < saTable.GetNVariable(); i++)
    {
        WriteSolidText(saTable.GetVariableName(i));
        colCount++;
        if (colCount == 5)
        {
            WriteLine();
            colCount = 0;
        }
    }
    if (colCount > 0)
        WriteLine();

    // and data
    SetRealConvFormat(SC_SciFormat(6));

    for (int k = 0; k < saTable.GetNVector(); k++)
    {
        for (int i = 0; i < saTable.GetNStep(); i++)
        {
            colCount = 0;
            for (int j = 0; j < saTable.GetNVariable(); j++)
            {
                WriteDouble(saTable.saData[i][j][k]);
                colCount++;
                if (colCount == 5)
                {
                    WriteLine();
                    colCount = 0;
                }
            }
            if (colCount > 0)
                WriteLine();
        }
        WriteLine();
    }

};


bool IO_SATable::ReadSATable(const char* inputFileName,
                                        SAFileFormat   saFileFormat,
                                        DC_SATable&    saTable)
{
    try
    {
        I_SATableFile cmFile(inputFileName);
        cmFile.CFsetMsgStuff("ReadSA", "SAFile", 50);
        cmFile.Init();
        saTable.DeAlloc();
        switch (saFileFormat)
        {
            case saffSandiaWIPP : {
                cmFile.ReadWIPPSATable(saTable);
                break;
            }
            case saffGoldsimDepVar : {
                cmFile.ReadGoldsimDVTable(saTable);
                break;
            }
            case saffGoldsimIndepVar : {
                cmFile.ReadGoldsimIVTable(saTable);
                break;
            }
            default: {
                cmFile.CFtextError("other is unsupported");
                break;
            }
        }
        CopyString(saTable.originalFile, inputFileName, stdFileStrLen);
        saTable.SetInitialIndexes();
    }
    catch (ConvFileC::CFReadError re) {
        saTable.DeAlloc();
        GenAppErrorMsg("ReadSATable", re.errMsg);
        return false;
    }

    {
        using namespace std;
        ostringstream ostr;
        ostr <<  "File read OK: "
                << saTable.GetNStep()  << " steps "
                << saTable.GetNVariable()  << " variables "
                << saTable.GetNVector()  << " vectors "
                << ends;
        GenAppInfoMsg("ReadTran", ostr.str().c_str());
    }
    return true;

}

void IO_SATable::WriterSATable(const char*      outputFileName,
                                        const DC_SATable& saTable)
{
    try
    {
        O_SATableFile cmFile(outputFileName);
        cmFile.Init();
        cmFile.WriteWIPPSATable(saTable);
    }
    catch (TextC::TextError re) {
        GenAppErrorMsg("WriteSATable", re.errMsg);
        return;
    }

    GenAppInfoMsg("WriteSATable", "file written successfully");
    return;
}

