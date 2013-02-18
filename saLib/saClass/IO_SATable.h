///////////////////////////////////////////////////////////////////////////////////
//
// IO_SATable.h
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

#ifndef IO_SATABLE_H
#define IO_SATABLE_H

#include <genClass/C_Common.h>
#include <genClass/C_ConvFile.h>
#include <genClass/C_TxtFile.h>

#include <saClass/DC_SATable.h>


class IO_SATable  {
    public:
        enum  SAFileFormat{saffSandiaWIPP,  saffGoldsimDepVar, saffGoldsimIndepVar, saffOther};

    private:

        class I_SATableFile :  public ConvFileC {
            private:
                int             nextInc;
                int             nextCount;

            public:
                                I_SATableFile(const char* inputFile);
                                ~I_SATableFile(){};

                void            Init();
                void            ReadWIPPSATable(DC_SATable&     saTable);
                void            ReadGoldsimDVTable(DC_SATable&  saTable);
                void            ReadGoldsimIVTable(DC_SATable&  saTable);


        };

        class O_SATableFile :  public TxtFileC {

            public:
                                O_SATableFile(const char* outputFile);
                                ~O_SATableFile(){};

                void            Init();
                void            WriteWIPPSATable(const DC_SATable&  saTable);
        };


    public:
                            IO_SATable(){};
                            ~IO_SATable(){};

        bool                ReadSATable(const  char*         inputFileName,
                                              SAFileFormat   saFileFormat,
                                              DC_SATable&    saTable);


        void                WriterSATable(const char*      outputFileName,
                                              const DC_SATable& saTable);

    private:
        //  not available
                            IO_SATable(const IO_SATable& a) {}
        IO_SATable&        operator=(const IO_SATable& a) {return *this;}


};

#endif // !IO_SATABLE_H

