///////////////////////////////////////////////////////////////////////////////////
//
// DSC_Parser.h
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

#ifndef DSC_PARSER_H
#define DSC_PARSER_H

#include <genClass/SC_SetupErr.h>

class DSC_Parser  {
public:

    enum FieldType  {ft_Character, ft_String, ft_Number, ft_NotSet};
    enum            {fieldStrLen = 80};

    class Field {
    public:
        FieldType   fieldType;
        char        fChar;
        char        fStr[fieldStrLen];
        double      fVal;

    public:
        Field();

        inline bool     IsCharacter()   const {return fieldType == ft_Character;}
        inline bool     IsString()      const {return fieldType == ft_String;}
        inline bool     IsNumber()      const {return fieldType == ft_Number;}
    };


    enum                {maxField = 256};  // initial resizable alloc

    T_SC_Array<Field>   fields;

public:
    DSC_Parser();
    DSC_Parser(const DSC_Parser& a);
    ~DSC_Parser() {};

    DSC_Parser& operator= (const DSC_Parser& a);


    bool                GetFields(const char*  inputStr,
                                  SC_SetupErr& err);
    // note that ** gets parsed as ^

private:
    void                LocalCopy(const DSC_Parser& a);     // common to copy constructor and = operator



    bool                FieldTooLong(char*           field,
                                     int             nChar,
                                     const char*     fType,
                                     SC_SetupErr&    err);

};

#endif
