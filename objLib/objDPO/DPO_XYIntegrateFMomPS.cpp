///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYIntegrateFMomPS.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClassPS/PS_UIBase.h>
#include "DPO_XYIntegrateFMom.h"
#include "DPO_XYIntegrateFMomPS.h"

DPO_XYIntegrateFMomPS::DPO_XYIntegrateFMomPS()
{
}

void DPO_XYIntegrateFMomPS::InitMenu()
{
    DPO_XYBasePS::InitMenu("XY Data to Analyze");

    ArgListC args;
    args.StdFormAttach();
    SetOutputDescUI(args);

    args.Clear();
    args.StdTopObjectAttach(outputDescUI);
    integrateValStatus.InitFramedUI("Absolute Curve Area", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(integrateValStatus);
    fmValStatus.InitFramedUI("First Moment", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(fmValStatus);
    mrtValStatus.InitFramedUI("Mean Residence Time", PPform.GetWidget(), args);

}

void DPO_XYIntegrateFMomPS::MenuOpen(DPO_XYIntegrateFMom& inData)
{
    DPO_XYBasePS::MenuOpen(inData);
}


void DPO_XYIntegrateFMomPS::SetMenuData(DPO_XYIntegrateFMom&  inData)
{
    DPO_XYBasePS::SetMenuData(inData, inData);
    if (inData.StatusOK()) {
        integrateValStatus.SetFieldValue(inData.integrateValDO.realVal);
        fmValStatus.SetFieldValue(inData.fmValDO.realVal);
        mrtValStatus.SetFieldValue(inData.mrtValDO.realVal);

    }
    else
        integrateValStatus.SetStatusNA();
}

bool DPO_XYIntegrateFMomPS::UIdataOK()
{
    DPO_XYBasePS::UIdataOK();
    return allOK;
}

void DPO_XYIntegrateFMomPS::GetMenuData(DPO_XYIntegrateFMom&  outData)
{
    DPO_XYBasePS::GetMenuData(outData, outData);
}

