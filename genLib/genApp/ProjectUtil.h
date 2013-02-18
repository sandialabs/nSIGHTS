///////////////////////////////////////////////////////////////////////////////////
//
// ProjectUtil.h
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
//              Sets/Gets registry data describing application settings and properties.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PROJECTUTIL_H
#define PROJECTUTIL_H

namespace projectUtilities {

    extern  void          SetModifiedFlag(bool bModified = true);
    extern  bool          IsModified();


    extern  bool          IsWindows2000();
    extern  bool          IsWindowsNT();
    extern  bool          IsWindows9X();

    // Settings
    extern  bool          IsPlotSmallFont();
    extern  void          SetPlotSmallFont(bool bSmall);

    extern  void          GetTreeViewSettings(int& nSingleLine, int& nSingleLineType, int& nShowUnused, int& nShowConnections);
    extern  void          SetTreeViewSettings(int nSingleLine, int nSingleLineType, int nShowUnused, int nShowConnections);

    extern  void          SetTransparent(bool bSet);
    extern  bool          GetTransparent();
    extern  void          SetAlpha(unsigned char nAlpha);
    extern  unsigned char GetAlpha();

    extern  void          SetOpenGL_PixelFormatDescriptor(int  nPFD, int pfdIndex);
    extern  void          GetOpenGL_PixelFormatDescriptor(int& nPFD, int& pfdIndex);

    extern  void          SetOpenGL2DAntiAlias(bool antiAlias);
    extern  void          GetOpenGL2DAntiAlias(bool& antiAlias);

    extern  void          SetOpenGL3DAntiAlias(bool antiAlias);
    extern  void          GetOpenGL3DAntiAlias(bool& antiAlias);

    extern  void          GetBitmapFontPlatformSetting(int& bmFontPlatformSetting);
    extern  void          SetBitmapFontPlatformSetting(int bmFontPlatformSetting);

    extern  int           GetCalculationPriority();
    // returns calc setting as thread priority
    // 0 THREAD_PRIORITY_HIGHEST
    // 1 THREAD_PRIORITY_ABOVE_NORMAL
    // 2 THREAD_PRIORITY_NORMAL
    // 3 THREAD_PRIORITY_BELOW_NORMAL
    // 4 THREAD_PRIORITY_LOWEST

    extern  void          GetCalculationPrioritySetting(int& prioritySetting);
    extern  void          SetCalculationPrioritySetting(int prioritySetting);

    extern  void          SetEuropeanReals(bool europeanReals);
    extern  void          GetEuropeanReals(bool& europeanReals);

    extern  void          SetTemplateDirectory(CString sTemplateDirectory);
    extern  void          GetTemplateDirectory(CString& sTemplateDirectory);

    extern  bool          OutbarIconsAreSmall();
    extern  void          SetOutbarIconSize(bool iconsAreSmall);

    extern  void          SetDefaultSettings();
    extern  void          LoadSettings();
    extern  void          SaveSettings();

};

#endif // _PROJECTUTIL_H

