///////////////////////////////////////////////////////////////////////////////////
//
// DC_CurveArray.h
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

#ifndef DC_CURVEARRAY_H
#define DC_CURVEARRAY_H

#include <genClass/T_SC_Array.h>
#include <genClass/SC_StringArray.h>
#include <objClass/DC_Curve.h>

// data class for functional approximation types

class DC_CurveWithID : public DC_Curve {

    public:
        enum                    {curveIDlen = 40};
        char                    curveID[curveIDlen];

    public:
                                DC_CurveWithID();
                                DC_CurveWithID(const DC_CurveWithID& a);
        virtual                 ~DC_CurveWithID();

        DC_CurveWithID&         operator= (const DC_CurveWithID& a);
        DC_CurveWithID&         operator= (const DC_Curve& a);

        void                    SetID(const char id[]);
        void                    GetID(char id[],
                                      int  maxIDlen);

    private:
        void                    LocalCopy(const DC_CurveWithID& a);  // implementation of copy

};

class DC_CurveArray: public T_SC_Array<DC_CurveWithID> {
    public :

                    DC_CurveArray() : T_SC_Array<DC_CurveWithID>() {};               //default cons
                    DC_CurveArray(int nAll) : T_SC_Array<DC_CurveWithID>(nAll) {};   //allocates to size nAll

        bool        GetIDIndex(const char* inID,
                               int&         idIndex);

        void        GetIDs(SC_StringArray& ids);
};


#endif //DC_CURVEARRAY_H
