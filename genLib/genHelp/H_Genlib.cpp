///////////////////////////////////////////////////////////////////////////////////
//
// H_Genlib.cpp
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
// DESCRIPTION: Help system mappings for genLib functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GenLibAfx.h"

#include <genApp/C_ObjHelp.h>
#include <genHelp/H_Genlib.h>

static HelpMap genHelpMap[] =

    {
    HelpMap("DPO_BlendColorMap",        IDH_GEN_DPO_BLENDCOLORMAP       ),
    HelpMap("DPO_DataPg",               IDH_GEN_DPO_DATAPG              ),
    HelpMap("DPO_EnterColorMap",        IDH_GEN_DPO_ENTERCOLORMAP       ),
    HelpMap("DPO_LinColorMap",          IDH_GEN_DPO_LINCOLORMAP         ),
    HelpMap("DPO_MergeColorMap",        IDH_GEN_DPO_MERGECOLORMAP       ),
    HelpMap("DPO_PenSet",               IDH_GEN_DPO_PENSET              ),
    HelpMap("DPO_ReadColorMap",         IDH_GEN_DPO_READCOLORMAP        ),

    HelpMap("PPO_ColorDataLegend",      IDH_GEN_PPO_COLORDATALEGEND     ),
    HelpMap("PPO_ColorLegend",          IDH_GEN_PPO_COLORLEGEND         ),
    HelpMap("PPO_DataLabels",           IDH_GEN_PPO_DATALABELS          ),
    HelpMap("PPO_GridLine",             IDH_GEN_PPO_GRIDLINE            ),
    HelpMap("PPO_SeriesLegend",         IDH_GEN_PPO_SERIESLEGEND        ),
    HelpMap("PPO_UserLabels",           IDH_GEN_PPO_USERLABELS          ),
    HelpMap("PPO_TimeAxes",             IDH_GEN_PPO_TIMEAXES            ),
    HelpMap("PPO_LabelAxes",            IDH_GEN_PPO_LABELAXES           ),

    HelpMap("PPD_2DPlotAnno",           IDH_GEN_PPD_2DPLOTANNO          ),
    HelpMap("PPD_2DXY",                 IDH_GEN_PPD_2DXY                ),
    HelpMap("PPD_3DAxesFormat",         IDH_GEN_PPD_3DAXESFORMAT        ),
    HelpMap("PPD_3DAxesLabel",          IDH_GEN_PPD_3DAXESLABEL         ),
    HelpMap("PPD_3DLighting",           IDH_GEN_PPD_3DLIGHTING          ),
    HelpMap("PPD_3DXYZ",                IDH_GEN_PPD_3DXYZ               ),
    HelpMap("PPD_LinLogAxis",           IDH_GEN_PPD_LINLOGAXIS          ),
    HelpMap("PPD_CompositeLayout",      IDH_GEN_PPD_COMPLAYOUT          ),
    HelpMap("PPD_Composite",            IDH_GEN_PPD_COMPOSITE           ),


    HelpMap("UPO_WriteColorMap",        IDH_GEN_UPO_WRITECOLORMAP       ),

    HelpMap(NULL, -1)

    };

static ObjHelpC genHelp(genHelpMap);

