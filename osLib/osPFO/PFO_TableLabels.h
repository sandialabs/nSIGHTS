///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableLabels.h
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
//      Plotting objects for covariance data
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_TABLELABELS_H
#define PFO_TABLELABELS_H

#include <genClass/C_Common.h>

#include <genClass/DO_TableData.h>
#include <genClass/SC_RealFormat.h>

#include <genPlotClass/PSC_XYLabelSpec.h>
#include <genPlotClass/C_PlotObj.h>


class PFO_TableLabels : public PlotObjC, public PSC_XYLabelSpec  {

    public:
        enum    MatrixPlotOption    {mpoFull, mpoUpper, mpoLower,
                                     mpoUpperDiag, mpoLowerDiag, mpoDiag};

    protected:
        //  input objects
        FuncObjRef                  tableObjRef;

        MatrixPlotOption            matrixPlotOption;

        SC_RealFormat               convFormat; // added v1

        // plotting data
        const DC_TableData*         tableLabelsDC;

    public:
                        PFO_TableLabels(PlotDefC& assPlt);
                        PFO_TableLabels(const PFO_TableLabels& a, PlotDefC& assPlt);
                        ~PFO_TableLabels();

        PFO_TableLabels& operator= (const PFO_TableLabels& a);

                        //  FuncObjC virtuals
        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

                        //  PlotObjC  draw virtuals in derived PFO_TableLabelsGL
        Limit3D         GetPlotObjLimits();

    protected:

        double          Shift(const int i, const int j, const int k, bool isX);

        bool            IncDiag() {return ((matrixPlotOption != mpoUpper)
                                        && (matrixPlotOption != mpoLower));}
        bool            IncUpper() { return ((matrixPlotOption == mpoFull)
                                        || (matrixPlotOption == mpoUpper)
                                        || (matrixPlotOption == mpoUpperDiag));}
        bool            IncLower() { return ((matrixPlotOption == mpoFull)
                                        || (matrixPlotOption == mpoLower)
                                        || (matrixPlotOption == mpoLowerDiag));}

    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_TableLabels& a);  // common to copy constructor and = operator
};

#endif // !PFO_TABLELABELS_H

