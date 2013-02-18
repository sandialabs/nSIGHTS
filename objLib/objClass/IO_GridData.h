///////////////////////////////////////////////////////////////////////////////////
//
// IO_GridData.h
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
//      I/O for DC_GridData
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef IO_GRIDDATA_H
#define IO_GRIDDATA_H

#include <genClass/C_Common.h>
#include <genClass/C_ConvFile.h>
#include <genClass/C_TxtFile.h>

#include <objClass/DC_GridData.h>


class IO_GridData  {
    public:
        enum  GridFileFormat{gffAppDev, gffSurfer, gffXYZList, gffArcInfoAscii, gffOther};

    private:

        class I_GridDataFile :  public ConvFileC {
            private:
                int             nextInc;
                int             nextCount;

            public:
                                I_GridDataFile(const char* inputFile);
                                ~I_GridDataFile(){};

                void            Init();
                void            ReadAppDevGrid(DC_GridData&    gridData);
                void            ReadSurferGrid(DC_GridData&    gridData);
                void            ReadXYZList(DC_GridData&    gridData);
                void            ReadArcInfoAsciiGrid(DC_GridData&    gridData);
            private:
                void            ReadSurferCoord(DC_DataCoord& inCoord);

                void            AddNewEntry(      SC_DoubleArray& inGrid,
                                            const double&         newVal);

                int             GetEntryIndex(const SC_DoubleArray& inGrid,
                                              const double&         newVal);

                double          GetArcInfoAsciiParameter(const char* parID);

        };

        class O_GridDataFile :  public TxtFileC {

            public:
                                O_GridDataFile(const char* outputFile);
                                ~O_GridDataFile(){};

                void            Init();
                void            WriteAppDevGrid(const DC_GridData& gridData);
                void            WriteSurferGrid(const DC_GridData& gridData);
                void            WriteXYZList(const DC_GridData& gridData);
                void            WriteArcInfoAsciiGrid(const DC_GridData& gridData);
            private:
                void            WriteSurferCoord(const DC_DataCoord& outCoord);


        };


    public:
                            IO_GridData(){};
                            ~IO_GridData(){};

        bool                ReadGridFile(       GridFileFormat  gridFileFormat,
                                         const  char*           inputFileName,
                                                DC_GridData&    gridData);

        void                WriteGridFile(      GridFileFormat  gridFileFormat,
                                          const char*           inputFileName,
                                          const DC_GridData&    gridData);

    private:
        //  not available
                            IO_GridData(const IO_GridData& a) {}
        IO_GridData&        operator=(const IO_GridData& a) {return *this;}

};

#endif // IO_GRIDDATA_H

