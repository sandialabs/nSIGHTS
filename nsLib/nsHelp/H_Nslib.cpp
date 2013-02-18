///////////////////////////////////////////////////////////////////////////////////
//
// H_Nslib.cpp
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
#include "NsLibAfx.h"

#include <genApp/C_ObjHelp.h>

#include <nsHelp/H_Nslib.h>

static HelpMap nsHelpMap[] =

    {
    HelpMap("DPO_BarometricCompensation"             ,IDH_NS_DPO_BAROMETRICCOMPENSATION                        ),
    HelpMap("DPO_BasicSequenceFit"                   ,IDH_NS_DPO_BASICSEQUENCEFIT                              ),
    HelpMap("DPO_BasicTimeExtract"                   ,IDH_NS_DPO_BASICTIMEEXTRACT                              ),
    HelpMap("DPO_BEETCompensation"                   ,IDH_NS_DPO_BEETCOMPENSATION                              ),
    HelpMap("DPO_CreateBEETResponseFunction"         ,IDH_NS_DPO_CREATEBEETRESPONSEFUNCTION                              ),
    HelpMap("DPO_Derivative"                         ,IDH_NS_DPO_DERIVATIVE                                    ),
    HelpMap("DPO_ExtractSequenceInterval"            ,IDH_NS_DPO_EXTRACTSEQUENCEINTERVAL                       ),
    HelpMap("DPO_PulseNormalize"                     ,IDH_NS_DPO_PULSENORMALIZE                                ),
    HelpMap("DPO_ReadMiniTroll"                      ,IDH_NS_DPO_READMINITROLL                                 ),
    HelpMap("DPO_ReadProfileSimResults"              ,IDH_NS_DPO_READPROFILESIMRESULTS                         ),
    HelpMap("DPO_ReadSequenceTimes"                  ,IDH_NS_DPO_READSEQUENCETIMES                             ),
    HelpMap("DPO_ReadXYSimResults"                   ,IDH_NS_DPO_READXYSIMRESULTS                              ),
    HelpMap("DPO_SelectProfilefromProfileSimResults" ,IDH_NS_DPO_SELECTPROFILEFROMPROFILESIMRESULTS            ),
    HelpMap("DPO_SelectXYfromXYSimResults"           ,IDH_NS_DPO_SELECTXYFROMXYSIMRESULTS                      ),
    HelpMap("DPO_TimeProcess"                        ,IDH_NS_DPO_TIMEPROCESS                                   ),
    HelpMap("DPO_ExtendedProfileScaleTransform"      ,IDH_NS_DPO_EXTPROFILESCALETRAN                           ),

    HelpMap("PPO_SequenceGridLines"                  ,IDH_NS_PPO_SEQUENCEGRIDLINES                             ),

    HelpMap(NULL, -1)

    };

static ObjHelpC nsHelp(nsHelpMap);



