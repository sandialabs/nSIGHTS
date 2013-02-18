///////////////////////////////////////////////////////////////////////////////////
//
// H_Genlib.h
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

#ifndef H_GENLIB_H
#define H_GENLIB_H

// IDH_GEN_BASE                        5000
// IDH_GEN_DPO                         5100
// IDH_GEN_PPO                         5200
// IDH_GEN_PPD                         5300
// IDH_GEN_UPO                         5400

#define IDH_OBJECT_TREE                     5000

#define IDH_GEN_DPO_BLENDCOLORMAP           5100
#define IDH_GEN_DPO_DATAPG                  5101
#define IDH_GEN_DPO_ENTERCOLORMAP           5102
#define IDH_GEN_DPO_LINCOLORMAP             5103
#define IDH_GEN_DPO_MERGECOLORMAP           5104
#define IDH_GEN_DPO_PENSET                  5105
#define IDH_GEN_DPO_READCOLORMAP            5106

#define IDH_GEN_PPO_COLORDATALEGEND         5200
#define IDH_GEN_PPO_COLORLEGEND             5201
#define IDH_GEN_PPO_DATALABELS              5202
#define IDH_GEN_PPO_GRIDLINE                5203
#define IDH_GEN_PPO_SERIESLEGEND            5204
#define IDH_GEN_PPO_USERLABELS              5205
#define IDH_GEN_PPO_TIMEAXES                5206
#define IDH_GEN_PPO_LABELAXES               5207

#define IDH_GEN_PPD_2DPLOTANNO              5300
#define IDH_GEN_PPD_2DXY                    5301
#define IDH_GEN_PPD_3DAXESFORMAT            5302
#define IDH_GEN_PPD_3DAXESLABEL             5303
#define IDH_GEN_PPD_3DLIGHTING              5304
#define IDH_GEN_PPD_3DXYZ                   5305
#define IDH_GEN_PPD_LINLOGAXIS              5306
#define IDH_GEN_PPD_COMPLAYOUT              5307
#define IDH_GEN_PPD_COMPOSITE               5308

#define IDH_GEN_UPO_WRITECOLORMAP           5400

#endif // H_GENLIB_H

