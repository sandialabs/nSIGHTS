///////////////////////////////////////////////////////////////////////////////////
//
// PFO_UserLabels.cpp
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
//      basic user entered text for labels.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genPlotClass/C_PlotDef.h>

#include <genPFO/PFO_UserLabels.h>

PFO_UserLabels::UserLabel::UserLabel(const UserLabel& a)
{
    LocalCopy(a);
}

PFO_UserLabels::UserLabel& PFO_UserLabels::UserLabel::operator= (const UserLabel& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}
void PFO_UserLabels::UserLabel::LocalCopy(const UserLabel& a)
{
    CopyString(userLabel, a.userLabel, userLabelLen);
    doLabel         = a.doLabel        ;
    labAlign        = a.labAlign       ;
}


PFO_UserLabels :: PFO_UserLabels(PlotDefC& assPlt)
    : AnnoObjC("User Labels", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_UserLabels::PFO_UserLabels(const PFO_UserLabels& a, PlotDefC& assPlt) : AnnoObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_UserLabels :: ~PFO_UserLabels ()
{
}

void PFO_UserLabels::InitCommon()
{
    DoStatusChk();
}

PFO_UserLabels& PFO_UserLabels::operator= (const PFO_UserLabels& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void  PFO_UserLabels::FullCopy(const PFO_UserLabels& a)
{
    AnnoObjC::FullCopy(a);
    LocalCopy(a);
}

void  PFO_UserLabels::LocalCopy(const PFO_UserLabels& a)
{
    for (int i = 0; i < maxUserLabels; i++)
        labelData[i] = a.labelData[i];
}

void PFO_UserLabels::SetDefault()
{
    AnnoObjC::SetDefault();
    for ( int i = 0; i < maxUserLabels; i++)
    {
        labelData[i].doLabel = false;
        labelData[i].userLabel[0] = '\0';
    }
}


void  PFO_UserLabels:: DoStatusChk()
{
    AnnoObjC::DoStatusChk();
    if (objStatus != os_OK)
        return;

    for ( int i = 0; i < maxUserLabels; i++)
        if (labelData[i].doLabel && (labelData[i].userLabel[0] != '\0'))
        {
            SetDefaultID(labelData[i].userLabel);
            return;
        }

    SetObjErrMsg("no label data set/selected");
}

