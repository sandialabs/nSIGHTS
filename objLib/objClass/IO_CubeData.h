///////////////////////////////////////////////////////////////////////////////////
//
// IO_CubeData.h
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
//      I/O for DC_CubeData
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef IO_CUBEDATA_H
#define IO_CUBEDATA_H

#include <genClass/C_Common.h>
#include <genClass/C_ConvFile.h>
#include <genClass/C_TxtFile.h>

#include <objClass/DC_CubeData.h>


class IO_CubeData  {
    public:
        enum  CubeFileFormat{cffAppDev, cffOther};

    private:

        class I_CubeDataFile :  public ConvFileC {
            private:
                int             nextInc;
                int             nextCount;

            public:
                                I_CubeDataFile(const char* inputFile);
                                ~I_CubeDataFile(){};

                void            Init();
                void            ReadAppDevCube(DC_CubeData&    cubeData);

        };

        class O_CubeDataFile :  public TxtFileC {

            public:
                                O_CubeDataFile(const char* outputFile);
                                ~O_CubeDataFile(){};

                void            Init();
                void            WriteAppDevCube(const DC_CubeData& cubeData);
        };


    public:
                            IO_CubeData(){};
                            ~IO_CubeData(){};

        bool                ReadCubeFile(       CubeFileFormat  cubeFileFormat,
                                         const  char*           inputFileName,
                                                DC_CubeData&    cubeData);

        void                WriteCubeFile(const char*        inputFileName,
                                          const DC_CubeData& cubeData);

    private:
        //  not available
                            IO_CubeData(const IO_CubeData& a) {}
        IO_CubeData&        operator=(const IO_CubeData& a) {return *this;}

};

#endif // IO_CUBEDATA_H

