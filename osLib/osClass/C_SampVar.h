///////////////////////////////////////////////////////////////////////////////////
//
// C_SampVar.h
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
//      variable to be sampled.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_SAMPVAR_H
#define C_SAMPVAR_H

#include <genClass/SC_SampVar.h>
#include <genClass/SC_Random.h>

#include <osClass/C_VarBase.h>

//  class containing static control variables
//  set up as super class so implementation can use in derived class without instantiating a
//  SampleVar

class SampleControlStatics  {
    public:
        //  static control variabless
        enum    UncertType  {utMonteCarlo, utFOSM, utPEM};

        static UncertType           uncertType;

        static bool                 forceCorrelationsToZero;
        // sets all non-user specified corr to 0.0
        // otherwise they are random for Monte Carlo, or error for FOSM or PEM

        //Monte Carlo options
        static bool                 samplingIsLHS;           // true if LHS, Monte-Carlo if false
        static bool                 specifyCorrelations;     // allows user specified corr
        static int                  numberOfTrials;         // number of samples
        static int                  randSeed;               // seed for random # gen
        static int                  singleRunRealization;   // realization used for single run

        //FOSM options
        static bool                 calcDerivSpan;
        static double               derivSpan;              //initial span if calculated

    protected:
        static int                  currentTrialIndex;      // current value of trial loop in derived class
                                                            // set to -1 if samples not created

        static  SC_Random           sampleRandGen;


    public:

        inline static bool          IsMonteCarlo()          {return (uncertType == utMonteCarlo);}
        inline static bool          IsFOSM()                {return (uncertType == utFOSM);}
        inline static int           GetnTrials()            {return numberOfTrials;}
        inline static int           GetTrialIndex()         {return currentTrialIndex;}
        static void                 SetTrialIndex(int indx);
        static bool                 TrialIndexOK();

};


//  forward ref for ptr def
class SampVar;

//  class for constructing (usually) sparse matrix of correlations
class   SVCorrelation {
    public:
        SampVar*        correlatedVar;          // correlated to this var
        double          correlationValue;       // correlation val (abs must be >0.0 and < 1.0)
                                                    //  in practice abs limits should be 0.01 to 0.99
    public:
                            SVCorrelation() {correlatedVar = 0; correlationValue = 0.0;}
                            SVCorrelation(SampVar* inVar, const double& inVal) {correlatedVar = inVar; correlationValue = inVal;}
        SVCorrelation&      operator=(const  SVCorrelation& a);
};


class SVCArray : public T_SC_Array<SVCorrelation> {
    public :

                    SVCArray() : T_SC_Array<SVCorrelation>() {};               //default cons
                    SVCArray(int nAll) : T_SC_Array<SVCorrelation>(nAll) {};   //allocates to size nAll

            void    IsBeingDeleted(SampVar*    testVar);   // clears out of list if present

            int     CorrelatedIndex(SampVar*    testVar);  // returns index of correlation to test Var
                                                           // -1 if not found
            bool    IsCorrelated(SampVar*    testVar);     // returns true if correlation to test Var is found

            void    Cleanup();      // removes all 0 entries
};


class SampVarArray : public T_SC_Array<SampVar*> {
    public:
                SampVarArray();         // ctor initializes with call to SampVar::GetActiveArray();
                SampVarArray(bool) {};  // no ctor initialization

        void    CreateFrom(const VarBaseArray& a);
};

class ListObjC;

// base class for all sampled variables
class  SampVar : public SC_SampVar, public SampleControlStatics, public VarBase  {

    friend class Sampler;

    friend class SampVarArray;

    public:
        SVCArray                correlations;           //  for LHS mode with correlations -- updating handled in destructor
                                                    //  note that each var may or may not have value for correlated var
                                                    //  as long as one of each pair has the value the GetMatrix routines work

        static SC_StringArray   corrGroupIDs;       // names of correlation groups
                                                    // all use/alloc by app

    protected:
        SC_DoubleArray*         sampledValues;          //  results of sampling, Monte-Carlo only
        double                  uncertVal;              //  uncertain value, non-MonteCarlo only

        static VarBase*         listHead;               //  head of list

    private:
        int                     corrGroup;              // only vars in same corr group can be correlated
                                                        // set by constructor

    public:
                                SampVar(bool& isActiveInit, int corrGroupInit, const char* varIDInit, bool defIsLinear = true);
                                SampVar(bool& isActiveInit, int corrGroupInit, const char* varShortIDInit, const char* varLongIDInit, bool defIsLinear = true);
        virtual                 ~SampVar();
        SampVar&                operator= (const SampVar& a);


    //  access to sampled value
        bool                    SampVarOK();
        double                  GetSampleVal() const {return GetCurrentValue(currentTrialIndex);}
        double                  GetFinalSampleVal() const {return GetFinalValue(currentTrialIndex);}
        bool                    SampleSetupOK(SC_SetupErr& errData); // returns true if basic setup is OK

        bool                    UsesAsInput(const FuncObjC* inputObj) const;

    //  access to uncertain value
        bool                    UncertVarOK();
        double                  GetUncertVal() const {return uncertVal;}
        void                    SetUncertVal(const double newVal) {uncertVal = newVal;}

    //  correlation access
        int                     GetCorrGroup() const {return corrGroup;}
        void                    SetCorrGroup(int inGrp);
        void                    InitializeCorrGroup(int inGrp) {corrGroup = inGrp;} // careful use only when no correlations are set

        inline bool             GroupMatches(int groupToMatch) const {return corrGroup == groupToMatch;}
        double                  GetCorrelation(SampVar* otherVar);  // 1.0 if otherVar == this
                                                                    // 0.0 if ! in correlations
                                                                    // value otherwise
        double                  GetUncertainCorrelation(SampVar* otherVar);  //forces to zero if required

    //  checking and transformation in derived classes (C_SampVarUnits specifically)
        // this is implemented to support unit conversion in derived classes
        // where GetFinalValue returns the display value (in user units)
        // and GetCurrentValue returs the internal value (in metric units)
        // overrides for GetCurrentValue are necessary because of limited parameter
        // ranges in the Poisson distribution, which unlike all other distributions requires
        // internal values in user units (or units of the Poisson mean) with conversion
        // to metric if required
        virtual   double        GetFinalValue(int sampleNum) const;    // base class gets internal sampled value
        virtual   double        GetCurrentValue(int sampleNum) const;  // base class gets internal sampled value
        virtual   bool          CheckLimits(int sampleNum) const;

        //  to process units correctly when using secondary samples
        virtual   double        GetCurrentValue(const double& userValue) const;    // base class gets internal sampled value

    // VarBase virtuals
        virtual double          GetVarValue()    const;
        virtual double          GetMinVarValue() const;
        virtual double          GetMaxVarValue() const;


    //  remainder are all statics
        static int              GetnActive();  // returns number of active sampled vars
        static void             GetActiveArray(SampVarArray& activeArray);
        static void             SetWasActive();


        static bool             CorrGroupOK(int groupToMatch); // returns true if more than 1 sampled var in group

        static void             GetCorrGroup(int            groupToMatch,
                                             SampVarArray&  corrGroup);

        //  get/setting of correlation matrix -- no sampled var creation should occur between get&set
        static  bool            GetCorrelationMatrix(int                matrixCorrGroup,    // only samples with same corr Group
                                                     SC_DoubleMatrix&   corrMatrix);        // returned as fully popuated nxn w
                                // returns true if more than 1 sampled var in group

        static  bool            GetUncertCorrelationMatrix(int                matrixCorrGroup,    // only samples with same corr Group
                                                            SC_DoubleMatrix&   corrMatrix);

        static  bool            GetCorrelationMatrix(int                matrixCorrGroup,    // only samples with same corr Group
                                                     SC_DoubleMatrix&   corrMatrix,         // returned as fully popuated nxn w
                                                     SC_StringArray&    corrIDs);           // names of rows/columns (len assumed set appropriately)

        static  void            SetCorrelationMatrix(int                     matrixCorrGroup,
                                                     const SC_DoubleMatrix&  corrMatrix);

        static  int             GetnCorrGroup(int groupToMatch); //required by uncertainty error checking in G_Sample

        // used only by config files for post read address fixups
        static SampVar*         GetFirst();
        SampVar*                GetNext()       const;

        void                    AddToListing(ListObjC& listObj) const;


    protected:
        void                    LocalCopy(const SampVar& a);  // implementation of copy


    private:
        SampVar*                GetNextActive() const; // returns next active from this
        static SampVar*         GetFirstActive(); // returns first active

                                // copy ctor not allowed
                                SampVar(const SampVar& a);

                                // common to all constructors
        void                    InitCommon();

        void                    ClearCorrelations(); // used by dtor and SetCorrGroup

                                //  main sampling driver
        void                    Sample(SC_DoubleArray&   x);

};



#endif // !C_SAMPVAR_H

