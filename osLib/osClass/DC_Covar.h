///////////////////////////////////////////////////////////////////////////////////
//
// DC_Covar.h
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
//      OsLib supports sampling and processing/plotting of optimization output
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_COVAR_H
#define DC_COVAR_H

#include <genClass/C_Common.h>

#include <genClass/SC_PointArray.h>
#include <genClass/SC_CoordArray.h>

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_StringArray.h>
#include <osClass/DC_OptSimResults.h>

class DC_Covar  {
    public:
        enum  ConfidenceLimits  {cl_990,  cl_954, cl_900, cl_683};
        enum                    {maxConfidenceLimits = 4};
        static double           snglParCnfLimits[];
        static double           dualParCnfLimits[];
        static double           tripleParCnfLimits[];

        enum                    {idLen = 80};
        char                    covarID[idLen];  // for use in listings

        SC_DoubleArray          covarFitVals;  // normalized vals
        SC_DoubleMatrix         covarMatrix;
        double                  sumSquaredError;

    public:
                        DC_Covar();
                        DC_Covar(const DC_Covar& a);
                        ~DC_Covar();
        DC_Covar&       operator= (const DC_Covar& a);

        void            SetActID(const char* runID, const char* caseID, const char* fitID);
        void            SetEstID(const char* runID, const char* caseID, const char* fitID);
        void            SetSubFitID(const char* runID, const char* caseID, const char* fitID, const char* subFitID);
        void            SetID(const char* runID, const char* caseID, const char* fitID, const char* subFitID, int index);


        inline bool     CovarOK(int indx) const {return (covarMatrix[indx][indx] > 0.0) && (covarMatrix[indx][indx] <= 1.0);}

        //  for confidence limit plotting -- all values returned normalized
        void            CalcSingleLimits(ConfidenceLimits   cl,         // single cnf
                                         int                indx,
                                         double&            minVal,
                                         double&            maxVal) const;

        void            CalcSingleLimits(ConfidenceLimits   cl,
                                         int                xIndx,
                                         int                yIndx,
                                         SC_PointArray&     stPoints,
                                         SC_PointArray&     endPoints) const;

        void            CalcSingleLimits  (ConfidenceLimits   cl,
                                           int                xIndx,
                                           int                yIndx,
                                           int                zIndx,
                                           SC_CoordArray&     stCoords,
                                           SC_CoordArray&     endCoords) const;

        // throws MathError
        void            CalcDualLimits  (ConfidenceLimits   cl,
                                         int                xIndx,
                                         int                yIndx,
                                         int                npts,
                                         SC_PointArray&     ellipsePoints) const;

        //same as above, but returns major and minor axes
        void            CalcDualLimits  (ConfidenceLimits   cl,
                                        int                xIndx,
                                        int                yIndx,
                                        int                npts,
                                        SC_PointArray&     ellipsePoints,
                                        double&            a,
                                        double&            b)  const;

        // throws MathError
        void            CalcDualLimits  (ConfidenceLimits   cl,
                                         int                xIndx,
                                         int                yIndx,
                                         int                zIndx,
                                         int                npts,
                                         SC_CoordArray&     ellipseCoords) const;

        // throws MathError
        void            CalcTripleLimits  (ConfidenceLimits   cl,
                                           int                xIndx,
                                           int                yIndx,
                                           int                zIndx,
                                           int                nmeridian,
                                           int                nslice,
                                           SC_CoordMatrix&    ellipseCoords) const;


    private:
        void            LocalCopy(const DC_Covar& a);  // implementation of copy

};


//  adds norm/denorm data transformations
class CovarVarDesc : public OptVarDescription{

    public:
                        CovarVarDesc();
                        CovarVarDesc(const CovarVarDesc& a);
                        ~CovarVarDesc();

        CovarVarDesc&   operator= (const CovarVarDesc& a);
        CovarVarDesc&   operator= (const OptVarDescription& a);  // from base

        double          Normalize(const double& userVal);    // converts from user val to norm space
        double          DeNormalize(const double& normVal);  // converts from norm val to user space

    private:
        void            LocalCopy(const CovarVarDesc& a);  // implementation of copy
};


class DC_CovarHeader : public T_SC_Array<CovarVarDesc> {

    public:
                        DC_CovarHeader() : T_SC_Array<CovarVarDesc>() {};
                        DC_CovarHeader(const DC_CovarHeader& a) : T_SC_Array<CovarVarDesc>(a) {};

        void            GetIDs(SC_StringArray& ids);


        void            DeNormalize(SC_PointArray&     points,
                                    int                xIndx,
                                    int                yIndx);

        void            DeNormalize(SC_CoordArray&     coords,
                                    int                xIndx,
                                    int                yIndx,
                                    int                zIndx);

        void            DeNormalize(SC_DoubleMatrix&  covarMatrix);

};


class DC_FullCovar : public DC_CovarHeader, public DC_Covar {

    public:
                        DC_FullCovar();
                        DC_FullCovar(const DC_CovarHeader& a);
                        DC_FullCovar(const DC_FullCovar& a);

        DC_FullCovar&   operator=(const DC_FullCovar& a);

        //  set up
        void            SetActualCovar( const OptSimRunResults& currRun,
                                        const OptSingleFit&     currFit);

        void            SetEstimatedCovar(  const OptSimRunResults& currRun,
                                            const OptSingleFit&     currFit);

        void            SetSubfitCovar( const OptSimRunResults& currRun,
                                        const OptSingleFit&     currFit,
                                              int               subfitIndex);

        void            SetCovar(const OptSimRunResults& currRun,
                                 const OptSingleFit&    currFit,
                                       int              index);  // -1 actual, 0 estimated, +ve subfit idex -1

        // processing
        void            DeNormalize();

    private:
        void            SetHeader(const OptSimRunResults& currRun);

};



#endif // !DC_COVAR_H

