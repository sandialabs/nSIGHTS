///////////////////////////////////////////////////////////////////////////////////
//
// E_Parameter.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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


#include "StdAfx.h"
#include <Var/E_Parameter.h>


namespace nsParameterEnums  {

const ParameterGroups formParGroup(pF_t, pF_k, pF_HK, pF_Ss, pF_C, pF_Phi, pF_Kf, pF_Sy, pF_kv, pF_HKv);
const ParameterGroups skinParGroup(-1, pS_k, pS_HK, pS_Ss, pS_C, pS_Phi, pS_Kf, pS_Sy, pS_k, pS_HK);

const ParameterGroups fracParGroup(-1, pFr_k, pFr_HK, pFr_Ss, pFr_C, pFr_Phi);
const ParameterGroups matParGroup(-1, pM_k, pM_HK, pM_Ss, pM_C, pM_Phi);

const ParameterGroups slParGroup(pL_t, pL_k, pL_HK, pL_Ss, pL_C, pL_Phi);
const ParameterGroups ulParGroup(pUL_t, pUL_k, pUL_HK, pUL_Ss, pUL_C, pUL_Phi);
const ParameterGroups llParGroup(pLL_t, pLL_k, pLL_HK, pLL_Ss, pLL_C, pLL_Phi);

ParameterGroups::ParameterGroups(int geoLayer)
{
  SetLayerParameter(geoLayer);
}

void ParameterGroups::SetLayerParameter(int geoLayer, bool isSkin)
{
  int baseIndex = GetLayerOffset(geoLayer);

  tIndx = -1;
  kIndx = baseIndex + pLay_k;
  hkIndx  = baseIndex + pLay_HK;
  ssIndx  = baseIndex + pLay_Ss;
  cIndx = baseIndex + pLay_C;
  phiIndx = baseIndex + pLay_Phi;
  kfIndx  = baseIndex + pLay_Kf;
  syIndx  = -1;
  kvIndx  = baseIndex + pLay_kV;
  hkvIndx = baseIndex + pLay_HKV;
}


};

