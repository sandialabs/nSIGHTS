///////////////////////////////////////////////////////////////////////////////////
//
// PS_MainWindow.cpp
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

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include <genApp/GenAppMessages.h>

#include <genClassPS/PS_MainWindow.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace genClassPS {


#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before alloc objects in app

    T_SC_BasicPtrArray<PS_MainWindow> PS_MainWindow::mainWindows(64);


    PS_MainWindow::PS_MainWindow(CWnd& inWnd, const char* wType)
        : currWnd(inWnd), windowType(wType), isVisible(true)
    {
        mainWindows.AddTo(this);
    }

    PS_MainWindow::~PS_MainWindow()
    {
        mainWindows.DeleteFrom(this);
    }

    bool PS_MainWindow::IsVisible()
    {
        return isVisible && (currWnd.m_hWnd != NULL);
    }

    void PS_MainWindow::MainWindowActivated()
    {
        isVisible = true;
        AfxGetMainWnd()->SendMessage(WM_GENAPP_FRAMEACTIVATE, 1, LPARAM(this));
    }

    void PS_MainWindow::MainWindowDeactivated()
    {
        isVisible = false;
        AfxGetMainWnd()->SendMessage(WM_GENAPP_FRAMEACTIVATE, 0);
    }

    void PS_MainWindow::MainWindowRenamed()
    {
        AfxGetMainWnd()->SendMessage(WM_GENAPP_FRAMERENAME, 1, LPARAM(this));
    }

    void PS_MainWindow::MainWindowToTop()
    {
        if (!currWnd.IsWindowVisible())
            {
                currWnd.ShowWindow(SW_SHOW);
            }

        currWnd.BringWindowToTop();

        if (currWnd.IsIconic())
            {
                currWnd.ShowWindow(SW_RESTORE);
            }
    }

    void PS_MainWindow::GetMainWindowTitle(char* windowTitle, bool addType, int titleLen)
    {
        currWnd.GetWindowText(windowTitle, titleLen);
        if (addType && (GetWindowType()[0] != ':'))
            {
                ConcatString(windowTitle, "::", titleLen);
                ConcatString(windowTitle, GetWindowType(), titleLen);
            }
    }

    void PS_MainWindow::SetMainWindowTitle(const char* windowTitle)
    {
        currWnd.SetWindowText(windowTitle);
        MainWindowRenamed();
    }

    void PS_MainWindow::ShowMainWindow(int nCmdShow)
    {
        currWnd.ShowWindow(nCmdShow);
    }

    void PS_MainWindow::ResizeMainWindow(int x, int y, int nWidth, int nHeight)
    {
        currWnd.LockWindowUpdate();

        if (!currWnd.IsWindowVisible())
            {
                currWnd.ShowWindow(SW_SHOW);
            }
        else if (currWnd.IsIconic())
            {
                currWnd.ShowWindow(SW_RESTORE);
            }

        currWnd.MoveWindow(x, y, nWidth, nHeight);
        currWnd.BringWindowToTop();

        currWnd.UnlockWindowUpdate();
    }


    bool PS_MainWindow::AllSameType()
    {
        if (mainWindows.IsEmpty())
            return true;

        const char* firstType = 0;

        for (int i = 0; i < mainWindows.Size(); i++)
            {
                PS_MainWindow& currMain = *(mainWindows[i]);
                if (!currMain.IsVisible())
                    continue;

                const char* currType = currMain.GetWindowType();
                if (firstType == 0)
                    firstType = currType;

                if (strcmp(firstType, currType) != 0)
                    return false;
            }

        return true;
    }


}

