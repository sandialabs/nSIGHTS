///////////////////////////////////////////////////////////////////////////////////
//
// DSC_FunctionSpec.h
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
// DESCRIPTION: this class converts a text equation into a postfix form and evaluates
//              for vectors of data - based on Amy Gilkey's Fortran for WIPP PA
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DSC_FUNCTIONSPEC_H
#define DSC_FUNCTIONSPEC_H

#include <genClass/SC_SetupErr.h>

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_StringArray.h>

#include <genDataClass/DSC_Parser.h>


class DSC_FunctionSpec : public DSC_Parser  {
public:
    class Variable;

private:
    enum Operators      {o_EqnEnd,
                         o_UnaryPlus, o_UnaryMinus,
                         o_Plus, o_Minus, o_Multiply, o_Divide, o_Exponent,
                         o_LeftPar, o_RightPar, o_Comma, o_Assign, o_NotSet};

    enum {maxFunc = 20};
    enum Functions      {f_Abs, f_Max, f_Min,
                         f_Sqrt, f_Exp, f_Ln, f_Log10,
                         f_Sin, f_Cos, f_Tan, f_arcSin, f_arcCos, f_arcTan, f_polar,
                         f_VecMax, f_VecMin,
                         f_IFLT0, f_IFEQ0, f_IFGT0, f_Indx, f_NotSet};

    enum TermType       {tt_Operator, tt_Function, tt_Number, tt_Variable, tt_NotSet};

    enum OpTblType      {ot_Fun, ot_End, ot_Unary, ot_PlusMinus, ot_MulDivide, ot_Exponent,
                         ot_LeftPar, ot_RightPar, ot_Comma, ot_Assign};


    enum        {fieldStrLen = 80};

    class Term {
    public:
        TermType    termType;
        Operators   termOp;
        Functions   termFun;
        double      numVal;
        Variable*   termVar;
        int         funNPar;  // number of function parameters expected
        int         valent;   // parameters required for fn/op

        Term();

        Term(Functions tFun);
        Term(Operators tOp);
        Term(const double& tVal);
        Term(Variable&  tVar);

        Term& operator=(const Term& a);

        inline bool IsFunction() const {return termType == tt_Function;}
        inline bool IsOperator() const {return termType == tt_Operator;}
        inline bool IsVariable() const {return termType == tt_Variable;}
        inline bool IsNumber() const   {return termType == tt_Number;}

        inline bool IsNotOperator() const {return termType != tt_Operator;}
        inline bool IsNotFunction() const {return termType != tt_Function;}

        void    ToString(char* termStr, int termStrLen) const;
    private:
        void SetDefault();

    };

    struct FnInfo {

    public:

        char*     fnStr;
        int       npar;
        Functions fType;

        FnInfo() {};
        FnInfo(char* str, int np, Functions ft)
            : fnStr(str), npar(np), fType(ft) {}

        FnInfo& operator=(const FnInfo& a) {
            fnStr = a.fnStr; npar = a.npar; fType = a.fType;
            return *this;
        }
    };

public:

    enum {varIDlen = 40};

    class Variable {
    public:
        char                    varID[varIDlen];
        const SC_DoubleArray*   varData;
        int                     inputClass;
        bool                    inputUsed;

    public:
        Variable();

        void                SetVar(const char* id,
                                   const SC_DoubleArray* vData,
                                   int vClass);

        void                Clear();

        const SC_DoubleArray&   GetVar() const {return *varData;}
    };

    typedef T_SC_Array<Variable> VariableArray;

    VariableArray       inputVars;

    enum                {maxVar = maxField};

private:
    int                 nvecData;

    T_SC_Array<Term>    terms;
    static T_SC_Array<FnInfo> fnInfo;

public:
    DSC_FunctionSpec();
    DSC_FunctionSpec(const DSC_FunctionSpec& a);
    ~DSC_FunctionSpec() {};

    DSC_FunctionSpec& operator= (const DSC_FunctionSpec& a);

    void                ClearForNext(); // keeps inputs, clears everything else

    void                SetVectorSize(int nVecIn);

    void                SetInputVariable(const char*           id,
                                         const SC_DoubleArray& inVar,
                                         int                   varClass = -1);
    // adds input to current list
    void                SetInputVariable(const char*  id,
                                         int          varClass = -1);
    // adds input to current list - for checking only

    bool                CheckInputVariables(SC_SetupErr& err); // checks for duplicate names


    bool                ParseEquation(const char*        inputEqn,
                                      SC_SetupErr&       err);
    // does parse and checks for format - varIDs
    // from inputVars;


    bool                Evaluate(SC_DoubleArray& fnResult,
                                 bool            abortErrors,
                                 SC_SetupErr&    err);
    // evaluates - result in result

private:
    void                LocalCopy(const DSC_FunctionSpec& a);     // common to copy constructor and = operator

    bool                ConvertToPostFix(SC_SetupErr& err);
    // transforms parsed equation for evaluation

};

#endif // DSC_FUNCTIONSPEC_H

