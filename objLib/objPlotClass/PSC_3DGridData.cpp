///////////////////////////////////////////////////////////////////////////////////
//
// PSC_3DGridData.cpp
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

#include <genClass/U_Real.h>

#include <objClass/DO_GridData.h>

#include <objPlotClass/PSC_3DGridData.h>

PSC_3DGridData :: PSC_3DGridData() :
      fixedZValueMS("FixedZVal")
{
    zvalueSource = zvs_Same;
}

PSC_3DGridData::PSC_3DGridData(const PSC_3DGridData& a) :
    fixedZValueMS(a.fixedZValueMS)
{
    LocalCopy(a);
}

PSC_3DGridData :: ~PSC_3DGridData ()
{
}

void PSC_3DGridData::InitCommon(FuncObjC& inFO)
{
    inFO.AddInPort(otherGridZObjRef, typeid(DO_GridData));
    otherGridCheckRef = inFO.GetLastInPortCheckRef();

    // no out port for Z
    inFO.AddMSInPort(fixedZValueMS.valueMasterObjRef, typeid(DO_Real), fixedZValueMS.valueIsMaster);
    fixedZCheckRef = inFO.GetLastInPortCheckRef();
}

PSC_3DGridData& PSC_3DGridData::operator= (const PSC_3DGridData& a)
{
    if (&a != this)
    {
        fixedZValueMS = a.fixedZValueMS;
        LocalCopy(a);
    }
    return *this;
}

void  PSC_3DGridData::LocalCopy(const PSC_3DGridData& a)
{
    zvalueSource        = a.zvalueSource;
    otherGridZObjRef    = a.otherGridZObjRef;
    coordMap            = a.coordMap;
}

void PSC_3DGridData::StatusChkSetup(PlotObjC&  inPO)
{
    isOther = inPO.is3DObject && (zvalueSource == zvs_OtherGrid);
    isFixed = inPO.is3DObject && (zvalueSource == zvs_FixedVal);

    *otherGridCheckRef = isOther;
    *fixedZCheckRef    = isFixed;
}


void  PSC_3DGridData::DoStatusChk(PlotObjC& inPO,
                                  const DC_GridData& currGridData)
{
    fixedZ = 0.0;
    otherGridData = 0;
    if (!inPO.is3DObject)
        return;

    if (isOther)
    {
        if (!inPO.CheckInputFO(otherGridZObjRef,  "Grid Z Value"))
            return;

        otherGridData = static_cast<DO_GridData*>(inPO.GetInPortData(otherGridZObjRef))->gridData;
        if (!currGridData.IsCompatible(*otherGridData))
        {
            inPO.SetObjErrMsg("grids must be the same size");
            return;
        }
    }

    if (isFixed)
    {
        fixedZValueMS.DoCheck("Fixed value B  master", inPO);
        if (inPO.StatusOK())
            fixedZ = fixedZValueMS.GetCurrentValue();
    }

    if (!coordMap.SetupOK())
        inPO.SetObjErrMsg("one or more coord maps map to same");
}


double PSC_3DGridData::GetZVal(int                indx,
                               const DC_GridData& currGridData)
{
    switch (zvalueSource)
    {
        case zvs_Same      : return currGridData[indx];
        case zvs_OtherGrid : return (*otherGridData)[indx];
        case zvs_FixedVal  : return fixedZ;
    }
    return 0.0;  // keep the compiler happy
}

