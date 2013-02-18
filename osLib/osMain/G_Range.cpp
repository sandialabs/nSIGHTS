///////////////////////////////////////////////////////////////////////////////////
//
// G_Range.cpp
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
//      support for multiple value runs.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <iostream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <osMain/G_Vary.h>

#include <osMain/G_Range.h>

namespace osRange  {


    RangeOutputFO::RangeOutputFO(const char* id) :
        GlobalFunc(id)
    {
        gridOutputDO.gridArray = &gridOutputDC;
        cubeOutputDO.cubeArray = &cubeOutputDC;
        AddOutPort(gridOutputDO);
        AddOutPort(cubeOutputDO);
        AddOutPort(rangeXDO);
        AddOutPort(rangeYDO);
        AddOutPort(rangeZDO);
        rangeXDO.Reset("Range X");
        rangeYDO.Reset("Range Y");
        rangeZDO.Reset("Range Z");

    }


    void RangeOutputFO::SetToInitial()
    {
        DeAllocDC();
        DoStatusChk();
    }

    bool RangeOutputFO::AllocSetup()
    {
        if (!vary.IsRange())
        {
            SetToInitial();
            return false;
        }
        vary.SetForFlags();
        doingCubes = orderedVary.Size() == 3;

        return true;
    }

    void RangeOutputFO::SetDCID(const char* id,
                                int&        nextID)
    {
        cubeOutputDC[nextID].SetID(id);
        gridOutputDC[nextID++].SetID(id);
    }


    bool RangeOutputFO::PreCalcSetup()
    {
        // get XY data
        DC_DataCoord xData, yData;
        if (!(orderedVary[0]->GetDataCoord(xData) && orderedVary[1]->GetDataCoord(yData)))
        {
            DeAllocDC();
            return false;
        }

        // and Z if necessary
        DC_DataCoord zData;
        if (doingCubes && (!orderedVary[2]->GetDataCoord(zData)))
        {
            DeAllocDC();
            return false;
        }

        rangeXDO.SetTypeLabel(orderedVary[0]->GetLongID());
        rangeYDO.SetTypeLabel(orderedVary[1]->GetLongID());

        char oldID[80];
        if (doingCubes)
        {
            rangeZDO.SetTypeLabel(orderedVary[2]->GetLongID());
            rangeZDO.InitLabelAndValue(0.0);

            for (int i = 0; i < cubeOutputDC.Size(); i++)
            {
                // save ID
                CopyString(oldID, cubeOutputDC[i].GetID(), 80);
                if (!cubeOutputDC[i].CreateFrom(xData, yData, zData))
                    return false;
                cubeOutputDC[i].SetID(oldID);
            }
        }
        else
        {
            rangeZDO.SetTypeLabel("unused");

            for (int i = 0; i < gridOutputDC.Size(); i++)
            {
                // save ID
                CopyString(oldID, gridOutputDC[i].GetID(), 80);
                if (!gridOutputDC[i].CreateFrom(xData, yData))
                    return false;
                gridOutputDC[i].SetID(oldID);
            }
        }
        return true;
    }



    void RangeOutputFO::DeAllocDC()
    {
        gridOutputDC.DeAlloc();
        cubeOutputDC.DeAlloc();
    }

    void RangeOutputFO::DoStatusChk()
    {
        FuncObjC::DoStatusChk();
    //  if (gridOutputDC.IsEmpty())
    //      SetObjErrMsg("range IDs not allocated ?");

    }

    void RangeOutputFO::SetNextRangeValue(const double& nextVal)
    {
        if (doingCubes)
        {
            cubeOutputDC[rangeOutputIndex].cData[xIndex][yIndex][zIndex] = nextVal;
        }
        else
        {
            gridOutputDC[rangeOutputIndex].gData[xIndex][yIndex] = nextVal;
        }
        rangeOutputIndex++;
    }


    void RangeOutputFO::SetIndexes()
    {
        xIndex = orderedVary[0]->varyIndex;
        yIndex = orderedVary[1]->varyIndex;
        rangeXDO.InitLabelAndValue(orderedVary[0]->GetFinalVaryVal());
        rangeYDO.InitLabelAndValue(orderedVary[1]->GetFinalVaryVal());
        if (doingCubes)
        {
            zIndex = orderedVary[2]->varyIndex;
            rangeZDO.InitLabelAndValue(orderedVary[2]->GetFinalVaryVal());
        }
    }



};

