///////////////////////////////////////////////////////////////////////////////////
//
// C_Sample.h
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
//      Main optimizer/sampler code and global variables and other support classes used by optimizer and application.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_SAMPLE_H
#define C_SAMPLE_H

#include <genClass/SC_DoubleArray.h>
#include <genClass/DC_TableData.h>
#include <genClass/SC_SetupErr.h>

#include <osClass/C_SampVar.h>

// class with actual sampling code -- all statics
class Sampler :  public SampleControlStatics   {
    private:
        //  convenience vars
        static  SampVarArray    sampVarData;              // packed into array for ease of access
        static  int             nsampVar;
        static  DC_TableData    sampleResults;
        static  bool            sampleResultsOK;          // sampling done and samples OK
        static  bool            sampleResultsAvailable;   // sampling done


    public:

        static bool             DoSampling(SC_SetupErr& errData);

        static bool             SamplesAreOK();             // sampling performed, all results within limits
        static bool             SamplesAreAvailable();      // sampling performed
        static bool             SamplesCurrent();           // available && matches current sample setup (ntrials, sampVar
        static void             ClearAvailableSamples();

    private:
                                // these are ports from the pascal sampling code, which was in turn a port of FORTRAN code
                                // the original FORTRAN data structures are used except that arrays are 0 based and use the
                                // T_SC_Array<> vectors


        static void             SiftXI(SC_DoubleArray&   xf,
                                       SC_IntArray&      ky,
                                       int               n);

        static  bool            InitSamplingLocals(SC_SetupErr& errData);
        static  void            DeAllocSamplingLocals();
        static  void            Ranker(SC_DoubleArray& rx,      // replaces entries of rx with their ranks
                                       int             n);      // ranks are 0.0 to n-1.0

        static  void            MixCorr(const SC_DoubleArray& corrX,  // decomposition of user-specified correlations on n variables.
                                              int             n);

        static bool             SetupCorrelations(SC_SetupErr& errData);

};



#endif // !C_SAMPLE_H

