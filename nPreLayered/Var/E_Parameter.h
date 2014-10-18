///////////////////////////////////////////////////////////////////////////////////
//
// E_Parameter.h
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


#ifndef E_PARAMETER_H
#define E_PARAMETER_H

// defined seperate for uncomplicated use by other than G_Parameter

namespace nsParameterEnums  {

enum    Parameters  {
  // Formation
  pF_t            = 0,      // Thickness
  pF_n            = 1,      // Flow dimension
  pF_Pf           = 2,      // Static pressure
  pF_ro           = 3,      // Boundary radius
  pF_k            = 4,      // Permeability
  pF_HK           = 5,      // Hydraulic conductivity
  pF_Ss           = 6,      // Specific storage
  pF_C            = 7,      // Compressibility
  pF_Phi          = 8,      // Porosity
  pF_Kf           = 9,      // Klinkenburg factor

  // Fracture
  pFr_k           = 10,     // Permeability
  pFr_HK          = 11,     // Hydraulic conductivity
  pFr_Ss          = 12,     // Specific storage
  pFr_C           = 13,     // Compressibility
  pFr_Phi         = 14,     // Porosity

  // Matrix
  pM_k            = 15,     // Permeability
  pM_HK           = 16,     // Hydraulic conductivity
  pM_Ss           = 17,     // Specific storage
  pM_C            = 18,     // Compressibility
  pM_Phi          = 19,     // Porosity
  pM_V            = 20,     // Volume factor
  pM_Alpha        = 21,     // Alpha
  pM_t            = 22,     // slab thickness
  pM_d            = 23,     // sphere diameter

  // Skin zone
  pS_k            = 24,     // Permeability
  pS_HK           = 25,     // Hydraulic conductivity
  pS_Ss           = 26,     // Specific storage
  pS_C            = 27,     // Compressibility
  pS_Phi          = 28,     // Porosity
  pS_Kf           = 29,     // Klinkenburg factor
  pS_t            = 30,     // Thickness

  // Fluid
  pFl_rho         = 31,      // Density
  pFl_mu          = 32,      // Viscosity
  pFl_C           = 33,      // Compressibility
  pFl_TX          = 34,      // Thermal expansion

  //  Gas
  pG_MW           = 35,      // Molecular weight
  pG_Tref         = 36,      // Reference temperature
  pG_mu           = 37,      // Viscosity
  pG_mu_m         = 38,      // Viscosity slope factor
  pG_Patm         = 39,      // Atmospheric pressure

  //  Test zone
  pTZ_r_w         = 40,      // Radius
  pTZ_r_s         = 41,      // Tubing string radius
  pTZ_dV          = 42,      // Volume change from normal
  pTZ_C           = 43,      // Compressibility

  // single leaky
  pL_t            = 44,      // Thickness
  pL_k            = 45,      // Permeability
  pL_HK           = 46,      // Hydraulic conductivity
  pL_Ss           = 47,      // Specific storage
  pL_C            = 48,      // Compressibility
  pL_Phi          = 49,      // Porosity

  // lower leaky
  pLL_t           = 50,      // Thickness
  pLL_k           = 51,      // Permeability
  pLL_HK          = 52,      // Hydraulic conductivity
  pLL_Ss          = 53,      // Specific storage
  pLL_C           = 54,      // Compressibility
  pLL_Phi         = 55,      // Porosity

  // upper leaky
  pUL_t           = 56,      // Thickness
  pUL_k           = 57,      // Permeability
  pUL_HK          = 58,      // Hydraulic conductivity
  pUL_Ss          = 59,      // Specific storage
  pUL_C           = 60,      // Compressibility
  pUL_Phi         = 61,      // Porosity

  // numeric
  pN_n_r          = 62,      // # of radial nodes
  pN_n_m          = 63,      // # of matrix nodes
  pN_n_s          = 64,      // # of skin nodes
  pN_n_l          = 65,      // # of leaky
  pN_P_tol        = 66,      // Pressure solution tolerance
  pN_Qv_tol       = 67,      // STP flow solution tolerance
  pN_Qm_tol       = 68,      // Mass flow solution tolerance

  // new parameters added for unconfined
  pTZ_pbo     = 69,   //partial penetration bottom offset if partial middle
  pF_Sy     = 70,   //specific yield in formation
  pS_Sy     = 71,   //specific yield in skin zone
  pF_kv     = 72,   //vertical permeability
  pF_HKv      = 73,   //vertical hydraulic conductivity
  pN_n_v2Dw       = 74,       // number of 2D well nodes
  // was # of variable vertical nodes
  // for full penetration is total #
  pTZ_pl      = 75,   // partial penetration well length
  pN_n_v2Du       = 76,       // # of nodes above well for not top partial
  // was # of constant vertical nodes
  pN_n_v2Dl       = 77,       // # of nodes below well for not bottom partial
  pN_kVMult     = 78,       // # of nodes below well for not bottom partial

  // new parameters for horizontal anistropy
  pF_KxyRatio   = 79,
  pF_KxyAngle   = 80,

  // new parameters for solvers
  pN_tol      = 81,
  pN_maxIter    = 82,

  ParametersEnd
};


enum    {maxParameters  = 83};


enum    LayerParameters  {

  // layer basic
  pLay_k            = 0,      // permeability
  pLay_kV           = 1,      // vertical permeability
  pLay_HK           = 2,      // hydraulic conductivity
  pLay_HKV          = 3,      // vertical hydraulic conductivity
  pLay_Ss           = 4,      // Specific storage
  pLay_C            = 5,      // Compressibility
  pLay_Phi          = 6,      // Porosity
  pLay_Kf           = 7,      // Klinkenburg factor

  // layer skin
  pLayS_k            = 8,     // Permeability
  pLayS_HK           = 9,     // Hydraulic conductivity
  pLayS_Ss           = 10,    // Specific storage
  pLayS_C            = 11,    // Compressibility
  pLayS_Phi          = 12,     // Porosity
  pLayS_Kf           = 13,     // Klinkenburg factor

  LayerParametersEnd
};


enum    {maxLayerParameters  = 14};

static int  GetLayerOffset(int layerIndex) {
  return  maxParameters + maxLayerParameters * layerIndex;
}

class ParameterGroups {
public:
  int tIndx;
  int kIndx;
  int hkIndx;
  int ssIndx;
  int cIndx;
  int phiIndx;
  int kfIndx;
  int syIndx;
  int kvIndx;
  int hkvIndx;

  ParameterGroups(int t, int k, int hk, int ss, int c, int phi) :
    tIndx(t), kIndx(k), hkIndx(hk), ssIndx(ss), cIndx(c), phiIndx(phi),
    kfIndx(-1), syIndx(-1), kvIndx(-1), hkvIndx(-1) {};

  ParameterGroups(int t, int k, int hk, int ss, int c, int phi, int kf, int sy, int kv, int hkv) :
    tIndx(t), kIndx(k), hkIndx(hk), ssIndx(ss), cIndx(c), phiIndx(phi),
    kfIndx(kf), syIndx(sy), kvIndx(kv), hkvIndx(hkv) {};

  ParameterGroups() {};
  ParameterGroups(int geoLayer);

  void SetLayerParameter(int geoLayer, bool isSkin = false);


};

// for layer suppprt


extern const ParameterGroups formParGroup, skinParGroup, fracParGroup, matParGroup,
       slParGroup,  ulParGroup, llParGroup;

};


//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace nsParameterEnums;


#endif
