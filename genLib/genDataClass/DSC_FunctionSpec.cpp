///////////////////////////////////////////////////////////////////////////////////
//
// DSC_FunctionSpec.cpp
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

#include <genClass/C_Common.h>
#include <genClass/SC_RealFormat.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/U_Real.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genDataClass/DSC_FunctionSpec.h>


DSC_FunctionSpec::Term::Term()
{
    SetDefault();
}

DSC_FunctionSpec::Term::Term(Functions tFun)
{
    SetDefault();
    termType = tt_Function;
    termFun  = tFun;
    funNPar  = 1;
    valent   = 0;
}

DSC_FunctionSpec::Term::Term(Operators tOp)
{
    SetDefault();
    termType = tt_Operator;
    termOp   = tOp;
    valent = 2;
}

DSC_FunctionSpec::Term::Term(const double& tVal)
{
    SetDefault();
    termType = tt_Number;
    numVal   = tVal;
}

DSC_FunctionSpec::Term::Term(Variable&  tVar)
{
    SetDefault();
    termType  = tt_Variable;
    termVar   = &tVar;
}


void DSC_FunctionSpec::Term::SetDefault()
{
    termType = tt_NotSet;
    termOp   = o_NotSet;
    termFun  = f_NotSet;
    termVar  = 0;
    numVal   = 0.0;
    funNPar  = 0;
    valent   = -999;
}

DSC_FunctionSpec::Term& DSC_FunctionSpec::Term::operator=(const Term& a)
{
    if (&a != this)
    {
        termType = a.termType;
        termOp = a.termOp;
        termFun = a.termFun;
        numVal = a.numVal;
        termVar = a.termVar;
        funNPar = a.funNPar;
        valent = a.valent;
    }
    return *this;

}



void DSC_FunctionSpec::Term::ToString(char* termStr, int termStrLen) const
{
    switch (termType) {
        case tt_Operator : {
            static const char* opStrs[] = {"#", "~+", "~-", "+", "-", "*", "/", "^",
                "(", ")", ",", "=", "??"};
            MakeString(termStr, "Op: ", opStrs[termOp], termStrLen);
            break;
        }
        case tt_Function : {
            static const char* fnStrs[] = {"ABS", "MAX", "MIN", "SQRT", "EXP",
                                            "LOG", "LOG10",
                                            "SIN", "COS", "TAN", "ARCSIN", "ARCTAN", "ARCCOS", "POLAR",
                                            "ENVMAX", "ENVMIN",
                                            "IFLT0", "IFEQ0", "IFGT0", "??"};
            MakeString(termStr, "Fn: ", fnStrs[termFun], termStrLen);
            break;
        }
        case tt_Number : {
            char tempStr[40];
            SC_RealFormat rForm;
            rForm.RealToString(numVal, tempStr, 40);
            MakeString(termStr, "Cons: ", tempStr, termStrLen);
            break;
        }
        case tt_Variable : {
            if (termVar == 0)
                CopyString(termStr, "Var: not set", termStrLen);
            else
                MakeString(termStr, "Var: ", termVar->varID, termStrLen);
            break;
        }
        case tt_NotSet : {
            CopyString(termStr, "not set ??", termStrLen);
            break;
        }
    }
}


DSC_FunctionSpec::Variable::Variable()
{
    Clear();
}

void DSC_FunctionSpec::Variable::Clear()
{
    SetToNull(varID);
    varData = 0;
    inputClass = -1;
    inputUsed = false;
}


void DSC_FunctionSpec::Variable::SetVar(const char* id,
                                        const SC_DoubleArray* vData,
                                        int vClass)
{
    CopyString(varID, id, varIDlen);
    UpperCase(varID);
    varData = vData;
    inputClass = vClass;
    inputUsed = false;
}



T_SC_Array<DSC_FunctionSpec::FnInfo> DSC_FunctionSpec::fnInfo;


DSC_FunctionSpec :: DSC_FunctionSpec()
{
    if (fnInfo.IsEmpty())
    {
        fnInfo.Alloc(maxFunc + 4);  //
        fnInfo += FnInfo("ABS",   1, f_Abs);
        fnInfo += FnInfo("MAX",  -1, f_Max);
        fnInfo += FnInfo("MIN",  -1, f_Min);
        fnInfo += FnInfo("SQRT",  1, f_Sqrt);
        fnInfo += FnInfo("EXP",   1, f_Exp);
        fnInfo += FnInfo("LOG",   1, f_Ln);
        fnInfo += FnInfo("LN",    1, f_Ln);
        fnInfo += FnInfo("LOG10", 1, f_Log10);
        fnInfo += FnInfo("LGT",   1, f_Log10);
        fnInfo += FnInfo("SIN",   1, f_Sin);
        fnInfo += FnInfo("COS",   1, f_Cos);
        fnInfo += FnInfo("TAN",   1, f_Tan);
        fnInfo += FnInfo("ARCSIN",1, f_arcSin);
        fnInfo += FnInfo("ARCCOS",1, f_arcCos);
        fnInfo += FnInfo("ARCTAN",1, f_arcTan);
        fnInfo += FnInfo("POLAR", 2, f_polar);
        fnInfo += FnInfo("VECMIN",1, f_VecMin);
        fnInfo += FnInfo("ENVMIN",1, f_VecMin);
        fnInfo += FnInfo("VECMAX",1, f_VecMax);
        fnInfo += FnInfo("ENVMAX",1, f_VecMax);
        fnInfo += FnInfo("IFLT0", 3, f_IFLT0);
        fnInfo += FnInfo("IFEQ0", 3, f_IFEQ0);
        fnInfo += FnInfo("IFGT0", 3, f_IFGT0);
        fnInfo += FnInfo("INDX",  1, f_Indx);
    }


    nvecData = -1;
}


DSC_FunctionSpec::DSC_FunctionSpec(const DSC_FunctionSpec& a)
{
    LocalCopy(a);
}


DSC_FunctionSpec& DSC_FunctionSpec::operator= (const DSC_FunctionSpec& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}



void DSC_FunctionSpec::ClearForNext()
{
    terms.SetResizable(maxVar);
    fields.SetResizable(maxVar);
}


void DSC_FunctionSpec::SetVectorSize(int nVecIn)
{
    nvecData = nVecIn;
}

void DSC_FunctionSpec::SetInputVariable(const char*             id,
                                        const SC_DoubleArray&   inVar,
                                              int               varClass)
{
    if (inputVars.IsFull())
        GenAppInternalError("DSC_FunctionSpec::SetInputVariable-1");

    if (inVar.Size() != nvecData)
        GenAppInternalError("DSC_FunctionSpec::SetInputVariable-2");

    inputVars.nlistData++;
    inputVars.LastIndex().SetVar(id, &inVar, varClass);
}

void DSC_FunctionSpec::SetInputVariable(const char*             id,
                                              int               varClass)
{
    if (inputVars.IsFull())
        GenAppInternalError("DSC_FunctionSpec::SetInputVariable-1");

    inputVars.nlistData++;
    inputVars.LastIndex().SetVar(id, 0, varClass);
}

bool DSC_FunctionSpec::CheckInputVariables(SC_SetupErr& err)
{
    const char* opStr = "~^*/+-(),=";
    int nop = StringLength(opStr);
    for (int i = 0; i < inputVars.Size(); i++)
        for (int j = 0; j < nop; j++)
        {
            if (strchr(inputVars[i].varID, opStr[j]) != NULL)
            {
                err.SetInternalError(inputVars[i].varID, " contains a reserved operator: ~^*/+-(),=");
                return false;
            }
        }

    for (int i = 0; i < inputVars.UpperBound(); i++)
        for (int j = i + 1; j < inputVars.Size(); j++)
            if (SameUCString(inputVars[i].varID, inputVars[j].varID))
            {
                err.SetInternalError(inputVars[i].varID, " is a duplicate variable ID");
                return false;
            }

    return true;
}

bool DSC_FunctionSpec::ParseEquation(const char* inputEqn,
                                          SC_SetupErr& err)
{
    if (!GetFields(inputEqn, err))
        return false;

    int nextField = 0;
    int nextTerm = 0;
    while (nextField < fields.Size())
    {
        const Field& currField = fields[nextField];
        nextField++;

        Term nextTerm;

        if (currField.IsString())
        {
            // variable or function ?
            // function has next field = "("
            if ((nextField < fields.UpperBound()) &&
                (fields[nextField].fieldType == ft_Character) &&
                (fields[nextField].fChar == '('))
            {
                nextTerm = Term(f_NotSet);
                for (int i = 0; i < fnInfo.Size(); i++)
                    if (SameUCString(currField.fStr, fnInfo[i].fnStr))
                    {
                        nextTerm.termFun = fnInfo[i].fType;
                        nextTerm.funNPar = fnInfo[i].npar;
                        break;
                    }
                if (nextTerm.termFun == f_NotSet)
                {
                    err.SetInternalError(currField.fStr, " is not a function");
                    return false;
                }
            }
            else
            {
                nextTerm.termVar = 0;
                for (int i = 0; i < inputVars.Size(); i++)
                    if (SameUCString(currField.fStr, inputVars[i].varID))
                    {
                        nextTerm = Term(inputVars[i]);
                        inputVars[i].inputUsed = true;
                        break;
                    }

                if (nextTerm.termVar == 0)
                {
                    err.SetInternalError(currField.fStr, " is not a variable");
                    return false;
                }
            }
        }
        else if (currField.IsNumber())
        {
            nextTerm = Term(currField.fVal);
        }
        else
        {
            Operators tempOp = o_NotSet;
            switch (currField.fChar) {
                case '+' : { tempOp = o_Plus; break;}
                case '-' : { tempOp = o_Minus; break;}
                case '*' : { tempOp = o_Multiply; break;}
                case '/' : { tempOp = o_Divide; break;}
                case '^' : { tempOp = o_Exponent; break;}
                case '(' : { tempOp = o_LeftPar; break;}
                case ')' : { tempOp = o_RightPar; break;}
                case ',' : { tempOp = o_Comma; break;}
                case '=' : { tempOp = o_Assign; break;}
                default : {
                    char opStr[5];
                    opStr[0] = currField.fChar;
                    opStr[1] = '\0';
                    err.SetInternalError(opStr, " is not an operator");
                    return false;
                }
            }

            nextTerm = Term(tempOp);
        }

        terms += nextTerm;
    }


    // add end
    terms += Term(o_EqnEnd);

    // convert binary +/- to unary
    for (int i = 0; i < terms.UpperBound(); i++)
    {
        Term& currTerm = terms[i];
        Operators currOp = currTerm.termOp;
        if (currTerm.IsOperator() &&
            ((currOp == o_Plus) || (currOp == o_Minus)))
        {
            Operators lastOp = o_Assign;
            if (i > 0)
            {
                const Term& lastTerm = terms[i-1];
                if (lastTerm.IsOperator() &&
                     ((lastTerm.termOp == o_LeftPar) ||
                      (lastTerm.termOp == o_Comma)))
                    lastOp = lastTerm.termOp;
                else
                    lastOp = o_NotSet;
            }

            if ((lastOp == o_Assign) || (lastOp == o_LeftPar) || (lastOp == o_Comma))
            {
                // check for -ve constant
                if ((currOp == o_Minus) && (terms[i+1].IsNumber()))
                {
                    // change term to plus
                    currOp = o_Plus;
                    terms[i+1].numVal = -terms[i+1].numVal;
                }

                // signed expression - change to unary +/-
                if (currOp == o_Plus)
                    currOp = o_UnaryPlus;
                else
                    currOp = o_UnaryMinus;
                currTerm.termOp = currOp;
                currTerm.valent = 1;
            }
        }
    }

    if (!ConvertToPostFix(err))
        return false;

//  Check the number of parameters for all functions

    for (int i = 0; i < terms.Size(); i++)
    {
        Term& currTerm = terms[i];
        if (currTerm.IsFunction())
        {
            if (currTerm.funNPar < 0)
                currTerm.funNPar = currTerm.valent;
            if ((currTerm.valent < 0) || (currTerm.valent != currTerm.funNPar))
            {
                char termStr[40];
                currTerm.ToString(termStr, 40);
                err.SetInternalError(termStr, " has incorrect # of parameters");
                return false;
            }
        }
    }

    return true;
}

bool DSC_FunctionSpec::ConvertToPostFix(SC_SetupErr& err)
// transforms parsed equation for evaluation
{
/*      INTEGER IOPTBL(-1:8,-1:8)
      SAVE IOPTBL
C      --IOPTBL - chart of stack actions to be performed
C      --IOPTBL(i,j) - i = top stack operator, j = current operator
C      --   = 1   Pop operator from stack into the equation, redo current
C      --   = 2   Push operator onto the stack
C      --   = 3   Pop matching '(' off the stack
C      --   = 4   Delete from equation
C      --   = 5   Process function parameters
C      --   = 6   Pop function from stack into the equation
C      --   = 7   Stop processing at end of equation
C      --   = 0   Error */


    enum StackOp {so_Pop, so_Push, so_PopPara, so_Delete,
        so_FnPar, so_PopFn, so_Stop, so_Error};

    enum {nopTable = 10};

    StackOp tblOps[nopTable][nopTable] =  {
//F             #           ~           +           *           ^           (           )           ,           =
//DATA (IOPTBL(-1,J),J=-1,8) / 2, 0, 2, 2, 2, 2, 5, 6, 5, 0 /
{so_Push,   so_Error,   so_Push,    so_Push,    so_Push,    so_Push,    so_FnPar,   so_PopFn,   so_FnPar,   so_Error},

//DATA (IOPTBL(0,J), J=-1,8) / 2,-7, 2, 2, 2, 2, 2, 0, 0, 2 /
{so_Push,   so_Error,   so_Push,    so_Push,    so_Push,    so_Push,    so_Push,    so_Error,   so_Error,   so_Push},

//DATA (IOPTBL(1,J), J=-1,8) / 2, 1, 0, 1, 1, 2, 2, 1, 1, 0 /
{so_Push,   so_Pop,     so_Error,   so_Pop,     so_Pop,     so_Push,    so_Push,    so_Pop,     so_Pop,     so_Error},

//DATA (IOPTBL(2,J), J=-1,8) / 2, 1, 2, 1, 2, 2, 2, 1, 1, 0 /
{so_Push,   so_Pop,     so_Push,    so_Pop,     so_Push,    so_Push,    so_Push,    so_Pop,     so_Pop,     so_Error},

//DATA (IOPTBL(3,J), J=-1,8) / 2, 1, 2, 1, 1, 2, 2, 1, 1, 0 /
{so_Push,   so_Pop,     so_Push,    so_Pop,     so_Pop,     so_Push,    so_Push,    so_Pop,     so_Pop,     so_Error},

//DATA (IOPTBL(4,J), J=-1,8) / 2, 1, 2, 1, 1, 2, 2, 1, 1, 0 /
{so_Push,   so_Pop,     so_Push,    so_Pop,     so_Pop,     so_Push,    so_Push,    so_Pop,     so_Pop,     so_Error},

//DATA (IOPTBL(5,J), J=-1,8) / 2, 0, 2, 2, 2, 2, 2, 3, 0, 0 /
{so_Push,   so_Error,   so_Push,    so_Push,    so_Push,    so_Push,    so_Push,    so_PopPara, so_Error,   so_Error},

//DATA (IOPTBL(6,J), J=-1,8) / 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /
{so_Error,  so_Error,   so_Error,   so_Error,   so_Error,   so_Error,   so_Error,   so_Error,   so_Error,   so_Error},

//DATA (IOPTBL(7,J), J=-1,8) / 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /
{so_Error,  so_Error,   so_Error,   so_Error,   so_Error,   so_Error,   so_Error,   so_Error,   so_Error,   so_Error},

//DATA (IOPTBL(8,J), J=-1,8) / 2, 7, 2, 2, 2, 2, 2, 0, 0, 0 /
{so_Push,   so_Stop,    so_Push,    so_Push,    so_Push,    so_Push,    so_Push,    so_Error,   so_Error,   so_Error}

    };

    T_SC_Array<Term> eqnStack;
    eqnStack.SetResizable(terms.Size());
    eqnStack += Term(o_EqnEnd);

    T_SC_Array<OpTblType> opStack;
    opStack.SetResizable(terms.Size());
    opStack += ot_End;


    bool expectPop = false;

    int eqnTOS = -1;
    for (int i = 0; i < terms.Size(); i++)
    {
        bool ok = false;
        const Term& nextTerm = terms[i];
        if (nextTerm.IsOperator())
        {
            if ((nextTerm.termOp == o_UnaryPlus) ||
                (nextTerm.termOp == o_UnaryMinus) ||
                (nextTerm.termOp == o_LeftPar))
                ok = ! expectPop;
            else
                ok = expectPop;
            expectPop = (nextTerm.termOp == o_RightPar);
        }
        else if (nextTerm.IsFunction())
        {
            ok = !expectPop;
            expectPop = false;
        }
        else
        {
            ok = !expectPop;
            expectPop = true;
        }

        if (!ok)
        {
            if (nextTerm.termOp == o_EqnEnd)
            {
                err.SetConstantError("unexpected end of eqn (#) found");
            }
            else if (i == 0)
            {
                err.SetConstantError("unexpected first operator ");
            }
            else
            {
                char currTStr[80], lastTStr[80];
                nextTerm.ToString(currTStr, 80);
                terms[i-1].ToString(lastTStr, 80);
                char outStr[80];
                MakeString(outStr, "Consecutive ", lastTStr, " and ", currTStr, " found", 80);
                err.SetInternalError(outStr);
            }
            return false;
        }

        if (nextTerm.IsNotFunction() && nextTerm.IsNotOperator())
        {
            //  Move entry up in the equation
            eqnTOS++;
            terms[eqnTOS] = nextTerm;
        }
        else
        {
            OpTblType tblType = ot_Fun;
            if (nextTerm.IsOperator())
            {
                // maps operators to opTable
                static const OpTblType opMap[] = {  ot_End,
                                                    ot_Unary, ot_Unary,
                                                    ot_PlusMinus, ot_PlusMinus,
                                                    ot_MulDivide, ot_MulDivide,
                                                    ot_Exponent,
                                                    ot_LeftPar, ot_RightPar,
                                                    ot_Comma, ot_Assign, ot_End};
                tblType = opMap[nextTerm.termOp];
            }

            StackOp nextOp = so_Pop;

            while (nextOp == so_Pop) {

                nextOp =  tblOps[opStack.LastIndex()][tblType];

                if (nextOp == so_Error)
                {
                    char currTStr[80], lastTStr[80];
                    nextTerm.ToString(currTStr, 80);
                    if (i > 0)
                        terms[i-1].ToString(lastTStr, 80);
                    else
                        CopyString(lastTStr, "before start??", 80);
                    char outStr[80];


                    if (nextTerm.termOp == o_Assign)
                    {
                        MakeString(outStr, "Misplaced = after ", lastTStr, 80);
                        err.SetInternalError(outStr);
                    }
                    else if (nextTerm.termOp == o_Comma)
                    {
                        MakeString(outStr, "Misplaced , after ", lastTStr, 80);
                        err.SetInternalError(outStr);
                    }
                    else if (nextTerm.termOp == o_RightPar)
                    {
                        MakeString(outStr, "Extra ) after ", lastTStr, 80);
                        err.SetInternalError(outStr);
                    }
                    else if (eqnStack.LastIndex().termOp == o_LeftPar)
                    {
                        MakeString(outStr, "Mismatched ( after ", lastTStr, 80);
                        err.SetInternalError(outStr);
                    }
                    else if ((opStack.LastIndex() == ot_Fun) && (nextTerm.termOp == o_EqnEnd))
                    {
                        eqnStack.LastIndex().ToString(lastTStr, 80);
                        MakeString(outStr, "Missing ) for function", lastTStr, 80);
                        err.SetInternalError(outStr);
                    }
                    else
                    {
                        eqnStack.LastIndex().ToString(lastTStr, 80);
                        MakeString(outStr, "Problem: TOS ", lastTStr, " entry: ", currTStr, 80);
                        err.SetInternalError(outStr);
                    }
                    return false;
                }

                if ((nextOp == so_Pop) || (nextOp == so_PopFn))
                {
                    // pop operator from stack onto the equation, and redo
                    eqnTOS++;
                    terms[eqnTOS] = eqnStack.LastIndex();
                    eqnStack.nlistData--;
                    opStack.nlistData--;
                }
            }

            bool done = (nextOp == so_PopFn);
            while (!done) {

                done = true;
                switch (nextOp) {
                    case so_Pop: case so_Error : {
                        GenAppInternalError("DSC_FunctionSpec::ConvertToPostFix");
                        break;
                    }
                    case so_Push : {
                        //  Push operator onto the stack
                        eqnStack += nextTerm;
                        opStack += tblType;
                        if (nextTerm.IsNotOperator())
                            eqnStack.LastIndex().valent = 0;
                        break;
                    }
                    case so_PopPara : {
                        eqnStack.nlistData--;
                        opStack.nlistData--;
                        break;
                    }
                    case so_Delete : {
                        break;
                    }
                    case so_FnPar : {
                        Term& tosTerm = eqnStack.LastIndex();
                        if (nextTerm.termOp == o_LeftPar)
                        {
                            // Handle beginning '(' for function
                            if (tosTerm.valent <= 0)
                                tosTerm.valent++;
                            else
                            {
                                // '(' within function
                                nextOp = so_Push;
                                done = false;
                            }
                        }
                        else
                            tosTerm.valent++;
                        break;
                    }
                    case so_Stop : {
                        break;
                    }
                }
            }
        }
    }

    terms.SetSize(eqnTOS + 1);


    return true;
}

bool DSC_FunctionSpec::Evaluate(SC_DoubleArray& fnResult,
                                bool             abortErrors,
                                SC_SetupErr&     err)
{
    SC_DoubleMatrix stack;
    stack.Alloc(terms.Size() * 2);


    // evaluate post fix eqn
    int itos = -1; // top of stack
    SC_BoolArray calcOK(nvecData, true);


    for (int i = 0; i < terms.Size(); i++)
    {
        const Term& currTerm = terms[i];
        stack.SetSize(itos + 1);

        if  (currTerm.IsNumber())
        {
            if (stack.IsFull())
            {
                err.SetConstantError("stack full cannot add number");
                return false;
            }

            // put constant on stack;
            itos++;
            stack[itos].AllocAndFill(nvecData, currTerm.numVal);
        }
        else if (currTerm.IsVariable())
        {
            if (stack.IsFull())
            {
                err.SetConstantError("stack full cannot add variable");
                return false;
            }

            // put all variable values on stack
            itos++;
            stack[itos] = currTerm.termVar->GetVar();

            // check for nulls
            const SC_DoubleArray& nextV = stack[itos];
            for (int j = 0; j < nvecData; j++)
                if (calcOK[j])
                    calcOK[j] = !RealIsNull(nextV[j]);

        }
        else if (currTerm.IsOperator())
        {
            Operators currOp = currTerm.termOp;

            int nop, iop1, iop2;

            if ((currOp == o_UnaryPlus) || (currOp == o_UnaryMinus))
            {
                nop = 1;
                iop1 = itos;
                iop2 = itos;
            }
            else
            {
                nop = 2;
                iop1 = itos - 1;
                iop2 = itos;
                itos--;
                if (itos < 0)
                {
                    err.SetConstantError("binary operator stack problem");
                    return false;
                }
            }
            SC_DoubleArray& result = stack[itos];
            SC_DoubleArray& op1 = stack[iop1];
            SC_DoubleArray& op2 = stack[iop2];

            switch (currOp) {
                case o_EqnEnd : {
                    break ;
                }
                case o_UnaryPlus : {
                    break ;
                }
                case o_UnaryMinus : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                            result[i] = -op1[i];
                    break;
                }
                case o_Plus : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                            result[i] = op1[i] + op2[i];
                    break;
                }
                case o_Minus : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                            result[i] = op1[i] - op2[i];
                    break;
                }
                case o_Multiply : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = op1[i] * op2[i];
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("overflow on multiply");
                                return false;
                            }
                        }
                    break;
                }
                case o_Divide : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = op1[i] / op2[i];
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable divide by zero");
                                return false;
                            }
                        }

                    break;
                }
                case o_Exponent : {
                    for (int i = 0; i < nvecData; i++)
                    {
                        result[i] = pow(op1[i], op2[i]);
                        calcOK[i] = IsFinite(result[i]);
                        if (abortErrors && (!calcOK[i]))
                        {
                            err.SetConstantError("probable exponent overflow");
                            return false;
                        }
                    }
                    break;
                }
                default : {
                    char termStr[40];
                    currTerm.ToString(termStr, 40);
                    err.SetInternalError(termStr, " :unexpected operator on stack ??");
                    return false;
                }
            }
        }
        else if (currTerm.IsFunction())
        {
            int npar = currTerm.valent;
            // first par on stack
            int iparm = itos - npar + 1;
            itos = iparm;
            if (itos < 0)
            {
                err.SetConstantError("function stack problem");
                return false;
            }

            SC_DoubleArray& result = stack[itos];

            switch (currTerm.termFun) {
                case f_Abs : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                            result[i] = fabs(result[i]);
                    break;
                 }
                case f_Max : {
                    // multiple param
                    // result already has par1, start at par2
                    for (int j = iparm + 1; j < iparm + npar; j++)
                    {
                        SC_DoubleArray& opn = stack[j];
                        for (int i = 0; i < nvecData; i++)
                            if (calcOK[i])
                                result[i] = DMax(opn[i], result[i]);
                    }
                    break;
                 }
                case f_Min : {
                    // multiple param
                    // result already has par1, start at par2
                    for (int j = iparm + 1; j < iparm + npar; j++)
                    {
                        SC_DoubleArray& opn = stack[j];
                        for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                            result[i] = DMin(opn[i], result[i]);
                    }
                    break;
                 }
                case f_Sqrt : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = sqrt(result[i]);
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable square root of -ve value");
                                return false;
                            }
                        }

                    break;
                 }
                case f_Exp : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = exp(result[i]);
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable overflow on exp");
                                return false;
                            }
                        }
                    break;
                 }
                case f_Ln : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = log(result[i]);
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable log of 0 or -ve value");
                                return false;
                            }
                        }
                    break;
                 }
                case f_Log10 : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = log10(result[i]);
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable log10 of 0 or -ve value");
                                return false;
                            }
                        }

                    break;
                 }
                case f_Sin : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = sin(result[i]);
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable sin of out of range value");
                                return false;
                            }
                        }

                    break;
                 }
                case f_Cos : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = cos(result[i]);
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable cos of out of range value");
                                return false;
                            }
                        }

                    break;
                 }
                case f_Tan : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = tan(result[i]);
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable tan of out of range value");
                                return false;
                            }
                        }

                    break;
                 }
                case f_arcSin : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = asin(result[i]);
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable arcsin of out of range value");
                                return false;
                            }
                        }

                    break;
                 }
                case f_arcCos : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = acos(result[i]);
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable arccos of out of range value");
                                return false;
                            }
                        }

                    break;
                 }
                case f_arcTan : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                        {
                            result[i] = atan(result[i]);
                            calcOK[i] = IsFinite(result[i]);
                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("probable arctan of out of range value");
                                return false;
                            }
                        }

                    break;
                 }
                //this function takes the x,y coords as arguments, and returns the polar coordinate angle in radians
                case f_polar : {
                    SC_DoubleArray& x = stack[iparm];
                    SC_DoubleArray& y = stack[iparm + 1];

                    for (int i = 0; i < nvecData; i++)

                        if (calcOK[i])
                        {
                            double radius = sqrt(x[i]*x[i]+y[i]*y[i]);
                            if (radius > stdEps)
                            {

                                if (y[i]>=0)
                                    result[i] = acos(x[i]/radius);
                                else
                                    result[i] = 2*pi - acos(x[i]/radius);
                                calcOK[i] = IsFinite(result[i]);

                            }
                            else
                                calcOK[i] = false;

                            if (abortErrors && (!calcOK[i]))
                            {
                                err.SetConstantError("error in function POLAR, probable radius equal to zero");
                                return false;
                            }
                        }
                    break;
                 }
                case f_VecMax : case f_VecMin : {
                    double minVal, maxVal;
                    for (int i = 0; i < nvecData; i++)
                        if (!calcOK[i])
                            result[i] = nullReal;
                    result.CalcMinMax(minVal, maxVal);
                    if (!RealIsNull(minVal))
                    {
                        if (currTerm.termFun == f_VecMax)
                            result.FillToAlloc(maxVal);
                        else
                            result.FillToAlloc(minVal);
                        calcOK.FillToAlloc(true);
                    }
                    break;
                 }
                case f_IFLT0 : {
                    SC_DoubleArray& testVal = stack[iparm];
                    SC_DoubleArray& val1 = stack[iparm + 1];
                    SC_DoubleArray& val2 = stack[iparm + 2];
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                            if (testVal[i] < 0.0)
                                result[i] = val1[i];
                            else
                                result[i] = val2[i];
                    break;
                 }
                case f_IFEQ0 : {
                    SC_DoubleArray& testVal = stack[iparm];
                    SC_DoubleArray& val1 = stack[iparm + 1];
                    SC_DoubleArray& val2 = stack[iparm + 2];
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                            if (testVal[i] == 0.0)
                                result[i] = val1[i];
                            else
                                result[i] = val2[i];
                    break;
                 }
                case f_IFGT0 : {
                    SC_DoubleArray& testVal = stack[iparm];
                    SC_DoubleArray& val1 = stack[iparm + 1];
                    SC_DoubleArray& val2 = stack[iparm + 2];
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                            if (testVal[i] > 0.0)
                                result[i] = val1[i];
                            else
                                result[i] = val2[i];
                    break;
                }
                case f_Indx : {
                    for (int i = 0; i < nvecData; i++)
                        if (calcOK[i])
                            result[i] += double(i + 1);
                    break;
                }
                default : {
                    char termStr[40];
                    currTerm.ToString(termStr, 40);
                    err.SetInternalError(termStr, " :unexpected function on stack ??");
                    return false;
                }

            }
        }
    }

    fnResult = stack[itos];
    for (int i = 0; i < nvecData; i++)
        if (!calcOK[i])
            fnResult[i] = nullReal;

    return true;
}



void  DSC_FunctionSpec::LocalCopy(const DSC_FunctionSpec& a)
{
}

