///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SingleFitBase.h
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
//      calculates fit metric for simulated and field data.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_SINGLEFITBASE_H
#define DFO_SINGLEFITBASE_H

#include <genClass/DC_XYData.h>
#include <genClass/DO_XYData.h>

#include <osDFO/DFO_FitBase.h>


class DFO_SingleFitBase : public DFO_FitBase {
    public:
        enum FitCalcType    {fctSumSquares, fctSumAbsDelta};
        FitCalcType         fitCalcType;
        double              fitSigma;
        bool                fitIsNormed;
        double              minCoverage;       // added v1
        double              coveragePenalty;

        bool                removeFieldDuplicates;  // added v2
        bool                removeSimDuplicates;  // added v2
        double              duplicateEps;

        //  info for fit debugging
        bool                fieldDataOK;
        bool                simDataOK;
        double              fieldMinX, fieldMaxX;
        double              simMinX, simMaxX;
        int                 nfieldPts, nfieldPre, nfieldPost;
        int                 nsimPts, nsimPre, nsimPost;

        FuncObjRef          simResultsObjRef;  // domain sim results must include all points in field data

    protected:
        //  input associated data -- must be set by derived class
        const DC_XYData*    fieldDataDC;
        const DC_XYData*    simResultsDC;

        //  output data
        DC_FitResults       fitResultsDC;
    private:
        //  output objects
        DO_XYData           residualsDO;
        double              fitMultiplier; // for coverage penaly

        //  if reduce specified
        DC_XYData           reducedFieldDC;
        DC_XYData           reducedSimDC;

    public:
                            DFO_SingleFitBase(const char* inID);
                            DFO_SingleFitBase(const DFO_SingleFitBase& a);
                            ~DFO_SingleFitBase();

        DFO_SingleFitBase&  operator= (const DFO_SingleFitBase& a);

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);  // must be called by derived

        void                UpdateOverlapStatus(); // recalcs overlaps for menu UI

    protected:
        void                SetFieldInfo();

    private:
        void                SetSimInfo();
        void                CheckOverlaps();

        void                InitCommon();                           // common to normal and copy constructor
        void                LocalCopy(const DFO_SingleFitBase& a);     // common to copy constructor and = operator
};

#endif // !DFO_SINGLEFITBASE_H

