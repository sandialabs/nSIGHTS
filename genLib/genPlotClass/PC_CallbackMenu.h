///////////////////////////////////////////////////////////////////////////////////
//
// PC_CallbackMenu.h
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
//      for defining/processing right click menus in ActiveObjC derived objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PC_CALLBACKMENU_H
#define PC_CALLBACKMENU_H

#include <genClass/T_SC_Array.h>

class InteractiveObj;


namespace genPlotCallback {


class CallbackMenuItem {
    public:
        enum            MenuItemType    {mitSeparator, mitEntry};

        MenuItemType    menuItemType;
        const char*     descStr;
        int             cbReason;
        bool            isActive;

    public:
                    CallbackMenuItem() :
                                descStr(0), menuItemType(mitSeparator), cbReason(-1), isActive(true)
                                {}
                    CallbackMenuItem(const char* desc, int reason) :
                                descStr(desc), menuItemType(mitEntry), cbReason(reason), isActive(true)
                                {}
                    CallbackMenuItem(const CallbackMenuItem& a) :
                                descStr(a.descStr), menuItemType(a.menuItemType), cbReason(a.cbReason), isActive(a.isActive)
                                {}
};


class CallbackItemArray : public T_SC_Array<CallbackMenuItem> {
    public:
                        CallbackItemArray();

            void        AddSeparator();
            void        AddEntry(const char* desc, int reason);

};


class PC_CallbackMenu {
    public:
       CallbackItemArray    menuDesc;
       const char*          menuTitle;
    private:
        InteractiveObj&     baseObj;

    public:
                            PC_CallbackMenu(InteractiveObj& assObj, const char* title);
        void                DoCallback(int index);

};


};

using namespace genPlotCallback;


#endif // !PC_CALLBACKMENU_H

