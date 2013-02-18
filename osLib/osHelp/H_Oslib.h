///////////////////////////////////////////////////////////////////////////////////
//
// H_Oslib.h
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

#ifndef H_OSLIB_H
#define H_OSLIB_H

// IDH_OS_BASE                         7500

//  IDH_OS_DPO                                 7500
//  IDH_OS_LPO                                 7600
//  IDH_OS_PPO                                 7700
//  IDH_OS_UPO                                 7800

#define IDH_OS_DPO_BASICRESIDUAL               7500
#define IDH_OS_DPO_CALCCONFIDENCE              7501
#define IDH_OS_DPO_CALCCONFIDENCEGRID          7502
#define IDH_OS_DPO_CALCCONFIDENCETABLE         7503
#define IDH_OS_DPO_COMPOSITEFIT                7504
#define IDH_OS_DPO_JACOBTOTABLE                7505
#define IDH_OS_DPO_READOPTSIMRESULTS           7506
#define IDH_OS_DPO_READRANGESIMRESULTS         7507
#define IDH_OS_DPO_RESIDUALDIAGNOSTIC          7508
#define IDH_OS_DPO_RESIDUALHISTOGRAM           7509
#define IDH_OS_DPO_SELECTOPTCOVAR              7510
#define IDH_OS_DPO_SELECTOPTJACOB              7511
#define IDH_OS_DPO_SELECTOPTRESID              7512
#define IDH_OS_DPO_SELECTOPTRESULTS            7513
#define IDH_OS_DPO_SELECTRANGECUBE             7514
#define IDH_OS_DPO_SELECTRANGEGRID             7515
#define IDH_OS_DPO_SELECTRTRANGECUBE           7516
#define IDH_OS_DPO_SELECTRTRANGEGRID           7517
#define IDH_OS_DPO_SINGLEFIT                   7518

#define IDH_OS_LPO_COVARIANCE                  7600
#define IDH_OS_LPO_JACOBIAN                    7601
#define IDH_OS_LPO_OPTRUN                      7602

#define IDH_OS_PPO_COVARLIMITS                 7700
#define IDH_OS_PPO_COVARLIMMATRIX              7701
#define IDH_OS_PPO_COVARLIMSINGLE              7702
#define IDH_OS_PPO_TABLELABELS                 7703


#endif //! H_OSLIB_H

