///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridColorRangeFill.cpp
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
//    Plots grids as varying colors on a 2D or 3D plot.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>
#include <genClass/SC_Triangulation.h>

#include <objPFO/PFO_GridColorRangeFill.h>


PFO_GridColorRangeFill :: PFO_GridColorRangeFill(PlotDefC& assPlt) :
    PFO_GridColorBase("Grid Color Fill", assPlt),
    assocNodes(3), assocCoord(3)
{
    edgeSpec.plotEdges  = false;
    reportFormat        = SC_SciFormat(3);
}

PFO_GridColorRangeFill::PFO_GridColorRangeFill(const PFO_GridColorRangeFill& a, PlotDefC& assPlt) :
    PFO_GridColorBase(a, assPlt),
    assocNodes(3), assocCoord(3)
{
    LocalCopy(a);
}

PFO_GridColorRangeFill :: ~PFO_GridColorRangeFill ()
{
}


PFO_GridColorRangeFill& PFO_GridColorRangeFill::operator= (const PFO_GridColorRangeFill& a)
{
    if (&a != this)
    {
        PFO_GridColorBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_GridColorRangeFill::LocalCopy(const PFO_GridColorRangeFill& a)
{
    edgeSpec = a.edgeSpec;
    reportFormat    = a.reportFormat;
}


void PFO_GridColorRangeFill:: CalcOutput(FOcalcType  calcType)
{
    PFO_GridColorBase::CalcOutput(calcType);
    if (StatusNotOK())
        return;

    if (!is3DObject)
    {
        cursorReport->ReportLabelSetup(0, "Block:");
        cursorReport->ReportTitleSetup(GetID());
    }
}


bool PFO_GridColorRangeFill::SupportsTransparency() const
{
    return true;
}

int PFO_GridColorRangeFill::GetnTransObjects() const
{
    return gridData->GetTriangulation().GetNTriangles();
}

void PFO_GridColorRangeFill::SetupForGetCoord()
{
    triLimits = GetCurrentViewLimits();
    // set Z limits to null for 2D limit check
    if (!is3DObject)
    {
        triLimits.minLim.cZ = nullReal;
        triLimits.maxLim.cZ = nullReal;
        GetAxesTypes(xLin, yLin);
    }
    else
        GetAxesTypes(xLin, yLin, zLin);
}

bool PFO_GridColorRangeFill::TriLimitsOK(int triIndex)
{
    const SC_Triangulation& gridTri = gridData->GetTriangulation();
    gridTri.GetTriangleNodes(triIndex, assocNodes);
    //  check that at least one node of tri is within limits
    bool triOK = false;
    for (int j = 0; j < 3; j++)
    {
        assocCoord[j] = GetCoord(assocNodes[j]);
        Coord3D& temp = assocCoord[j];

        MapCoords(temp);

        if (((!xLin) && (temp.cX < stdEps)) || ((!yLin) && (temp.cY < stdEps)))
            return false;

        if (is3DObject && (!zLin) && (temp.cZ < stdEps))
            return false;

        if (triLimits.WithinLimits(temp))
            triOK = true;
    }
    return triOK;

}

Coord3D PFO_GridColorRangeFill::GetTransObjectCoord(int objIndex)
{
    if (!TriLimitsOK(objIndex))
        return Coord3D();

    return assocCoord.CalcPolyCentroid();
}


