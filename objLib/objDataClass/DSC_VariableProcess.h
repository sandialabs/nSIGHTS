///////////////////////////////////////////////////////////////////////////////////
//
// DSC_VariableProcess.h
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

#ifndef DSC_VARIABLEPROCESS_H
#define DSC_VARIABLEPROCESS_H

#include <genClass/C_TxtFile.h>
#include <genClass/SC_SetupErr.h>
#include <objClass/DC_VariableArray.h>
#include <genClass/SC_DoubleArray.h>


class DSC_VariableProcess {

    class ExtendedVar : public DC_Variable {
        public:
            char    varString[varIDLen];
            int     specifiedLength;

            // to avoid repetive strlens
            int     varIDStrLen;
            int     varStrLen;

        public:
                        ExtendedVar();
                        ExtendedVar(const ExtendedVar& a);
                        ~ExtendedVar() {};

            ExtendedVar&    operator= (const ExtendedVar& a);
            ExtendedVar&    operator= (const DC_Variable& a);

            void        SetLens();

            bool        ProcessLine(char*        inputLine,
                                    int          maxLen,
                                    bool&       lineModified,
                                    SC_SetupErr& processErrors);
        private:
            void        LocalCopy(const ExtendedVar& a);
    };

    class ExtendedVarArray : public T_SC_Array<ExtendedVar> {
        public:
                        ExtendedVarArray() : T_SC_Array<ExtendedVar>() {}
                        ExtendedVarArray(int nAll):T_SC_Array<ExtendedVar>(nAll) {}
                        ExtendedVarArray(int nAll, const ExtendedVar& initVal):T_SC_Array<ExtendedVar>(nAll, initVal) {}
                        ExtendedVarArray(ExtendedVar* buffer, int bufSize):T_SC_Array<ExtendedVar>(buffer, bufSize) {}

        int             GetVarIndex(const char* varID) const;
    };

    private:
        TxtFileC            outFile;
        bool                outFileIsOpen;
        ExtendedVarArray    currVars;

        enum            {maxInputLength = 2048};
        char            inputLine[maxInputLength];
        SC_StringArray  tokens;
        bool            skipProcessing;
        bool            commentsOn;
        char            commentStr[40];
        bool            forceRecordLen;
        int             recordLength;

        public:
                            DSC_VariableProcess();
                            DSC_VariableProcess(const DSC_VariableProcess& a);
                            ~DSC_VariableProcess() {};

            DSC_VariableProcess&    operator= (const DSC_VariableProcess& a);

            bool            ProcessVariables(const DC_VariableArray& inVars,
                                             const char* templateFile,
                                             const char* outputFile,
                                             SC_SetupErr& processErrors);

        private:
            void            LocalCopy(const DSC_VariableProcess& a);

            bool            ProcessOneFile(const char* fName,
                                           SC_SetupErr& processErrors);

            bool            ProcessOneLine(SC_SetupErr& processErrors);

            bool            CheckCommand(const char* cmdText);
            void            ExtractToken(char*& tokenPos, char* nextToken);
            void            ExtractTokens();

            bool            ProcessVarCommand(SC_SetupErr& processErrors);

            bool            ProcessIfCommand(SC_SetupErr& processErrors,
                                             bool&  conditionTrue);

            bool            ProcessVarValue(ExtendedVar& var,
                                            const char*  token,
                                            SC_SetupErr& processErrors);

            bool            ProcessVarFormat(ExtendedVar& var,
                                            const char*  token,
                                            SC_SetupErr& processErrors);

            void            WriteComment(const char* comment);
            void            WriteProcessedInput();
    };


#endif // DSC_VARIABLEPROCESS_H

