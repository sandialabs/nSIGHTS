///////////////////////////////////////////////////////////////////////////////////
//
// PSC_ContourSpec.cpp
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
//      define contouring of data
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genPlotClass/PSC_ContourSpec.h>

PSC_ContourSpec :: PSC_ContourSpec()
{
    specIsMaster            = true;

    calcIncVal              = true;
    doLogInc                = false;
    startVal                = 0.0;
    endVal                  = 1.0;
    incVal                  = 0.1;
    nIncs                   = 10;

    overrideMasterLineSpec  = false;
    contourLineSpec.linePen = 1;
}

PSC_ContourSpec::PSC_ContourSpec(const PSC_ContourSpec& a)
{
    LocalCopy(a);
}

PSC_ContourSpec :: ~PSC_ContourSpec ()
{
}

void PSC_ContourSpec::InitCommon(FuncObjC& inFO)
{
    inFO.AddMSInPort(specMasterObjRef, typeid(DO_ContourSpec), specIsMaster);

    inFO.AddOutPort(legendOutputDO);
    legendOutputDO.Alloc(1);

    inFO.AddOutPort(contourSpecDO);
    contourSpecDO.contourSpec = &contourSpec;
}

PSC_ContourSpec& PSC_ContourSpec::operator= (const PSC_ContourSpec& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void  PSC_ContourSpec::LocalCopy(const PSC_ContourSpec& a)
{
    dataObjRef           = a.dataObjRef;

    specMasterObjRef     = a.specMasterObjRef;
    specIsMaster         = a.specIsMaster;

    calcIncVal           = a.calcIncVal;
    doLogInc             = a.doLogInc;

    startVal             = a.startVal;
    endVal               = a.endVal;
    incVal               = a.incVal;
    nIncs                = a.nIncs;

    contourLineSpec      = a.contourLineSpec;

    legendGC             = a.legendGC;

    overrideMasterLineSpec = a.overrideMasterLineSpec;
}

void  PSC_ContourSpec::DoStatusChk(PlotObjC& inPO)
{
    int ngridData = 0;
    contourSpec.DeAlloc();

    if (!specIsMaster)
    {
        if (!inPO.CheckInputFO(specMasterObjRef,  "Contour spec master"))
            return;

        DO_ContourSpec* specDO = static_cast<DO_ContourSpec*> (inPO.GetInPortData(specMasterObjRef));

        contourSpec = (*specDO->contourSpec);

   }
    else
    {
        if (doLogInc)
        {
            if (startVal < stdEps)
                inPO.SetObjErrMsg("Start val must be > 0.0");

            if (calcIncVal && (endVal < stdEps))
                inPO.SetObjErrMsg("End val must be > 0.0");

            if ((!calcIncVal) && ((fabs(incVal) < 0.001) || (fabs(incVal) > 5.0)))
                inPO.SetObjErrMsg("abs(inc val) must be >0.001 and < 5");
        }

        if ((nIncs < 0) || (nIncs > maxContourIncs))
            inPO.SetObjErrMsg("# incs  must be < 100");
        if (inPO.StatusNotOK())
            return;

        double sVal = startVal;
        if (doLogInc)
            sVal = Lgt(sVal);

        if (calcIncVal && (nIncs > 0))
        {
            double eVal = endVal;
            if (doLogInc)
                eVal = Lgt(eVal);
            incVal = (eVal - sVal) / double(nIncs);
        }

        contourSpec.Alloc(nIncs + 1);
        contourSpec.doLogContours = doLogInc;

        DC_ContourSpec tempSpec;
        for (int i = 0; i < (nIncs + 1); i++)
        {
            if (doLogInc)
                tempSpec.contourVal = InvLgt(sVal);
            else
                tempSpec.contourVal = sVal;

            contourSpec += tempSpec;
            sVal += incVal;
        }
    }

    if (specIsMaster || overrideMasterLineSpec)
    {
        SC_ColorSpec  conColor = inPO.objPenSet->GetColor(contourLineSpec.linePen);

        for (int i = 0; i < contourSpec.Size(); i++)
        {
            DC_ContourSpec& currSpec = contourSpec[i];
            currSpec.contourColor  = conColor;
            currSpec.lineThickness = contourLineSpec.lineThickness;
            currSpec.lineType      = contourLineSpec.lineType;
            currSpec.mitreLines    = contourLineSpec.mitreLines;
        }
    }

    if (inPO.StatusNotOK())
        return;

    legendOutputDO.Clear();
    PSC_SeriesSpec tempSpec;
    tempSpec.seriesPlotType = psl_LineOnly;
    tempSpec.seriesPen      = contourLineSpec.linePen;
    tempSpec.lineThickness  = contourLineSpec.lineThickness;
    tempSpec.lineType       = contourLineSpec.lineType;

    legendOutputDO.AddSeries(legendGC.GetLabel(inPO.GetID()), tempSpec);
}


