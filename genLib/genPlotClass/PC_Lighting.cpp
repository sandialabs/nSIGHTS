///////////////////////////////////////////////////////////////////////////////////
//
// PC_Lighting.cpp
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
//      defines OpenGL lights for 3D plots.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genPlotClass/PC_Lighting.h>

PC_Lighting :: PC_Lighting()
{
    SetDefault();
}

PC_Lighting :: PC_Lighting(const PC_Lighting& a)
{
    LocalCopy(a);
}

PC_Lighting& PC_Lighting::operator= (const PC_Lighting& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void PC_Lighting::LocalCopy(const PC_Lighting& a)
{
    useLighting                     = a.useLighting                  ;       //  overall control


    ambientPen                      = a.ambientPen                   ;
    ambientIntensity                = a.ambientIntensity             ;
    ambientLightAlpha               = a.ambientLightAlpha            ;

    useDefaultLight                 = a.useDefaultLight              ;
    showDefaultLight                = a.showDefaultLight             ;

    defaultDiffuseLightPen          = a.defaultDiffuseLightPen       ;
    defaultDiffuseLightIntensity    = a.defaultDiffuseLightIntensity ;
    defaultDiffuseLightAlpha        = a.defaultDiffuseLightAlpha     ;

    defaultSpecularLightPen         = a.defaultSpecularLightPen      ;
    defaultSpecularLightIntensity   = a.defaultSpecularLightIntensity;
    defaultSpecularLightAlpha       = a.defaultSpecularLightAlpha    ;

    defaultLightIsFixed             = a.defaultLightIsFixed          ;
    fixedElevation                  = a.fixedElevation               ;
    fixedAzimuth                    = a.fixedAzimuth                 ;
    relativeElevation               = a.relativeElevation            ;
    relativeAzimuth                 = a.relativeAzimuth              ;
    defaultLightW                   = a.defaultLightW                ;

}


void PC_Lighting::SetDefault()
{
    useLighting                     = true;       //  overall control

    ambientPen                      = 0;
    ambientIntensity                = 0.1;
    ambientLightAlpha               = 1.0;

    useDefaultLight                 = true;
    showDefaultLight                = false;

    defaultDiffuseLightPen          = 0;
    defaultDiffuseLightIntensity    = 0.9;
    defaultDiffuseLightAlpha        = 1.0;

    defaultSpecularLightPen         = 0;
    defaultSpecularLightIntensity   = 0.5;
    defaultSpecularLightAlpha       = 1.0;

    defaultLightIsFixed             = false;
    fixedElevation                  = 45.0;
    fixedAzimuth                    = 45.0;
    relativeElevation               = 20.0;
    relativeAzimuth                 = 20.0;
    defaultLightW                   = 0.0;
}

