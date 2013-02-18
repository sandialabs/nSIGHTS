///////////////////////////////////////////////////////////////////////////////////
//
// DSC_Parser.cpp
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
// DESCRIPTION: this class converts a string into a parsed representation
//              based on Amy Gilkey's Fortran for WIPP PA
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/SC_RealFormat.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/U_Real.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genDataClass/DSC_Parser.h>


DSC_Parser::Field::Field()
{
    fieldType = ft_NotSet;
    fChar = ' ';
    SetToNull(fStr);
    fVal = 0.0;
}



DSC_Parser :: DSC_Parser()
{
    fields.SetResizable(maxField);
}


DSC_Parser::DSC_Parser(const DSC_Parser& a)
{
    LocalCopy(a);
}


DSC_Parser& DSC_Parser::operator= (const DSC_Parser& a)
{
    if (&a != this)
        {
            LocalCopy(a);
        }
    return *this;
}


bool DSC_Parser::GetFields(const char* inputStr,
                           SC_SetupErr& err)
{
    fields.SetEmpty();
    int strLen = StringLength(inputStr);
    int nCh = 0;
    while (nCh < strLen) {

        char nextChar = inputStr[nCh];

        // skip white space
        if ((nextChar == ' ') || (nextChar == '\t') || (nextChar == '\n'))
            {
                nCh++;
                continue;
            }

        if (fields.IsFull())
            {
                err.SetConstantError("too many fields (max 256)");
                return false;
            }

        fields.nlistData++;
        Field& nextField = fields.LastIndex();
        nextField = Field(); // clear older - debug only

        char tempStr[fieldStrLen];
        int nextN = 0;
        tempStr[0] = nextChar;

        if (isdigit(nextChar) || (nextChar == '.'))
            {
                nCh++;
                nextChar = inputStr[nCh];
                bool eLast = false;
                while (isdigit(nextChar) || (nextChar == '.')
                       || (nextChar == 'e') || (nextChar == 'E')
                       || (eLast && ((nextChar == '+') || (nextChar == '-'))))
                    {
                        nextN++;
                        if (FieldTooLong(tempStr, nextN, " : numeric constant is too long", err))
                            return false;

                        eLast = (nextChar == 'e') || (nextChar == 'E');

                        tempStr[nextN] = nextChar;
                        nCh++;
                        nextChar = inputStr[nCh];
                    }

                nextN++;
                tempStr[nextN] = '\0';
                if (!IsValidReal(tempStr, nextField.fVal))
                    {
                        err.SetInternalError(tempStr, " is not a valid real number");
                        return false;
                    }

                nextField.fieldType = ft_Number;
            }
        else if (isalpha(nextChar))
            {
                nCh++;
                nextChar = inputStr[nCh];
                while (isalpha(nextChar) || isdigit(nextChar) || (nextChar == '_'))
                    {
                        nextN++;
                        if (FieldTooLong(tempStr, nextN, " : constant is too long", err))
                            return false;

                        tempStr[nextN] = nextChar;

                        nCh++;
                        nextChar = inputStr[nCh];
                    }

                nextN++;
                tempStr[nextN] = '\0';
                UpperCase(tempStr);
                CopyString(nextField.fStr, tempStr, fieldStrLen);
                nextField.fieldType = ft_String;
            }
        else if ((nextChar == '\"') || (nextChar == '\''))
            {
                char startQuote = nextChar;
                nCh++;
                nextChar = inputStr[nCh];

                while (nextChar != startQuote)
                    {
                        if (FieldTooLong(tempStr, nextN, " : quoted constant is too long", err))
                            return false;

                        tempStr[nextN] = nextChar;
                        nextN++;

                        nCh++;
                        nextChar = inputStr[nCh];
                    }

                tempStr[nextN] = '\0';
                UpperCase(tempStr);
                CopyString(nextField.fStr, tempStr, fieldStrLen);
                nextField.fieldType = ft_String;
            }
        else
            {
                nextField.fieldType = ft_Character;
                nextField.fChar = nextChar;
                nCh++;
                nextChar = inputStr[nCh];
                // special case for exponent **
                if ((nextField.fChar == '*') && (nextChar == '*'))
                    {
                        nextField.fChar = '^';
                        nCh++;
                        nextChar = inputStr[nCh];
                    }
            }
    }

    return true;
}

bool DSC_Parser::FieldTooLong(char*          field,
                              int           nChar,
                              const char*   fType,
                              SC_SetupErr&  err)
{
    if (nChar == fieldStrLen)
        {
            field[fieldStrLen - 1] = '\0';
            err.SetInternalError(field, fType);
            return true;
        }

    return false;
}


void  DSC_Parser::LocalCopy(const DSC_Parser& a)
{
}

