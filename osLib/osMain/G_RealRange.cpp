///////////////////////////////////////////////////////////////////////////////////
//
// G_RealRange.cpp
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
//      support for extracting single values from multiple value runs.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <osMain/G_Vary.h>

#include <osMain/G_RealRange.h>


namespace osRange  {


    #pragma warning(disable : 4073)
    #pragma init_seg(lib)  // make sure this is constructed before alloc objects in app

    RealRangeOutputFO   realRangeOutputFO;


    RealRangeOutputFO::RealRangeOutputFO() :
        RangeOutputFO("RealRangeOutput")
    {
        inputObjRef.AllocAndSetSize(maxRealRange);
        writeInputObj.AllocAndFill(maxRealRange, false);

        currRangeOutput.Alloc(maxRealRange);

        for (int i = 0; i < maxRealRange; i++)
            AddUseFlagInPort(inputObjRef[i], typeid(DO_Real), writeInputObj[i]);

        DoStatusChk();
    }



    bool RealRangeOutputFO::AllocOutputIDs()
    {
        if (!AllocSetup())
            return false;

        if (!CheckInput())
        {
            RangeOutputFO::SetToInitial();
            return false;
        }

        int nAlloc = currRangeOutput.Size();
        // reAlloc required ?
        if (nAlloc != gridOutputDC.Size())
        {
            DeAllocDC();
            gridOutputDC.AllocAndSetSize(nAlloc);
            cubeOutputDC.AllocAndSetSize(nAlloc);
        }

        // reset IDs
        int nextID = 0;
        char tempStr[80];
        for (int i = 0; i < nAlloc; i++)
        {
            // fit value
            CopyString(tempStr, currRangeOutput.GetRef(i).GetTypeLabel(), 80);
            SetDCID(tempStr, nextID);
        }

        DoStatusChk();
        return true;
    }



    bool RealRangeOutputFO::PreCalcSetup()
    {
        GlobalFunc::PreCalcSetup();
        if (!vary.VaryOK())
            return false;

        if (!AllocOutputIDs())
            return false;

        if (!RangeOutputFO::PreCalcSetup())
            return false;

        return true;
    }


    void RealRangeOutputFO::SetToInitial()
    {
        writeInputObj.FillToAlloc(false);
        inputObjRef.FillToAlloc(FuncObjRef());

        RangeOutputFO::SetToInitial();
    }

    void RealRangeOutputFO::DoStatusChk()
    {
        RangeOutputFO::DoStatusChk();
        CheckInput();
    }

    bool RealRangeOutputFO::CheckInput()
    {
        currRangeOutput.SetEmpty();
        for (int i = 0; i < maxRealRange; i++)
            if (writeInputObj[i])
            {
                if (!CheckInputFO(inputObjRef[i], "Range real"))
                    return false;
                currRangeOutput += static_cast<DO_Real*>(GetInPortData(inputObjRef[i]));
            }

        if (currRangeOutput.IsEmpty())
        {
            SetObjErrMsg("no reals selected for writing");
            return false;
        }

        return true;
    }

    void RealRangeOutputFO::CalcOutput(FOcalcType  calcType)
    {
        DoStatusChk();
    }


    void RealRangeOutputFO::AddRangeRun()
    {
        DoStatusChk();
        if (!StatusOK())
            GenAppInternalError("RealRangeOutputFO::CalcOutput");

        rangeOutputIndex = 0;
        SetIndexes();
        for (int i = 0; i < currRangeOutput.Size(); i++)
            SetNextRangeValue(currRangeOutput.GetRef(i).realVal);
    }



};

