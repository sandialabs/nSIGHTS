///////////////////////////////////////////////////////////////////////////////////
//
// PC_DumpTGA.h
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
//      Platform independent base classes for plot definitions and plot functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PC_DUMPTGA_H
#define PC_DUMPTGA_H

#include <genClass/C_Common.h>

class PC_DumpTGA {

    public:
            enum            DumpFormat {dumpTGA, dumpJPEG, dumpBMP};

            enum            TGADumpMethod
                                {dtga_FullAuto,     // new file is written every replot, counter incremented
                                dtga_SemiAuto,     // new file is when TGA Dump button is pressed, counter incremented
                                dtga_Manual};      // current file is written when TGA Dump button is pressed

            char            tgaRootFile[stdFileStrLen];
            DumpFormat      dumpFormat;
            TGADumpMethod   dumpMethod;
            int             nextIncrement;
            int             dumpFrameCount;  // number of frames on TGA SemiAuto dump
            int             qualityJPEG;

    public:
                        PC_DumpTGA();
                        PC_DumpTGA(const PC_DumpTGA& a);

            PC_DumpTGA& operator=(const PC_DumpTGA& a);


            void        GetNextFileName(char  nextFileName[],
                                        int   maxLen);

            void        SetDefault();

    protected:
            void        LocalCopy(const PC_DumpTGA& a);
};


#endif // !PC_DUMPTGA_H

