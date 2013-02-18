///////////////////////////////////////////////////////////////////////////////////
//
// IO_VariableArray.h
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
//      I/O for DC_XXX.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef IO_VARIABLEARRAY_H
#define IO_VARIABLEARRAY_H

#include <genClass/C_Common.h>
#include <genClass/C_ConvFile.h>
#include <genClass/C_TxtFile.h>
#include <objClass/DO_VariableArray.h>

class IO_VariableArray  {
    public:
        enum    VariableFileFormat{vffPaCalc, vffOther};

    private:

        class I_VariableArrayFile :  public ConvFileC {
            private:
                int             nextInc;
                int             nextCount;

            public:
                                I_VariableArrayFile(const char* inputFile);
                                ~I_VariableArrayFile(){};

                void            Init();
                void            ReadPaCalc(DC_VariableArray&    variableArray);

        };

        class O_VariableArrayFile :  public TxtFileC {

            public:
                                O_VariableArrayFile(const char* outputFile);
                                ~O_VariableArrayFile(){};

                void            Init();
                void            WritePaCalc(const DC_VariableArray& variableArray);
        };


    public:
                            IO_VariableArray(){};
                            ~IO_VariableArray(){};

        bool                ReadVariableFile(       VariableFileFormat  variableFileFormat,
                                             const char*             inputFileName,
                                                 DC_VariableArray&   variableArray);

        void                WriteVariableFile(const char*           inputFileName,
                                              const DC_VariableArray& variableArray);

    private:
        //  not available
                            IO_VariableArray(const IO_VariableArray& a) {}
        IO_VariableArray&        operator=(const IO_VariableArray& a) {return *this;}


};

#endif // IO_VARIABLEARRAY_H

