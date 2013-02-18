///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectLabelArray.cpp
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

#include <genClass/U_Str.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/SC_DoubleArray.h>

#include <objDFO/DFO_SelectLabelArray.h>


DFO_SelectLabelArray :: DFO_SelectLabelArray() :
    DFO_SelectBase("SelectLabelArray", baseMS), baseMS(false, true)
{
    sortSpec = ssNone;
    sortBasis = sbX;
    InitCommon();
}

DFO_SelectLabelArray::DFO_SelectLabelArray(const DFO_SelectLabelArray& a) :
    DFO_SelectBase(a, baseMS), baseMS(false, true)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectLabelArray::InitCommon()
{
    InitMSPorts();

    AddOutPort(selLabOutputDO);
    AddOutPort(notSelLabOutputDO);
    AddInPort(labDataArrayObjRef, typeid(DO_LabelArray));

    baseMS.enteredIndexIsSingle = false;

    selLabOutputDO.SetTypeLabel("Selected variables");
    notSelLabOutputDO.SetTypeLabel("Not selected variables");
    DoStatusChk();
}

DFO_SelectLabelArray& DFO_SelectLabelArray::operator= (const DFO_SelectLabelArray& a)
{
    if (&a != this)
    {
        DFO_SelectBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectLabelArray::LocalCopy(const DFO_SelectLabelArray& a)
{
    labDataArrayObjRef  = a.labDataArrayObjRef;
    sortSpec = a.sortSpec;
    sortBasis = a.sortBasis;
}

void  DFO_SelectLabelArray:: DoStatusChk()
{
    DFO_SelectBase::DoStatusChk();
    indexMS.SetTypeLabel("Selected Variable");

    if (!CheckInputFO(labDataArrayObjRef, "Data source"))
        return;

    labDataDO = static_cast<DO_LabelArray*>(GetInPortData(labDataArrayObjRef));

    if (!IndexRangeOK(labDataDO->dataLabels.Size()))
        SetObjErrMsg("selected indexes out of range??");

}

void DFO_SelectLabelArray:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    const SC_CoordArray& labCoord = labDataDO->dataCoords;
    const SC_StringArray& labStrs = labDataDO->dataLabels;

    selLabOutputDO.Reset("Selected labels");
    notSelLabOutputDO.Reset("Not selected labels");

    int ninput = labCoord.Size();

    SC_BoolArray selIndex(ninput, false);
    selIndex.SetIndexes(indexMS.GetCurrentIndexArray());

    for (int i = 0; i <  selIndex.Size(); i++)
        if (selIndex[i])
        {
            selLabOutputDO.dataCoords += labCoord[i];
            selLabOutputDO.dataLabels += labStrs[i];
        }
        else
        {
            notSelLabOutputDO.dataCoords += labCoord[i];
            notSelLabOutputDO.dataLabels += labStrs[i];
        }

    if (sortSpec != ssNone)
    {
        int noutput = selLabOutputDO.Size();
        SC_DoubleArray sortVal(noutput);
        for (int i = 0; i < noutput; i++)
        {
            const Coord3D& outCrd = selLabOutputDO.dataCoords[i];
            double sVal;
            switch (sortBasis) {
                case sbX : {sVal = outCrd.cX; break;}
                case sbY : {sVal = outCrd.cY; break;}
                case sbZ : {sVal = outCrd.cZ; break;}
            }
            sortVal[i] = sVal;
        }

        SC_IntArray indexes;
        indexes.AllocAndIndex(noutput);
        sortVal.Sort(sortSpec == ssAscending, indexes);

        DO_LabelArray temp(selLabOutputDO);
        for (int i = 0; i < noutput; i++)
        {
            selLabOutputDO.dataCoords[i] = temp.dataCoords[indexes[i]];
            selLabOutputDO.dataLabels.SetString(temp.dataLabels[indexes[i]], i);
        }
    }

}

