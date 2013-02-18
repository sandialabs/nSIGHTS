///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ExtendedProfileColorBasePS.cpp
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
//      File I/O and user interface for all nsPFOGL functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#include "NsLibAfx.h"

#include <genPPO/PPO_Base.h>

#include <nsClass/DO_ExtendedProfile.h>

#include "PPO_ExtendedProfileColorBasePS.h"


PPO_ExtendedProfileColorBasePS::PPO_ExtendedProfileColorBasePS() :
    PPO_ColorBasePS(typeid(DO_ExtendedProfile))
{
}

void PPO_ExtendedProfileColorBasePS::InitMenu(const PPO_Base&   inData)
{
    const char* viewStrings[] = {"All data", "Grid in View"};

    ArgListC args;

    if (inData.basePO.is3DObject)
    {
        PPO_BasePS::InitMenu(inData);
        extendedProfileDataUI.StdInit(PPform.GetWidget(), inputDataObjRefUI, topRowForm);

        args.StdTopObjectAttach(extendedProfileDataUI.fixedZValueMSUI);

        PPO_ColorBasePS::InitMenu(inData, args, viewStrings, 2);
    }
    else
        PPO_ColorBasePS::InitMenu(inData, false, viewStrings, 2);
}

void PPO_ExtendedProfileColorBasePS::InitMapAndOffset(const PPO_Base&  inData,
                                                 ArgListC&  attachments)
{
    if (inData.basePO.is3DObject)
    {
        coordMapUI.StdInit(PPform.GetWidget(), attachments);

        ArgListC args;
        args.StdTopObjectAttach(coordMapUI.coordMapFrame);
        offsetCoordUI.StdInit("Offset", PPform.GetWidget(), args);
    }

}

void PPO_ExtendedProfileColorBasePS::MenuOpen(PFO_ExtendedProfileColorBase& inData)
{
    PPO_ColorBasePS::MenuOpen(inData);
    if (inData.is3DObject)
    {
        extendedProfileDataUI.MenuOpen(inData, inData);
    }
}

void PPO_ExtendedProfileColorBasePS::SetMenuData(PPO_Base&          inBase,
                                      PFO_ExtendedProfileColorBase& inData)
{
    PPO_ColorBasePS::SetMenuData(inBase, inData);

    if (inData.is3DObject)
    {
        extendedProfileDataUI.Set3DExtendedProfileData(inData);
        coordMapUI.Set3DCoordMap(inData.coordMap);
    }
}

bool PPO_ExtendedProfileColorBasePS::UIdataOK()
{
    PPO_ColorBasePS::UIdataOK();

    if (useOffsetCoord)                 // proxy for is 3D
    {
        extendedProfileDataUI.UIdataOK(allOK);
    }

    return allOK;
}

void PPO_ExtendedProfileColorBasePS::GetMenuData(PPO_Base&          outBase,
                                   PFO_ExtendedProfileColorBase& outData)
{
    PPO_ColorBasePS::GetMenuData(outBase, outData);
    if (outData.is3DObject)
    {
        extendedProfileDataUI.Get3DExtendedProfileData(outData);
        coordMapUI.Get3DCoordMap(outData.coordMap);
    }
}


