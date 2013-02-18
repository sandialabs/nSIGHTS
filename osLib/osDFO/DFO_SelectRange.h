///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectRange.h
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
//      For calculating fits, and for reading, selecting, and operating on optimizer results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_SELECTRANGE_H
#define DFO_SELECTRANGE_H

#include <osDataClass/DSC_IndexMSSpecRange.h>
#include <osClass/DO_RangeSimResults.h>

#include <genDFO/DFO_SelectBase.h>


// single selection only

class DFO_SelectRange : public DFO_SelectBase {

        friend class DPO_SelectRange;
        friend class DPO_SelectRangePS;

    protected:
        DC_RangeSimResults* rangeSimResultsDC;
        FuncObjRef          rangeSimResultsObjRef;

        //  maps menu selection to run/output indexes for use by derived class
        //  done in StatusChk()
        int                 runIndex;
        int                 outputIndex;

    protected:
        DSC_IndexMSSpecRange   baseMS;


    public:
                            DFO_SelectRange(const char* inID);
                            DFO_SelectRange(const DFO_SelectRange& a);
                            ~DFO_SelectRange(){};

        DFO_SelectRange&       operator= (const DFO_SelectRange& a);

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);

    private:
        void                AddIndex(int menuIndex);
        void                InitCommon();                        // common to normal and copy constructor
        void                LocalCopy(const DFO_SelectRange& a);  // common to copy constructor and = operator

};

#endif // !DFO_SELECTRANGE_H

