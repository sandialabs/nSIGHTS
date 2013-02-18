///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell1DLinear.h
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

#ifndef C_SINGLEWELL1DLINEAR_H
#define C_SINGLEWELL1DLINEAR_H

#include <genClass/SC_BoolArray.h>

#include <Sim/C_StaticCondensation.h>
#include <Sim/C_SingleWell1D.h>


class SingleWell1DLinear : public SingleWell1D  {

    private:
        //  formation/fracture
        SC_DoubleArray      fracDDiag, fracDUpper, fracSolveDiag, fracSolveUpper;
        SC_DoubleArray      fracSVector, fracSSolve, fracWork, fracRhs;

        int                 scStartNode;
        DualPorosity        matrix;
        Leakage             upperLeak, lowerLeak, singleLeak;
        SC_DoubleArray      leakArea;

        double              wellDTerm;

        bool                allAreConstant;
        double              parameterTime;  
        SC_DoubleArray      dConstantTerm, sConstantTerm;
        SC_BoolArray        dTermIsConstant, sTermIsConstant;
        bool                lastWasConstant;

    public:
                            SingleWell1DLinear();
        virtual             ~SingleWell1DLinear();

    protected:

        // optimizable virtuals
        virtual bool        SetupForRun(SC_SetupErr&    errData);

        // SingleWell virtuals
        virtual void        SetRuntimeSize();
        virtual void        SetRunParameters();
        virtual void        DoAllocateAll();

        // just used for factor calcs
        virtual double      GetRockPar(int  parIndx,
                                       int  nodeNum);
        virtual double      GetFluidPar(int parIndx);

        virtual void        MatSolve(bool& setupRequired); 

    private:
        double              CalcStoreTerm(const ParameterGroups& parGroup);
        double              CalcKTerm(const ParameterGroups& parGroup);

        double              GetDTerm(int nodeNum);
        double              GetSTerm(int nodeNum);

        double              LinGetRockPar(int parIndx,
                                          int nodeNum);

        double              LinGetFluidPar(int  parIndx);


        //  dual por & leakag
        void                AddSCLhsTerm(StaticCondensation& inSC);
        void                AddSCRhsTerm(StaticCondensation& inSC);
        void                SCGaussSolve(StaticCondensation& inSC);


        //  linear setup and solve
        int                 SolveNodeOffset();
        void                ScAddQTerm(const SC_DoubleArray&    scScaleVector,
                                       const double             scQVal);

        void                BuildMatrices();
        void                LinearMatSetup();
        void                LinearMatSolve();

        // dynamic TS support
        virtual double      GetPChange();  // delta P over all soln pressures
        virtual void        SaveCurrentPressure();
        virtual void        RestoreCurrentPressure();

        void                StaticCondVectorAddToMinMax(SC_DoubleMatrix& scMatrix,
                                                            double&          minP,
                                                            double&          maxP);
};

#endif // C_SINGLEWELL1DLINEAR_H

