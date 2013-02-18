///////////////////////////////////////////////////////////////////////////////////
//
// C_ExposedObj.h
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
//              base class for exposing selected properties in a modeless dialogs.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_EXPOSEDOBJ_H
#define C_EXPOSEDOBJ_H

#include <genClass/T_SC_BasicPtrArray.h>

#ifndef ADCONSOLEAPP
#include <genClassPS/PS_DialogShell.h>
#include <genApp/C_AppMenu.h>
#endif

#include <genApp/C_CfgFileObj.h>


class FuncObjC;

#ifndef ADCONSOLEAPP
using namespace genClassPS;

class ExposedObjC : public DialogShell, public CfgFileObj {
    friend class AppMenuC;
protected:
    DialogShellFormManager      exposedObjectDialogForm;
    bool                animating;
    int                 animationIndex;
    PushButton          animateButton;
#else
    class ExposedObjC : public CfgFileObj {
    protected:
#endif  // ADCONSOLEAPP
        FuncObjC&           exposedFO;
        const char*         mainTitle;

    protected:
        static T_SC_BasicPtrArray<ExposedObjC> exposedObjects;

        bool                m_bProcessing;

        static int          exposedReadVersion;

    public:
        ExposedObjC(FuncObjC& FOref,
                    const char* mainT);
        virtual             ~ExposedObjC();

#ifndef ADCONSOLEAPP

        bool                IsExposed();
        void                SetExposed();
        void                UnExpose();

        bool                IsAnimating() {return animating;}

        virtual     void    Execute();  // executes FO if managed

        virtual void        Update();  // base class updates title if managed

        static void         CloseIfCreated(ExposedObjC* inPtr);
        static void         UpdateIfCreated(ExposedObjC* inPtr);
        static bool         IsCreatedAndExposed(ExposedObjC* inPtr);
        static void         UpdateAllExposed();
#endif // ADCONSOLEAPP
        //  file read/write support
        static bool         DoExposedObjWrite(ExposedObjC* inPtr);
        static bool         DoExposedObjRead();

        void                            GetTitle(char strTitle[80]);
    protected:

        void                WriteObjectHeader(int objWriteVer);
        //  reads version numbers
        void                ReadObjectHeader();

#ifndef ADCONSOLEAPP
        //  animation support
        virtual void        NextAnimationStep() = 0;
        void                StdAnimateCallback(int stIndx = 0);
        void                SetAnimateText();
        virtual void        SetAllSensitive() {};

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(ExposedObjC)
    public:
        //}}AFX_VIRTUAL

    protected:
        //{{AFX_MSG(ExposedObjC)
        afx_msg void OnSize(UINT nType, int cx, int cy);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
#endif // ADCONSOLEAPP
    };


    class ExposedWrapper {
    public:
        ExposedObjC*    exposedObj;

    public:
        ExposedWrapper() : exposedObj(0) {};
        virtual            ~ExposedWrapper();

        //      file io
        void               ReadExposed();
        void               WriteExposed();

        virtual void       CreateObject() = 0;

#ifndef ADCONSOLEAPP
        bool               IsExposed();
        void               UpdateIfCreated();

        //      menu support
        void               ToggleExposed(bool close);
        void               CloseIfCreated();
        void               CreateAndUpdate();
#endif // ADCONSOLEAPP

    };

#endif // C_EXPOSEDOBJ_H

