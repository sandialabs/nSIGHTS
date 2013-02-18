///////////////////////////////////////////////////////////////////////////////////
//
// SC_Array.h
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
// DESCRIPTION: base class for all array classes
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef SC_ARRAY_H
#define SC_ARRAY_H

class SC_Array {
protected:
    bool    resizable;
    bool    sorted;

public :
    int     nlistAlloc;
    int     nlistData;

public:
    // exception class
    class   AllocationError {
    public:
        const char* errMsg;
    public:
        AllocationError(const char* eMsg) : errMsg(eMsg) {}
    };

public :
    SC_Array(){nlistAlloc = 0; nlistData = 0;
        resizable = false; sorted = false;}
    virtual         ~SC_Array(){};

    inline int      Size()       const  {return nlistData;}
    inline int      UpperBound() const  {return nlistData - 1;}

    inline int      AllocSize() const   {return nlistAlloc;}
    inline bool     IsFull()    const   {return nlistAlloc == nlistData;}
    inline bool     IsEmpty()   const   {return nlistData == 0;}
    inline bool     IsNotEmpty()const   {return nlistData > 0;}
    void            SetSize(int newSize);
    inline void     SetEmpty()          {nlistData = 0;}
    inline void         SetSizeToAlloc()    {nlistData = nlistAlloc;}

    inline bool     IsSorted()  const   {return sorted;}
    inline void     SetSorted()         {sorted = true;}
    inline void     SetUnsorted()       {sorted = false;}

    inline bool     IsResizable() const {return resizable;}
    inline void     SetResizable()      {resizable = true;}
    void            SetResizable(int nAlloc);
    inline void     SetFixed()          {resizable = false;}

    void            Alloc(int nAll);                // throws alloc error if unable to alloc
    // calls ChkAlloc
    void            AllocAndSetSize(int nAll);      // allocs, sets sizethrows alloc error if unable to alloc

    virtual bool    ChkAlloc(int nAll) = 0;         // returns false if unable to alloc
    virtual void    DeAlloc() = 0;
    virtual void    ReAlloc(int nnewAll) = 0;       // increases size and copies data

protected:
    bool            BadIndex(int indxToCheck) const;
    void            SC_ArrayError(const char* errStr) const;  // internal errors for debug
};


#endif // SC_ARRAY_H

