///////////////////////////////////////////////////////////////////////////////////
//
// PSC_ColorBase.h
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
//      maps data to color ranges
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PSC_COLORBASE_H
#define PSC_COLORBASE_H

#include <genClass/C_Common.h>
#include <genClass/C_Graphic.h>
#include <genClass/C_FuncObj.h>

#include <genClass/DC_DataLimit.h>
#include <genClass/DC_ColorMap.h>

#include <genClass/DO_ColorLimit.h>


class PSC_ColorBase  {

    friend class PC_ShadeTriangle;

    public:
        //  input objects
        FuncObjRef      inputDataObjRef;      // input data to derived class
        FuncObjRef      colorMapObjRef;       // color map
        FuncObjRef      limitMasterObjRef;    // limits

        //  properties
        bool            limitIsMaster;      // gets range limit from input
        bool            autoLimits;         //  range based on data
        int             baseAutoDataSource; //  kluge - mapped to enum by derived class
        DC_DataLimit    rangeLimit;         //  also output

        bool            clipToRange;        //  clip/extend colors outside range

        enum ShadeClipSpec{scsNoClip, scsClipAbove, scsClipBelow};


    protected:
        //  input associated data
        const DC_ColorMap*    colorMap;           // color map
        const DC_DataLimit*   masterLimits;       // limits to use if limit is slave

        //  output object
        DO_ColorLimit   colorLimitDO;      // range limits & color map

        bool            oneColor;           // for ocassional cases when all colors in map are the same

    public:
                        PSC_ColorBase();
                        PSC_ColorBase(const PSC_ColorBase& a);
                        ~PSC_ColorBase();

    protected:
        void            InitCommon(FuncObjC& inFO);  // used by derived class constructors
        void            DoStatusChk(FuncObjC& inFO);  // used by derived class DoStatusChk()

        PSC_ColorBase&   operator= (const PSC_ColorBase& a);

        void            CheckRangeLimits(FuncObjC& inFO);

    private:
        void            LocalCopy(const PSC_ColorBase& a);  // common to copy constructor and = operator
};

#endif // !PSC_COLORBASE_H

