///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectRangePS.cpp
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
//      I/O and user interface for all osDFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClass/U_Msg.h>

#include "DPO_SelectRangePS.h"

DPO_SelectRangePS::DPO_SelectRangePS(bool forCubes) :
    rangeSimResultsObjRefUI(typeid(DO_RangeSimResults)), mustBeCubes(forCubes)
{
}

void DPO_SelectRangePS::SetAllSensitive()
{
    indexMSUI.indexListUI.SetSensitive(listDataOK);
}

void DPO_SelectRangePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_SelectRangePS*>(inObj)->SetAllSensitive();
}

void DPO_SelectRangePS::SetIndexList(bool fromCB)
{
    dataDC = 0;
    SC_StringArray dataIDs;
    if (rangeSimResultsObjRefUI.CheckDataObj())
    {
        dataDC = static_cast<DO_RangeSimResults*>(rangeSimResultsObjRefUI.GetDataObj())->rangeResults;
        bool isCubes = dataDC->IsCubes();
        if ((mustBeCubes && isCubes) || ((!mustBeCubes) && (!isCubes)))
            dataDC->GetMenuStrings(dataIDs);
        else
            if (fromCB)
                if (mustBeCubes)
                    GenAppErrorMsg("ReadRangeResults", "does not contain cube data");
                else
                    GenAppErrorMsg("ReadRangeResults", "does not contain grid data");
    }

    listDataOK = !dataIDs.IsEmpty();
    if (listDataOK)
    {
        indexMSUI.indexListUI.SetListText(dataIDs);
    }
    else
    {
        indexMSUI.indexListUI.SetListNA();
    }
    SetAllSensitive();
}

void DPO_SelectRangePS::SetIndexListCB(void* inObj)
{
    static_cast<DPO_SelectRangePS*>(inObj)->SetIndexList(true);
}

void DPO_SelectRangePS::InitMenu()
{
    DPO_SelectBasePS::InitMenu(true, false);

    ArgListC args;
    args.AllFormAttach();
    rangeSimResultsObjRefUI.InitFramedUI("nSIGHTS Range Results To Select From", URform.GetWidget(), args);
    rangeSimResultsObjRefUI.AddOtherObjectCallback(SetIndexListCB, this);

}

void DPO_SelectRangePS::MenuOpen(DPO_SelectRange& inData)
{
    DPO_SelectBasePS::MenuOpen(inData);
    rangeSimResultsObjRefUI.StdMenuInit(inData.currRangeFO.rangeSimResultsObjRef);
}


void DPO_SelectRangePS::SetMenuData(DPO_SelectRange&  inData)
{
    rangeSimResultsObjRefUI.SetFuncObjRef(inData.currRangeFO.rangeSimResultsObjRef);

    DPO_SelectBasePS::SetMenuData(inData);

    SetIndexList(false);

    SetAllSensitive();
}

bool DPO_SelectRangePS::UIdataOK()
{
    DPO_SelectBasePS::UIdataOK();
    rangeSimResultsObjRefUI.FuncObjRefOK(allOK, "Data source");

    return allOK;
}

void DPO_SelectRangePS::GetMenuData(DPO_SelectRange&  outData)
{
    DPO_SelectBasePS::GetMenuData(outData);
    outData.currRangeFO.rangeSimResultsObjRef = rangeSimResultsObjRefUI.GetFuncObjRef();

}

