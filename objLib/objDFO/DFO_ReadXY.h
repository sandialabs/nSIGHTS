///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadXY.h
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

#ifndef DFO_READXY_H
#define DFO_READXY_H

#include <genClass/C_FuncObj.h>

#include <genClass/DO_Label.h>
#include <genClass/DO_XYData.h>
#include <genClass/IO_XYData.h>
#include <genClass/C_MPISupport.h>


class DFO_ReadXY : public FuncObjC, public DSC_XYInputSpec {
    private:
        // output
        DO_Label            fileNameDO;
        DO_XYData           xyDataDO;

    protected:
        DC_XYData           xyData;

        //  parameters
        char                        xyFname[stdFileStrLen];

        char                        dataID[DC_XYData::dataIDLen];  // user-specd data ID
        NodeFileArray               mpiFiles;

    public:
                            DFO_ReadXY();
                            DFO_ReadXY(const DFO_ReadXY& a);
                            ~DFO_ReadXY(){};

        DFO_ReadXY&         operator= (const DFO_ReadXY& a);

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);

    private:
        void                InitCommon();                        // common to normal and copy constructor
        void                LocalCopy(const DFO_ReadXY& a);  // common to copy constructor and = operator

};

#endif //DFO_READXY_H

