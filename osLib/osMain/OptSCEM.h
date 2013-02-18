///////////////////////////////////////////////////////////////////////////////////
//
// OptSCEM.h
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

#ifndef OPTSCEM_H
#define OPTSCEM_H

#include <osMain/C_Optimize.h>

    class SCEMOptimizer : public Optimizer {
        private:
            int                 nsamplePoints;
            SC_DoubleMatrix     samplePoints;
            SC_DoubleCube       complexes;
            SC_DoubleMatrix     subComplex;
            SC_DoubleMatrix     complexF;
            SC_DoubleArray      sampleF;
            SC_DoubleArray      subComplexF;

            SC_DoubleArray      centroid;

            int                 nReflect;
            int                 nContract;
            int                 nMutate;

            // scem random # generator
            SC_Random           scemRand;

            int                 nFail;

        public:
                                SCEMOptimizer();
            virtual             ~SCEMOptimizer();

            virtual bool        Initialize(UpdateCB       upCB,
                                           RunnableC&  simulator);

            virtual void        Optimize();

            //  for extracting optimizer specific data
            virtual int         GetnExtraOutput() const;                          // number of extra data to put in output tables
            virtual void        GetExtraOutputID(SC_StringArray& extraIDs) const; // table column IDs
            virtual void        GetExtraOutput(SC_DoubleArray& extraData) const; // table column data


        private:
            //  SCEM routines
            double              EvalFunction(const SC_DoubleArray& normParEstimates);
            void                SCEMStartOpt();
            void                SCEMDoOpt();
            void                Evolve(SC_DoubleMatrix&  complex,
                                       SC_DoubleArray&   complexFitVals);

            double              SCEMReflect(SC_DoubleArray&  r);
            double              SCEMMutate(SC_DoubleArray&  r);
            double              SCEMContract(SC_DoubleArray&  r);
    };



#endif // !OPTSCEM_H

