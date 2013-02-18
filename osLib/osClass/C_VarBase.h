///////////////////////////////////////////////////////////////////////////////////
//
// C_VarBase.h
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
//      base class for different variables types.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_VARBASE_H
#define C_VARBASE_H

#include <genClass/T_SC_Array.h>

//  base class for all sample/opt/range vars
class   VarBase  {

    friend class VarBaseArray;

    public:
        VarBase*                nextVB;             //  all variables are in double linked list
        VarBase*                prevVB;

        bool&                   isActive;           // reference to var in class where VB is used
                                                    // only isActive vars will be processed
        bool                    wasActive;          // for restoring previously active after settings changes
        const char*             varShortID;
        const char*             varLongID;

    public:
                                VarBase(bool& isActiveInit, const char* varIDInit, VarBase*& listHead);
                                VarBase(bool& isActiveInit, const char* varShortIDInit, const char* varLongIDInit, VarBase*& listHead);
                                ~VarBase() {};

        const char*             GetShortID()    const {return varShortID;}
        const char*             GetLongID()     const {return varLongID;}
        bool                    IsActive()      const {return isActive;}
        bool                    WasActive()     const {return wasActive;}

        //  virtuals for access
        virtual double          GetVarValue()    const = 0;
        virtual double          GetMinVarValue() const = 0;
        virtual double          GetMaxVarValue() const = 0;

        // unit string
        virtual void            MakeUnitString(char unitStr[], int maxLen); // default is []


    protected:

        VarBase*                GetNextActive() const;                  // returns next active from this
        static int              GetnActive(VarBase* listHead);          // returns number of active vars
        static VarBase*         GetFirstActive(VarBase* listHead);      // returns first active
        static void             SetWasActive(VarBase* listHead);        // updates wasActive from isActive

        void                    RemoveVarBase(VarBase*& listHead);       // replacement for destructor

    private:
                                // no copying or assigning
                                VarBase(const VarBase& a);
        VarBase&                operator= (const VarBase& a) {return *this;}
        void                    InitCommon(VarBase*& listHead);

};


class VarBaseArray : public T_SC_Array<VarBase*>  {

    public:
        void        GetActiveArray(VarBase* listHead);
        void        GetSortedArray(VarBase* listHead);   // returns sorted by longID

};


#endif // !C_VARBASE_H

