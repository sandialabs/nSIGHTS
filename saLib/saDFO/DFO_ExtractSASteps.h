///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ExtractSASteps.h
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

#ifndef DFO_EXTRACTSASTEPS_H
#define DFO_EXTRACTSASTEPS_H

#include <genClass/C_FuncObj.h>

#include <genClass/DO_Label.h>
#include <saClass/DO_SATable.h>

class DFO_ExtractSASteps : public FuncObjC {
    protected:
        FuncObjRef      inputTableObjRef;
        bool            extractByIndexes;
        int             minIndex;               // input is FORTAN index #s
        int             maxIndex;
        int             indexIncrement;
        double          minValue;
        double          maxValue;

        // visible for status
        DC_SATable      outputTableDC;
        const DC_SATable*   inputTableDC;

    private:
        DO_SATable      outputTableDO;
    public:
                            DFO_ExtractSASteps();
                            DFO_ExtractSASteps(const DFO_ExtractSASteps& a);
                            ~DFO_ExtractSASteps(){};

        DFO_ExtractSASteps&         operator= (const DFO_ExtractSASteps& a);

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);

    private:
        void                InitCommon();                        // common to normal and copy constructor
        void                LocalCopy(const DFO_ExtractSASteps& a);  // common to copy constructor and = operator

};

#endif // !DFO_EXTRACTSASTEPS_H

