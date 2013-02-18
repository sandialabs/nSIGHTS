///////////////////////////////////////////////////////////////////////////////////
//
// G_CalcParameter.cpp
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
//      Calculates and lists parameter values
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>

#include <osMain/G_Vary.h>
#include <osClass/C_OptVar.h>

#include <nsClass/nSightConst.h>

#include <Var/G_Control.h>
#include <Var/G_Sequence.h>

#include <Var/C_Parameter.h>
#include <Var/G_Parameter.h>

#include <Var/G_CalcParameter.h>

namespace nsParameterGlobals  {



    ParameterCalculator::ParameterCalculator(UnitType pUnits,
                                            const char* pID)
        : parUnits(pUnits), parID(pID), poverrideFormat(0)
    {
        calcPars.SetResizable(10);
    }

    ParameterCalculator::ParameterCalculator(UnitType pUnits,
                                            const char* pID,
                                            const SC_RealFormat& rForm)
        : parUnits(pUnits), parID(pID), overrideFormat(rForm)
    {
        calcPars.SetResizable(10);
        poverrideFormat = &overrideFormat;
    }

    void ParameterCalculator::Override(UnitType pUnits, const char* pID,
                                             const SC_RealFormat& rForm)
    {
        parUnits.unitType = pUnits;
        parID = pID;
        overrideFormat = rForm;
        poverrideFormat = &overrideFormat;
    }

    void ParameterCalculator::Override(UnitType pUnits, const char* pID)
    {
        parUnits.unitType = pUnits;
        parID = pID;
    }



    bool ParameterCalculator::AllFixed()
    {
        for (int i = 0; i < calcPars.Size(); i++)
        {
            Parameter& currPar = *(allParam[calcPars[i]]);
            if (currPar.IsActive() && (!currPar.IsFixed()))
                return false;
        }
        return true;
    }


    bool ParameterCalculator::IsFP()
    {
        for (int i = 0; i < calcPars.Size(); i++)
        {
            Parameter& currPar = *(allParam[calcPars[i]]);
            if (currPar.IsActive() && currPar.IsNonLinear())
                return true;
        }
        return false;
    }

    bool ParameterCalculator::IsFR()
    {
        for (int i = 0; i < calcPars.Size(); i++)
        {
            Parameter& currPar = *(allParam[calcPars[i]]);
            if (currPar.IsActive() && currPar.IsRadial())
                return true;
        }
        return false;
    }

    bool ParameterCalculator::IsFT()
    {
        for (int i = 0; i < calcPars.Size(); i++)
        {
            Parameter& currPar = *(allParam[calcPars[i]]);
            if (currPar.IsActive() && currPar.IsTime())
                return true;
        }
        return false;
    }

    bool ParameterCalculator::IsMinMax()
    {
        for (int i = 0; i < calcPars.Size(); i++)
        {
            Parameter& currPar = *(allParam[calcPars[i]]);
            if (currPar.IsActive() && 
               (currPar.IsOpt() || currPar.IsSampled() ||
                (currPar.IsVary() && VaryControlStatics::IsRange())))
                return true;
        }
        return false;
    }


    int ParameterCalculator::GetnVary()
    {
        int nVary = 0;
        for (int i = 0; i < calcPars.Size(); i++)
        {
            Parameter& currPar = *(allParam[calcPars[i]]);
            if (currPar.IsActive() && currPar.IsVary())
                nVary++;
        }
        return nVary;
    }
    void ParameterCalculator::SetCalcVals(bool doVary)
    {
        for (int i = 0; i < calcPars.Size(); i++)
        {
            int parIndex = calcPars[i];

            Parameter& currPar = *(allParam[parIndex]);
            if (!currPar.IsActive())
                continue;

            if (currPar.IsFixed() || (doVary && currPar.IsVary() && VaryControlStatics::IsSuite()))
            {
                currCalcVals[parIndex] = currPar.GetParValue();
                minCalcVals[parIndex] = currCalcVals[parIndex];
                maxCalcVals[parIndex] = currCalcVals[parIndex];
            }
            else if (currPar.IsOpt())
            {
                minCalcVals[parIndex] = currPar.optParVal.GetMinVarValue();
                maxCalcVals[parIndex] = currPar.optParVal.GetMaxVarValue();
            }
            else if (currPar.IsSampled())
            {
                minCalcVals[parIndex] = currPar.sampParVal.GetMinVarValue();
                maxCalcVals[parIndex] = currPar.sampParVal.GetMaxVarValue();
            }
            else if (currPar.IsVary())
            {
                minCalcVals[parIndex] = currPar.varyParVal.GetMinVarValue();
                maxCalcVals[parIndex] = currPar.varyParVal.GetMaxVarValue();
            }
            else
                GenAppInternalError("ParameterCalculator::SetCalcVals()");

        }
    }


    void ParameterCalculator::SetMinToCurrent()
    {
        for (int i = 0; i < calcPars.Size(); i++)
        {
            int parIndex = calcPars[i];
            currCalcVals[parIndex] = minCalcVals[parIndex];
        }
    }

    void ParameterCalculator::SetMinToCurrent(Parameters par)
    {
        currCalcVals[par] = minCalcVals[par];
    }


    double  ParameterCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        return GetCalcVal();
    }

    void ParameterCalculator::SetMaxToCurrent()
    {
        for (int i = 0; i < calcPars.Size(); i++)
        {
            int parIndex = calcPars[i];
            currCalcVals[parIndex] = maxCalcVals[parIndex];
        }
    }
    void ParameterCalculator::SetMaxToCurrent(Parameters par)
    {
        currCalcVals[par] = maxCalcVals[par];
    }


    double  ParameterCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        return GetCalcVal();
    }


    void ParameterCalculator::GetUserString(char* calcStr, int strLen)
    {
        UnitReal temp(parUnits, GetCalcVal());
        temp.GetUserString(calcStr, strLen, poverrideFormat);
    }

    double ParameterCalculator::GetDensity()
    {
        if (NSUnitsBase::headFlag)
           return NSUnitsBase::headDensity.GetMetricVal();

        return currCalcVals[pFl_rho];
    }

    double ParameterCalculator::GetViscosity()
    {
        return currCalcVals[pFl_mu];   // default to metric unit conv
    }

    //  ********************************** conductivity calculator

    class KCalculator : public ParameterCalculator {
         public:
                            KCalculator(Parameters permParam);

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();

    };


    KCalculator::KCalculator(Parameters permParam) 
        : ParameterCalculator(uK, "Hydraulic conductivity")
    {
        AddPar(permParam);
        AddPar(pFl_rho);        // density 
        AddPar(pFl_mu);     // viscosity
    }

    double  KCalculator::GetCalcVal()
    {
        double kval =  currCalcVals[calcPars[0]] * gravConst * GetDensity() / GetViscosity();
        return kval;
    }

    double  KCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pFl_mu);
        return GetCalcVal();
    }

    double  KCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pFl_mu);
        return GetCalcVal();
    }

    // ********************* basic transmissivity (formation/skin)

    class TCalculator : public ParameterCalculator {
         public:
                            TCalculator(Parameters permParam, Parameters condParam); 

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();

    };

    TCalculator::TCalculator(Parameters permParam, Parameters condParam) 
        : ParameterCalculator(uTran, "Transmissivity")
    {
        AddPar(permParam);
        AddPar(condParam);
        AddPar(pFl_rho);        // density 
        AddPar(pFl_mu);     // viscosity
        AddPar(pF_t);       // thickness 
    }

    double  TCalculator::GetCalcVal()
    {
        double kval;
        if (enterPermeabilityData)
        {
            kval =  currCalcVals[calcPars[0]] * gravConst * GetDensity() / GetViscosity();
        }
        else
            kval = currCalcVals[calcPars[1]];

       return kval * currCalcVals[pF_t];
    }

    double  TCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pFl_mu);
        return GetCalcVal();
    }

    double  TCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pFl_mu);
        return GetCalcVal();
    }

    // ********************* perm thickness - gas 

    class KTCalculator : public ParameterCalculator {
         public:
                            KTCalculator(Parameters permParam); 

            virtual double  GetCalcVal();

    };

    KTCalculator::KTCalculator(Parameters permParam) 
        : ParameterCalculator(uVol, "Permeability Thickness")
    {
        AddPar(permParam);
        AddPar(pF_t);       // thickness 
    }

    double  KTCalculator::GetCalcVal()
    {
       return currCalcVals[calcPars[0]] * currCalcVals[pF_t];
    }



    // ********************* spec storage

    class SpecSCalculator : public ParameterCalculator {
         public:
                            SpecSCalculator(Parameters crParam, Parameters phiParam); 

            virtual double  GetCalcVal();

    };

    SpecSCalculator::SpecSCalculator(Parameters crParam, Parameters phiParam)
        : ParameterCalculator(uStor, "Specific storage")
    {
        AddPar(crParam);
        AddPar(phiParam);
        AddPar(pFl_rho);    // density 
        AddPar(pFl_C);      // C_w
    }

    double  SpecSCalculator::GetCalcVal()
    {
    //  s = p g (C_r + porosity * C_w)

        double sval =  gravConst * GetDensity() *(currCalcVals[calcPars[0]] + 
                            currCalcVals[calcPars[1]] * currCalcVals[pFl_C]);

        return sval;
    }


    // ********************* storage

    class SCalculator : public ParameterCalculator {
         public:
                            SCalculator(Parameters crParam, Parameters phiParam, Parameters ssParam); 

            virtual double  GetCalcVal();

    };

    SCalculator::SCalculator(Parameters crParam, Parameters phiParam, Parameters ssParam)
        : ParameterCalculator(uNone, "Storativity", SC_SciFormat(5))
    {
        AddPar(crParam);
        AddPar(phiParam);
        AddPar(ssParam);
        AddPar(pFl_rho);    // density 
        AddPar(pFl_C);      // C_w
        AddPar(pF_t);       // thickness 
    }

    double  SCalculator::GetCalcVal()
    {
    //  s = t p g (C_r + porosity * C_w)
        double sval;
        if (enterSpecificStorageData)
        {
            sval =  currCalcVals[calcPars[2]];

        }
        else
        {
            sval =  gravConst * GetDensity() *(currCalcVals[calcPars[0]] + 
                            currCalcVals[calcPars[1]] * currCalcVals[pFl_C]);
        }

        return sval * currCalcVals[pF_t];
    }


    //  ********************************** diffusivity calculator

    class DCalculator : public ParameterCalculator {
         public:
                            DCalculator(Parameters permParam, Parameters condParam, Parameters crParam, Parameters phiParam, Parameters ssParam);

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();

    };


    DCalculator::DCalculator(Parameters permParam, Parameters condParam, Parameters crParam, Parameters phiParam, Parameters ssParam)
        : ParameterCalculator(uTran, "Diffusivity")
    {
        AddPar(permParam);
        AddPar(condParam);
        AddPar(crParam);
        AddPar(phiParam);
        AddPar(ssParam);

        AddPar(pFl_rho);    // density 
        AddPar(pFl_mu);     // viscosity
        AddPar(pFl_C);      // C_w
    }

    double  DCalculator::GetCalcVal()
    {
        double kval;
        if (enterPermeabilityData)
        {
            kval =  currCalcVals[calcPars[0]] * gravConst * GetDensity() / GetViscosity();
        }
        else
            kval = currCalcVals[calcPars[1]];

        double sval;
        if (enterSpecificStorageData)
        {
            sval =  currCalcVals[calcPars[4]];

        }
        else
        {
            sval =  gravConst * GetDensity() * (currCalcVals[calcPars[2]] + 
                            currCalcVals[calcPars[3]] * currCalcVals[pFl_C]);
        }
        
        
        return kval / sval;

    }

    double  DCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pFl_mu);
        SetMaxToCurrent(pFl_C);
        SetMaxToCurrent(pFl_rho);
        SetMaxToCurrent(calcPars[4]);

        return GetCalcVal();
    }

    double  DCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pFl_mu);
        SetMinToCurrent(pFl_C);
        SetMinToCurrent(pFl_rho);
        SetMinToCurrent(calcPars[4]);

        return GetCalcVal();
    }


    //  ********************************** TZ calcs wellbore storage

    class TZVolCalculator : public ParameterCalculator {
         public:
                            TZVolCalculator();

            virtual double  GetCalcVal();

    };


    TZVolCalculator::TZVolCalculator()
        : ParameterCalculator(uVol, "Test-zone volume")
    {
        AddPar(pF_t);
        AddPar(pTZ_r_w);
        AddPar(pTZ_dV);
    }


    double  TZVolCalculator::GetCalcVal()
    {
    //     V = pi r_w^2 b + V_extra

        return (pi * Sqr(currCalcVals[pTZ_r_w])* currCalcVals[pF_t] + currCalcVals[pTZ_dV]);
    }


    class CIsolatedCalculator : public ParameterCalculator {
         public:
                            CIsolatedCalculator();

            virtual double  GetCalcVal();

    };


    CIsolatedCalculator::CIsolatedCalculator()
        : ParameterCalculator(uWbor, "Isolated well-bore storage", SC_SciFormat(5))
    {
        AddPar(pF_t);
        AddPar(pTZ_r_w);
        AddPar(pTZ_dV);
        AddPar(pTZ_C);
    }


    double  CIsolatedCalculator::GetCalcVal()
    {
    //     V = pi r_w^2 b + V_extra
    //     C = V C_tz
        double volume = (pi * Sqr(currCalcVals[pTZ_r_w])* currCalcVals[pF_t] + currCalcVals[pTZ_dV]);
        return currCalcVals[pTZ_C] * volume; 
    }


    class COpenHoleCalculator : public ParameterCalculator {
         public:
                            COpenHoleCalculator();

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();

    };


    COpenHoleCalculator::COpenHoleCalculator()
        : ParameterCalculator(uWbor, "Open hole well-bore storage", SC_SciFormat(5))
    {
        AddPar(pTZ_r_s);
        AddPar(pFl_rho);    // density 
    }


    double COpenHoleCalculator::GetCalcVal()
    {

    //       C = pi r_c^2 / (pg)

        return pi * Sqr(currCalcVals[pTZ_r_s]) / (gravConst * GetDensity());
    }

    double  COpenHoleCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pFl_rho);
        return GetCalcVal();
    }

    double  COpenHoleCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pFl_rho);
        return GetCalcVal();
    }


    //  ********************************** geometry calcs

    class DeltaACalculator : public ParameterCalculator {
        protected:      
            double  insideRadius;

         public:
                            DeltaACalculator();

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();


    };


    DeltaACalculator::DeltaACalculator()
        : ParameterCalculator(uNone, "Grid increment delta", SC_DecFormat(5))
    {
        AddPar(pTZ_r_w);
        AddPar(pF_ro);
        AddPar(pN_n_r);

        AddPar(pS_t);
        AddPar(pN_n_s); 
    }


    double DeltaACalculator::GetCalcVal()
    {
    /*
        {    if skin flag then
              inside_radius = r_w + t_s
              n_nodes = n_radial - n_skin
         else
              inside_radius = r_w
              n_nodes = n_radial

         grid_inc = LN( outside_radius / inside_radius ) / (n_nodes - 1)
    */

        insideRadius = currCalcVals[pTZ_r_w];
        double nNodes = currCalcVals[pN_n_r];
        if (control.HasSkin())
        {
            insideRadius += currCalcVals[pS_t];
            nNodes -= currCalcVals[pN_n_s];
        }
        
        return log(currCalcVals[pF_ro] / insideRadius) / (nNodes - 1.0);
    }

    double  DeltaACalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pN_n_r);
        SetMaxToCurrent(pTZ_r_w);
        SetMaxToCurrent(pS_t);
        return GetCalcVal();
    }

    double  DeltaACalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pN_n_r);
        SetMinToCurrent(pTZ_r_w);
        SetMinToCurrent(pS_t);

        return GetCalcVal();
    }


    class FirstIncCalculator : public DeltaACalculator {
         public:
                            FirstIncCalculator();

            virtual double  GetCalcVal();
    };


    FirstIncCalculator::FirstIncCalculator()
    {
        Override(uDist, "First grid increment", SC_SciFormat(5));
    }


    double FirstIncCalculator::GetCalcVal()
    {

        
        double deltaA = DeltaACalculator::GetCalcVal();

        return insideRadius * (exp(deltaA) - 1.0);
    }


    class NNodeCalculator : public ParameterCalculator {

         public:
                            NNodeCalculator();

            virtual double  GetCalcVal();


    };


    NNodeCalculator::NNodeCalculator()
        : ParameterCalculator(uNone, "Number of nodes", SC_DecFormat(0))
    {
        AddPar(pN_n_r);
        AddPar(pN_n_s); 
        AddPar(pN_n_m);
        AddPar(pN_n_l);
        AddPar(pN_n_v2Dw);
        AddPar(pN_n_v2Du);
        AddPar(pN_n_v2Dl);
    }


    double NNodeCalculator::GetCalcVal()
    {
    /*
        {    if skin flag then
              inside_radius = r_w + t_s
              n_nodes = n_radial - n_skin
         else
              inside_radius = r_w
              n_nodes = n_radial

         grid_inc = LN( outside_radius / inside_radius ) / (n_nodes - 1)
    */
        double nSCRadial = currCalcVals[pN_n_r];
        double nSCVert = 0.0;

        if (control.Is1DRadial())
        {
            double nskinNodes = 0.0;
            if (control.HasSkin())
            {
                nskinNodes = currCalcVals[pN_n_s];
                nSCRadial -= nskinNodes;
            }


            if (control.IsDual())
                nSCVert += currCalcVals[pN_n_m];

            if (control.IsLeaky())
            {
                double nleaky = currCalcVals[pN_n_l];
                nSCVert += nleaky;
                if (leakageType != ltSingle)
                    nSCVert += nleaky;
            }
        
            return nskinNodes + (nSCRadial - 1.0) * nSCVert;
        }

        nSCVert = currCalcVals[pN_n_v2Dw];
        if (control.IsPartialPenetration())
        {
            if (control.IsTopPartialPenetration())
                nSCVert += currCalcVals[pN_n_v2Dl];
            else if (control.IsMiddlePartialPenetration())
            {
                nSCVert += currCalcVals[pN_n_v2Dl];
                nSCVert += currCalcVals[pN_n_v2Du]; 
            }
            else
                nSCVert += currCalcVals[pN_n_v2Du]; 
        }
        return nSCRadial * nSCVert;
    }


    class FractureThickCalculator : public ParameterCalculator {

         public:
                            FractureThickCalculator();

            virtual double  GetCalcVal();


    };


    FractureThickCalculator::FractureThickCalculator()
        : ParameterCalculator(uDist, "Fracture thickness")
    {
        AddPar(pF_t);
        AddPar(pM_V);   
    }


    double FractureThickCalculator::GetCalcVal()
    {
        //   t_f = b * (1.0 - V_m)
        return currCalcVals[pF_t] * (1.0 - currCalcVals[pM_V]);
    }


    class MatrixThickCalculator: public ParameterCalculator {

         public:
                            MatrixThickCalculator();

            virtual double  GetCalcVal();


    };


    MatrixThickCalculator::MatrixThickCalculator()
        : ParameterCalculator(uDist, "Matrix thickness")
    {
        AddPar(pF_t);
        AddPar(pM_V);   
    }


    double MatrixThickCalculator::GetCalcVal()
    {
        //   t_f = b *  V_m
        return currCalcVals[pF_t] * currCalcVals[pM_V];
    }



    class AlphaCalculator : public ParameterCalculator {

         public:
                            AlphaCalculator();

            virtual double  GetCalcVal();


    };


    AlphaCalculator::AlphaCalculator()
        : ParameterCalculator(uGeom, "Alpha -- shape factor")
    {
        AddPar(pM_t);
        AddPar(pM_d);
    }


    double AlphaCalculator::GetCalcVal()
    {
        if (dualGeometryIsPrismatic)
            return 12.0 / Sqr(currCalcVals[pM_t]);
            
        return 15.0 / Sqr(currCalcVals[pM_d] / 2.0);
    }


    class LambdaCalculator : public AlphaCalculator {
         public:
                            LambdaCalculator();

            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();

        protected:
            double  GetAlpha();
    };


    LambdaCalculator::LambdaCalculator()
    {
        AddPar(pM_Alpha);

        AddPar(pM_k);
        AddPar(pM_HK);

        AddPar(pFr_k);
        AddPar(pFr_HK);

        AddPar(pTZ_r_w);
    }

    double  LambdaCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pFr_k);
        SetMaxToCurrent(pFr_HK);
        return GetCalcVal();
    }

    double  LambdaCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pFr_k);
        SetMinToCurrent(pFr_HK);
        return GetCalcVal();
    }

    double  LambdaCalculator::GetAlpha()
    {
        if (enterDualAlpha)
            return currCalcVals[pM_Alpha]; 

        return AlphaCalculator::GetCalcVal();
    }



    class NsightLambdaCalculator : public LambdaCalculator {
         public:
                            NsightLambdaCalculator();

            virtual double  GetCalcVal();
    };

    NsightLambdaCalculator::NsightLambdaCalculator()
    {
        Override(uNone, "nSights Lambda - interporosity flow", SC_SciFormat(5));
    }


    double NsightLambdaCalculator::GetCalcVal()
    {
    //  lambda = alpha k_m / k_f * r_w^2

        double kratio;
        if (enterPermeabilityData)
            kratio = currCalcVals[pM_k] / currCalcVals[pFr_k];
        else
            kratio = currCalcVals[pM_HK] / currCalcVals[pFr_HK];

        return GetAlpha() * kratio * Sqr(currCalcVals[pTZ_r_w]);
    }

    class InterpretLambdaCalculator : public LambdaCalculator {
         public:
                            InterpretLambdaCalculator();

            virtual double  GetCalcVal();
    };

    InterpretLambdaCalculator::InterpretLambdaCalculator()
    {
        // only using anaytic for now 
        Override(uNone, "Lambda - interporosity flow", SC_SciFormat(5));
        AddPar(pM_V);
    }


    double InterpretLambdaCalculator::GetCalcVal()
    {
    //  lambda = alpha k_m / k_f * r_w^2

        double kratio;
        if (enterPermeabilityData)
            kratio = currCalcVals[pM_k] / currCalcVals[pFr_k];
        else
            kratio = currCalcVals[pM_HK] / currCalcVals[pFr_HK];

        kratio /= (1.0 - currCalcVals[pM_V]);

        return GetAlpha() * kratio * Sqr(currCalcVals[pTZ_r_w]);
    }


    class OmegaCalculator : public ParameterCalculator {
         public:
                            OmegaCalculator();

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();
    };


    OmegaCalculator::OmegaCalculator()
        : ParameterCalculator(uNone, "Omega - storativity ratio", SC_SciFormat(5))
    {
        AddPar(pM_Ss);
        AddPar(pM_C);
        AddPar(pM_Phi);
        AddPar(pM_V);

        AddPar(pFr_Ss);
        AddPar(pFr_C);
        AddPar(pFr_Phi);

        AddPar(pFl_C);
    }


    double OmegaCalculator::GetCalcVal()
    {
    /*
         if spec_store_flag then
              frac_term =  s_f
              mat_term =   s_m
         else
              frac_term = C_f + porosity_f * C_water
              mat_term = C_m + porosity_m * C_water
         frac_term = frac_term * b * (r_1 - V_m)
         mat_term = mat_term * b *  V_m)

         omega = frac_term / (frac_term + mat_term)
    */
        double fracTerm, matTerm;

        if (enterSpecificStorageData)
        {
            fracTerm = currCalcVals[pFr_Ss];
            matTerm = currCalcVals[pM_Ss];
        }
        else
        {
            fracTerm = currCalcVals[pFr_C] + currCalcVals[pFr_Phi] * currCalcVals[pFl_C];
            matTerm = currCalcVals[pM_C] + currCalcVals[pM_Phi] * currCalcVals[pFl_C];
        }

        fracTerm *= (1.0 - currCalcVals[pM_V]);
        matTerm *= currCalcVals[pM_V];

        return fracTerm / (fracTerm + matTerm);
    }

    double  OmegaCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pM_Ss);
        SetMaxToCurrent(pM_Phi);
        SetMaxToCurrent(pM_C);

        return GetCalcVal();
    }

    double  OmegaCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pM_Ss);
        SetMinToCurrent(pM_Phi);
        SetMinToCurrent(pM_C);
        return GetCalcVal();
    }


    class NsightBetaCalculator : public NsightLambdaCalculator {
         public:
                            NsightBetaCalculator();

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();

    };


    NsightBetaCalculator::NsightBetaCalculator()
    {
        Override(uNone, "nSights Beta - transient parameter");

        AddPar(pM_Ss);
        AddPar(pM_C);
        AddPar(pM_Phi);
        AddPar(pM_V);

        AddPar(pFr_Ss);
        AddPar(pFr_C);
        AddPar(pFr_Phi);

        AddPar(pFl_C);
    }


    double NsightBetaCalculator::GetCalcVal()
    {
    /*
         if spec_store_flag then
              frac_term =  s_f
              mat_term =   s_m
         else
              frac_term = C_f + porosity_f * C_water
              mat_term = C_m + porosity_m * C_water

         frac_term = frac_term * b * (r_1 - V_m)
         mat_term = mat_term * b *  V_m)

         omega = frac_term / (frac_term + mat_term)
         lambda = alpha k_m / k_f * r_w^2
         beta = lambda / omega
    */


        double fracTerm, matTerm;

        if (enterSpecificStorageData)
        {
            fracTerm = currCalcVals[pFr_Ss];
            matTerm = currCalcVals[pM_Ss];
        }
        else
        {
            fracTerm = currCalcVals[pFr_C] + currCalcVals[pFr_Phi] * currCalcVals[pFl_C];
            matTerm = currCalcVals[pM_C] + currCalcVals[pM_Phi] * currCalcVals[pFl_C];
        }

        fracTerm *= (1.0 - currCalcVals[pM_V]);
        matTerm *= currCalcVals[pM_V];

        double omega = fracTerm / (fracTerm + matTerm);
        double lambda = NsightLambdaCalculator::GetCalcVal();

        return lambda / omega;
    }

    double  NsightBetaCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pFr_k);
        SetMaxToCurrent(pFr_HK);
        SetMaxToCurrent(pFr_Ss);
        SetMaxToCurrent(pFr_Phi);
        SetMaxToCurrent(pFr_C);
        return GetCalcVal();
    }

    double  NsightBetaCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pFr_k);
        SetMinToCurrent(pFr_HK);
        SetMinToCurrent(pFr_Ss);
        SetMinToCurrent(pFr_Phi);
        SetMinToCurrent(pFr_C);
        return GetCalcVal();
    }

    class InterpretBetaCalculator : public InterpretLambdaCalculator {
         public:
                            InterpretBetaCalculator();

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();

    };


    InterpretBetaCalculator::InterpretBetaCalculator()
    {
        // only using interpret
        Override(uNone, "Beta - transient parameter");

        AddPar(pM_Ss);
        AddPar(pM_C);
        AddPar(pM_Phi);
        AddPar(pM_V);

        AddPar(pFr_Ss);
        AddPar(pFr_C);
        AddPar(pFr_Phi);

        AddPar(pFl_C);
    }


    double InterpretBetaCalculator::GetCalcVal()
    {
    /*
         if spec_store_flag then
              frac_term =  s_f
              mat_term =   s_m
         else
              frac_term = C_f + porosity_f * C_water
              mat_term = C_m + porosity_m * C_water

         frac_term = frac_term * b * (r_1 - V_m)
         mat_term = mat_term * b *  V_m)

         omega = frac_term / (frac_term + mat_term)
         lambda = alpha k_m / k_f * r_w^2
         beta = lambda / omega
    */


        double fracTerm, matTerm;

        if (enterSpecificStorageData)
        {
            fracTerm = currCalcVals[pFr_Ss];
            matTerm = currCalcVals[pM_Ss];
        }
        else
        {
            fracTerm = currCalcVals[pFr_C] + currCalcVals[pFr_Phi] * currCalcVals[pFl_C];
            matTerm = currCalcVals[pM_C] + currCalcVals[pM_Phi] * currCalcVals[pFl_C];
        }

        fracTerm *= (1.0 - currCalcVals[pM_V]);
        matTerm *= currCalcVals[pM_V];

        double omega = fracTerm / (fracTerm + matTerm);
        double lambda = InterpretLambdaCalculator::GetCalcVal();

        return lambda / omega;
    }

    double  InterpretBetaCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pFr_k);
        SetMaxToCurrent(pFr_HK);
        SetMaxToCurrent(pFr_Ss);
        SetMaxToCurrent(pFr_Phi);
        SetMaxToCurrent(pFr_C);
        return GetCalcVal();
    }

    double  InterpretBetaCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pFr_k);
        SetMinToCurrent(pFr_HK);
        SetMinToCurrent(pFr_Ss);
        SetMinToCurrent(pFr_Phi);
        SetMinToCurrent(pFr_C);
        return GetCalcVal();
    }


    // *******************************************  skin related

    class SFactorCalculator : public ParameterCalculator {
         public:
                            SFactorCalculator();

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();


    };


    SFactorCalculator::SFactorCalculator()
        : ParameterCalculator(uNone, "Skin factor", SC_SciFormat(5))
    {
        AddPar(pS_k);
        AddPar(pS_HK);

        AddPar(pFr_k);
        AddPar(pFr_HK);

        AddPar(pF_k);
        AddPar(pF_HK);

        AddPar(pTZ_r_w);
        AddPar(pS_t);
    }



    double SFactorCalculator::GetCalcVal()
    {

    //    s_factor = (k_f / k_s - 1) * LOG ((r_w + t_s)/ r_w)


        double fk, sk;

        if (enterPermeabilityData)
        {
            sk = currCalcVals[pS_k];
            if (control.IsDual())
                fk = currCalcVals[pFr_k];
            else
                fk = currCalcVals[pF_k];
        }
        else
        {
            sk = currCalcVals[pS_HK];
            if (control.IsDual())
                fk = currCalcVals[pFr_HK];
            else
                fk = currCalcVals[pF_HK];
        }

        double rw = currCalcVals[pTZ_r_w];

        return (fk / sk - 1.0) * log((rw + currCalcVals[pS_t]) / rw);
    }

    double  SFactorCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pTZ_r_w);
        SetMaxToCurrent(pS_HK);
        SetMaxToCurrent(pS_k);
        return GetCalcVal();
    }

    double  SFactorCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pTZ_r_w);
        SetMinToCurrent(pS_HK);
        SetMinToCurrent(pS_k);
        return GetCalcVal();
    }


    class SkinDeltaACalculator : public ParameterCalculator {
        protected:      
            double  rw, ro;

         public:
                            SkinDeltaACalculator();

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();
    };


    SkinDeltaACalculator::SkinDeltaACalculator()
        : ParameterCalculator(uNone, "Skin grid increment delta", SC_DecFormat(5))
    {
        AddPar(pTZ_r_w);
        AddPar(pS_t);
        AddPar(pN_n_s);
    }


    double SkinDeltaACalculator::GetCalcVal()
    {
    //    delta_a = LN((t_s + r_w)/r_w) / (n_s - 1)    /
        
        rw = currCalcVals[pTZ_r_w];
        ro = currCalcVals[pS_t] + rw;

        return log(ro / rw) / (currCalcVals[pN_n_s]  - 1.0);
    }

    double  SkinDeltaACalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pN_n_s);
        SetMaxToCurrent(pTZ_r_w);
        return GetCalcVal();
    }

    double  SkinDeltaACalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pN_n_s);
        SetMinToCurrent(pTZ_r_w);

        return GetCalcVal();
    }


    class SkinFirstIncCalculator : public SkinDeltaACalculator {
         public:
                            SkinFirstIncCalculator();

            virtual double  GetCalcVal();
    };


    SkinFirstIncCalculator::SkinFirstIncCalculator()
    {
        Override(uDist, "Skin first grid increment", SC_SciFormat(5));
    }


    double SkinFirstIncCalculator::GetCalcVal()
    {

        double deltaA = SkinDeltaACalculator::GetCalcVal();

        return rw * (exp(deltaA) - 1.0);
    }


    class SkinLastIncCalculator : public SkinDeltaACalculator {
         public:
                            SkinLastIncCalculator();

            virtual double  GetCalcVal();
    };


    SkinLastIncCalculator::SkinLastIncCalculator()
    {
        Override(uDist, "Skin last grid increment", SC_SciFormat(5));
    }


    double SkinLastIncCalculator::GetCalcVal()
    {
    //  skin_radius * (r_1 - EXP(-delta_a));

        double deltaA = SkinDeltaACalculator::GetCalcVal();

        return ro * (1.0 - exp(-deltaA));
    }




    class IncRatioCalculator : public SkinLastIncCalculator{

         public:
                            IncRatioCalculator();

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();

    };


    IncRatioCalculator::IncRatioCalculator()
    {
        Override(uNone, "Increment ratio", SC_SciFormat(5));

        AddPar(pF_ro);
        AddPar(pN_n_r);

    }


    double IncRatioCalculator::GetCalcVal()
    {
    /*   r_s        = t_s + r_w
         delta_a_s  = LN(r_s/r_w) / (n_s - 1)
         skin_last  = r_s * (1.0 - EXP(- delta_a_s))
         delta_a_f  = LN(r_o/r_s) / (n_radial - n_s - 1)
         frac_first = r_s * (EXP(delta_a_f) - 1.0);
         inc_ratio  = frac_first / skin_last
    */

        double skinLast = SkinLastIncCalculator::GetCalcVal();

        double deltaAFrac = log(currCalcVals[pF_ro] / ro) / (currCalcVals[pN_n_r] - currCalcVals[pN_n_s] - 1.0);

        double  fracFirst = ro * (exp(deltaAFrac) - 1.0);

        return fracFirst / skinLast;
    }


    double  IncRatioCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pN_n_r);
        SetMaxToCurrent(pS_t);
        return GetCalcVal();
    }

    double  IncRatioCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pN_n_r);
        SetMinToCurrent(pS_t);
        return GetCalcVal();
    }


    // *******************************************  leakage related

    class LFactorCalculator : public ParameterCalculator {
         public:
                            LFactorCalculator();

            virtual double  GetCalcVal();
            virtual double  GetMinCalcVal();
            virtual double  GetMaxCalcVal();

    };


    LFactorCalculator::LFactorCalculator()
        : ParameterCalculator(uDist, "Leakage factor", SC_SciFormat(5))
    {
        AddPar(pL_k);
        AddPar(pL_HK);
        AddPar(pL_t);

        AddPar(pFr_k);
        AddPar(pFr_HK);

        AddPar(pF_k);
        AddPar(pF_HK);

        AddPar(pF_t);
    }



    double LFactorCalculator::GetCalcVal()
    {

    //    l_factor = SQRT(K_f * F_t  * L_t / K_l)
        double fk, lk;

        if (enterPermeabilityData)
        {
            lk = currCalcVals[pL_k];
            fk = currCalcVals[pF_k];
        }
        else
        {
            lk = currCalcVals[pL_HK];
            fk = currCalcVals[pF_HK];
        }

        double b = currCalcVals[pF_t];
        double bl = currCalcVals[pL_t];

        return sqrt(fk * b * bl / lk);
    }

    double  LFactorCalculator::GetMinCalcVal()
    {
        SetMinToCurrent();
        SetMaxToCurrent(pL_HK);
        return GetCalcVal();
    }

    double  LFactorCalculator::GetMaxCalcVal()
    {
        SetMaxToCurrent();
        SetMinToCurrent(pL_HK);
        return GetCalcVal();
    }


    CalcParameterListing::CalcParameterListing()
        :OSListObj("nPreCalcParameters")
    {
    }



    void CalcParameterListing::ListGasCalc()
    {
        {
            KTCalculator fKT(pF_k);
            ListCalcPar(fKT);
        }

        if (control.HasSkin())
        {
            {
                SFactorCalculator sC;
                ListCalcPar(sC);
            }
        }   

        {
            SubTitle("Test Zone");
            TZVolCalculator volC;
            ListCalcPar(volC);
        }

    }


    bool CalcParameterListing::ListHKS(const char* subTitle,
                                       const ParameterGroups& group)
    {
        if ((!enterPermeabilityData) && enterSpecificStorageData)
            return false;

        SubTitle(subTitle);

        if (enterPermeabilityData)
        {
            KCalculator fK(Parameters(group.kIndx));
            ListCalcPar(fK);
        }
            
        if (!enterSpecificStorageData)
        {
            SpecSCalculator fSs(Parameters(group.cIndx), Parameters(group.phiIndx));
            ListCalcPar(fSs);
        }

        return true;
    }



    void CalcParameterListing::ListFluidCalc()
    {
        if (control.IsSingle())
        {
            SubTitle("Formation");
            if (enterPermeabilityData)
            {
                KCalculator fK(pF_k);
                ListCalcPar(fK);
            }

            TCalculator fT(pF_k, pF_HK);
            ListCalcPar(fT);
            
            if (!enterSpecificStorageData)
            {
                SpecSCalculator fSs(pF_C, pF_Phi);
                ListCalcPar(fSs);
            }

            SCalculator fS(pF_C, pF_Phi, pF_Ss);
            ListCalcPar(fS);

            DCalculator fD(pF_k, pF_HK, pF_C, pF_Phi, pF_Ss);
            ListCalcPar(fD);

        }

        if (control.IsDual())
        {
            ListHKS("Fracture", fracParGroup);
            ListHKS("Matrix", matParGroup);

            SubTitle("Dual Porosity");

            {
                FractureThickCalculator ftC;
                ListCalcPar(ftC);
            }

            {
                MatrixThickCalculator mtC;
                ListCalcPar(mtC);
            }

            if (!enterDualAlpha)
            {
                AlphaCalculator aC;
                ListCalcPar(aC);
            }
            {
                InterpretLambdaCalculator lC;
                ListCalcPar(lC);
            }
            {
                OmegaCalculator oC;
                ListCalcPar(oC);
            }
            {
                InterpretBetaCalculator bC;
                ListCalcPar(bC);
            }

        }

        if (control.IsLeaky())
        {
            if (leakageType == ltSingle)
            {
                ListHKS("Leaky", slParGroup);
                if (!control.IsDual())
                {
                    LFactorCalculator lF;
                    ListCalcPar(lF);
                }
            }
            else
            {
                ListHKS("Upper Leaky", ulParGroup);
                ListHKS("Lower Leaky", llParGroup);
            }
        }

        if (control.HasSkin())
        {

            SubTitle("Skin Zone");

            if (enterPermeabilityData)
            {
                KCalculator fK(pS_k);
                ListCalcPar(fK);
            }

            {
                TCalculator fT(pS_k, pS_HK);
                ListCalcPar(fT);
            }

            if (!enterSpecificStorageData)
            {
                SpecSCalculator fSs(pS_C, pS_Phi);
                ListCalcPar(fSs);
            }
            
            {
                SCalculator fS(pS_C, pS_Phi, pS_Ss);
                ListCalcPar(fS);
            }

            {
                DCalculator fD(pS_k, pS_HK, pS_C, pS_Phi, pS_Ss);
                ListCalcPar(fD);
            }

            {
                SFactorCalculator sC;
                ListCalcPar(sC);
            }
        }

        if (allParam[pTZ_C]->IsActive() || allParam[pTZ_r_s]->IsActive())
        {
            SubTitle("Test Zone");
            if (allParam[pTZ_C]->IsActive())
            {
                TZVolCalculator volC;
                ListCalcPar(volC);

                CIsolatedCalculator isolC;
                ListCalcPar(isolC);
            }

            if (allParam[pTZ_r_s]->IsActive())
            {
                COpenHoleCalculator openC;
                ListCalcPar(openC);
            }
        }


    }

    void CalcParameterListing::ListGeomCalc()
    {
        SubTitle("Grid Properties");

        {
            DeltaACalculator daC;
            ListCalcPar(daC);
        }

        {
            FirstIncCalculator fiC;
            ListCalcPar(fiC);
        }


        if (control.HasSkin())
        {
            SkinDeltaACalculator sdaC;
            ListCalcPar(sdaC);

            SkinFirstIncCalculator sfiC;
            ListCalcPar(sfiC);

            SkinLastIncCalculator sliC;
            ListCalcPar(sliC);

            IncRatioCalculator irC;
            ListCalcPar(irC);
        }


        if (control.IsDual() || control.IsLeaky())
        {
            NNodeCalculator nnC;
            ListCalcPar(nnC);
        }


    }


    void CalcParameterListing::CreateListing()
    {
        StdTitle("Calculated Parameters");

        if (control.IsGas())
        {
            ListGasCalc();
        }
        else
        {
            ListFluidCalc();
        }

        ListGeomCalc();

    }

    void CalcParameterListing::ListCalcPar(ParameterCalculator& calc)
    {
        if (calc.IsFP())
            AddStdText(calc.parID, "f(P)");
        else if (calc.IsFR())
            AddStdText(calc.parID, "f(r)");
        else if (calc.IsFT())
            AddStdText(calc.parID, "f(t)");
        else if (calc.AllFixed())
        {
            calc.SetCalcVals(false);
            UnitReal temp(calc.parUnits, calc.GetCalcVal());
            AddUnitReal(calc.parID, temp, calc.poverrideFormat);
        }
        else if (calc.IsMinMax())
        {
            calc.SetCalcVals(false);
            AddStdText(calc.parID, "min/max");
            UnitReal temp(calc.parUnits, calc.GetMinCalcVal());
            AddUnitReal("   Minimum", temp, calc.poverrideFormat);
            temp.metricVal = calc.GetMaxCalcVal();
            AddUnitReal("   Maximum", temp, calc.poverrideFormat);
        }
        else
        {
            int nVary = calc.GetnVary();
            if ((nVary == 0) || (nVary > 3) || VaryControlStatics::IsRange() || (!VaryStaticSupport::VaryOK()))
                GenAppInternalError("CalcParameterListing::ListCalcPar");

            // contains 1 or more vary

            VaryVarArray listVary;
            listVary.AllocAndSetSize(maxVary);
            listVary.FillToAlloc(0);

            SC_IntArray varyOrder(maxVary);
            SC_IntArray varyPar(maxVary);

            int maxVary = orderedVary.Size();
            for (int i = 0; i < calc.calcPars.Size(); i++)
            {
                Parameter& currPar = *(allParam[calc.calcPars[i]]);
                if (currPar.IsActive() && currPar.IsVary())
                {
                    for (int j = 0; j < maxVary; j++)
                        if ((&currPar.varyParVal) == orderedVary[j])
                        {
                            listVary[j] = orderedVary[j];
                            varyPar[j] = i;
                        }
                }
            }
            
            AddConditionalBlankLine();

            char unitStr[80];
            char caseStr[40];
            char valStr[40];
            SC_IntArray  colWidth(5);
            SC_BoolArray colLeft(5);
            colWidth.SetEmpty();
            colLeft.SetEmpty();
            

            switch (nVary) {
                case 1 : {

                    int varyIndex = 0;
                    if (listVary[1])
                        varyIndex = 1;
                    if (listVary[2])
                        varyIndex = 2;
                    VaryVar& currVary = *(listVary[varyIndex]);

                    Parameter& currVaryPar = *(allParam[calc.calcPars[varyPar[varyIndex]]]);

                    colWidth += 5;
                    colWidth += 25;
                    colWidth += 25;
                    colLeft  += true;
                    colLeft  += false;
                    colLeft  += false;

                    TableSetup(colWidth, colLeft, 5, 0);
                    SetTableCol(0, "CaseID");
                    SetTableCol(1, currVaryPar.fullName);
                    SetTableCol(2, calc.parID);
                    AddNextLine();

                    currVaryPar.parVal.MakeUnitString(unitStr, 80);
                    SetTableCol(1, unitStr);

                    calc.parUnits.MakeUnitString(unitStr, 80);
                    SetTableCol(2, unitStr);

                    AddNextLine();


                    for (int i = 0; i < currVary.GetnVary(); i++)
                    {
                        char intStr[20];
                        IntToString(i + 1, intStr, 20);

                        //  brute force ugly mess
                        switch (maxVary) {
                            case 1 : {
                                CopyString(caseStr, intStr, 40);
                                break;
                            }
                            case 2 : {
                                if (varyIndex == 0)
                                    MakeString(caseStr, intStr, "X", 40);
                                else
                                    MakeString(caseStr, "X", intStr, 40);
                                break;
                            }
                            case 3 : {
                                switch (varyIndex) {
                                    case 0: {
                                        MakeString(caseStr, intStr, "XX", 40);
                                        break;
                                    }
                                    case 1: {
                                        MakeString(caseStr, "X", intStr, "X", 40);
                                        break;
                                    }
                                    case 2: {
                                        MakeString(caseStr, "XX", intStr, 40);
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        SetTableCol(0, caseStr);

                        currVary.varyIndex = i;
                        currVaryPar.ParValToString(currVary.GetVaryVal(), valStr, 40);
                        SetTableCol(1, valStr);

                        calc.SetCalcVals(true);
                        calc.GetUserString(valStr, 40);
                        SetTableCol(2, valStr);
                        AddNextLine();
                    }
                    break;
                };
                case 2 : {

                    int slowVaryIndex, fastVaryIndex, nonVaryIndex;
                    if (!listVary[0])
                    {
                        nonVaryIndex = 0;
                        slowVaryIndex = 1;
                        fastVaryIndex = 2;
                    }
                    else if (!listVary[1])
                    {
                        nonVaryIndex = 1;
                        slowVaryIndex = 0;
                        fastVaryIndex = 2;
                    }
                    else 
                    {
                        nonVaryIndex = 2;
                        slowVaryIndex = 0;
                        fastVaryIndex = 1;
                    }

                    VaryVar& slowVary = *(listVary[slowVaryIndex]);
                    VaryVar& fastVary = *(listVary[fastVaryIndex]);

                    Parameter& slowVaryPar = *(allParam[calc.calcPars[varyPar[slowVaryIndex]]]);
                    Parameter& fastVaryPar = *(allParam[calc.calcPars[varyPar[fastVaryIndex]]]);

                    colWidth += 5;
                    colWidth += 20;
                    colWidth += 20;
                    colWidth += 20;
                    colLeft  += true;
                    colLeft  += false;
                    colLeft  += false;
                    colLeft  += false;

                    TableSetup(colWidth, colLeft, 5, 0);
                    SetTableCol(0, "CaseID");
                    SetTableCol(1, slowVaryPar.fullName);
                    SetTableCol(2, fastVaryPar.fullName);
                    SetTableCol(3, calc.parID);
                    AddNextLine();

                    slowVaryPar.parVal.MakeUnitString(unitStr, 80);
                    SetTableCol(1, unitStr);

                    fastVaryPar.parVal.MakeUnitString(unitStr, 80);
                    SetTableCol(2, unitStr);

                    calc.parUnits.MakeUnitString(unitStr, 80);
                    SetTableCol(3, unitStr);

                    AddNextLine();

                    for (int i = 0; i < slowVary.GetnVary(); i++)
                    {
                        char slowStr[20];
                        IntToString(i + 1, slowStr, 20);

                        for (int j = 0; j < fastVary.GetnVary(); j++)
                        {
                            char fastStr[20];
                            IntToString(j + 1, fastStr, 20);

                            if (maxVary == 2)
                            {
                                MakeString(caseStr, slowStr, fastStr, 40);
                            }
                            else
                            {
                                switch (nonVaryIndex) {
                                    case 0: {
                                        MakeString(caseStr, "X", slowStr, fastStr, 40);
                                        break;
                                    }
                                    case 1: {
                                        MakeString(caseStr, slowStr, "X", fastStr, 40);
                                        break;
                                    }
                                    case 2: {
                                        MakeString(caseStr, slowStr, fastStr, "X", 40);
                                        break;
                                    }
                                }
                            }

                            SetTableCol(0, caseStr);

                            slowVary.varyIndex = i;
                            slowVaryPar.ParValToString(slowVary.GetVaryVal(), valStr, 40);
                            SetTableCol(1, valStr);

                            fastVary.varyIndex = j;
                            fastVaryPar.ParValToString(fastVary.GetVaryVal(), valStr, 40);
                            SetTableCol(2, valStr);

                            calc.SetCalcVals(true);
                            calc.GetUserString(valStr, 40);
                            SetTableCol(3, valStr);
                            AddNextLine();
                        }
                    }

                    break;
                };
                case 3 : {
                    VaryVar& slowVary = *(listVary[0]);
                    VaryVar& midVary  = *(listVary[1]);
                    VaryVar& fastVary = *(listVary[2]);

                    Parameter& slowVaryPar = *(allParam[calc.calcPars[varyPar[0]]]);
                    Parameter& midVaryPar = *(allParam[calc.calcPars[varyPar[1]]]);
                    Parameter& fastVaryPar = *(allParam[calc.calcPars[varyPar[2]]]);

                    colWidth += 5;
                    colWidth += 15;
                    colWidth += 15;
                    colWidth += 15;
                    colWidth += 20;
                    colLeft  += true;
                    colLeft  += false;
                    colLeft  += false;
                    colLeft  += false;
                    colLeft  += false;

                    TableSetup(colWidth, colLeft, 5, 0);
                    SetTableCol(0, "CaseID");
                    SetTableCol(1, slowVaryPar.shortName);
                    SetTableCol(2, midVaryPar.shortName);
                    SetTableCol(3, fastVaryPar.shortName);
                    SetTableCol(4, calc.parID);
                    AddNextLine();

                    slowVaryPar.parVal.MakeUnitString(unitStr, 80);
                    SetTableCol(1, unitStr);

                    midVaryPar.parVal.MakeUnitString(unitStr, 80);
                    SetTableCol(2, unitStr);
                    
                    fastVaryPar.parVal.MakeUnitString(unitStr, 80);
                    SetTableCol(3, unitStr);

                    calc.parUnits.MakeUnitString(unitStr, 80);
                    SetTableCol(4, unitStr);

                    AddNextLine();

                    for (int i = 0; i < slowVary.GetnVary(); i++)
                    {
                        char slowStr[20];
                        IntToString(i + 1, slowStr, 20);

                        for (int j = 0; j < midVary.GetnVary(); j++)
                        {
                            char midStr[20];
                            IntToString(j + 1, midStr, 20);

                            for (int k = 0; k < fastVary.GetnVary(); k++)
                            {
                                char fastStr[20];
                                IntToString(k + 1, fastStr, 20);

                                MakeString(caseStr, slowStr, midStr, fastStr, 40);

                                SetTableCol(0, caseStr);

                                slowVary.varyIndex = i;
                                slowVaryPar.ParValToString(slowVary.GetVaryVal(), valStr, 40);
                                SetTableCol(1, valStr);

                                midVary.varyIndex = j;
                                midVaryPar.ParValToString(midVary.GetVaryVal(), valStr, 40);
                                SetTableCol(2, valStr);

                                fastVary.varyIndex = k;
                                fastVaryPar.ParValToString(fastVary.GetVaryVal(), valStr, 40);
                                SetTableCol(3, valStr);

                                calc.SetCalcVals(true);
                                calc.GetUserString(valStr, 40);
                                SetTableCol(4, valStr);
                                AddNextLine();
                            }
                        }
                    }

                    break;
                };
            }

            AddBlankLine();
        }
    }

};  // end namespace


