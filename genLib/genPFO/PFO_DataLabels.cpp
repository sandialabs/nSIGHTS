///////////////////////////////////////////////////////////////////////////////////
//
// PFO_DataLabels.cpp
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
//      labels that are linked to values produced by other objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genPlotClass/C_PlotDef.h>

#include <genPFO/PFO_DataLabels.h>


PFO_DataLabels::DataLabel::DataLabel()
{
    doLabel             = false;
    labAlign            = hta_Right;
    overrideFormat      = false;
    overrideMainLabel   = false;
    mainLabel[0]        = '\0';
    unitsLabel[0]       = '\0';
}
PFO_DataLabels::DataLabel::DataLabel(const DataLabel& a)
{
    LocalCopy(a);
}
PFO_DataLabels::DataLabel& PFO_DataLabels::DataLabel::operator= (const DataLabel& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}
void PFO_DataLabels::DataLabel::LocalCopy(const DataLabel& a)
{
    labelObjRef     = a.labelObjRef    ;
    doLabel         = a.doLabel        ;
    labAlign        = a.labAlign       ;

    overrideFormat  = a.overrideFormat ;
    labelFormat     = a.labelFormat    ;

    overrideMainLabel = a.overrideMainLabel;
    CopyString(mainLabel, a.mainLabel, DO_Label::maxLabelLen);
    CopyString(unitsLabel, a.unitsLabel, DO_Label::maxLabelLen);
}


PFO_DataLabels :: PFO_DataLabels(PlotDefC& assPlt)
    : AnnoObjC("Data Labels", assPlt)
{
    SetDefault();
    InitCommon();
}

PFO_DataLabels::PFO_DataLabels(const PFO_DataLabels& a, PlotDefC& assPlt) : AnnoObjC(a, assPlt)
{
    LocalCopy(a);
    InitCommon();
}

PFO_DataLabels :: ~PFO_DataLabels ()
{
}

void PFO_DataLabels::InitCommon()
{
    for ( int i = 0; i < maxDataLabels; i++)
        AddUseFlagInPort(labelData[i].labelObjRef, typeid(DO_Label), labelData[i].doLabel);

    minDataFieldWidth = 6;
    DoStatusChk();
}

PFO_DataLabels& PFO_DataLabels::operator= (const PFO_DataLabels& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}


void  PFO_DataLabels::FullCopy(const PFO_DataLabels& a)
{
    AnnoObjC::FullCopy(a);
    LocalCopy(a);
}

void  PFO_DataLabels::LocalCopy(const PFO_DataLabels& a)
{
    for (int i = 0; i < maxDataLabels; i++)
        labelData[i] = a.labelData[i];
    minDataFieldWidth = a.minDataFieldWidth;
}


void PFO_DataLabels::SetDefault()
{
    AnnoObjC::SetDefault();
    for (int i = 0; i < maxDataLabels; i++)
        labelData[i].doLabel = false;
}


void  PFO_DataLabels:: DoStatusChk()
{
    AnnoObjC::DoStatusChk();
    if (objStatus != os_OK)
        return;

    bool labFound = false;
    for ( int i = 0; i < maxDataLabels; i++)
        if (labelData[i].doLabel)
        {
            if (!CheckInputFO(labelData[i].labelObjRef, "Label"))
                return;
            labelData[i].dataSource = (DO_Label*) GetInPortData(labelData[i].labelObjRef);
            if (!labFound)
                SetDefaultID(labelData[i].dataSource->dataTypeLabel);
            labFound = true;
        }

    if (!labFound)
        SetObjErrMsg("no label data selected");

}

