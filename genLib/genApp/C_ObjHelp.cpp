///////////////////////////////////////////////////////////////////////////////////
//
// C_ObjHelp.cpp
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              Maps help ID numbers to object names for use by Help system.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <appHelp.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include "C_ObjHelp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before alloc objects in app

T_SC_BasicPtrArray<ObjHelpC> ObjHelpC::objHelp(8);


ObjHelpC::ObjHelpC(HelpMap  inMapData[])
{
    objHelp += this;
    mapData = inMapData;
}


int  ObjHelpC::GetHelpIndex(const char* inID)
{
    for (int i = 0; i < objHelp.Size(); i++)
        {
            HelpMap* currMap = objHelp[i]->mapData;
            while (currMap->allocID != NULL)
                {
                    if (SameString(inID, currMap->allocID))
                        return currMap->helpIndex;
                    currMap++;
                }
        }
    /*

      char msgStr[80];
      MakeString(msgStr, "No help found for object: ", inID, 80);
      GenAppInfoMsg("HelpSystem", msgStr);
    */
    return IDH_NO_OBJECT_HELP;
}

