///////////////////////////////////////////////////////////////////////////////////
//
// PS_OGLBase.h
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

#ifndef PS_OGLBASE_H
#define PS_OGLBASE_H

#include <genClass/T_SC_Array.h>
#include <genClass/T_SC_BasicPtrArray.h>
#include <GL/GL.h>

// description of open GL data taken from pfd
// use in pre-init selection combo box
class OpenGLPfd {
public:
    //  I n   R n     G n     B n     C n     Z n     Dble/Sngl

    int     pfdIndex;
    int     redDepth;
    int     greenDepth;
    int     blueDepth;
    int     colorDepth;
    int     zDepth;
    bool    isDoubleBuffered;   // false is single
    bool    isStereo;           // false is mono only

    //  available but not used yet
    int     alphaDepth;
    bool    hasOverlay;

public:
    OpenGLPfd();

    void    SetFromPfd(int                      indx,
                       PIXELFORMATDESCRIPTOR&   inPFD);

};
typedef T_SC_Array<OpenGLPfd> OpenGLpfdArray;


class PS_OGLBase {
public:
    enum  BitmapFontPlatform       {bmf_Automatic, bmf_Win9X, bmf_WinNT2K};

protected:
    static  int             oglMajorVersion;
    static  int             oglMinorVersion;

private:
    //  for maintenance of shared context
    static  T_SC_BasicPtrArray<PS_OGLBase> oglBaseArray;

    //  old GL information -- retained
    static  PIXELFORMATDESCRIPTOR   oglPFD;
    static  int             oglPixelFormat;
    static  bool            oglIsDoubleBuffered;
    static  bool            oglOverlayIsDoubleBuffered;
    static  bool            oglIsStereo;
    static  int             oglRedDepth;
    static  int             oglGreenDepth;
    static  int             oglBlueDepth;
    static  int             oglZDepth;
    static  bool            oglHasOverlay;

    //  new GL information
    static OpenGLPfd        currentPfd;
    static bool             setToDefaultPfd;
    static int              specifiedPfdIndex;

    static bool             antiAlias2DLines;
    static bool             antiAlias3DLines;


    static  PS_OGLBase*     lastContext;
    // current base context
    static  HGLRC           baseContext;

    static BitmapFontPlatform   bitmapFontPlatform;

public:
    CClientDC*              oglDisplay;
    HGLRC                   oglContext;
    HGLRC                   oglOverlayContext;
    int                     numberDebug;

public:
    PS_OGLBase();
    virtual                 ~PS_OGLBase();
    /// **************************** new code for tab settings/selection support

    //  for use by OpenGL settings
    static bool             IsInitialized();  // true if init performed -- show status tab
    // if false, show settings tab

    //  for pre-init settings tab
    static void             GetAllPfds(CWnd*            pWnd,   // need a window -- I guess any one would do ?
                                       OpenGLpfdArray&  availPfds);         // list of all relevant pfds

    static void             GetPfdSelectionSettings(bool& setToDefault,     // current settings
                                                    int&  specifiedIndex);  // if not setToDefault
    // match specifiedIndex to pfdIndex field to determine
    // ComboBox index

    static void             SetPfdSelectionSettings(bool  setToDefault,
                                                    int   specifiedIndex);  // if not setToDefault,
    // should be .pfdIndex field of
    // selected pfd

    static void             Set2DAntiAliasLines(bool antiAlias);
    static bool             Get2DAntiAliasLines() {return antiAlias2DLines;}
    static void             Set3DAntiAliasLines(bool antiAlias);
    static bool             Get3DAntiAliasLines() {return antiAlias3DLines;}

    // support for selecting correct bitmap font rotations
    static BitmapFontPlatform  GetBitmapFontPlatformSettings() {return bitmapFontPlatform;}
    static void                SetBitmapFontPlatformSettings(BitmapFontPlatform inBMF);
    static bool                UseWin9XFonts();



    // for post-init tab
    static const OpenGLPfd& GetCurrentPfd();                         // only valid if IsInitialized()
    static void             GetOpenGLStrings(char* glVersionStr,     // put these on tab as well
                                             char* glVendorStr,
                                             char* glRendererStr,
                                             int   maxLen);          // assumed same len

    /// **************************** end selection support code

    //  initialization routines
    void                    InitOpenGL(CWnd* pWnd);

    // capability access
    static bool             GLisDoubleBuffered()    {return oglIsDoubleBuffered;}
    static int              GetRedDepth()           {return oglRedDepth;}
    static int              GetGreenDepth()             {return oglGreenDepth;}
    static int              GetBlueDepth()          {return oglBlueDepth;}
    static int              GetZDepth()                 {return oglZDepth;}
    static bool             GLisStereo()            {return oglIsStereo;}

    //  context access
    static HGLRC            GetBaseContext();
    static CClientDC*       GetBaseDisplay();

    //  basic usage
    bool                    MakeCurrent();      // wglMakeCurrent
    virtual void            Flush();  // does glx stuff necessary (swap double buffers);

    bool                    IsCurrent();

private:
    static void                 InitStatics(HDC initHDC);


};

#endif // PS_OGLBASE_H

