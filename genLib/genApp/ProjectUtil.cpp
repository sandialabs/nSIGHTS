///////////////////////////////////////////////////////////////////////////////////
//
// ProjectUtil.cpp
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

#include "GenLibAfx.h"
#include <genClass/SC_RealFormat.h>
#include <genApp/ProjectUtil.h>

static bool bModifiedFlag = false;

void projectUtilities::SetModifiedFlag(bool bModified)
{
    bModifiedFlag = bModified;
}

bool projectUtilities::IsModified()
{
    return bModifiedFlag;
}


bool projectUtilities::IsWindows2000()
{
    OSVERSIONINFO osVersionInfo;
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    VERIFY(GetVersionEx(&osVersionInfo));
    if (osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT
        && osVersionInfo.dwMajorVersion == 5)
        {
            return true;
        }
    return false;
}

bool projectUtilities::IsWindowsNT()
{
    OSVERSIONINFO osVersionInfo;
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    VERIFY(GetVersionEx(&osVersionInfo));
    if (osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT
        && osVersionInfo.dwMajorVersion < 5)
        {
            return true;
        }
    return false;
}

bool projectUtilities::IsWindows9X()
{
    OSVERSIONINFO osVersionInfo;
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    VERIFY(GetVersionEx(&osVersionInfo));
    return (osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);
}


// Settings

static bool bPlotFontSmall = true;

bool projectUtilities::IsPlotSmallFont()
{
    return bPlotFontSmall;
}

void projectUtilities::SetPlotSmallFont(bool bSmall)
{
    bPlotFontSmall = bSmall;
}

static int punSingleLine = 1;
static int punSingleLineType = 2;
static int punShowUnused = 0;
static int punShowConnections= 0;


void projectUtilities::GetTreeViewSettings(int& nSingleLine, int& nSingleLineType, int& nShowUnused, int& nShowConnections)
{
    nSingleLine = punSingleLine;
    nSingleLineType = punSingleLineType;
    nShowUnused = punShowUnused;
    nShowConnections = punShowConnections;
}

void projectUtilities::SetTreeViewSettings(int nSingleLine, int nSingleLineType, int nShowUnused, int nShowConnections)
{
    punSingleLine = nSingleLine;
    punSingleLineType = nSingleLineType;
    punShowUnused = nShowUnused;
    punShowConnections = nShowConnections;
}

static BYTE nAlphaBlendingCoefficient;
static bool     bIsSelectorTransparent2000;

void projectUtilities::SetAlpha(BYTE nAlpha)
{
    nAlphaBlendingCoefficient = nAlpha;
}

void projectUtilities::SetTransparent(bool bSet)
{
    bIsSelectorTransparent2000 = bSet;
}

bool projectUtilities::GetTransparent()
{
    return bIsSelectorTransparent2000;
}

BYTE projectUtilities::GetAlpha()
{
    return nAlphaBlendingCoefficient;
}

static int nOpenGLPixelFormatDescriptor = 0;
static int OpenGLPixelFormatDescriptorIndex = 0;

void projectUtilities::SetOpenGL_PixelFormatDescriptor(int nPFD, int pfdIndex)
{
    nOpenGLPixelFormatDescriptor = nPFD;
    OpenGLPixelFormatDescriptorIndex = pfdIndex;
}

void projectUtilities::GetOpenGL_PixelFormatDescriptor(int& nPFD, int& pfdIndex)
{
    nPFD = nOpenGLPixelFormatDescriptor;
    pfdIndex = OpenGLPixelFormatDescriptorIndex;
}

static bool openGLAntiAlias2DLines = 0;
void projectUtilities::SetOpenGL2DAntiAlias(bool antiAlias)
{
    openGLAntiAlias2DLines = antiAlias;
}

void projectUtilities::GetOpenGL2DAntiAlias(bool& antiAlias)
{
    antiAlias = openGLAntiAlias2DLines;
}
static bool openGLAntiAlias3DLines = 0;
void projectUtilities::SetOpenGL3DAntiAlias(bool antiAlias)
{
    openGLAntiAlias3DLines = antiAlias;
}

void projectUtilities::GetOpenGL3DAntiAlias(bool& antiAlias)
{
    antiAlias = openGLAntiAlias3DLines;
}

static int bitmapFontPlatformSetting = 0;

void  projectUtilities::GetBitmapFontPlatformSetting(int& bmFontPlatformSetting)
{
    bmFontPlatformSetting = bitmapFontPlatformSetting;
}
void  projectUtilities::SetBitmapFontPlatformSetting(int bmFontPlatformSetting)
{
    bitmapFontPlatformSetting = bmFontPlatformSetting;
}


static int calcPrioritySetting = 2;

int projectUtilities::GetCalculationPriority()
{
    static int priorityMap[] = {THREAD_PRIORITY_HIGHEST,
                                THREAD_PRIORITY_ABOVE_NORMAL,
                                THREAD_PRIORITY_NORMAL,
                                THREAD_PRIORITY_BELOW_NORMAL,
                                THREAD_PRIORITY_LOWEST};
    return priorityMap[calcPrioritySetting];
}


void projectUtilities::GetCalculationPrioritySetting(int& prioritySetting)
{
    prioritySetting = calcPrioritySetting;
}
void projectUtilities::SetCalculationPrioritySetting(int prioritySetting)
{
    calcPrioritySetting = prioritySetting;
}


static bool realsAreEuropean = false;

void projectUtilities::SetEuropeanReals(bool europeanReals)
{
    realsAreEuropean = europeanReals;
}

void projectUtilities::GetEuropeanReals(bool& europeanReals)
{
    europeanReals = realsAreEuropean;
}

static CString templateDirectory = "";

void projectUtilities::SetTemplateDirectory(CString sTemplateDirectory)
{
    templateDirectory = sTemplateDirectory;
}

void projectUtilities::GetTemplateDirectory(CString& sTemplateDirectory)
{
    sTemplateDirectory = templateDirectory;
}

static bool outbarIconsAreSmall = true;

bool projectUtilities::OutbarIconsAreSmall()
{
    return outbarIconsAreSmall;
}
void projectUtilities::SetOutbarIconSize(bool iconsAreSmall)
{
    outbarIconsAreSmall = iconsAreSmall;
}

void projectUtilities::SetDefaultSettings()
{
    bPlotFontSmall = true;
    punSingleLine = 1;
    punSingleLineType = 2;
    punShowUnused = 0;
    bIsSelectorTransparent2000 = false;
    nAlphaBlendingCoefficient = 120;
    nOpenGLPixelFormatDescriptor = 0;
    OpenGLPixelFormatDescriptorIndex = 0;
    bitmapFontPlatformSetting = 0;
    calcPrioritySetting = 2;
    outbarIconsAreSmall = true;
    openGLAntiAlias2DLines = false;
    openGLAntiAlias3DLines = false;
    realsAreEuropean = false;
    templateDirectory = "C:\\nSights\\Projects";
}

void projectUtilities::LoadSettings()
{
    SetDefaultSettings();

    CWinApp* pApp = AfxGetApp();
    bPlotFontSmall                   = pApp->GetProfileInt("Settings", "SmallFont", bPlotFontSmall);
    punSingleLine                    = pApp->GetProfileInt("Settings", "SingleLine", punSingleLine);
    punSingleLineType                = pApp->GetProfileInt("Settings", "SingleLineType", punSingleLineType);
    punShowUnused                    = pApp->GetProfileInt("Settings", "ShowUnused", punShowUnused);
    punShowConnections               = pApp->GetProfileInt("Settings", "ShowConnections", punShowConnections);
    bIsSelectorTransparent2000       = pApp->GetProfileInt("Settings", "SelectorTransparent2000", bIsSelectorTransparent2000);
    nAlphaBlendingCoefficient        = pApp->GetProfileInt("Settings", "AlphaBlend", nAlphaBlendingCoefficient);
    nOpenGLPixelFormatDescriptor     = pApp->GetProfileInt("Settings", "PixelFormatD", nOpenGLPixelFormatDescriptor);
    OpenGLPixelFormatDescriptorIndex = pApp->GetProfileInt("Settings", "PixelFormatIndex", OpenGLPixelFormatDescriptorIndex);
    openGLAntiAlias2DLines           = pApp->GetProfileInt("Settings", "AntiAlias2DLines", openGLAntiAlias2DLines);
    openGLAntiAlias3DLines           = pApp->GetProfileInt("Settings", "AntiAlias3DLines", openGLAntiAlias3DLines);
    bitmapFontPlatformSetting        = pApp->GetProfileInt("Settings", "BitmapFontPlatform", bitmapFontPlatformSetting);
    calcPrioritySetting              = pApp->GetProfileInt("Settings", "CalcPriority", calcPrioritySetting);
    outbarIconsAreSmall              = pApp->GetProfileInt("Settings", "SmallOutbarIcon", outbarIconsAreSmall);
    realsAreEuropean                 = pApp->GetProfileInt("Settings", "EuropeanReals", realsAreEuropean);
    templateDirectory                = pApp->GetProfileString("Settings", "TemplateDirectory", templateDirectory);

}

void projectUtilities::SaveSettings()
{
    CWinApp* pApp = AfxGetApp();
    pApp->WriteProfileInt("Settings", "SmallFont", bPlotFontSmall);
    pApp->WriteProfileInt("Settings", "SingleLine", punSingleLine);
    pApp->WriteProfileInt("Settings", "SingleLineType", punSingleLineType);
    pApp->WriteProfileInt("Settings", "ShowUnused", punShowUnused);
    pApp->WriteProfileInt("Settings", "ShowConnections", punShowConnections);
    pApp->WriteProfileInt("Settings", "SelectorTransparent2000", bIsSelectorTransparent2000);
    pApp->WriteProfileInt("Settings", "AlphaBlend", nAlphaBlendingCoefficient);
    pApp->WriteProfileInt("Settings", "PixelFormatD", nOpenGLPixelFormatDescriptor);
    pApp->WriteProfileInt("Settings", "PixelFormatIndex", OpenGLPixelFormatDescriptorIndex);
    pApp->WriteProfileInt("Settings", "AntiAlias2DLines", openGLAntiAlias2DLines);
    pApp->WriteProfileInt("Settings", "AntiAlias3DLines", openGLAntiAlias3DLines);
    pApp->WriteProfileInt("Settings", "BitmapFontPlatform", bitmapFontPlatformSetting);
    pApp->WriteProfileInt("Settings", "CalcPriority", calcPrioritySetting);
    pApp->WriteProfileInt("Settings", "SmallOutbarIcon", outbarIconsAreSmall);
    pApp->WriteProfileInt("Settings", "EuropeanReals", realsAreEuropean);
    pApp->WriteProfileString("Settings", "TemplateDirectory", templateDirectory);
}

