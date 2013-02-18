///////////////////////////////////////////////////////////////////////////////////
//
// C_AllocObj.h
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
//              object factory for creating DPO_XXX, LPO_XXX, PPO_XXX, and
//              UPO_XXX functional objects given an object identifier.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_ALLOCOBJ_H
#define C_ALLOCOBJ_H

#include <genClass/T_SC_BasicPtrArray.h>

class MenuObjC;
class MenuPageC;

class AllocObjC {

protected:
    enum                {maxObjectNames = 5};
    const char*          objectNames[maxObjectNames];   // element 0 is current name
    //  1 through 4 are to deal with future name changes

    static T_SC_BasicPtrArray<AllocObjC> allocObjs;

public:
    AllocObjC();
    ~AllocObjC() {};  // never destroyed


    static MenuObjC*    CreateMenuObject(const char*      objToCreate,
                                         const MenuPageC* objPage);
    static MenuObjC*    CopyMenuObject(const MenuObjC&    objToCopy,
                                       const MenuPageC*   objPage);

    static  bool        MenuObjectExists(const char*      objToCheck);      // checks if object found


protected:
    //  implemented in allocator -- checks objToCreate against name
    //  return = 0 if objToMake is not the type of the allocator
    virtual MenuObjC*   CreateObject(const MenuPageC*   objPage) = 0;

    //  return = 0 if objToCopy is not the type of the allocator
    //  based on typeid comparison
    virtual MenuObjC*   CopyObject(const MenuObjC&    objToCopy,
                                   const MenuPageC*   objPage) = 0;

    bool                ObjectNameMatch(const char*  nameToMatch);

};


template <class T>

class T_AllocObj : public AllocObjC {

public:

    T_AllocObj(const char* objID)
    {
        objectNames[0] = objID;
    }

protected:

    virtual MenuObjC*   CreateObject(const MenuPageC*   objPage)
    {
        return (new T());
    }


    virtual MenuObjC*   CopyObject(const MenuObjC&    objToCopy,
                                   const MenuPageC*   objPage)
    {
        return (new T((T&) objToCopy));
    }
};


#endif // C_ALLOCOBJ_H

