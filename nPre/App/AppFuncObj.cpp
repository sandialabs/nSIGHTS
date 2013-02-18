///////////////////////////////////////////////////////////////////////////////////
//
// AppFuncObj.cpp
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
//      Performs application specific DO_XXX type conversions.
//
///////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <genClass/DO_Label.h>
#include <genClass/DO_Real.h>
#include <genClass/DO_Indx.h>
#include <genClass/DO_XYData.h>

#include <genClass/C_FuncObj.h>

    class NprePortCheck {
        private:
            bool reqLabel;
            bool reqReal;
            bool reqIndx;
            bool reqXY;

            const std::type_info& reqSig;

        public:
                    NprePortCheck(const std::type_info& rSig);
            bool    PortFound(const DataObjC* inPort);
    };

NprePortCheck::NprePortCheck(const std::type_info& rSig)
: reqSig(rSig)
{
    reqLabel         = (reqSig == typeid(DO_Label));
    reqReal          = (reqSig == typeid(DO_Real));
    reqIndx          = (reqSig == typeid(DO_Indx));
    reqXY            = (reqSig == typeid(DO_XYData));
}

bool NprePortCheck::PortFound(const DataObjC* inPort)
{
    const DataObjC* tmpDO = 0;
    if (typeid(*inPort) == reqSig)
    {
        tmpDO = inPort;
    }
    else if (reqLabel)
    {
        tmpDO = dynamic_cast <const DO_Label*> (inPort);
    }
    else if (reqReal)
    {
        tmpDO = dynamic_cast <const DO_Real*> (inPort);
    }
    else if (reqIndx)
    {
        tmpDO = dynamic_cast <const DO_Indx*> (inPort);
    }
    else if (reqXY)
    {
        tmpDO = dynamic_cast <const DO_XYData*> (inPort);
    }

    return (tmpDO != 0);
}

int  FuncObjC::AppHasOutPort(const DataObjArray&     objOutPorts,
                              const std::type_info&       reqSig)
{
    NprePortCheck portCheck(reqSig);
    int count = 0;
    for (int i = 0; i < objOutPorts.Size(); i++)
    {
        if (portCheck.PortFound(objOutPorts[i]))
            count++;
    }
    return count;
}

DataObjC*   FuncObjC::AppGetOutPortData(const DataObjArray&  objOutPorts,
                                          const std::type_info&  reqSig,
                                                int         indx)
{
    NprePortCheck portCheck(reqSig);
    int tcount = 0;
    for (int i = 0; i < objOutPorts.Size(); i++)
    {
        if (portCheck.PortFound(objOutPorts[i]))
        {
            if (tcount == indx)
                return objOutPorts[i];
            tcount++;
        }
    }
    return 0;
}


