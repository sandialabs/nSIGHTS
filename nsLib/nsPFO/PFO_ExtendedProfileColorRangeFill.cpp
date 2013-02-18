///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ExtendedProfileColorRangeFill.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////


#include <nsPFO/PFO_ExtendedProfileColorRangeFill.h>


PFO_ExtendedProfileColorRangeFill :: PFO_ExtendedProfileColorRangeFill(PlotDefC& assPlt) :
    PFO_ExtendedProfileColorBase("Profile Color Fill", assPlt)
{
    edgeSpec.plotEdges  = false;
    reportFormat        = SC_SciFormat(3);
}

PFO_ExtendedProfileColorRangeFill::PFO_ExtendedProfileColorRangeFill(const PFO_ExtendedProfileColorRangeFill& a, PlotDefC& assPlt) :
    PFO_ExtendedProfileColorBase(a, assPlt)
{
    LocalCopy(a);
}

PFO_ExtendedProfileColorRangeFill :: ~PFO_ExtendedProfileColorRangeFill ()
{
}


PFO_ExtendedProfileColorRangeFill& PFO_ExtendedProfileColorRangeFill::operator= (const PFO_ExtendedProfileColorRangeFill& a)
{
    if (&a != this)
    {
        PFO_ExtendedProfileColorBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_ExtendedProfileColorRangeFill::LocalCopy(const PFO_ExtendedProfileColorRangeFill& a)
{
    edgeSpec = a.edgeSpec;
    reportFormat    = a.reportFormat;
}




void PFO_ExtendedProfileColorRangeFill:: CalcOutput(FOcalcType  calcType)
{
    PFO_ExtendedProfileColorBase::CalcOutput(calcType);
    if (StatusOK() && (!is3DObject))
    {
        cursorReport->ReportLabelSetup(0, "Block:");
        cursorReport->ReportTitleSetup(GetID());
    }
}

