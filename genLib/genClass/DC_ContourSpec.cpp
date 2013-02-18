///////////////////////////////////////////////////////////////////////////////////
//
// DC_ContourSpec.cpp
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
// DESCRIPTION: Platform independent and application independent base classes and
//              support classes.
//              Data Classes (DC) that define basic types of data used within the
//              framework: contour values, colors, and line type
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/DC_ContourSpec.h>

DC_ContourSpec::DC_ContourSpec(const DC_ContourSpec& a)
{
    LocalCopy(a);
}
DC_ContourSpec&  DC_ContourSpec::operator= (const DC_ContourSpec& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void  DC_ContourSpec::LocalCopy(const DC_ContourSpec& a)
{
    contourColor    = a.contourColor;
    lineThickness   = a.lineThickness;
    lineType        = a.lineType;
    contourVal      = a.contourVal;
    mitreLines      = a.mitreLines;
}

DC_ContourSpecArray::DC_ContourSpecArray(const DC_ContourSpecArray& a)
{
    LocalCopy(a);
}
DC_ContourSpecArray&  DC_ContourSpecArray::operator= (const DC_ContourSpecArray& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void  DC_ContourSpecArray::LocalCopy(const DC_ContourSpecArray& a)
{
    T_SC_Array<DC_ContourSpec>::operator= (a);
    doLogContours = a.doLogContours;
}

