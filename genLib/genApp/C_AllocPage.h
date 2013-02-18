///////////////////////////////////////////////////////////////////////////////////
//
// C_AllocPage.h
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
//              object factory for creating new menu pages.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_ALLOCPAGE_H
#define C_ALLOCPAGE_H

#include <genApp/C_MenuPage.h>

#ifndef ADCONSOLEAPP
#include <genApp/PullMenu.h>
#endif

#include <genClass/SC_StringArray.h>
#include <genClass/T_SC_BasicPtrArray.h>

class AllocPageC {

public:
    //  page class specific stuff added here
#ifndef ADCONSOLEAPP
    PullMenuC                pageObjectMenu;
#endif
    SC_ConstStringArray      pageObjectIDs;   // all objects supported for page type

protected:

    static T_SC_BasicPtrArray<AllocPageC> allocPages;

public:
    AllocPageC();
    ~AllocPageC() {};  // never destroyed

#ifndef ADCONSOLEAPP
    // for page setup/access
    void                    SetMenuData(const ObjectCascadeArray& inObjects);

    bool                    IsSameType(const char* inType)          const;
    bool                    IsSameType(const AllocPageC& inAlloc)   const;
    bool                    ObjectOKForPage(const char*  objID)     const;

    // main virtuals
    virtual bool            AcceptAsChild(const char*  pageToCheck) const;
#endif // ADCONSOLEAPP
    virtual const char*     GetPageTypeName() const = 0;

    // statics for factory access
    virtual MenuPageC*      CreatePage() const = 0;
    static MenuPageC*       CreateMenuPage(const char*  pageToCreate);

    static  bool            MenuPageExists(const char*  pageToCheck);       // checks if page found
    static  AllocPageC*     GetPageAllocPtr(const char*  typeToFind);

#ifndef ADCONSOLEAPP
    // for copy/paste support to check pages before creation
    static  bool            IsPlotPage(const char*  pageToCheck);

    static  bool            AcceptAsChild(const char*  parentPageType,
                                          const char*  childPageType);


    static  bool            ObjectOKForPage(const char*  pageTypeDesc,
                                            const char*  objID);

    static  bool            AreSameTypes(const char* pageTypeA,
                                         const char* pageTypeB);

    static  bool            AreSameTypes(const char*  pageIdentifier,
                                         const MenuPageC* pageData);


    static  const PullMenuC*   GetObjectMenuForPage(const char*  pageTypeDesc);

#endif // ADCONSOLEAPP

protected:
    bool                    PageNameMatch(const char*  nameToMatch) const;
    //  implemented in allocator -- checks objToCreate against name
#ifndef ADCONSOLEAPP
    virtual bool            IsPlotPage() const;
#endif
};


#ifndef ADCONSOLEAPP
class AllocPlotPage : public AllocPageC {

private:
    virtual bool            IsPlotPage() const {return true;}
    virtual bool            AcceptAsChild(const char*  pageToCheck) const;
};

#endif // ADCONSOLEAPP


#endif // C_ALLOCPAGE_H
