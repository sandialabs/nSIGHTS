///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell2D.h
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

#ifndef C_SINGLEWELL2D_H
#define C_SINGLEWELL2D_H

#include <genClass/U_Real.h>
#include <genClass/U_Matrix.h>

#include <Sim/C_WorkDataCapture.h>

#include <Sim/C_SingleWell.h>


class SingleWell2D : public SingleWellSimulator  {

    protected:

       #ifdef LAPACK
        SC_LapackBandedMatrix matA;
        SC_DoubleMatrix     nodePressure, oldNodePressure;
        SC_IntArray ipiv;
        //SC_DoubleMatrix     matA, nodePressure, oldNodePressure;
       #else // LAPACK
        SC_DoubleMatrix     matA, nodePressure, oldNodePressure;
       #endif // LAPACK
        SC_DoubleArray      solutionVector, rhsVector;
        SC_DoubleArray      constantNodeZ, constantNodeDeltaZ, lastWellPressure;
        SC_BoolArray        isWellNode;

        // 2D partial penetration
        double              baseVerticalWellTerm;
        int                 nverticalNodes, wellNode, nwellNodes, firstWellNode, lastWellNode;

        SC_DoubleArray      dTerm, sTerm;  // calculated for unit thick
        SC_DoubleArray      qvTerm;        // vertical conductivity

        // common to solutions
        int                 npartialEquations;
        double              fixedTZPressure;

        // communication to private functions in derived
        double              verticalWellTerm;
        int                 ntotalSoln;
        int                 topVertIndex;

    private:
       #ifndef LAPACK
        SC_DoubleMatrix     matA1;
        SC_IntArray         matSolveIndx;
       #endif // LAPACK



    public:
                            SingleWell2D();
        virtual             ~SingleWell2D();

    protected:

        // optimizable virtuals
        virtual bool        SetupForRun(SC_SetupErr&    errData);

        // SingleWell virtuals
        virtual void        DoAllocateAll();
        virtual void        SetRuntimeSize();
        virtual void        SetRunParameters();
        virtual void        SequenceSetup();
        virtual void        InitializeTZData();         // for type specific TZ initialization at first sequence
        virtual void        UpdateTZData();

        // dynamic TS support
        virtual void        SaveCurrentPressure();
        virtual void        RestoreCurrentPressure();

        //  setup
        void                SetupForSolve();
        //  solver
        void                Solve2D(int neqn, int nvert);

    private:

        void                Calc2DVerticalGeometry();

        void                AddConstantVerticalNodes(int            nnode,
                                                     const double&  interval);

};



#endif // C_SINGLEWELL2D_H

