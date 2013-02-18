///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYScaleTransform.h
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
//      provides extended scale/transform operations for either X or Y components.
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_XYSCALETRANSFORM_H
#define DFO_XYSCALETRANSFORM_H

#include <genClass/C_FuncObj.h>

#include <genDataClass/DSC_Threshold.h>
#include <genClass/DO_XYData.h>

#include <genDFO/DFO_ScaleTransform.h>

#include <objDataClass/DSC_XYColSel.h>

class DFO_XYScaleTransform : public DFO_ScaleTransform, public DSC_XYColSel {
    public:
        enum                        {descLen = 40};
        char                        outputTypeDesc[descLen];

    protected:
        DSC_Threshold               thresholdOp;

    private:
        DC_XYData                   outputData;
        DO_XYData                   outputDataDO;

    public:
                                    DFO_XYScaleTransform();
                                    DFO_XYScaleTransform(const DFO_XYScaleTransform& a);
                                    ~DFO_XYScaleTransform();

        DFO_XYScaleTransform&   operator= (const DFO_XYScaleTransform& a);

        void                        DoStatusChk();
        void                        CalcOutput(FOcalcType  calcType);

    private:
        void                        InitCommon();                               // common to normal and copy constructor
        void                        LocalCopy(const DFO_XYScaleTransform& a);     // common to copy constructor and = operator

        void                        DoScaleTransform(SC_DoubleArray& stData);
};

#endif //DFO_XYSCALETRANSFORM_H
