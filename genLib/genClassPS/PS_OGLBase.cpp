///////////////////////////////////////////////////////////////////////////////////
//
// PS_OGLBase.cpp
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
//              Base class for OpenGL visual. Enumerates available visual and
//              selects. Provides information about current visual properties.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <iostream>

#include <genClass/C_Common.h>
#include <genClass/SC_IntArray.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genClassPS/PS_OGLBase.h>
#include <genUIExt/ErrorMessage.h>

#include <genApp/ProjectUtil.h>         // just for platform stuff


int                            PS_OGLBase::oglMajorVersion = 0;
int                            PS_OGLBase::oglMinorVersion = 0;


T_SC_BasicPtrArray<PS_OGLBase> PS_OGLBase::oglBaseArray(16);

PIXELFORMATDESCRIPTOR          PS_OGLBase::oglPFD;
int                            PS_OGLBase::oglPixelFormat = 0;

int                            PS_OGLBase::oglRedDepth    = 0;
int                            PS_OGLBase::oglGreenDepth  = 0;
int                            PS_OGLBase::oglBlueDepth   = 0;
int                            PS_OGLBase::oglZDepth      = 0;

bool                           PS_OGLBase::oglIsDoubleBuffered     = false;
bool                           PS_OGLBase::oglOverlayIsDoubleBuffered  = false;
bool                           PS_OGLBase::oglHasOverlay   = false;
bool                           PS_OGLBase::oglIsStereo     = false;

OpenGLPfd                      PS_OGLBase::currentPfd;
bool                           PS_OGLBase::setToDefaultPfd   = true;
int                            PS_OGLBase::specifiedPfdIndex = 0;
PS_OGLBase::BitmapFontPlatform PS_OGLBase::bitmapFontPlatform = bmf_Automatic;
bool                           PS_OGLBase::antiAlias2DLines = false;
bool                           PS_OGLBase::antiAlias3DLines = false;

PS_OGLBase*                    PS_OGLBase::lastContext    = 0;
HGLRC                          PS_OGLBase::baseContext    = 0;


OpenGLPfd::OpenGLPfd()
{
    pfdIndex         = 0;
    redDepth         = 0;
    greenDepth       = 0;
    blueDepth        = 0;
    colorDepth       = 0;
    zDepth           = 0;
    isDoubleBuffered = false;
    isStereo         = false;

    alphaDepth       = 0;
    hasOverlay       = false;
}
void  OpenGLPfd::SetFromPfd(int                      indx,
                            PIXELFORMATDESCRIPTOR&   inPFD)
{
    pfdIndex         = indx;
    redDepth         = inPFD.cRedBits;
    greenDepth       = inPFD.cGreenBits;
    blueDepth        = inPFD.cBlueBits;
    colorDepth       = inPFD.cColorBits;
    zDepth           = inPFD.cDepthBits;
    isDoubleBuffered = inPFD.dwFlags & PFD_DOUBLEBUFFER;
    isStereo         = inPFD.dwFlags & PFD_STEREO;

    alphaDepth       = inPFD.cAlphaBits;
    hasOverlay       = inPFD.bReserved > 0;
}

static int nOGLNumber;


PS_OGLBase::PS_OGLBase()
{
    numberDebug         = nOGLNumber++;
    oglDisplay          = 0;
    oglContext          = 0;
    oglOverlayContext   = 0;

    oglBaseArray.AddTo(this);

}

PS_OGLBase::~PS_OGLBase()
{
    if (oglContext == baseContext)
        {
            // update base context
            baseContext = 0;
            for (int i = 0; i < oglBaseArray.Size(); i++)
                if ((oglBaseArray[i] != this) && (oglBaseArray[i]->oglContext != NULL))
                    {
                        baseContext = oglBaseArray[i]->oglContext;
                        break;
                    }
        }


    if ((oglDisplay != 0) && (oglContext !=0 ))
        {
            ::wglMakeCurrent(NULL, NULL);
            ::wglDeleteContext(oglContext);
            delete oglDisplay;
        }

    oglBaseArray.DeleteFrom(this);
    if (oglBaseArray.IsEmpty())
        oglPixelFormat = 0; // because of IsInitialized
}

HGLRC PS_OGLBase::GetBaseContext()
{
    if (baseContext != NULL)
        return baseContext;

    for (int i = 0; i < oglBaseArray.Size(); i++)
        if (oglBaseArray[i]->oglContext != NULL)
            {
                baseContext = oglBaseArray[i]->oglContext;
                return baseContext;
            }

    return 0;
}

CClientDC* PS_OGLBase::GetBaseDisplay()
{
    if (oglBaseArray.IsEmpty())
        return 0;
    return oglBaseArray[0]->oglDisplay;
}


bool PS_OGLBase::IsCurrent()
{
    return (lastContext == this);
}


bool PS_OGLBase::MakeCurrent()
{
    bool isOK = ::wglMakeCurrent(oglDisplay->GetSafeHdc(), oglContext) != FALSE;
    if (!isOK) {
        CErrorMessage errorMessage;
        CString sNumber;
        sNumber.Format("Context: %d", numberDebug);
        errorMessage.MessageBox("::wglMakeCurrent", sNumber, MB_ICONEXCLAMATION);
        // 2000 ERROR_INVALID_PIXEL_FORMAT
    }

    //  bool isOK = wglMakeCurrent(oglDisplay->GetSafeHdc(), oglContext) != 0;
    if (isOK && oglMajorVersion == 0)
        {
            char* venStr = (char*) glGetString(GL_VENDOR);
            char* renStr = (char*) glGetString(GL_RENDERER);
            char* verStr = (char*) glGetString(GL_VERSION);
            if (verStr == 0 )
                {
                    //            cout << "GL version  : unknown (assumed 1.0)" << endl;
                    oglMajorVersion = 1;
                    oglMinorVersion = 0;
                }
            else
                {
                    oglMajorVersion = int(verStr[0]) - int('0');
                    oglMinorVersion = int(verStr[2]) - int('0');
                    //            cout << "GL version  : " << verStr << "(" << oglMajorVersion << "." << oglMinorVersion << ")" << endl;
                }

            //  these should match currentPfd
            GLint params[4];
            glGetIntegerv(GL_RED_BITS, params);
            oglRedDepth = params[0];
            glGetIntegerv(GL_BLUE_BITS, params);
            oglBlueDepth  = params[0];
            glGetIntegerv(GL_GREEN_BITS, params);
            oglGreenDepth  = params[0];
            glGetIntegerv(GL_DEPTH_BITS, params);
            oglZDepth = params[0];

            GLboolean  glstereo;
            glGetBooleanv(GL_STEREO, &glstereo);
            oglIsStereo = glstereo;

            //        cout << "GL RGB/depth: " << oglRedDepth << " " << oglGreenDepth << " " << oglBlueDepth << " / " << oglZDepth << endl;
        }
    lastContext = this;
    return isOK;
}

void PS_OGLBase::Flush()
{
    if (GLisDoubleBuffered()) {
        // bug on Radim's machine --
        //          VERIFY(wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE));
        //  Win32 OpenGL SDK
        bool isOK = ::SwapBuffers(oglDisplay->GetSafeHdc()) != FALSE;
        if (!isOK) {
            CErrorMessage errorMessage;
            if (errorMessage.GetMessageCode() != 0)
                errorMessage.MessageBox("::SwapBuffers", NULL, MB_ICONEXCLAMATION);
            else
                GenAppWarningMsg("GenLib", "PS_OGLBase:Flush() - swap error with 0 code");
            // 6 ERROR_INVALID_HANDLE
        }
    }

}

void PS_OGLBase::InitStatics(HDC initHDC)
{
    if (setToDefaultPfd)
        {
            // use std selection
            PIXELFORMATDESCRIPTOR selPFD = {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                PFD_TYPE_RGBA,
                24,                    // 24 bits color
                0, 0, 0, 0, 0, 0,      // specific color bits ignored
                0,                     // no alpha buffer
                0,                     // shift bit ignored
                0,                     // no accumulation buffer
                0, 0, 0, 0,            // accum bits ignored
                32,                    // 32-bit z-buffer
                0,                     // no stencil buffer
                0,                     // no auxiliary buffer
                PFD_MAIN_PLANE,        // main layer
                0,                     // reserved
                0, 0, 0                // layer masks ignored
            };

            oglPixelFormat = ::ChoosePixelFormat(initHDC, &selPFD);
            if (oglPixelFormat == 0) {
                CErrorMessage errorMessage;
                errorMessage.MessageBox("::ChoosePixelFormat", NULL, MB_ICONEXCLAMATION);
            }
        }
    else
        {
            oglPixelFormat = specifiedPfdIndex;
        }

    // overlay or not ....
    PIXELFORMATDESCRIPTOR pfd;
    if (!::SetPixelFormat(initHDC, oglPixelFormat, &pfd)) {
        CErrorMessage errorMessage;
        errorMessage.MessageBox("::SetPixelFormat", NULL, MB_ICONEXCLAMATION);
        GenAppInternalError("InitOpenGL - cannot set pixel format");
    }

    //  get the description
    if (!::DescribePixelFormat(initHDC, oglPixelFormat, sizeof(oglPFD), &oglPFD)) {
        CErrorMessage errorMessage;
        errorMessage.MessageBox("::DescribePixelFormat", NULL, MB_ICONEXCLAMATION);
    }

    currentPfd.SetFromPfd(oglPixelFormat, oglPFD);

    //  set the overlay data
    oglHasOverlay   = currentPfd.hasOverlay;
    if (oglHasOverlay)
        {
            // get layer plane data
            LAYERPLANEDESCRIPTOR lpd;
            VERIFY(::wglDescribeLayerPlane(initHDC,
                                           oglPixelFormat, 1, sizeof(lpd), &lpd));
            oglOverlayIsDoubleBuffered = lpd.dwFlags & LPD_DOUBLEBUFFER;
        }

    // final check for pallete PFDs (paranoia)
    if (oglPFD.dwFlags & PFD_NEED_PALETTE)
        GenAppInternalError("InitOpenGL - no true color visual");

    //  set status
    oglIsDoubleBuffered     = (oglPFD.dwFlags & PFD_DOUBLEBUFFER);
    oglIsStereo = (oglPFD.dwFlags & PFD_STEREO);
}


void PS_OGLBase::InitOpenGL(CWnd* pWnd)
{
    oglDisplay = new CClientDC(pWnd);
    HDC currHDC = oglDisplay->GetSafeHdc();

    //  first call ??
    if (oglPixelFormat == 0)
        InitStatics(currHDC);
    else
        //  no use curent settings
        if (!SetPixelFormat(currHDC, oglPixelFormat, &oglPFD))
            GenAppInternalError("InitOpenGL - cannot set pixel format ???");

    // main context
    oglContext = ::wglCreateContext(currHDC);
    if (oglContext == NULL) {
        CErrorMessage errorMessage;
        TRACE( "::wglCreateContext -- %d: %s", errorMessage.GetMessageCode(), errorMessage.GetMessage());
    }
    if (oglHasOverlay)
        {
            oglOverlayContext = ::wglCreateLayerContext(currHDC, 1);
            oglHasOverlay = oglOverlayContext != NULL;
        }

    // if not base then share
    if (GetBaseContext() != oglContext)
        VERIFY(::wglShareLists(GetBaseContext(), oglContext));

}



bool PS_OGLBase::IsInitialized()
{
    return oglPixelFormat != 0;
}

const OpenGLPfd& PS_OGLBase::GetCurrentPfd()
{
    if (!IsInitialized())
        GenAppInternalError("PS_OGLBase::GetCurrentPfd");
    return currentPfd;
}

void  PS_OGLBase::GetAllPfds(CWnd* pWnd,
                             OpenGLpfdArray& availPfds)
{
    CClientDC currDisplay(pWnd);
    HDC currHDC = currDisplay.GetSafeHdc();

    // number of pixel formats
    PIXELFORMATDESCRIPTOR pfd;
    int nPixelFormat = ::DescribePixelFormat(currHDC, 1, sizeof(pfd), NULL);

    availPfds.Alloc(nPixelFormat);
    int nOK = 0;
    for (int i = 1; i <= nPixelFormat; i++)
        {
            //  check this one
            ::DescribePixelFormat(currHDC, i, sizeof(pfd), &pfd);

            //  only care about true color, OpenGL, draw to window
            if  ((pfd.dwFlags & PFD_NEED_PALETTE) ||
                 (!(pfd.dwFlags & PFD_SUPPORT_OPENGL)) ||
                 (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW)))
                continue;


            availPfds[nOK++].SetFromPfd(i, pfd);
        }
    availPfds.SetSize(nOK);
}

void PS_OGLBase::GetPfdSelectionSettings(bool& setToDefault,
                                         int&  specifiedIndex)
{
    setToDefault  = setToDefaultPfd;
    specifiedIndex = specifiedPfdIndex;
}

void   PS_OGLBase::SetPfdSelectionSettings(bool  setToDefault,
                                           int   specifiedIndex)
{
    setToDefaultPfd  = setToDefault;
    specifiedPfdIndex = specifiedIndex;
}

void  PS_OGLBase::SetBitmapFontPlatformSettings(BitmapFontPlatform inBMF)
{
    bitmapFontPlatform = inBMF;
}

void PS_OGLBase::Set2DAntiAliasLines(bool antiAlias)
{
    antiAlias2DLines = antiAlias;
}
void PS_OGLBase::Set3DAntiAliasLines(bool antiAlias)
{
    antiAlias3DLines = antiAlias;
}


bool PS_OGLBase::UseWin9XFonts()
{
    if (bitmapFontPlatform == bmf_Automatic)
        return projectUtilities::IsWindows9X();

    return (bitmapFontPlatform == bmf_Win9X);
}


void PS_OGLBase::GetOpenGLStrings(char* glVersionStr,
                                  char* glVendorStr,
                                  char* glRendererStr,
                                  int   maxLen)
{
    if (!IsInitialized())
        GenAppInternalError("PS_OGLBase::GetOpenGLString");
    const char* verStr = (const char*) glGetString(GL_VERSION);
    CopyString(glVersionStr, verStr, maxLen);

    const char* venStr = (const char*) glGetString(GL_VENDOR);
    CopyString(glVendorStr, venStr, maxLen);

    const char* renStr = (const char*) glGetString(GL_RENDERER);
    CopyString(glRendererStr, renStr, maxLen);
}

