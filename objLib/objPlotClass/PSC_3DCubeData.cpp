///////////////////////////////////////////////////////////////////////////////////
//
// PSC_3DCubeData.cpp
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

#include <objClass/DO_CubeData.h>

#include <objPlotClass/PSC_3DCubeData.h>

PSC_3DCubeData :: PSC_3DCubeData()
{
}

PSC_3DCubeData::PSC_3DCubeData(const PSC_3DCubeData& a)
{
    LocalCopy(a);
}

PSC_3DCubeData :: ~PSC_3DCubeData ()
{
}

void PSC_3DCubeData::InitCommon(FuncObjC& inFO)
{
    inFO.AddInPort(cubeIndexesObjRef, typeid(DO_CubeIndxArray));
}

PSC_3DCubeData& PSC_3DCubeData::operator= (const PSC_3DCubeData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void  PSC_3DCubeData::LocalCopy(const PSC_3DCubeData& a)
{
    cubeIndexesObjRef  = a.cubeIndexesObjRef;
    coordMap           = a.coordMap;
}


void  PSC_3DCubeData::DoStatusChk(PlotObjC& inPO,
                                  DC_CubeData& currCubeData)
{
    if (!inPO.is3DObject)
        return;

    if (!inPO.CheckInputFO(cubeIndexesObjRef,  "Cube Indexes"))
        return;

    cubeIndexesDO = static_cast<DO_CubeIndxArray*>(inPO.GetInPortData(cubeIndexesObjRef));
    if (!currCubeData.IsCompatible(*(cubeIndexesDO->baseCube)))
    {
        inPO.SetObjErrMsg("cubes must be the same size");
        return;
    }

    if (!coordMap.SetupOK())
        inPO.SetObjErrMsg("one or more coord maps map to same");
}



