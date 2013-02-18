///////////////////////////////////////////////////////////////////////////////////
//
// G_Control.h
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
//      Basic simulation configuration vars, file I/O, error checking and listing
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef G_CONTROL_H
#define G_CONTROL_H

#include <genListClass/C_ErrorListObj.h>
#include <genListClass/C_ListFactory.h>

#include <genApp/C_CfgFileObj.h>

#include <osClass/C_SampVar.h>
#include <osMain/C_OSListObj.h>

#include <Var/C_CurveFile.h>
#include <Var/C_Units.h>


//  namespace containing main static control variables
namespace nsControlStatics  {

        enum                        SimulationType {stForward, stOptimization};
        enum                        SimulationSubType {sstNormal, sstRange, sstSample};

        enum                        LeakageType {ltNone, ltSingle, ltUpperLower};
        enum                        PartialPenetrationType {pptTop, pptMiddle, pptBottom};
        enum                        WellModelType {wmtAnalytic, wmtMultiplier, wmtUniform};
#ifdef LAPACK
        enum                        MatrixSolver {msBanded};
#else // LAPACK
        enum                        MatrixSolver {msBanded, msCholesky, msBiconjugate, msConjugate};
#endif // LAPACK
        enum                        SolverConvCriteria {scc1, scc2, scc3, scc4};
        enum                        SolverPreconditioner{spSimple, spModCholesky, spVaidya};

        extern SimulationType       simulationType;
        extern SimulationSubType    simulationSubType;

        //  general configuration
        extern bool                 systemIsLiquid;                 // always valid
        extern bool                 hasSkin;                        // always valid
        extern bool                 externalBoundaryIsFixedP;       // always valid
        extern bool                 useCurveObjects;                // always valid added Sept 03 

        //  1D liquid only
        extern bool                 dualPorosity;
        extern bool                 dualGeometryIsPrismatic;
        extern LeakageType          leakageType;
        extern bool                 horizontalAnisotropy;
        extern UnitReal             wellXLocation;              // for anisotropy only
        extern UnitReal             wellYLocation;              // for anisotropy only

        //  liquid only
        extern bool                 allowNegativePressure;

        extern bool                 simulationIs2D;

        // liquid 2D 
        extern bool                 twoDisConfined;
        extern bool                 twoDisIsotropic;
        extern bool                 twoDisPartialPenetration;
        extern PartialPenetrationType partialPenetrationType;
//          extern bool                 useSudickyWellModel;        // replaced by WellModelType
        extern WellModelType        wellModelType;
        extern MatrixSolver         matrixSolver;
        extern bool                 reorder;
        extern SolverConvCriteria   solverConvCriteria;
        extern SolverPreconditioner solverPreconditioner;
        extern double               dropTolerance;

        // gas only
        extern bool                 gasUseKlinkenburg;                // use klinkenburg factor
        extern bool                 gasUseSimpleViscositySlope;       // gas is f(P) with slope factor
        extern bool                 gasFlowIsMass;                    // use mass flow or vol flow at STP
        extern bool                 gasDebugFlag;                    //  for debugging non-lin sims -- NOT saved in config
        extern UnitReal             gasStdTemperature;
        extern UnitReal             gasStdPressure;

        //  geometry
        extern bool                 compensateFlowDimensionGeometry;   // for liquid only

        //  data entry options
        extern bool                 enterPermeabilityData;
        extern bool                 enterSpecificStorageData;
        extern bool                 enterDualAlpha;

        // test zone related
        extern bool                 testZoneVolumeVaries;
        extern bool                 testZoneCompressibilityVaries;
        extern bool                 testZoneTemperatureVaries;
        extern UnitReal             defaultTestZoneTemperature;

        //  forward range options
        extern bool                 rangeFitIsChisq;

#ifndef ADCONSOLEAPP
        // test description text
        extern CString              testDescription;
#else
        extern SC_StringArray       testDescription;

#endif

        class ControlListing : public OSListObj {

            public: 
                                ControlListing();
                                ~ControlListing() {};

                                                    // C_Listing virtuals
                virtual void    CreateListing();

            private:
                void            ListCurveFile(const char* crvDesc,
                                              const CurveFile& crv);


        };


        class ControlErrorListing : public ErrorListObjC {
            public: 
                        ControlErrorListing();
                        ~ControlErrorListing() {};

                virtual void  CreateListing();

        };

        extern ControlErrorListing controlErrorListing;

        //  this should be singleton
        class ControlStaticSupport :    public ListFactory, 
                                        public CfgFileObj,
                                        public SampleControlStatics        // IO & listing support
         {
            private:
                // object versioning support
                static const char*  groupHeader;
                static const int        majorVersion;
                static const int        minorVersion;

            public:
                        ControlStaticSupport() {};
                        ~ControlStaticSupport() {};

                        // preferred namespace global access
            inline static bool         IsOptimization()    {return simulationType == stOptimization;}
            inline static bool         IsForward()         {return simulationType == stForward;}
            inline static bool         IsNormal()          {return simulationSubType == sstNormal;}
            inline static bool         IsRange()           {return simulationSubType == sstRange;}
            inline static bool         IsSampleSim()         {return simulationSubType == sstSample;}

            inline static bool         IsLiquid ()         {return systemIsLiquid;}
            inline static bool         IsGas ()            {return !systemIsLiquid;}

            inline static bool         HasSkin()           {return hasSkin;}
            inline static bool         ExtBoundIsFixedP()  {return externalBoundaryIsFixedP;}
            inline static bool         ExtBoundIsZeroFlow(){return !externalBoundaryIsFixedP;}
            inline static bool         IsLeaky()           {return leakageType != ltNone;}
            inline static bool         IsDual()            {return dualPorosity && IsLiquid();}
            inline static bool         Is2DRadial()        {return simulationIs2D && IsLiquid();}
            inline static bool         Is1DRadial()        {return (!simulationIs2D) || IsGas();}
            inline static bool         IsConfined()         {return twoDisConfined || Is1DRadial();}
            inline static bool         IsUnconfined()      {return (!twoDisConfined) && Is2DRadial();}
            inline static bool         IsVerticalAnisotropic()     {return (!twoDisIsotropic) && Is2DRadial();}
            inline static bool         IsPartialPenetration()       {return twoDisPartialPenetration;}
            inline static bool         IsTopPartialPenetration()    {return partialPenetrationType == pptTop;}
            inline static bool         IsMiddlePartialPenetration() {return partialPenetrationType == pptMiddle;}
            inline static bool         IsBottomPartialPenetration() {return partialPenetrationType == pptBottom;}
            inline static bool         IsAnalyticWellModel()        {return wellModelType == wmtAnalytic;}
            inline static bool         IsMultiplierWellModel()      {return wellModelType == wmtMultiplier;}
            inline static bool         IsUniformWellModel()         {return wellModelType == wmtUniform;}
            inline static bool         IsModCholPrecond()           {return solverPreconditioner != spSimple;}
            inline static bool         IsNotBanded()                {return matrixSolver != msBanded;}


            inline static bool         IsHorizontalAnisotropic() {return Is1DRadial() && horizontalAnisotropy && IsLiquid();}

            inline static bool         IsSingle()          {return !IsDual();}
            inline static bool         NonLinearOK()       {return !(Is2DRadial() || IsDual() || IsLeaky() || IsUnconfined());}
            inline static bool         TimeVarOK()         {return !(Is2DRadial() || IsUnconfined());}

            inline static bool         UseCurveObjects()   {return useCurveObjects;}
            inline static bool         UseCurveFiles()     {return !useCurveObjects;}
                        // flag ops
            static void         SetToInitial();     // for file New
            static void         SetConsistent();    // ensures all flag settings are consistent
                                                    //(also modifies headFlag in C_Units)

                        // implementation of virtuals
            virtual void        WriteToFile();
            virtual bool        ReadFromFile();

            void                WriteDefaultUnitsToFile();
            void                ReadDefaultUnitsFromFile();

            virtual ListObjC*       CreateListObject() {return new ControlListing();}
            virtual ErrorListObjC*  GetErrorListObject() {return &controlErrorListing;}
        };

        extern  ControlStaticSupport   control;

};


//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace nsControlStatics;


#endif // G_CONTROL_H
