///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CreateXYArray.h
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
//      Combines XY data into DC_XYDataArray
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_CREATEXYARRAY_H
#define DFO_CREATEXYARRAY_H

#include <objClass/DO_XYDataArray.h>

#include <genClass/C_FuncObj.h>

class DFO_CreateXYArray : public FuncObjC {
    public:

        class XYDataDesc  {
            public:
                bool                doXY;
                FuncObjRef          xyObjRef;
                char                newID[DC_XYData::dataIDLen];
                const DC_XYData*    xyDataDC;

                            XYDataDesc();
                            XYDataDesc(const XYDataDesc& a);
                XYDataDesc&  operator= (const XYDataDesc& a);

            private:
                void         LocalCopy(const XYDataDesc& a);  // common to copy constructor and = operator

        };

        enum        {maxXYDataDesc = 24};


        XYDataDesc          xyData[maxXYDataDesc];
    protected:
        DC_XYDataArray      outputDC;
        DO_XYDataArray      outputDO;

    private:
        int                 nSelected;

    public:
                        DFO_CreateXYArray();
                        DFO_CreateXYArray(const DFO_CreateXYArray& a);
                        ~DFO_CreateXYArray();

        DFO_CreateXYArray& operator= (const DFO_CreateXYArray& a);

        void            SetDefault();

                        //  FuncObjC virtuals
        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

    private:
        void            InitCommon();                             // common to normal and copy constructor
        void            LocalCopy(const DFO_CreateXYArray& a);  // common to copy constructor and = operator
};

#endif // DFO_CREATEXYARRAY_H

