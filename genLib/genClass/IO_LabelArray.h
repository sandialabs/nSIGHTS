///////////////////////////////////////////////////////////////////////////////////
//
// IO_LabelArray.h
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
//              and support classes: Reads/Wrietes data class from/to files.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef IO_LABELARRAY_H
#define IO_LABELARRAY_H

#include <genClass/C_Common.h>
#include <genClass/C_ConvFile.h>
#include <genClass/C_TxtFile.h>
#include <genClass/DO_LabelArray.h>

class IO_LabelArray  {
public:
    enum                    LabelFileFormat{lffPoints, lffDXF, lffOther};

private:

    class I_LabelArrayFile :  public ConvFileC {
    private:
        int             nextInc;
        int             nextCount;

    public:
        I_LabelArrayFile(const char* inputFile);
        ~I_LabelArrayFile(){};

        void            Init();
        void            ReadPoints(DO_LabelArray&  labelArray);
        void            ReadDXF(DO_LabelArray&     labelArray);

    };

    class O_LabelArrayFile :  public TxtFileC {

    public:
        O_LabelArrayFile(const char* outputFile);
        ~O_LabelArrayFile(){};

        void            Init();
        void            WritePoints(const DO_LabelArray& labelArray);
    };


public:
    IO_LabelArray(){};
    ~IO_LabelArray(){};

    bool                ReadLabelFile(       LabelFileFormat  labelFileFormat,
                                             const   char*    inputFileName,
                                             DO_LabelArray&   labelArray);

    void                WriteLabelFile(const char*        inputFileName,
                                       const DO_LabelArray& LabelArray);

private:
    //  not available
    IO_LabelArray(const IO_LabelArray& a) {}
    IO_LabelArray&        operator=(const IO_LabelArray& a) {return *this;}


};

#endif
