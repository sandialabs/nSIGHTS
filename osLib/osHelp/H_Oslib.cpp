///////////////////////////////////////////////////////////////////////////////////
//
// H_Oslib.cpp
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

#include "StdAfx.h"
#include "OsLibAfx.h"

#include <genApp/C_ObjHelp.h>
#include <osHelp/H_Oslib.h>

static HelpMap osHelpMap[] =

    {
    HelpMap("DPO_BasicResidual"         ,IDH_OS_DPO_BASICRESIDUAL                 ),
    HelpMap("DPO_CalcConfidence"        ,IDH_OS_DPO_CALCCONFIDENCE                ),
    HelpMap("DPO_CalcConfidenceGrid"    ,IDH_OS_DPO_CALCCONFIDENCEGRID            ),
    HelpMap("DPO_CalcConfidenceTable"   ,IDH_OS_DPO_CALCCONFIDENCETABLE           ),
    HelpMap("DPO_CompositeFit"          ,IDH_OS_DPO_COMPOSITEFIT                  ),
    HelpMap("DPO_JacobToTable"          ,IDH_OS_DPO_JACOBTOTABLE                  ),
    HelpMap("DPO_ReadOptSimResults"     ,IDH_OS_DPO_READOPTSIMRESULTS             ),
    HelpMap("DPO_ReadRangeSimResults"   ,IDH_OS_DPO_READRANGESIMRESULTS           ),
    HelpMap("DPO_ResidualDiagnostic"    ,IDH_OS_DPO_RESIDUALDIAGNOSTIC            ),
    HelpMap("DPO_ResidualHistogram"     ,IDH_OS_DPO_RESIDUALHISTOGRAM             ),

    HelpMap("DPO_SelectOptCovar"        ,IDH_OS_DPO_SELECTOPTCOVAR                ),
    HelpMap("DPO_SelectOptJacob"        ,IDH_OS_DPO_SELECTOPTJACOB                ),
    HelpMap("DPO_SelectOptResid"        ,IDH_OS_DPO_SELECTOPTRESID                ),
    HelpMap("DPO_SelectOptResults"      ,IDH_OS_DPO_SELECTOPTRESULTS              ),
    HelpMap("DPO_SelectRangeCube"       ,IDH_OS_DPO_SELECTRANGECUBE               ),
    HelpMap("DPO_SelectRangeGrid"       ,IDH_OS_DPO_SELECTRANGEGRID               ),
    HelpMap("DPO_SelectRTRangeCube"     ,IDH_OS_DPO_SELECTRTRANGECUBE             ),
    HelpMap("DPO_SelectRTRangeGrid"     ,IDH_OS_DPO_SELECTRTRANGEGRID             ),
    HelpMap("DPO_SingleFit"             ,IDH_OS_DPO_SINGLEFIT                     ),

    HelpMap("LPO_Covariance"         ,IDH_OS_LPO_COVARIANCE            ),
    HelpMap("LPO_Jacobian"           ,IDH_OS_LPO_JACOBIAN              ),
    HelpMap("LPO_OptRun"             ,IDH_OS_LPO_OPTRUN                ),

    HelpMap("PPO_CovarLimits"        ,IDH_OS_PPO_COVARLIMITS           ),
    HelpMap("PPO_CovarLimMatrix"     ,IDH_OS_PPO_COVARLIMMATRIX        ),
    HelpMap("PPO_SingleConfLimits"   ,IDH_OS_PPO_COVARLIMSINGLE        ),
    HelpMap("PPO_TableLabels"        ,IDH_OS_PPO_TABLELABELS           ),

    HelpMap(NULL, -1)

    };

static ObjHelpC osHelp(osHelpMap);



