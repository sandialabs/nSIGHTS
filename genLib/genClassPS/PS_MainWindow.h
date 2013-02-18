///////////////////////////////////////////////////////////////////////////////////
//
// PS_MainWindow.h
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
// DESCRIPTION: Platform specific but application independent code.
//              Keeps track of all top-level windows for use by window selector
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PS_MAINWINDOW_H
#define PS_MAINWINDOW_H

#include <genClass/T_SC_BasicPtrArray.h>

// interface for use by window selector & general control of the multitude of top-level windows

namespace genClassPS {


    class PS_MainWindow  {
    public:
        static T_SC_BasicPtrArray<PS_MainWindow> mainWindows;

    private:
        CWnd&           currWnd;
        const char*     windowType;
        bool            isVisible;      // default true on creation

    public:
        PS_MainWindow(CWnd& inWnd, const char* wType);
        virtual         ~PS_MainWindow();

        inline const char*  GetWindowType() {return windowType;}

        bool            IsVisible();

        void            MainWindowActivated();
        void            MainWindowDeactivated();
        void            MainWindowRenamed();

        void            MainWindowToTop();

        void            ShowMainWindow(int nCmdShow);
        void            ResizeMainWindow(int x, int y, int nWidth, int nHeight);

        void            GetMainWindowTitle(char* windowTitle, bool addType, int titleLen);
        void            SetMainWindowTitle(const char* windowTitle);

        static bool     AllSameType();

    protected:
        inline void     SetVisible(bool isVis) {isVisible = isVis;}

    };

};

using namespace genClassPS;

#endif // PS_MAINWINDOW_H

