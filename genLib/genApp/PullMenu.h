///////////////////////////////////////////////////////////////////////////////////
//
// PullMenu.h
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PULLMENU_H
#define PULLMENU_H

#include <genClass/T_SC_Array.h>
#include <genClass/SC_StringArray.h>


class ObjectItem {
public:
    const char* objID;
    const char*     objDesc;
public:
    ObjectItem() {objID = 0; objDesc = 0;}
    ObjectItem (const char* inID, const char* inDesc)
    {objID = inID; objDesc = inDesc;}
};

class ObjectItemArray : public T_SC_Array<ObjectItem> {
public:
    ObjectItemArray() : T_SC_Array<ObjectItem>() {resizable = true;}
};

class ObjectCascade {
public:
    const char*     cascadeID;
    ObjectItemArray cascadeItems;

public:
    ObjectCascade() {cascadeID = 0;}
};

class ObjectCascadeArray : public T_SC_Array<ObjectCascade> {
public:
    ObjectCascadeArray() : T_SC_Array<ObjectCascade> () {resizable = true;}
};


class PullMenuC : public CMenu {

public:
    static SC_ConstStringArray idMap;
    static int initialIDValue;

public:
    PullMenuC();
    ~PullMenuC();
public:

    void    Add(PullMenuC& child, const char* id);
    void    Add(const char* objDesc, const char* objID);

    void    Add(const ObjectCascadeArray& cascade);
    void    Add(const ObjectItemArray& items);

    const char*   GetCBName(UINT nItem, char* pName, UINT cch);

    static  void InitializeMapping(int startIDValue);

};

#endif // PULLMENU_H

