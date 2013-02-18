///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ColorLegendGL.cpp
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
//      Implements renderer dependent code to actually draw genPFO functional
// objects on a plot.  Is platform independent but
// relies on platform dependent code in genPlotClassPS.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/PC_Axes.h>
#include <genPFOGL/GL_Base.h>
#include <genPFOGL/GL_ColorLegendSpec.h>

#include <genPFOGL/PFO_ColorLegendGL.h>

PFO_ColorLegendGL :: PFO_ColorLegendGL(PlotDefC& assPlt) :
                        PFO_ColorLegend(assPlt),
                        OGLBaseObj(this)
{
}
PFO_ColorLegendGL::PFO_ColorLegendGL(const PFO_ColorLegendGL& a, PlotDefC& assPlt) :
                        PFO_ColorLegend(a, assPlt),
                        OGLBaseObj(this)
{
}
PFO_ColorLegendGL::~PFO_ColorLegendGL()
{
}

PFO_ColorLegendGL& PFO_ColorLegendGL::operator= (const PFO_ColorLegendGL& a)
{
    PFO_ColorLegend::operator=(a);
    return *this;
}


void PFO_ColorLegendGL::GetIncVals(SC_StringArray& incLabels,
                                   SC_DoubleArray& incTVal)
{
    SC_DoubleArray incVals;
    incVals.SetResizable();

    // autoInc not implemented yet
    int nIncs;
    double stVal = colorLimits->minLimit;
    double eVal = colorLimits->maxLimit;
    if (colorLimits->logRange)
    {
        stVal = log10(stVal);
        eVal = log10(eVal);
    }

    bool fixedIncs = true;
    if (specIncs)
    {
        nIncs = labelArrayDO->Size();
        if (useLabelX)
        {
            fixedIncs = false;
            incVals.AllocAndSetSize(nIncs);
            for (int i = 0; i < nIncs; i++)
                incVals[i] = labelArrayDO->dataCoords[i].cX;
            if (colorLimits->logRange)
                incVals.Log10();
        }
        else
        {
            stVal = 0.0;
            eVal = 1.0;
        }
    }
    else
    {

        if (autoIncs)
        {
            fixedIncs = false;

            LinLogAxis1D incTemp;
            incTemp.autoScaleAxes = true;
            incTemp.axesAreLinear = !colorLimits->logRange;
            incTemp.SetAxisLimits(colorLimits->minLimit, colorLimits->maxLimit);
            incTemp.AdjustAxisLimits();
            incTemp.SetMajorIncs();
            incTemp.GetAxesMajorIncs(incVals);
            if (colorLimits->logRange)
                incVals.Log10();

            if (incVals[0] < stVal)
            {
                if (includeSE)
                    incVals[0] = stVal;
                else
                    incVals.DeleteItem(0);
            }
            else
                if ((incVals[0] > stVal) && includeSE)
                    incVals.InsertItem(stVal, 0);

            if (incVals.IsNotEmpty())
            {
                if (incVals.LastIndex() > eVal)
                {
                    if (includeSE)
                        incVals.LastIndex() = eVal;
                    else
                        incVals.nlistData--;
                }
                else
                    if ((incVals.LastIndex() < eVal) && includeSE)
                        incVals += eVal;
            }
            nIncs = incVals.Size();

            // pathological case
            if (nIncs == 0)
            {
                nIncs = 2;
                incVals += stVal;
                incVals += eVal;
            }
        }
        else
        {
            // manual
            nIncs = nmanualIncs + 1;
        }
    }

    if (fixedIncs)
    {
        incVals.Alloc(nIncs);
        double dVal = (eVal - stVal) / (nIncs - 1);
        double iVal = stVal;
        for (int i = 0; i < nIncs; i++)
        {
            incVals += iVal;
            iVal += dVal;
        }
    }


    // make labels & convert log incs back
    incTVal.AllocAndSetSize(nIncs);
    incLabels.AllocAndSetSize(nIncs);
    for (int i = 0; i < nIncs; i++)
    {
        incTVal[i] = (incVals[i] - stVal) / (eVal - stVal);
        if (specIncs)
            incLabels.SetString(labelArrayDO->dataLabels[i], i);
        else
        {
            if (colorLimits->logRange)
                incVals[i] = pow(10.0, incVals[i]);
            legendFormat.RealToString(incVals[i], incLabels[i], 80);
        }
    }

    // kluge for log auto exp
    if (autoIncs && colorLimits->logRange && (legendFormat.format > ncf_Scientific2))
    {
        SC_SciFormat temp(0);
        temp.format = ncf_Scientific2;
        if (strstr(incLabels[0], "x10") != NULL)
            temp.RealToString(incVals[0], incLabels[0], incLabels.GetStringLen());
        if (strstr(incLabels.LastIndex(), "x10") != NULL)
            temp.RealToString(incVals.LastIndex(), incLabels.LastIndex(), incLabels.GetStringLen());
    }

}

void  PFO_ColorLegendGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    GL_ColorLegendSpec legendSpec(*this, *this, *objectBase);

    GetIncVals(legendSpec.incLabels,  legendSpec.incTVal);

    legendSpec.CalculateLegendPosition();
    legendSpec.DrawLegendBase();
    legendSpec.DrawColorBar();
    legendSpec.DrawMainLabel();
    legendSpec.DrawIncLabels();

    GL_Base::GetAnnoPixelXY(legendSpec.fullFrame.ul, rectLimits.ulXpos, rectLimits.ulYpos);
    GL_Base::GetAnnoPixelXY(legendSpec.fullFrame.lr, rectLimits.lrXpos, rectLimits.lrYpos);
    SetRectOffset(legendSpec.fullFrame.ul);

    CloseDrawObject();

}

