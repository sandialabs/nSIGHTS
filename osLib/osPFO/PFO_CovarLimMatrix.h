///////////////////////////////////////////////////////////////////////////////////
//
// PFO_CovarLimMatrix.h
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

#ifndef PFO_COVARLIMMATRIX_H
#define PFO_COVARLIMMATRIX_H

#include <genClass/C_Common.h>

#include <osClass/DC_CovarArray.h>

#include <genClass/DC_ColorMap.h>

#include <genPlotClass/PSC_LegendOverride.h>

#include <genPlotClass/DO_SeriesLegendData.h>

#include <genPFO/PFO_ColorBase.h>


class PFO_CovarLimMatrix : public PFO_ColorBase  {

    public:
        enum    CovarEllipseType    {cetLine, cetFill};
        enum    ColorAttribute      {caIndx, caMainDiag, caOffDiag, caSSE};
        enum    MatrixPlotOption    {mpoFull, mpoUpper, mpoLower,
                                     mpoUpperDiag, mpoLowerDiag, mpoDiag};

    protected:
        //  input objects
        //FuncObjRef                  covarArrayObjRef; now inputDataObjRef with ColorBase
        DC_Covar::ConfidenceLimits  cLimits;

        bool                        plotEllipse;
        bool                        plotBestEst;
        bool                        plotBadCov;

        MatrixPlotOption            matrixPlotOption;

        CovarEllipseType            covarEllipseType;
        int                         ndualPts;   //  dual ellipse desc
        int                         covarLineThk;  // for line

        bool                        centerEllipse;

        PSC_SymbolSpec              bestEstSymbol;

        bool                        plotWithColorMap;
        int                         covarPen;
        //FuncObjRef                    colorMapDataObjRef;
        ColorAttribute              colorAttribute;

        double                      areaPct;

        bool                        plotAreaLim;
        int                         plotAreaLimPen;
        int                         plotAreaLimThk;

        int                         badCovXPen;
        int                         badCovXThk;

        PSC_LegendOverride          legendOverride;

        // plotting data
        DC_CovarArray*              covarArrayDC;
        DC_CovarHeader*             covarHeaderDC;

        //DC_ColorMap*              colorMap;
        SC_DoubleArray              colorData;
        int                         colorIndx;
        SC_IntCube                  colorIndxMap;

        int                         maxCovar;
        int                         maxParam;

        double                      minDiag;
        double                      maxDiag;

        double                      percent;

    private:
        // output objects
        DO_SeriesLegendData         seriesLegendData;    // output for legend

        double                      minVal;  //if norm limit below 0.0
        double                      maxVal;  //if norm limit above 1.0

        SC_DoubleCube               centerEllX;
        SC_DoubleCube               centerEllY;

        double                      minPercent;

    public:
                        PFO_CovarLimMatrix(PlotDefC& assPlt);
                        PFO_CovarLimMatrix(const PFO_CovarLimMatrix& a, PlotDefC& assPlt);
                        ~PFO_CovarLimMatrix();

        PFO_CovarLimMatrix& operator= (const PFO_CovarLimMatrix& a);

                        //  FuncObjC virtuals
        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

                        //  PlotObjC  draw virtuals in derived PFO_CovarLimMatrixGL
        Limit3D         GetPlotObjLimits();

    protected:
        void            FullCopy(const PFO_CovarLimMatrix& a);

        bool            Get2DSingle(int xIndx,
                                 int covarIndx,
                                 double& minimum,
                                 double& maximum);

        bool            Get2DDual(int xIndx,
                               int yIndx,
                               int covarIndx,
                               SC_PointArray& ellipse,
                               double& aDiag,
                               double& bDiag);

        void            MinMaxDiag(const double aDiag, const double bDiag,
                                const int i, const int j, const int k);

        double          Normalize(const double userVal);
        double          Reduce(const double userVal);
        double          Shift(const int i, const int j, const int k, bool isX);

        bool            IncDiag() {return ((matrixPlotOption != mpoUpper)
                                        && (matrixPlotOption != mpoLower));}
        bool            IncUpper() { return ((matrixPlotOption == mpoFull)
                                        || (matrixPlotOption == mpoUpper)
                                        || (matrixPlotOption == mpoUpperDiag));}
        bool            IncLower() { return ((matrixPlotOption == mpoFull)
                                        || (matrixPlotOption == mpoLower)
                                        || (matrixPlotOption == mpoLowerDiag));}

        bool            ColorAttributeIsDiag() { return ((colorAttribute == caMainDiag)
                                                      || (colorAttribute == caOffDiag));}

    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_CovarLimMatrix& a);  // common to copy constructor and = operator
};

#endif // !PFO_COVARLIMMATRIX_H

