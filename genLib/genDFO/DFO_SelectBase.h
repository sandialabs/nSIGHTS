///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectBase.h
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
// DESCRIPTION: Basic functional objects (all derived from FuncObjC)
//              used by all applications, primarily concerned with
//              defining/operating on pen sets and colour maps:
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_SELECTBASE_H
#define DFO_SELECTBASE_H

#include <genClass/C_FuncObj.h>
#include <genDataClass/DSC_IndexMSSpec.h>


class DFO_SelectBase : public FuncObjC {

protected:
    friend class DPO_SelectBase;
    friend class DPO_SelectBasePS;

    DSC_IndexMSSpec&    indexMS;

public:
    DFO_SelectBase(const char* selID, DSC_IndexMSSpec& inMS);
    DFO_SelectBase(const DFO_SelectBase& a, DSC_IndexMSSpec& inMS);
    ~DFO_SelectBase(){};

    DFO_SelectBase&     operator= (const DFO_SelectBase& a);

    void                DoStatusChk();
    bool                IndexRangeOK(int maxIndex);


protected:
    void                InitMSPorts();  // delayed until after derived class
    // indexMS is constructed
private:
    void                LocalCopy(const DFO_SelectBase& a);  // common to copy constructor and = operator
    bool                CheckIndex(int indx, int maxIndex);

};

#endif // DFO_SELECTBASE_H

