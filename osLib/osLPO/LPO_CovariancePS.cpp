///////////////////////////////////////////////////////////////////////////////////
//
// LPO_CovariancePS.cpp
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
//      File I/O and user interface for all osLFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClass/U_Str.h>

#include <osClass/DO_CovarArray.h>

#include "LPO_Covariance.h"
#include "LPO_CovariancePS.h"

LPO_CovariancePS::LPO_CovariancePS() :
    covarObjRefUI(typeid(DO_CovarArray))
{
}


void LPO_CovariancePS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    covarObjRefUI.InitFramedUI("Covariance Data To List", URform.GetWidget(), args);
}


void LPO_CovariancePS::MenuOpen(LPO_Covariance&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    covarObjRefUI.StdMenuInit(inData.covarObjRef);
}


void LPO_CovariancePS::SetMenuData(LPO_Covariance&    inData)
{
    MenuBaseC::SetMenuData(inData);

    covarObjRefUI.SetFuncObjRef(inData.covarObjRef);
}

bool LPO_CovariancePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    covarObjRefUI.FuncObjRefOK(allOK, "Covariance data source");

    return allOK;
}

void LPO_CovariancePS::GetMenuData(LPO_Covariance&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.covarObjRef = covarObjRefUI.GetFuncObjRef();
}

