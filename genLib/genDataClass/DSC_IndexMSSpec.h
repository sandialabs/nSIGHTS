///////////////////////////////////////////////////////////////////////////////////
//
// DSC_IndexMSSpec.h
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

#ifndef DSC_INDEXMSSPEC_H
#define DSC_INDEXMSSPEC_H

#include <genClass/C_FuncObj.h>
#include <genClass/DO_Indx.h>
#include <genClass/SC_IntArray.h>

class DSC_IndexMSSpec {
public:
    bool            indexIsMaster;
    FuncObjRef      indexMasterObjRef;

    int             enteredIndexValue;
    bool            enteredIndexIsSingle;
    SC_IntArray     enteredIndexArray;

    DO_Indx&        indexDO;

    const char*     indexID;
private:
    int             slaveIndexValue;
    bool            slaveIsSingle;
    SC_IntArray     slaveIndexArray;

public:
    DSC_IndexMSSpec(const char* ndxID, DO_Indx& inDO);
    ~DSC_IndexMSSpec();

    DSC_IndexMSSpec&   operator= (const DSC_IndexMSSpec& a);

    void                DoCheck(const char* specID,
                                FuncObjC&   specFO);

    void                AddPort(FuncObjC&   specFO);

    bool                CurrentIndexIsSingle()   const;
    int                 GetCurrentIndex()        const;
    const SC_IntArray&  GetCurrentIndexArray()   const;

    void                GetIndexArray(SC_IntArray& selIndexes,
                                      int          maxIndex);

    void                Clear();

    void                SetValid(int        maxIndex, // resets input to be within range
                                 FuncObjC&  specFO);  // if single and greater sets to 0
    // if multiple and none pass sets errmssg

    //  index status access
    inline bool         MultipleOK() const {return indexDO.multipleOK;}
    inline bool         SingleOK()   const {return indexDO.singleOK;}

    void                Reset(const char* typeLabel)        {indexDO.Reset(typeLabel);}
    void                SetIndexLabel()                         {indexDO.SetIndexLabel();}
    void                SetTypeLabel(const char* typeLabel) {indexDO.SetTypeLabel(typeLabel);}

private:
    void                LocalCopy(const DSC_IndexMSSpec& a);

};

#endif // DSC_INDEXMSSPEC_H

