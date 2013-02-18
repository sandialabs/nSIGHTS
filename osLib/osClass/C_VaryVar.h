///////////////////////////////////////////////////////////////////////////////////
//
// C_VaryVar.h
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
//      variable that can be multiply valued (suite or range).
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_VARYVAR_H
#define C_VARYVAR_H

#include <genClass/T_SC_Array.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_SetupErr.h>

#include <objClass/DC_DataCoord.h>

#include <osClass/C_VarBase.h>



class  VaryControlStatics  {
    public:
        enum {maxSuiteVal = 9};

        //  static control variables basic
        static bool  varyIsRange;

    private:
        static int   currVaryIndex;

    public:

        inline static bool          IsRange()      {return varyIsRange;}
        inline static bool          IsSuite()      {return !varyIsRange;}

        inline static void          SetVaryControlIndex(int inIndex) {currVaryIndex = inIndex;}
        inline static int           GetVaryControlIndex() {return currVaryIndex;}

};


class VaryVar;                       // forward ref for ptr

class VaryVarArray : public T_SC_Array<VaryVar*> {
    public:
                VaryVarArray();         // ctor initializes with call to SampVar::GetActiveArray();

        void    CreateFrom(const VarBaseArray& a);
};

class ListObjC;

// base class for all Optimized variables
class   VaryVar  :  public VaryControlStatics, public VarBase {

    friend class VaryVarArray;

    public:
        double          rangeMinLim;    //  varyIsRange
        double          rangeMaxLim;
        bool            rangeIsLinear;
        int             rangeNStep;

        SC_DoubleArray  suiteValues;    //  ! varyIsRange

        int             varyOrder;      // set
        int             varyIndex;

    private:
        static VarBase*     listHead;  //  head of list


    public:
                                VaryVar(bool& isActiveInit, const char* varIDInit, bool defIsLinear = true);
                                VaryVar(bool& isActiveInit, const char* varShortIDInit, const char* varLongIDInit, bool defIsLinear = true);
        virtual                 ~VaryVar();
        VaryVar&                 operator= (const VaryVar& a);

        //  access to curr value
        int                     GetnVary() const;
        double                  GetVaryVal() const;
        double                  GetVaryVal(int inIndex);

        virtual double          GetFinalVaryVal() const;     // default returns GetVaryVal()

        bool                    VarySetupOK(SC_SetupErr& errData);      // returns true if basic setup is OK
        void                    GetMenuStr(char* menuStr, int maxLen);

        void                    GetMinMax(double& minVal,
                                          double& maxVal) const;

        bool                    GetDataCoord(DC_DataCoord& coord);

    // VarBase virtuals
        virtual double          GetVarValue()    const;
        virtual double          GetMinVarValue() const;
        virtual double          GetMaxVarValue() const;

        void                    AddToListing(ListObjC& listObj) const;

    //  remainder are all statics
        static int              GetnActive();  // returns number of active Optimized vars
        static void             GetActiveArray(VaryVarArray& activeArray);
        static void             SetWasActive();

        static void             ResetAllVaryIndexes(); // resets to 0 after suite/range change

    protected:
        void                    LocalCopy(const VaryVar& a);  // implementation of copy

    private:
        static VaryVar*         GetFirstActive(); // returns first active
        VaryVar*                GetNextActive() const; // returns next active from this


                                // copy ctor not allowed
                                VaryVar(const VaryVar& a);
        void                    InitCommon();


};


#endif // !C_VARYVAR_H

