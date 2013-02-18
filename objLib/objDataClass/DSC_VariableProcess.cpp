///////////////////////////////////////////////////////////////////////////////////
//
// DSC_VariableProcess.cpp
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

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_File.h>

#include <objDataClass/DSC_VariableProcess.h>

DSC_VariableProcess::ExtendedVar::ExtendedVar()
{
    SetToNull(varString);
    varIDStrLen = 0;
    varStrLen = 0;
}

DSC_VariableProcess::ExtendedVar::ExtendedVar(const ExtendedVar& a)
{
    LocalCopy(a);
}

DSC_VariableProcess::ExtendedVar&  DSC_VariableProcess::ExtendedVar::operator= (const ExtendedVar& a)
{
    if (&a != this)
    {
        DC_Variable::operator =(a);
        LocalCopy(a);
    }
    return *this;
}

void DSC_VariableProcess::ExtendedVar::LocalCopy(const ExtendedVar& a)
{
    CopyString(varString, a.varString, varIDLen);
    specifiedLength = a.specifiedLength;
    varIDStrLen = a.varIDStrLen;
    varStrLen = a.varStrLen;
}

DSC_VariableProcess::ExtendedVar&  DSC_VariableProcess::ExtendedVar::operator= (const DC_Variable& a)
{
    DC_Variable::operator =(a);
    RealToString(SC_SciFormat(10), varValue, varString, varIDLen);
    SetLens();
    return *this;
}

void DSC_VariableProcess::ExtendedVar::SetLens()
{
    varIDStrLen = StringLength(varID);
    varStrLen = StringLength(varString);
}

bool DSC_VariableProcess::ExtendedVar::ProcessLine(char*        inputLine,
                                                   int          maxLen,
                                                   bool&        lineModified,
                                                   SC_SetupErr& processErrors)
{
    while (true) {
        // repeat
        char* foundPos = strstr(inputLine, varID);
        if (foundPos == NULL)
            return true;

        enum RepType {rpInsert, rpLeft, rpRight};
        RepType repType = rpInsert;

        char* insertPos = foundPos;
        if (foundPos != inputLine)
        {
            // not at beginning could have justify spec
            insertPos--;
            if (*insertPos == '~')
            {
                repType = rpLeft;
            }
            else if (*insertPos == '^')
            {
                repType = rpRight;
            }
            else if (*insertPos != ' ')
            {
                // vars must be delimted with blanks or just markers
                return true;
            }
        }

        // must have blank or be at end
        char* endPos = foundPos + varIDStrLen;
        if ((*endPos != ' ') && (*endPos != '\0'))
            return true;

        lineModified = true;

        if (repType != rpInsert)
        {
            char repStr[varIDLen];
            CopyString(repStr, varString, varIDLen);
            int repStrLen = IMax(varIDStrLen+1, varStrLen, specifiedLength);

            int foundStartIndex = int(insertPos - inputLine);
            if (repType == rpRight)
            {
                int foundEndIndex = foundStartIndex + varIDStrLen + 1;
                foundStartIndex = foundEndIndex - repStrLen;
                if (foundStartIndex < 0)
                {
                    processErrors.SetInternalError(varID, " is right justified before line start");
                    return false;
                }
                if (repStrLen > varStrLen)
                    RightJustify(repStr, repStrLen);
            }
            else
            {
                if (repStrLen > varStrLen)
                    LeftJustify(repStr, repStrLen);
            }
            DeleteChar(inputLine, foundStartIndex, repStrLen);
            InsertString(inputLine, repStr, foundStartIndex, DSC_VariableProcess::maxInputLength);
        }
        else
        {
            int foundStartIndex = int(foundPos - inputLine);
            DeleteChar(inputLine, foundStartIndex, varIDStrLen);
            InsertString(inputLine, varString, foundStartIndex, DSC_VariableProcess::maxInputLength);
        }
    }
    return true;
}

int DSC_VariableProcess::ExtendedVarArray::GetVarIndex(const char* varID) const
{
    for (int i = 0; i < Size(); i++)
        if (SameUCString((*this)[i].GetID(), varID))
            return i;
    return -1;
}


DSC_VariableProcess::DSC_VariableProcess()
{
    tokens.SetStringLen(maxInputLength);
    tokens.SetResizable(8);
    outFileIsOpen = false;
    currVars.SetResizable(16);
    forceRecordLen = false;
    recordLength = 80;

}

DSC_VariableProcess::DSC_VariableProcess(const DSC_VariableProcess& a)
{

}


DSC_VariableProcess& DSC_VariableProcess::operator= (const DSC_VariableProcess& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void DSC_VariableProcess::LocalCopy(const DSC_VariableProcess& a)
{
}

void DSC_VariableProcess::ExtractToken(char*& tokenPos,
                                       char* nextToken)
{
    while(NotWhiteSpace(*tokenPos) && (*tokenPos != '\0')) {
        *nextToken = *tokenPos;
        nextToken++;
        tokenPos++;
    }
    *nextToken = '\0';

    // move to next string start
    while(!NotWhiteSpace(*tokenPos) && (*tokenPos != '\0')) {
        tokenPos++;
    }
}

void DSC_VariableProcess::ExtractTokens()
{
    tokens.SetEmpty();
    char* tokenPos = &inputLine[2]; // skip $$
    char nextToken[maxInputLength];
    while (*tokenPos != '\0')
    {
        ExtractToken(tokenPos, nextToken);
        tokens += nextToken;
    }
}


bool DSC_VariableProcess::CheckCommand(const char* cmdText)
{
    return SameUCString(cmdText, tokens[0]);
}

bool DSC_VariableProcess::ProcessVarFormat(ExtendedVar& var,
                                            const char*  token,
                                            SC_SetupErr& processErrors)
{
    char formatStr[40];
    CopyString(formatStr, token, 40);
    UpperCase(formatStr);

    SC_RealFormat varForm;
    bool requiresPrecision = true;
    switch (formatStr[0]) {
        case 'G' : {
            varForm.format = ncf_General;
            requiresPrecision = false;
            break;
        }
        case 'D' : case 'F' : {
            varForm.format = ncf_Decimal;
            break;
        }
        case 'E' : {
            varForm.format = ncf_Scientific;
            break;
        }
        default : {
            processErrors.SetInternalError(token, ": unrecognized format, expecting G(eneral), D(ecimal), E(xponential/scientific), F(ixed)");
            return false;
        }
    }

    if (requiresPrecision)
    {
        char* precSrc = strchr(formatStr, '.');
        if (precSrc != NULL)
            precSrc++;
        else
            precSrc= &formatStr[1];

        char precString[40];
        char* precDest = &precString[0];
        char* tempSrc = precSrc;
        while (*tempSrc != '\0')
        {
            *precDest = *tempSrc;
            tempSrc++;
            precDest++;
        }
        *precDest = '\0';

        if (!IsValidInt(precString, varForm.digitsPrecision, false))
        {
            processErrors.SetInternalError(precString, ": invalid precision");
            return false;
        }
        // clear precision
        *precSrc = '\0';
    }

    // do basic conversion
    varForm.RealToString(var.GetValue(), var.varString, ExtendedVar::varIDLen);


    // check length
    var.specifiedLength = -1;
    char* lenSrc = &formatStr[1];
    if (!IsNull(lenSrc))
    {
        if (!IsValidInt(lenSrc, var.specifiedLength, false))
        {
            processErrors.SetInternalError(lenSrc, ": invalid length");
            return false;
        }
    }

    var.SetLens();
    return true;
}

bool DSC_VariableProcess::ProcessVarValue(ExtendedVar& var,
                                            const char*  token,
                                            SC_SetupErr& processErrors)
{
    // must be newVal;
    CopyString(var.varString, token, DC_Variable::varIDLen);
    FullTrim(var.varString);
    var.specifiedLength = StringLength(var.varString);
    ReplaceUnderscores(var.varString);
    FullTrim(var.varString);

    if (!IsValidReal(var.varString, var.varValue))
    {
        processErrors.SetInternalError(var.varString, " is not a valid real");
        return false;
    }
    // pathological errors
    if (RealIsNull(var.varValue))
    {
        processErrors.SetInternalError(var.varString, " -- null real not allowed");
        return false;
    }
    if (!IsFinite(var.varValue))
    {
        processErrors.SetInternalError(var.varString, " -- must be finite");
        return false;
    }

    var.SetLens();
    return true;
}

bool DSC_VariableProcess::ProcessVarCommand(SC_SetupErr& processErrors)
{
//  VAR newID newVal
//  VAR newID oldID format
//  VAR oldID newVal
//  VAR oldID format

    if (tokens.Size() < 3)
    {
        processErrors.SetConstantError("VAR command requires at least 2 arguments");
        return false;
    }

    int varIndex = currVars.GetVarIndex(tokens[1]);

    if (varIndex < 0)
    {
        // new Var - next token is value and string
        //  VAR newID newVal
        //  VAR newID oldID format

        ExtendedVar newVar;
        newVar.SetID(tokens[1]);

        int oldVar = currVars.GetVarIndex(tokens[2]);
        if (oldVar < 0)
        {
            // must be value
            if (!ProcessVarValue(newVar, tokens[2], processErrors))
                return false;
        }
        else
        {
            // reformatted new string
            newVar.varValue = currVars[oldVar].GetValue();
            if (tokens.Size() != 4)
            {
                processErrors.SetConstantError("Expecting VAR newID oldID format");
                return false;
            }
            if (!ProcessVarFormat(newVar, tokens[3], processErrors))
                return false;
        }
        currVars += newVar;
    }
    else
    {
        //  VAR oldID newVal
        //  VAR oldID format
        ExtendedVar& currVar = currVars[varIndex];

        // try value
        double oldVal = currVar.GetValue();
        if (ProcessVarValue(currVar, tokens[2], processErrors))
            return true;

        // not a value, must be a format, restore value
        currVar.SetValue(oldVal);
        if (!ProcessVarFormat(currVar, tokens[2], processErrors))
            return false;
    }

    return true;
}

bool DSC_VariableProcess::ProcessIfCommand(SC_SetupErr& processErrors,
                                           bool& conditionTrue)
{
//  IF|ELSEIF val|var condition val|var
//  condition is one of <, <=, =, >=, >, !=

    if (tokens.Size() != 4)
    {
        processErrors.SetConstantError("IF|ELSEIF command requires 3 arguments");
        return false;
    }

    // LHS of condition
    int lsVarIndex = currVars.GetVarIndex(tokens[1]);
    double lsVarValue = 0.0;
    if (lsVarIndex < 0)
    {
        if (!IsValidReal(tokens[1], lsVarValue))
        {
            processErrors.SetConstantError("IF|ELSEIF LHS argument not variable or value");
            return false;
        }
    }
    else
        lsVarValue = currVars[lsVarIndex].GetValue();


    // condition
    static const char* conditionStrs[] = {"<", "<=", "=", "==", ">=", ">", "!="};
    int conditionCode = -1;
    for (int i = 0; i < 7; i++)
        if (SameString(conditionStrs[i], tokens[2]))
        {
            conditionCode = i;
            break;
        }
    if (conditionCode < 0)
    {
        processErrors.SetConstantError("IF|ELSEIF condition argument not recognized");
        return false;
    }

    int rsVarIndex = currVars.GetVarIndex(tokens[3]);
    double rsVarValue = 0.0;
    if (rsVarIndex < 0)
    {
        if (!IsValidReal(tokens[3], rsVarValue))
        {
            processErrors.SetConstantError("IF|ELSEIF RHS argument not variable or value");
            return false;
        }
    }
    else
        rsVarValue = currVars[rsVarIndex].GetValue();


//  static const char* conditionStrs[] = {"<", "<=", "=", "==", ">=", ">", "!="};
    switch (conditionCode) {
        case 0 : {  // <
            conditionTrue = lsVarValue < rsVarValue;
            break;
        }
        case 1 : {  // <=
            conditionTrue = lsVarValue <= rsVarValue;
            break;
        }
        case 2 : case 3 : {  // =, ==
            conditionTrue = lsVarValue == rsVarValue;
            break;
        }
        case 4 : {  // >=
            conditionTrue = lsVarValue >= rsVarValue;
            break;
        }
        case 5 : {  // >
            conditionTrue = lsVarValue > rsVarValue;
            break;
        }
        case 6 : {  // !=
            conditionTrue = lsVarValue != rsVarValue;
            break;
        }
    }

    return true;
}



void DSC_VariableProcess::WriteComment(const char* comment)
{
    if (commentsOn && outFileIsOpen)
    {
        *outFile.GetStream() << commentStr << inputLine << std::endl;
    }
}

void DSC_VariableProcess::WriteProcessedInput()
{
    if (outFileIsOpen)
    {
        if (forceRecordLen)
            LeftJustify(inputLine, recordLength);
        *outFile.GetStream() << inputLine << std::endl;
    }
}

bool DSC_VariableProcess::ProcessOneLine(SC_SetupErr& processErrors)
{
    if (!skipProcessing)
    {
        char originalLine[maxInputLength];
        CopyString(originalLine, inputLine, maxInputLength);
        bool lineModified = false;
        for (int i = 0; i < currVars.Size(); i++)
            if (!currVars[i].ProcessLine(inputLine, maxInputLength, lineModified, processErrors))
                return false;
        if (lineModified)
            WriteComment(originalLine);

    }
    WriteProcessedInput();
    return true;
}

bool DSC_VariableProcess::ProcessOneFile(const char* fName,
                                           SC_SetupErr& processErrors)
{
    TxtFileC inputF(fName, true);

    bool processingIF = false;
    bool ifConditionTrue = false;
    bool trueConditionProcessed = false;

    while (!inputF.IsEOF())
    {
        inputF.GetStream()->getline(inputLine, maxInputLength);
        if (!inputF.IsEOF())
            inputF.CheckTextError("read error on getline ??");

        // skip zero length lines
        if (IsNull(inputLine))
        {
            WriteProcessedInput();
            continue;
        }

        // all commands start with $$
        if ((inputLine[0] != '$') || (inputLine[1] != '$'))
        {
            // skip if in IF
            if (processingIF && (!ifConditionTrue))
                continue;

            if (!ProcessOneLine(processErrors))
                return false;
            continue;
        }

        // assume commands, process all
        ExtractTokens();
        if (tokens.IsEmpty())
        {
            // not a command, just echo input
            WriteProcessedInput();
            continue;
        }

        // process commands
        if (CheckCommand("VAR"))
        {
            if (!ProcessVarCommand(processErrors))
                return false;
        }
        else if (CheckCommand("SKIPON"))
        {
            skipProcessing = true;
        }
        else if (CheckCommand("SKIPOFF"))
        {
            skipProcessing = false;
        }
        else if (CheckCommand("INCLUDE"))
        {
            if (tokens.Size() != 2)
            {
                processErrors.SetConstantError("INCLUDE command requires single file name argument");
                return false;
            }

            if (!FileExists(tokens[1]))
            {
                processErrors.SetInternalError("INCLUDE file not found:", tokens[1]);
                return false;
            }

            // recursive call
            if (!ProcessOneFile(tokens[1], processErrors))
                return false;
        }
        else if (CheckCommand("RECLEN"))
        {
            if (tokens.Size() != 2)
            {
                processErrors.SetConstantError("RECLEN requires single integer argument");
                return false;
            }
            if (!IsValidInt(tokens[1], recordLength))
            {
                processErrors.SetConstantError("RECLEN argument not integer");
                return false;
            }
            if (recordLength > maxInputLength)
            {
                processErrors.SetConstantError("max record length is 2048");
                return false;
            }
            forceRecordLen = (recordLength > 0);
        }
        else if (CheckCommand("CLOSEOUT"))
        {
            if (outFileIsOpen)
                outFile.Close();
            outFileIsOpen = false;
        }
        else if (CheckCommand("COMMENT"))
        {
            if (tokens.Size() == 1)
                commentsOn = false;
            else
            {
                CopyString(commentStr, tokens[1], 40);
                commentsOn = true;
            }
        }
        else if (CheckCommand("OPENOUT"))
        {
            if (tokens.Size() != 2)
            {
                processErrors.SetConstantError("OPENOUT command requires single file name argument");
                return false;
            }

            if (outFileIsOpen)
                outFile.Close();
            outFile.OpenFile(tokens[1], false);
            outFileIsOpen = true;
        }
        else if (CheckCommand("IF"))
        {
            if (processingIF)
            {
                processErrors.SetConstantError("nested IFs not allowed - use ELSEIF");
                return false;
            }

            if (!ProcessIfCommand(processErrors, ifConditionTrue))
                return false;
            processingIF = true;
            trueConditionProcessed = ifConditionTrue;
        }
        else if (CheckCommand("ELSEIF"))
        {
            if (!processingIF)
            {
                processErrors.SetConstantError("ELSEIF before initial IF");
                return false;
            }

            // already done ?
            if (trueConditionProcessed)
            {
                ifConditionTrue = false;
            }
            else
            {
                if (!ProcessIfCommand(processErrors, ifConditionTrue))
                    return false;
                trueConditionProcessed = ifConditionTrue;
            }
        }
        else if (CheckCommand("ELSE"))
        {
            if (!processingIF)
            {
                processErrors.SetConstantError("ELSE before initial IF");
                return false;
            }
            // already done ?
            if (trueConditionProcessed)
            {
                ifConditionTrue = false;
            }
            else
            {
                trueConditionProcessed = true;
                ifConditionTrue = true;
            }
        }
        else if (CheckCommand("ENDIF"))
        {
            if (!processingIF)
            {
                processErrors.SetConstantError("ENDIF before initial IF");
                return false;
            }

            processingIF = false;
        }
        else
        {
            WriteProcessedInput();
            continue;
        }

        // was a command - echo
        WriteComment(inputLine);
    }

    return true;
}



bool DSC_VariableProcess::ProcessVariables(const DC_VariableArray& inVars,
                                             const char* templateFile,
                                             const char* outputFile,
                                             SC_SetupErr& processErrors)
{
    int nvar = inVars.Size();
    currVars.AllocAndSetSize(nvar);
    for (int i = 0; i < nvar; i++)
        currVars[i] = inVars[i];

    skipProcessing = false;
    bool processOK = false;
    outFileIsOpen = false;
    commentsOn = false;
    try {
        processErrors.SetConstantError("error opening output file");
        if (IsNotNull(outputFile))
        {
            outFile.OpenFile(outputFile, false);
            outFileIsOpen = true;
        }
        processOK = ProcessOneFile(templateFile, processErrors);
        if (outFileIsOpen)
            outFile.Close();
    }
    catch (TextC::TextError re) {
        processErrors.SetInternalError("text I/O", re.errMsg);
        return false;
    }

    return processOK;
}



