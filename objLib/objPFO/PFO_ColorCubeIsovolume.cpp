///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ColorCubeIsovolume.cpp
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
//    Plotting functional objects for cube, grid, tables and XY data.
//
//
///////////////////////////////////////////////////////////////////////////////////


#include <genClass/SC_RealFormat.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>
#include <genClass/U_Str.h>


#include <objPFO/PFO_ColorCubeIsovolume.h>

PFO_ColorCubeIsovolume :: PFO_ColorCubeIsovolume(PlotDefC& assPlt)
    : PFO_CubeColorBase("Cube Isovolume", assPlt, false),
    isoValueMS("Isovolume value")
{
    reportOK        = false;
    clipToRange     = false;
    usePen          = false;
    isothermPen     = 4;
    InitCommon();
}

PFO_ColorCubeIsovolume::PFO_ColorCubeIsovolume(const PFO_ColorCubeIsovolume& a, PlotDefC& assPlt) :
    PFO_CubeColorBase(a, assPlt),
    isoValueMS("Isovolume value")
{
    InitCommon();
    LocalCopy(a);
}

PFO_ColorCubeIsovolume :: ~PFO_ColorCubeIsovolume ()
{
}

void PFO_ColorCubeIsovolume::InitCommon()
{
    isoValueMS.AddPort(*this);
    usePolyOffset   = true;
    triToPlot.SetResizable(128);
    DoStatusChk();
}

PFO_ColorCubeIsovolume& PFO_ColorCubeIsovolume::operator= (const PFO_ColorCubeIsovolume& a)
{
    if (&a != this)
    {
        PFO_CubeColorBase::operator=(a);
        isoValueMS = a.isoValueMS;
        LocalCopy(a);
    }
    return *this;
}

void  PFO_ColorCubeIsovolume::LocalCopy(const PFO_ColorCubeIsovolume& a)
{
    usePen = a.usePen;
    isothermPen = a.isothermPen;
}

void  PFO_ColorCubeIsovolume:: DoStatusChk()
{
    PFO_CubeColorBase::DoStatusChk();
    isoValueMS.DoCheck("Isovolume master", *this);
    if (StatusNotOK())
        return;

    // only one color
    isoValue = isoValueMS.GetCurrentValue();
    SC_DoubleArray currValues(1);
    currValues[0] = isoValue;

    if (rangeLimit.logRange)
    {
        if (!IsPositive(isoValue))
            SetObjErrMsg("for log range iso value must be +ve");
        else
            isoValue = log10(isoValue);
    }

    if (!usePen)
    {
        rangeLimit.MapDataToRange(currValues, cubeColors, false, colorMap->ncolorMap);
        if (clipToRange)
        {
            double tVal = isoValueMS.GetCurrentValue();
            if (tVal < rangeLimit.minLimit)
                SetObjErrMsg("isovalue less than clipped minimum");
            if (tVal > rangeLimit.maxLimit)
                SetObjErrMsg("isovalue greater than clipped maximum");
        }
    }
}


void PFO_ColorCubeIsovolume:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    ntriX = cubeData->xData.Size() - 1;
    ntriY = cubeData->yData.Size() - 1;

    ntri = ntriX * ntriY  * 2;
    nlayers = cubeData->zData.Size() - 1;

    SC_DoubleArray nextTriData(3);        // tri data

    triToPlot.SetEmpty();

    for (int i = 0; i < ntri; i++)
    {
        for (int j = 0; j < nlayers; j++)
        {
            int triNum = j * ntri + i;
            GetTriData(triNum);
            if (rangeLimit.logRange)
                isoCalc.currTriData.Log10();

            if (isoCalc.currTriData.DataOK())
            {
                double triMin, triMax;
                isoCalc.currTriData.CalcMinMax(triMin, triMax);

                if ((triMin <= isoValue) && (triMax >= isoValue))
                    triToPlot += triNum;
            }
        }
    }
}

bool PFO_ColorCubeIsovolume::SupportsTransparency() const
{
    return true;
}
int PFO_ColorCubeIsovolume::GetnTransObjects() const
{
    return triToPlot.Size();
}

Coord3D PFO_ColorCubeIsovolume::GetTransObjectCoord(int objIndex)
{
    GetTriPoly(objIndex);
    return GetTransPolyCentroid(isoCalc.polyCoords);
}

Limit3D PFO_ColorCubeIsovolume::GetPlotObjLimits()
{
    Limit3D  currLimits;
    DoStatusChk();
    if (StatusNotOK())
        return currLimits;

    for (int i = 0; i < triToPlot.Size(); i++ )
    {
        GetTriData(triToPlot[i]);
        for (int j = 0; j < isoCalc.currTriCoords.Size(); j++)
        {
            cubeData->UnTransformCoord(isoCalc.currTriCoords[j]);
            MapCoords(isoCalc.currTriCoords[j]);
        }
        currLimits.AddToLimits(isoCalc.currTriCoords);
    }

    return currLimits;
}

void PFO_ColorCubeIsovolume::GetTriData(int triIndex)
{
    int triNum =  triIndex % ntri;

    int blockNum = triNum / 2;

    int x0 = blockNum / ntriY;
    int y0 = blockNum % ntriY;
    int z0 = triIndex / ntri;

    if ((triIndex % 2) == 0)
    {
        // even tri
        SetTriData(x0, y0, z0, 0);
        SetTriData(x0, y0 + 1, z0, 1);
        SetTriData(x0 + 1, y0, z0, 2);

        z0++;
        SetTriData(x0, y0, z0, 3);
        SetTriData(x0, y0 + 1, z0, 4);
        SetTriData(x0 + 1, y0, z0, 5);
    }
    else
    {
        // odd tri
        SetTriData(x0, y0 + 1, z0, 0);
        SetTriData(x0 + 1, y0 + 1, z0, 1);
        SetTriData(x0 + 1, y0, z0, 2);

        z0++;
        SetTriData(x0, y0 + 1, z0, 3);
        SetTriData(x0 + 1, y0 + 1, z0, 4);
        SetTriData(x0 + 1, y0, z0, 5);
    }
}

void PFO_ColorCubeIsovolume::SetTriData(int x, int y, int z, int i)
{
    isoCalc.currTriData[i] = cubeData->cData[x][y][z];
    Coord3D& currC = isoCalc.currTriCoords[i];
    currC.cX = cubeData->xData[x];
    currC.cY = cubeData->yData[y];
    currC.cZ = cubeData->zData[z];
    cubeData->TransformCoord(currC);
}

void PFO_ColorCubeIsovolume::GetTriPoly(int triIndex)
{
    GetTriData(triToPlot[triIndex]);
    if (rangeLimit.logRange)
        isoCalc.currTriData.Log10();

    if (isoCalc.CalcTriPoly(isoValue))
    {
        for (int j = 0; j < isoCalc.polyCoords.Size(); j++)
        {
            cubeData->UnTransformCoord(isoCalc.polyCoords[j]);
            MapCoords(isoCalc.polyCoords[j]);
        }
    }
}



