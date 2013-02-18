///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYConcat.h
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

#ifndef DFO_XYConcat_H
#define DFO_XYConcat_H

#include <genClass/T_SC_BasicPtrArray.h>
#include <genClass/DO_XYData.h>

#include <genClass/C_FuncObj.h>

class DFO_XYConcat : public FuncObjC {
    public:

        class XYInputData  {
            public:
                bool        doXY;
                FuncObjRef  xyObjRef;

                            XYInputData();
                            XYInputData(const XYInputData& a);
                XYInputData&  operator= (const XYInputData& a);

            private:
                void         LocalCopy(const XYInputData& a);  // common to copy constructor and = operator

        };

        enum                {maxInputXY = 24};


        XYInputData         xyToConcat[maxInputXY];

        bool                deleteOverlaps;

    private:
        DC_XYData           outputXYDC;
        DO_XYData           outputXYDO;

    protected:
        T_SC_BasicPtrArray<const DC_XYData>  inputXYDC;
        int                 nXY;

    public:
                        DFO_XYConcat();
                        DFO_XYConcat(const DFO_XYConcat& a);
                        ~DFO_XYConcat();

        DFO_XYConcat&   operator= (const DFO_XYConcat& a);

                        //  FuncObjC virtuals
        void            DoStatusChk();

                        // output table must be allocd and initialized to 0.0 before call
        void            CalcOutput(FOcalcType  calcType);

    private:
        void            InitCommon();                             // common to normal and copy constructor
        void            LocalCopy(const DFO_XYConcat& a);  // common to copy constructor and = operator
};

#endif //DFO_XYConcat_H

