///////////////////////////////////////////////////////////////////////////////////
//
// C_ListObj.h
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
//      base class for objects that list data to ListDefC functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_LISTOBJ_H
#define C_LISTOBJ_H

#include <genClass/C_FuncObj.h>

#include <genClass/SC_StringArray.h>
#include <genClass/SC_IntArray.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/SC_RealFormat.h>

#include <genListClass/C_ListDef.h>
#include <genListClass/LSC_Listing.h>

//  This is the base class for all listing  objects

class ListObjC : public FuncObjC, public LSC_Listing  {

    friend class ListDefC;

    public:
        ListDefC*       assocList;          // associated list definition

    public:
        //  support for 2 uses:
        //      1) as objects created on listing page for dynamic non-modal listing
        //      2) as objects (probably statically allocated) for modal app listings

                        ListObjC(const char* listType, ListDefC& assList);  // non-modal
                        ListObjC(const char* listType);  // modal
                        ListObjC(const ListObjC& a, ListDefC& assList);  // non-modal

        virtual         ~ListObjC();

        ListObjC&       operator= (const ListObjC& a);


                        //  FuncObjC virtuals
        virtual void    DoStatusChk();
        virtual void    CalcOutput(FOcalcType  calcType);

                        //  main virtual
        virtual void    CreateListing() {};   // adds listing data

        virtual void    BuildListing();       // this is overriden by special cases
                                              // that dont use CalcOutput to create the listing
                                              // default use CalcOutput through Execute
    protected:
        void            DoListing();

    private:
        void            LocalCopy(const ListObjC& a);  // common to copy constructor and = operator
};

#endif // !C_LISTOBJ_H

