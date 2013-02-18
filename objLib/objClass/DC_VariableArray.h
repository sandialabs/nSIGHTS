///////////////////////////////////////////////////////////////////////////////////
//
// DC_VariableArray.h
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

#ifndef DC_VARIABLEARRAY_H
#define DC_VARIABLEARRAY_H

#include <genClass/T_SC_Array.h>
#include <genClass/SC_SetupErr.h>
#include <genClass/SC_StringArray.h>


class DC_Variable {
    public:
        enum    {varIDLen = 80};
        char    varID[varIDLen];
        double  varValue;
    public:
                        DC_Variable();
                        DC_Variable(const DC_Variable& a);

        DC_Variable&    operator=(const DC_Variable& a);
        void            SetID(const char* inID);
        const char*     GetID() const   {return varID;}
        void            SetValue(const double& inVal) {varValue = inVal;}
        double          GetValue() const {return varValue;}

        bool            IsOK() const;
        bool            IsOK(SC_SetupErr& errData) const;
    private:
        void            LocalCopy(const DC_Variable& a);

};


class DC_VariableArray: public T_SC_Array<DC_Variable> {

    public :
            //  constructors
                        DC_VariableArray();
                        DC_VariableArray(int nAll):T_SC_Array<DC_Variable>(nAll) {}
                        DC_VariableArray(int nAll, const DC_Variable& initVal):T_SC_Array<DC_Variable>(nAll, initVal) {}
                        DC_VariableArray(DC_Variable* buffer, int bufSize):T_SC_Array<DC_Variable>(buffer, bufSize) {}


            //  type specific methods
        bool            GetValue(const char* varID, double& varVal) const;

        bool            AllOK() const;
        void            SortByID();
        void            SortByValue();
        int             GetVarIndex(const char* varID) const;

        void            GetIDs(SC_StringArray& ids) const;

};



#endif // DC_VARIABLEARRAY_H

