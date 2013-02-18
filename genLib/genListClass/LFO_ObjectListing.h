///////////////////////////////////////////////////////////////////////////////////
//
// LFO_ObjectListing.h
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
//      creates listing of defined FuncObjC connections.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef LFO_OBJECTLISTING_H
#define LFO_OBJECTLISTING_H

#include <genListClass/C_ListObj.h>

// base class for object for listing object connections
// must be used in modal class

class LFO_ObjectListing : public ListObjC {
    protected:
        enum            {objStrLen = 120};
        FuncObjArray    pageObjects;    // user defined objects on pages is definition order
        SC_IntArray     objNameIndexes;  // if 0 or +ve indexes to entry in pageObjeNames, else systemObjectNames
        SC_StringArray  pageObjectNames;
        SC_StringArray  systemObjectNames;
        int             nobj;

    private:
        int             nextPageFO;
        const char*     rootID;
        bool            oneRoot;

    public:
                LFO_ObjectListing(const char* inID);
                ~LFO_ObjectListing() {}


    protected:
        void            PrepareVars();
        bool            ListConnections(FuncObjC* objToList, bool listNone, const char* listTitle);
        const char*     GetObjString(int foIndex);
        void            SetBaseFont();
        void            AddPageToList(const MenuPageC& currPage);


};


class LFO_ListOneObject : public LFO_ObjectListing {
    private:
        FuncObjC*       objToList;

    public:
                        LFO_ListOneObject(FuncObjC*   inObj);
                        ~LFO_ListOneObject() {}

        virtual void    CreateListing();

};

class LFO_ListAllObjects : public LFO_ObjectListing {
    public:
                        LFO_ListAllObjects();
                        ~LFO_ListAllObjects() {}

        virtual void    CreateListing();

};



#endif // !LFO_OBJECTLISTING_H

