///////////////////////////////////////////////////////////////////////////////////
//
// SC_SampVar.h
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
// DESCRIPTION: class defining distribution an containing static menu strings
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef SC_SAMPVAR_H
#define SC_SAMPVAR_H

#include <genClass/C_FuncObj.h>
#include <genClass/DC_XYCDF.h>
#include <genClass/DC_TableData.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_Random.h>
#include <genClass/SC_StringArray.h>
#include <genClass/SC_SetupErr.h>


class SampleDistributionBase {
public:
    enum SVDistribution  {svdNormal, svdTruncNormal,
                          svdLogNormal, svdTruncLogNormal,
                          svdUniform, svdLogUniform,
                          svdTriangular, svdLogTriangular,
                          svdExponential, svdPoisson, svdWeibull,   // added for paCalc
                          svdBeta, svdStudentT,                     // added for paCalc, student T for nSights
                          svdUserCDF, svdTableLookup, };            // added for paCalc


private:
    static SC_StringArray       distStrs;                   // all distributions

public:

    static const char*              GetDistString(SVDistribution dist); // single

    static void                     GetDistStrings(SC_StringArray& outStrs);    // for use in menus etc
    static const SC_StringArray&    GetDistStrings();

    // for UI support
    static bool                     UsesMean(SVDistribution dist);
    static bool                     UsesStdDev(SVDistribution dist);
    static bool                     UsesMinMax(SVDistribution dist);

    // basic dists only
    static bool                     IsLinear(SVDistribution dist);
    static bool                     IsLog(SVDistribution dist);
    static bool                     IsNormal(SVDistribution dist); // norm or logNorm
    static bool                     IsNotTruncNormal(SVDistribution dist);
    static bool                     IsTruncNormal(SVDistribution dist);
    static bool                     IsUniform(SVDistribution dist); //
    static bool                     IsTriangular(SVDistribution dist); //
    static bool                     IsExponential(SVDistribution dist); //basic to me anyway

    // PA dists
    static bool                     IsPoisson(SVDistribution dist);
    static bool                     IsWeibull(SVDistribution dist);
    static bool                     IsBeta(SVDistribution dist);
    static bool                     IsStudentT(SVDistribution dist);
    static bool                     IsUserCDF(SVDistribution dist);
    static bool                     IsTableLookup(SVDistribution dist);

private:
    static void                     InitDistStrings();

};

// base class for all sampled variables
class SC_SampVar  :  public SampleDistributionBase  {

public:

    SVDistribution      svDistribution;

    double              distMean;               //  for normal/lognormal
    double              distStdDev;             //  for normal
    double              distLogStdDev;          //  for lognormal

    double              distPeak;               //  for triangular, log-triangular
    double              distMin;                //  for non-normal
    double              distMax;                //  for non-normal

    //  extra PA distribution parameters
    double              poissonLambda;          // rate parameter < 1E6
    double              weibullShape;           // weibull
    double              weibullScale;
    double              betaP;                  // beta exponents
    double              betaQ;                  // also uses max & min

    FuncObjRef          userCDFInputObjRef;

    FuncObjRef          tableLookupObjRef;
    int                     tableLookupColumn;

    static const double     poissonMaxLambda;   // 1.0E+06
    static const double     minBetaExp;         // 0.001
    static const double     maxBetaExp;         // 1000.0


protected:
    const DC_XYCDF*         userCDFDC;
    const DC_TableData*     tableLookupDC;

private:
    static double           stdDevMultiplier;   // for get upper/lower
    // default 3.0

    static const double     samplRMin;  // default 4.0E-36
    static const double     samplRMax;  // default 0.9999999

    mutable SC_Random*      currRGen;

    mutable int             degF;       //degrees of freedom for student's t (equal to nsamples-1)

public:
    SC_SampVar(bool defIsLinear = true);
    SC_SampVar(const SC_SampVar& a);
    virtual                 ~SC_SampVar();
    SC_SampVar&             operator= (const SC_SampVar& a);

    //  value usage
    inline bool             UsesMean()      const {return SampleDistributionBase::UsesMean(svDistribution);}
    inline bool             UsesMinMax()    const {return SampleDistributionBase::UsesMinMax(svDistribution);}
    inline bool             IsLinear()      const {return SampleDistributionBase::IsLinear(svDistribution);}
    inline bool             IsPoisson()     const {return SampleDistributionBase::IsPoisson(svDistribution);}
    inline bool             IsWeibull()     const {return SampleDistributionBase::IsWeibull(svDistribution);}
    inline bool             IsBeta()        const {return SampleDistributionBase::IsBeta(svDistribution);}
    inline bool             IsStudentT()    const {return SampleDistributionBase::IsStudentT(svDistribution);}
    inline bool             IsUserCDF()     const {return SampleDistributionBase::IsUserCDF(svDistribution);}
    inline bool             IsTableLookup() const {return SampleDistributionBase::IsTableLookup(svDistribution);}

    //  access to sampled value
    bool                    SampleSetupOK(SC_SetupErr& errData,
                                          const char*  varID);      // returns true if basic setup is OK
    void                    GetMenuStr(char* menuStr, int maxLen);

    //  for use as fixed/forward values
    virtual double          GetMean() const;    // virtual for override in units
    // see discussion below
    virtual double          GetMedian() const;  // virtual for override in units
    // see discussion below
    virtual double          GetStdDev() const;
    // overrides necessary because of limited parameter
    // ranges in the Poisson distribution, which unlike all other distributions requires
    // internal values in user units (or units of the Poisson mean) with conversion
    // to metric if required

    //returns mean/std dev values - log values if distribution is log
    virtual double          GetLogMean() const;
    virtual double          GetLogStdDev() const;

    // gets min and max -- to minR/maxR if not dist defined
    double                  GetDistMin(const double minR = 0.05) const;
    double                  GetDistMax(const double maxR = 0.95) const;

    //  misc access
    const char*             GetDistString() const {return SampleDistributionBase::GetDistString(svDistribution);}


    //  support for user CDFs & table lookup
    //  checks user CDF
    void                    IsBeingDeleted(FuncObjC* delObj);
    //  object recalculation
    bool                    UsesAsInput(const FuncObjC* inputObj) const;


    static  void            SetStdDevMultiplier(const double& mult) {stdDevMultiplier = mult;}
    static  double          GetStdDevMultiplier() {return stdDevMultiplier;}

    //  main sampling driver
    void                    Sample(SC_Random&       randGen,
                                   int              nsamples,
                                   bool             stratify,
                                   SC_DoubleArray&  x) const;

protected:
    void                    LocalCopy(const SC_SampVar& a);  // implementation of copy

private:

    // support
    double          AreaR(const double& probInc,
                          const double& startPt,
                          const double& lowerB,
                          const double& upperB) const;

    double          SetProbInc(int          nsamples,
                               bool             stratify,
                               const double     baseProbInc = 1.0) const;

    double          Norm(double x) const;       //calculates quantile for std. normal dist.

    // for individual distributions
    void            NormalSample(int                nsamples,
                                 bool               stratify,
                                 SC_DoubleArray&    x) const;
    void            TriangleSample(int              nsamples,
                                   bool             stratify,
                                   SC_DoubleArray&  x) const;
    void            UniformSample(int               nsamples,
                                  bool              stratify,
                                  SC_DoubleArray&   x) const;
    void            ExponentialSample(int               nsamples,
                                      bool              stratify,
                                      SC_DoubleArray&   x) const;

    // these are ports from the pascal sampling code, which was in turn a port of FORTRAN code
    // the original FORTRAN data structures are used except that arrays are 0 based and use the
    // T_SC_Array<> vectors


    // Poisson, Weibull & Beta code from Dennis Longsine VB - 20 Jan 03
    // Poisson
    void            PoissonSample(int               nsamples,
                                  bool              stratify,
                                  SC_DoubleArray&   x) const;

    int             GetProbNdx(const double&          r,
                               const SC_IntArray&     tableN,
                               const SC_DoubleArray&  tableP,
                               int&                   startIndx) const;
    // Weibull
    void            WeibullSample(int               nsamples,
                                  bool              stratify,
                                  SC_DoubleArray&   x) const;

    // Beta
    void            BetaSample(int                  nsamples,
                               bool                 stratify,
                               SC_DoubleArray&      x) const;

    double          BetaFact(double& x) const;

    void            MakeBetaTable(int               stepSize,
                                  SC_DoubleArray&   betaX,
                                  SC_DoubleArray&   betaFX) const;

    //Student t
    void            StudentTSample(int              nsamples,
                                   bool             stratify,
                                   SC_DoubleArray&  x) const;

    double          StudentFact(double& t, double maxT) const;

    void            MakeStudentTable(int                 stepSize,
                                     SC_DoubleArray&     betaX,
                                     SC_DoubleArray&     betaFX) const;

    //for Beta and Student t
    double          InterpProb(const double&         r,
                               const SC_DoubleArray& tableX,
                               const SC_DoubleArray& tableFX,
                               int&                  startIndx,
                               bool                  linInterp) const;

    void            UserCDFSample(int               nsamples,
                                  bool              stratify,
                                  SC_DoubleArray&   x) const;

    void            TableLookupSample(int               nsamples,
                                      SC_DoubleArray&   x) const;

    double          PoissonSampleOnce(const double& r) const;
    double          WeibullSampleOnce(const double& r) const;

    const SC_DoubleArray& CheckTable(const char* procName) const;
};

#endif // SC_SAMPVAR_H

