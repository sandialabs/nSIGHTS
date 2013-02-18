///////////////////////////////////////////////////////////////////////////////////
//
// C_WorkParameter.h
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

#ifndef C_WORKINGPARAMETER_H
#define C_WORKINGPARAMETER_H

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_SetupErr.h>

#include <Var/E_Parameter.h>
#include <Var/C_Parameter.h>

    //  these are the parameter classes used by the simulator;

namespace nsWorkingParameter  {

    class WorkingParameterArray;

    class WorkingParameter {

        friend bool WorkingParameterSetup(WorkingParameterArray&    wPar,
                                          SC_DoubleArray&           nodeRadii,
                                          SC_SetupErr&              errData);


        public:
            enum    WorkingParameterType  { wptNotSet,  // for sanity checks
                                            wptFixed,        // constant
                                            wptRadius,      // f(r) at points
                                            wptFP,          // f(P)
                                            wptFt           // f(t)
                                           };


            WorkingParameterType    parameterType;

            double                  fixedVal;   // wptFixed value
            SC_DoubleArray          nodeVals;   // f(r)
            WorkingParameterCurve   fpCurve;

        private:
            const SC_DoubleArray*   nodeRadii;

        public:
                                WorkingParameter()
                                        : nodeRadii(0), parameterType(wptNotSet) {};
                                ~WorkingParameter() {};

            inline bool         IsFixed()  const    {return parameterType == wptFixed;}
            inline bool         IsRadius() const    {return parameterType == wptRadius;}
            inline bool         IsFP()     const    {return parameterType == wptFP;}
            inline bool         IsFt()     const    {return parameterType == wptFt;}

                                // to satify compiler
            WorkingParameter&   operator= (const WorkingParameter& a) {return *this;}


            bool                SetData(Parameter&      inData,
                                        SC_SetupErr&    errData);

            int                 GetIntData();    // returns val if fixed, 0 if notSet

            double              GetData();                              // access signature for wptFixed
            double              GetData(int i);                         // access signature for wptRadius
            double              GetData(const double& metricInputP);    // access signature for wptFP
            double              GetData(int i, const double& metricInputP); // access signature for Radius or wptFP
    };

    class WorkingParameterArray : public T_SC_Array<WorkingParameter> {
        private:
            double  fluidDensity;
            double  fluidComp;
            double  fluidPermTerm;

        public:
                    WorkingParameterArray() {};
                    ~WorkingParameterArray() {};

            // for use by linear sims only - init has staticPressure param to allow
            // call in nonlinear useage - kluge
            void          InitLinearFluidVars(const double& staticPressure);
            const double& GetFluidDensity() const {return fluidDensity;}
            double        CalcStoreTerm(const ParameterGroups& parGroup) const;
            double        CalcKTerm(const ParameterGroups& parGroup) const;
    };



    extern bool WorkingParameterSetup(WorkingParameterArray&    wPar,
                                      SC_DoubleArray&           nodeRadii,
                                      SC_SetupErr&              errData);
};

using namespace nsWorkingParameter;

#endif // C_WORKINGPARAMETER_H
