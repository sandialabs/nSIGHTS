///////////////////////////////////////////////////////////////////////////////////
//
// IO_VariableArray.cpp
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
//      I/O for DC_VariableArray
//
///////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <objClass/IO_VariableArray.h>

IO_VariableArray::I_VariableArrayFile::I_VariableArrayFile(const char* inputFile)
    : ConvFileC(inputFile, 8192)
{
}

void IO_VariableArray::I_VariableArrayFile::Init()
{
    nextInc = 5;
    nextCount = 50;
    CFsetComment("#!");
    SetParseChars(",");
}

void IO_VariableArray::I_VariableArrayFile::ReadPaCalc(DC_VariableArray& variableArray)
{
    variableArray.SetResizable(128);

    DC_Variable currVar;
    while (!CFeof())
    {
        if (!CFreadText())
            continue;

        // value first
        currVar.varValue = CFgetNextDouble();

        // remainder is variable 
        FullTrim(parsedLine);
        CopyString(currVar.varID, parsedLine, DC_Variable::varIDLen);

        variableArray += currVar;
    }
}



IO_VariableArray::O_VariableArrayFile::O_VariableArrayFile(const char* outputFile)
    :TxtFileC(outputFile, false)
{
}

void IO_VariableArray::O_VariableArrayFile::Init()
{
}

void IO_VariableArray::O_VariableArrayFile::WritePaCalc(const DC_VariableArray& variableArray)
{
    SetRealConvFormat(SC_SciFormat(12));
    
    for ( int i = 0; i < variableArray.Size(); i++)
    {
        WriteDouble(variableArray[i].varValue);
        WriteSolidText("\t");
        WriteText(variableArray[i].varID);
    }
};



bool IO_VariableArray::ReadVariableFile(       VariableFileFormat    variableFileFormat,
                                   const char*              inputFileName,
                                         DC_VariableArray&     variableArray)
{
    try
    {
        I_VariableArrayFile cmFile(inputFileName);
        cmFile.CFsetMsgStuff("ReadVariableArray", "VariableArrayFile", 100);
        cmFile.Init();
        switch (variableFileFormat)
        {
            case vffPaCalc : {
                cmFile.ReadPaCalc(variableArray);
                break;
            }
            case vffOther : {
                GenAppErrorMsg("ReadVariableArray", "other not supported");
                return false;
                break;
            }
        }
    }
    catch (ConvFileC::CFReadError re) {
        variableArray.SetEmpty();
        GenAppErrorMsg("ReadVariableArray", re.errMsg);
        return false;
    }

    {
        using namespace std;
        ostringstream ostr;
        ostr <<  "Label Data read OK: " << variableArray.Size() << " variables "<< ends;
        GenAppInfoMsg("ReadVariableArray", ostr.str().c_str());
    }
    return true;
}

void IO_VariableArray::WriteVariableFile(const char*          outputFileName,
                                   const DC_VariableArray& variableArray)
{
    try
    {
        O_VariableArrayFile cmFile(outputFileName);
        cmFile.Init();
        cmFile.WritePaCalc(variableArray);
    }
    catch (TextC::TextError re) {
        GenAppErrorMsg("WriteVariableArray", re.errMsg);
        return;
    }

    GenAppInfoMsg("WriteVariableArray", "file written successfully");
    return;
}



