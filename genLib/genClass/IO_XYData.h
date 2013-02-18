///////////////////////////////////////////////////////////////////////////////////
//
// IO_XYData.h
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
// DESCRIPTION: Platform independent and application independent base classes
//              and support classes: Reads/Writes DC_XYData in a variety of formats.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef IO_XYDATA_H
#define IO_XYDATA_H

#include <genClass/C_Common.h>
#include <genClass/C_ConvFile.h>
#include <genClass/C_TxtFile.h>
#include <genClass/DC_DateTime.h>

#include <genClass/DC_XYData.h>

class DSC_XYInputSpec {
public:
    enum  XYFileFormat  {xyffAppDev,            // X Y
                         xyffTable,             // two columns from table
                         xyffDate,              // YYYY MM DD Yval
                         xyffOther};


    XYFileFormat                xyFileFormat;

    // for xyffTable
    int                             tableXcol;
    int                             tableYcol;
    bool                        readColumnID;

    // for xyffDate
    DC_DateTimeSpec                 dateSpec;

public:
    DSC_XYInputSpec();
    DSC_XYInputSpec(const DSC_XYInputSpec& a);
    ~DSC_XYInputSpec(){};

    DSC_XYInputSpec&        operator= (const DSC_XYInputSpec& a);

private:
    void                LocalCopy(const DSC_XYInputSpec& a);  // common to copy constructor and = operator

};


class IO_XYData  {
public:

private:

    class I_XYDataFile :  public ConvFileC {
    private:
        int             nextInc;
        int             nextCount;

    public:
        I_XYDataFile(const char* inputFile);
        ~I_XYDataFile(){};

        void            Init();
        void            ReadTableXY(bool          readColIDs,
                                    int           tableXcol,
                                    int           tableYcol,
                                    DC_XYData&    xyData);

        void            ReadDateXY(const DC_DateTimeSpec& dateSpec,
                                   DC_XYData&    xyData);
    };

    class O_XYDataFile :  public TxtFileC {

    public:
        O_XYDataFile(const char* outputFile);
        ~O_XYDataFile(){};

        void            Init();
        void            WriteAppDevXY(bool              writeColHeaders,
                                      const DC_XYData&  xyData);
    };


public:
    IO_XYData(){};
    ~IO_XYData(){};

    bool                ReadXYFile(const  char*            inputFileName,
                                   const  DSC_XYInputSpec& inputSpec,
                                   DC_XYData&              xyData);


    void                WriteXYFile(const char*      outputFileName,
                                    bool             writeColIDs,
                                    const DC_XYData& xyData);

private:
    //  not available
    IO_XYData(const IO_XYData& a) {}
    IO_XYData&        operator=(const IO_XYData& a) {return *this;}

};

#endif// IO_XYDATA_H

