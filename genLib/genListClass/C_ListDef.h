///////////////////////////////////////////////////////////////////////////////////
//
// C_ListDef.h
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
//      basic definition of a listing window.   Maintains lists of connected listing functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_LISTDEF_H
#define C_LISTDEF_H

#include <genClass/C_Common.h>

#include <genClass/C_FuncObj.h>
#include <genClass/T_SC_BasicPtrArray.h>
#include <genClass/SC_StringArray.h>



class ListObjC;                     // forward decl

typedef T_SC_BasicPtrArray<ListObjC> ListObjArray;


//  This is the base class for all listing windows
class ListDefC : public FuncObjC  {
    public:
        //  tracking lists for all list windows - probably not needed -- may kill later

        static T_SC_BasicPtrArray<ListDefC> listDefList;

        ListObjArray        connectedListObj;       //  pointer to first list object in window

        static  ListObjArray    defaultListObj;     // objects listed at the start of all listings

    protected:
        bool                    displayDefaults;

    public:
                            ListDefC(const char* dType);
                            ListDefC(const ListDefC& a);
                            ~ListDefC();

        ListDefC&           operator= (const ListDefC& a);

        //  FuncObjC virtuals
        virtual void        DoStatusChk();
        virtual bool        UsesAsInput(const FuncObjC* inputObj) const;
        virtual void        CalcOutput(FOcalcType  calcType);

        //  management of connected objects
        int                 GetNListObject() {return connectedListObj.Size();}   // returns number of list objects attached
        void                AddTo(ListObjC* newObj) {connectedListObj.AddTo(newObj);}
        void                DeleteFrom(ListObjC* delObj) {connectedListObj.DeleteFrom(delObj);}

        void                SuppressDefaults() {displayDefaults = false;}

    protected:

        //  implemented in derived class -- does actual display tasks
        virtual void        ClearDisplay() = 0;
        virtual void        SendToDisplay(const SC_StringArray& dispText) = 0;
        virtual void        ChangeTitle(const char* newName) = 0;

        void                ListToFile(const char* fName);

    private:
        void                InitCommon();  // common to normal and copy constructor

        void                LocalCopy(const ListDefC& a);  // common to copy constructor and = operator

};


#endif // !C_LISTDEF_H

