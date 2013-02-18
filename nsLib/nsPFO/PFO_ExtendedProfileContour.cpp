///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ExtendedProfileContour.cpp
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
//      plot grid lines and labels at sequence levels.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genClass/SC_Triangulation.h>

#include <nsClass/DO_ExtendedProfile.h>

#include <nsPFO/PFO_ExtendedProfileContour.h>

PFO_ExtendedProfileContour :: PFO_ExtendedProfileContour(PlotDefC& assPlt) :
    PlotObjC("Profile Contour Lines", assPlt)
{
    plotAsTubes     = false;
    tubeData.adjustForScaling = true;

    InitCommon();
}

PFO_ExtendedProfileContour::PFO_ExtendedProfileContour(const PFO_ExtendedProfileContour& a, PlotDefC& assPlt) :
    PlotObjC(a, assPlt), PSC_3DExtendedProfileData(a)
{
    LocalCopy(a);
    InitCommon();
}

PFO_ExtendedProfileContour :: ~PFO_ExtendedProfileContour ()
{
}

void PFO_ExtendedProfileContour::InitCommon()
{
    PSC_ContourSpec::InitCommon(*this);
    PSC_3DExtendedProfileData::InitCommon(*this);
    AddInPort(dataObjRef, typeid(DO_ExtendedProfile));

    reportOK        = false;
    usePolyOffset   = true;

    DoStatusChk();
}

PFO_ExtendedProfileContour& PFO_ExtendedProfileContour::operator= (const PFO_ExtendedProfileContour& a)
{
    if (&a != this)
    {
        PlotObjC::operator=(a);
        PSC_3DExtendedProfileData::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_ExtendedProfileContour::LocalCopy(const PFO_ExtendedProfileContour& a)
{
    PSC_ContourSpec::operator=(a);
    plotAsTubes      = a.plotAsTubes   ;
    tubeData         = a.tubeData      ;
}

void  PFO_ExtendedProfileContour::DoStatusChk()
{
    contourLines.DeAlloc();

    PSC_3DExtendedProfileData::StatusChkSetup(*this);

    PlotObjC::DoStatusChk();
    PSC_ContourSpec::DoStatusChk(*this);
    CheckInputFO(dataObjRef, "Grid Data");

    if (StatusNotOK())
        return;

    extendedProfileData = static_cast<DO_ExtendedProfile*> (GetInPortData(dataObjRef))->profileResults;

    PSC_3DExtendedProfileData::DoStatusChk(*this, *extendedProfileData);

    // check and set triang
    SC_Triangulation& currTri = extendedProfileData->GetTriangulation();
    if (!currTri.TriangulationOK())
        SetObjErrMsg("cannot triangulate ??");
}


void PFO_ExtendedProfileContour:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    SC_DoubleArray contourData;
    extendedProfileData->GetData(contourData);

    if (contourSpec.doLogContours)
        contourData.Log10();

    if (contourData.GetnDataOK() < 3)
    {
        SetObjErrMsg("less than 3 valid data");
        return;
    }

    SC_Triangulation& currTri = extendedProfileData->GetTriangulation();

    contourLines.Alloc(contourSpec.Size());
    SC_IntArray  orderedPoints;
    Line3D       cLine;
    Coord3D      nextPoint;

    for (int i = 0; i < contourSpec.Size(); i++)
    {
        double currVal = contourSpec[i].contourVal;
        if (contourSpec.doLogContours)
            currVal = log10(currVal);

        if (currTri.GetConnectedEdgeList(contourData, currVal, orderedPoints))
        {
            contourLines[i].Alloc(orderedPoints.Size());
            for (int j = 0; j < orderedPoints.Size(); j++)
            {
                int edgeIndex = orderedPoints[j];
                if (edgeIndex < 0)
                {
                    nextPoint = Coord3D();
                }
                else
                {
                    const SC_Triangulation::TriangleEdge& currEdge = currTri.GetEdge(edgeIndex);
                    currTri.GetNodeCoord(currEdge.stNode, cLine.stPt);
                    currTri.GetNodeCoord(currEdge.endNode, cLine.endPt);

                    // slight kluge: if Z is same, honor log stuff
                    if ((zvalueSource == zvs_Same) && contourSpec.doLogContours)
                    {
                        cLine.stPt.cZ = contourData[currEdge.stNode];
                        cLine.endPt.cZ = contourData[currEdge.endNode];
                        nextPoint = cLine.PointOnLine(currEdge.cPos);
                        nextPoint.cZ = InvLgt(nextPoint.cZ);
                    }
                    else
                    {
                        cLine.stPt.cZ = GetZVal(currEdge.stNode, *extendedProfileData);
                        cLine.endPt.cZ = GetZVal(currEdge.endNode, *extendedProfileData);
                        nextPoint = cLine.PointOnLine(currEdge.cPos);
                    }

                    MapCoords(nextPoint);
                }


                contourLines[i] += nextPoint;
            }
        }
    }
    contourLines.SetSize(contourSpec.Size());
}


bool PFO_ExtendedProfileContour::ContainsLinesOnly()
{
    return true;
}


Limit3D PFO_ExtendedProfileContour::GetPlotObjLimits()
{
    Limit3D  currLimits;
    if (StatusNotOK() || (contourLines.IsEmpty()))
        return currLimits;

    for (int i = 0; i < contourLines.Size(); i++)
        for (int j = 0; j < contourLines[i].Size(); j++)
            currLimits.AddToLimits(contourLines[i][j]);

    return currLimits;
}

