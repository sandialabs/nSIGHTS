///////////////////////////////////////////////////////////////////////////////////
//
// C_Uncertain.h
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


#ifndef C_UNCERTAIN_H
#define C_UNCERTAIN_H

#include <genClass/T_SC_Array.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_Random.h>

#include <osClass/C_SimErr.h>
#include <osClass/C_SampVar.h>
#include <osClass/C_Runnable.h>

    class UncertaintyPropagation : public SampleControlStatics {  // for easy access to control vars

        friend class UncertaintyPropagationPackage;

        public:
            typedef void (*UpdateCB)(UncertaintyPropagation& uncert);

        public:
            int                 nUncertVars;
            int                 numM;
            SampVarArray        uncertainVars;
            SC_DoubleArray      meanVarValues;
            SC_DoubleArray      stdDevVarValues;
            SC_BoolArray        isLogVar;
            SC_DoubleArray      meanMetricValues;
            SC_DoubleArray      stdDevMetricValues;
            SC_DoubleArray      varRange;
            SC_DoubleMatrix     meanComponentValues;
            SC_DoubleMatrix     stdDevComponentValues;
            SC_DoubleMatrix     derivValues;


        protected:
            UpdateCB            updateCB;
            //  slight kluge to retain semi-fatal error mesage
            SimError            lastSemiFatal;

        private:
            RunnableC*    currSimulator;

        public:
                                UncertaintyPropagation();
            virtual             ~UncertaintyPropagation();

            virtual bool        Initialize(RunnableC&  simulator);

            virtual void        CalcUncert();

        protected:

            bool                CalcFitValue(const SC_DoubleArray& varVals,
                                            SC_DoubleArray&     metricVals);
                                            //     double&         nextMetric);


    };


    class UncertaintyPropagationPackage : public SampleControlStatics  {
        public:
            SimError    lastErr;

            UncertaintyPropagation* uncertProp;

        public:
                        UncertaintyPropagationPackage();
                        ~UncertaintyPropagationPackage();

            void        SetUncertPropTechnique();
            void        ClearUncertPropTechnique();

            bool        Initialize(RunnableC&       simulator);

            void        MultiStageUncertRun();//int metricIndex);

        private:
            UncertaintyPropagation* GetUncertTechnique(UncertType  uncertType);
    };






#endif // !C_UNCERTAIN_H

