///////////////////////////////////////////////////////////////////////////////////
//
// G_Control.cpp
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
//      Basic simulation configuration vars, file I/O, error checking and listing.
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <genClass/U_Str.h>

#include <osClass/C_VaryVar.h>

#include <osDFO/DFO_FitBase.h>
#include <osMain/G_Optimize.h>

#include <Var/G_PreConfigFile.h>

#include <Var/G_CurveFiles.h>

#include <Var/G_Control.h>

namespace nsControlStatics  {


    //  declarations and initial values
    SimulationType      simulationType      = stForward;
    SimulationSubType   simulationSubType   = sstNormal;

    //  general configuration
    bool                systemIsLiquid            = true;
    bool                hasSkin                   = false;
    bool                externalBoundaryIsFixedP  = true;
    bool                useCurveObjects           = true;
    bool                simulationIs2D            = false;

    // liquid only
    bool                allowNegativePressure     = true;

    //  liquid and 1D only
    bool                dualPorosity              = false;
    bool                dualGeometryIsPrismatic   = true;
    LeakageType         leakageType               = ltNone;
    bool                horizontalAnisotropy      = false;
    UnitReal            wellXLocation(uDist, 0.0);  // for anisotropy only
    UnitReal            wellYLocation(uDist, 0.0);  // for anisotropy only

    // liquid and 2D only
    bool                twoDisConfined              = true;
    bool                twoDisIsotropic             = true;
    bool                twoDisPartialPenetration    = false;
    PartialPenetrationType partialPenetrationType   = pptTop;
    //bool              useSudickyWellModel         = true;
    WellModelType       wellModelType               = wmtAnalytic;
    MatrixSolver        matrixSolver                = msBanded;
    bool                reorder                     = true;
    SolverConvCriteria  solverConvCriteria          = scc1;
    SolverPreconditioner solverPreconditioner       = spSimple;
    double              dropTolerance               = 0.0;

    //  gas only
    bool                 gasUseKlinkenburg           = false;
    bool                 gasUseSimpleViscositySlope  = false;
    bool                 gasFlowIsMass               = true;
    bool                 gasDebugFlag                = false;

    UnitReal             gasStdTemperature(uTemp, 20.0);
    UnitReal             gasStdPressure(uPress, 100.0);

    //  geometry
    bool                compensateFlowDimensionGeometry   = true;

    //  data entry options
    bool                enterPermeabilityData             = false;
    bool                enterSpecificStorageData          = true;
    bool                enterDualAlpha                    = true;

    // test zone related
    bool                testZoneVolumeVaries              = false;
    bool                testZoneCompressibilityVaries     = false;
    bool                testZoneTemperatureVaries         = false;
    UnitReal            defaultTestZoneTemperature(uTemp, 20.0);

    //  forward range options
    bool                rangeFitIsChisq                   = true;


    #ifndef ADCONSOLEAPP
    CString             testDescription;
    #else
    SC_StringArray      testDescription;
    #endif

    ControlErrorListing controlErrorListing;


    ControlListing::ControlListing()
        :OSListObj("nPreControl")
    {
    }

    void ControlListing::CreateListing()
    {
        StdTitle("Control Settings");


    #ifndef ADCONSOLEAPP
        if (!testDescription.IsEmpty())
        {
            SubTitle("Test Description");

            SC_StringArray strings;
            strings.CreateFromString(testDescription, "\n");
            int i;
            int lastNonBlank = 0;
            for (i = 0; i < strings.Size(); i++)
            {
                char tempStr[1024];
                CopyString(tempStr, strings[i], 1024);
                for (int j = 0; j < 1024; j++)
                    if ((tempStr[j] < ' ') && (tempStr[j] != '\t'))
                    {
                        tempStr[j] = '\0';
                        break;
                    }
                TrimLeft(tempStr);
                strings.SetString(tempStr, i);
                if (IsNotNull(tempStr))
                    lastNonBlank = i;
            }

            for (i = 0; i <= lastNonBlank; i++)
                AddLine(strings[i], 0);
        }
    #endif

        SubTitle("Main Settings");
        ClearEnumStrs();
        AddEnumStr("Forward", stForward);
        AddEnumStr("Optimization", stOptimization);
        AddStdEnum("Simulation type", simulationType);

        ClearEnumStrs();
        AddEnumStr("Normal", sstNormal);
        AddEnumStr("Range", sstRange);
        AddEnumStr("Sample", sstSample);
        AddStdEnum("Simulation subtype", simulationSubType);

        AddBoolText("Phase to simulate", control.IsLiquid(), "Liquid", "Gas");
        AddYesNo("Skin zone ?", control.HasSkin());
        AddBoolText("External boundary", externalBoundaryIsFixedP, "Fixed Pressure", "Zero Flow");
        //AddBoolText("Curve data source", useCurveObjects, "Objects", "Files");

        if (control.IsLiquid())
        {
            SubTitle("Liquid Phase Settings");
            AddBoolText("Aquifer type ", control.IsConfined(), "Confined", "Unconfined");

            if (control.Is2DRadial())
            {
                AddBoolText("Aquifer vertical permeability  ", control.IsVerticalAnisotropic(), "Anisotropic", "Isotropic");
                AddBoolText("Well penetration", control.IsPartialPenetration(), "Partial", "Full");
                if (control.IsPartialPenetration())
                {
                    ClearEnumStrs();
                    AddEnumStr("Top/Water Table", pptTop);
                    AddEnumStr("Middle", pptMiddle);
                    AddEnumStr("Bottom", pptMiddle);
                    AddStdEnum("Partial penetration type", partialPenetrationType);
                }

                ClearEnumStrs();
                AddEnumStr("Analytic", wmtAnalytic);
                AddEnumStr("kV Multipler", wmtMultiplier);
                AddEnumStr("Uniform", wmtUniform);
                AddStdEnum("Well model", wellModelType);

                ClearEnumStrs();
                AddEnumStr("Banded", msBanded);
                AddStdEnum("Matrix solver", matrixSolver);

                AddBoolText("Re-Order Matrix? ", reorder, "Yes", "No");

            }
            else
            {
                AddBoolText("Aquifer horizontal permeability  ", control.IsHorizontalAnisotropic(), "Anisotropic", "Isotropic");
                if (control.IsHorizontalAnisotropic())
                {
                    AddUnitReal("Well X location", wellXLocation);
                    AddUnitReal("Well Y location", wellYLocation);
                }
            }

            AddBoolText("System porosity", control.IsDual(), "Dual", "Single");

            if (control.IsDual())
            {
                AddBoolText("Matrix block type", dualGeometryIsPrismatic, "Prismatic", "Spherical");
            }

            AddYesNo("Compensate flow dimension geometry", compensateFlowDimensionGeometry);

            ClearEnumStrs();
            AddEnumStr("None", ltNone);
            AddEnumStr("Single Layer", ltSingle);
            AddEnumStr("Upper/Lower Layers", ltUpperLower);
            AddStdEnum("Leakage", leakageType);


            SubTitle("Test Zone Settings");
            AddYesNo("Test zone volume can vary", testZoneVolumeVaries);
            AddYesNo("Test zone compressibility can vary", testZoneCompressibilityVaries);
            AddYesNo("Test zone temperature can vary", testZoneTemperatureVaries);

            AddUnitReal("Default test-zone temperature", defaultTestZoneTemperature);

            AddBoolText("Solution variable", NSUnitsBase::headFlag, "Head", "Pressure");
            if (NSUnitsBase::headFlag)
            {
                AddUnitReal("Density for head/pressure conversion", NSUnitsBase::headDensity);
            }
            AddYesNo("Allow negative head/pressure", allowNegativePressure);

        }
        else
        {
            SubTitle("Gas Phase Settings");
            AddYesNo("Klinkenburg effects ", gasUseKlinkenburg);
            AddYesNo("Viscosity as simple f(P)", gasUseSimpleViscositySlope);
            AddBoolText("Gas flow units", gasFlowIsMass, "Mass Flow", "Vol Flow @STP");

            if (!gasFlowIsMass)
            {
                AddUnitReal("Standard temperature", gasStdTemperature);
                AddUnitReal("Standard pressure", gasStdPressure);
            }
        }


        if ((control.UseCurveFiles()) &&
            (IsNotNull(testZoneBCCurveFile.fileName) ||
             IsNotNull(frParameterCurveFile.fileName) ||
             IsNotNull(fPParameterCurveFile.fileName)))
        {
            SubTitle("Curve Files");
            ListCurveFile("Test-zone boundary conditions file:", testZoneBCCurveFile);
            ListCurveFile("f(r) parameters file:", frParameterCurveFile);
            ListCurveFile("f(P) parameters file:", fPParameterCurveFile);
        }

    };


    void ControlListing::ListCurveFile(const char* crvDesc,
                                       const CurveFile& crv)
    {
        if (IsNull(crv.fileName))
            return;

        AddFileName(crvDesc, crv.fileName);
        SC_StringArray crvIDs;
        crv.GetIDs(crvIDs);
        if (crvIDs.IsEmpty())
            AddText("   No curves loaded from file", 5);
        else
        {
            char temp[40];
            for (int i = 0; i < crvIDs.Size(); i++)
            {
                CopyString(temp, "   Curve #", 40);
                ConcatInt(i + 1, temp, 40);
                AddStdText(temp, crvIDs[i]);
            }
            AddBlankLine();
        }
    }


    ControlErrorListing::ControlErrorListing()
        :ErrorListObjC("Control")
    {
    }

    void ControlErrorListing::CreateListing()
    {
        StdTitle("Configuration Errors");

        if (control.IsRange())
        {
            // add least one fit must be selected
            int nfitSel = 0;
            for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
                if (optimizeInput.inputFits[i].optimizeOnFit)
                {
                    DFO_FitBase* currFit = static_cast<DFO_FitBase*>(optimizeInput.inputFits[i].fitObject.objRef);
                    if (currFit)
                        nfitSel++;
                }
            if (nfitSel == 0)
            {
                AddError("At least one fit must be defined and selected for range runs");
            }
        }
    }


    // the one and only
    ControlStaticSupport control;

    void ControlStaticSupport::SetToInitial()
    {
        simulationType      = stForward;
        simulationSubType   = sstNormal;
        VaryControlStatics::varyIsRange = false;

        useCurveObjects           = true;

        simulationIs2D            = false;
        systemIsLiquid            = true;
        twoDisConfined            = true;
        twoDisIsotropic           = true;
        twoDisPartialPenetration  = false;
        wellModelType             = wmtAnalytic;
        matrixSolver              = msBanded;
        reorder                   = true;
        solverConvCriteria        = scc1;
        solverPreconditioner      = spSimple;
        dropTolerance             = 0.01;

        partialPenetrationType    = pptMiddle;
        hasSkin                   = false;
        externalBoundaryIsFixedP  = true;
        allowNegativePressure     = true;

        horizontalAnisotropy      = false;
        wellXLocation             = UnitReal(uDist, 0.0);
        wellYLocation             = UnitReal(uDist, 0.0);

        dualPorosity              = false;
        dualGeometryIsPrismatic   = false;
        leakageType               = ltNone;

        gasUseKlinkenburg                = false;
        gasUseSimpleViscositySlope       = false;
        gasFlowIsMass                    = true;
        gasStdTemperature.SetMetricVal(20.0);
        gasStdPressure.SetMetricVal(100.0);

        compensateFlowDimensionGeometry   = true;

        enterPermeabilityData             = false;
        enterSpecificStorageData          = true;
        enterDualAlpha                    = true;

        testZoneVolumeVaries              = false;
        testZoneCompressibilityVaries     = false;

        rangeFitIsChisq                   = true;

    #ifndef ADCONSOLEAPP
        testDescription.Empty();
    #else
        testDescription.SetEmpty();
    #endif

        testZoneBCCurveFile.SetToInitial();
        frParameterCurveFile.SetToInitial();
        fPParameterCurveFile.SetToInitial();

    }

    void ControlStaticSupport::SetConsistent()
    {
        // main test
        if (IsGas())
        {
            // no dual P or leakage
            dualPorosity = false;
            simulationIs2D = false;
            leakageType   = ltNone;

            //  permeability and porosity directly
            enterPermeabilityData    = true;
            enterSpecificStorageData = false;

            //  no test zone volume modifiers
            testZoneVolumeVaries              = false;
            testZoneCompressibilityVaries     = false;

            //  must use pressure units
            NSUnitsBase::headFlag = false;
            NSUnitsBase::ResetHeadPressureConversions();
            allowNegativePressure     = false;
        }

        if (Is2DRadial())
        {
            dualPorosity = false;
            leakageType  = ltNone;
        }

        VaryControlStatics::varyIsRange = (simulationSubType == sstRange);
    }

    const char* ControlStaticSupport::groupHeader = "ControlStatics";
    const int  ControlStaticSupport::majorVersion = 12;     // added option for reordering matrix
    //const int  ControlStaticSupport::majorVersion = 11;       // added matrix preconditioners
    //const int  ControlStaticSupport::majorVersion = 10;       // added matrix solver
    //const int  ControlStaticSupport::majorVersion = 9;        // changed well model to enum
    //const int  ControlStaticSupport::majorVersion = 8;        // added horizontal anisotropy
    //const int  ControlStaticSupport::majorVersion = 7;        // added well model
    //const int  ControlStaticSupport::majorVersion = 6;        // added 2D uncon confined/unconfined
    //const int  ControlStaticSupport::majorVersion = 5;        // added confined/unconfined
    //const int  ControlStaticSupport::majorVersion = 4;    // use curve objects
    //const int  ControlStaticSupport::majorVersion = 3;    // allowNegativePressure
    //const int  ControlStaticSupport::majorVersion = 2;    // added missing enterDualAlpha
    //const int  ControlStaticSupport::majorVersion = 1;    // moverd opt vars to seperate group
    //const int  ControlStaticSupport::majorVersion = 0;
    const int  ControlStaticSupport::minorVersion = 0;


    void ControlStaticSupport::WriteToFile()
    {

        WriteGroupHeader(groupHeader, majorVersion, minorVersion);

                //  general configuration
        WriteInt(int(simulationType));
        WriteInt(int(simulationSubType));

        WriteBool(systemIsLiquid);
        WriteBool(hasSkin);
        WriteBool(externalBoundaryIsFixedP);
        WriteBool(useCurveObjects); // added v4

        WriteBool(simulationIs2D); // added v6

        WriteBool(twoDisConfined);              // added v5, modified v6
        WriteBool(twoDisIsotropic);             // added v5, modified v6
        WriteBool(twoDisPartialPenetration);    // added v5, modified v6
        WriteInt(int(partialPenetrationType));  // added v5, modified v6
        WriteInt(int(wellModelType));           // bool added v7, changed to enum v9
        WriteInt(int(matrixSolver));            // added v10
        WriteBool(reorder);                     // added v12
        WriteInt(int(solverConvCriteria));      // added v10
        WriteInt(int(solverPreconditioner));    // added v11
        WriteDouble(dropTolerance);             // added v11
        WriteLine();

                //  liquid only
        WriteBool(dualPorosity);
        WriteBool(dualGeometryIsPrismatic);
        WriteInt(int(leakageType));
        WriteBool(allowNegativePressure); // added v3

        WriteBool(horizontalAnisotropy);  // added v8
        WriteUnitReal(wellXLocation);
        WriteUnitReal(wellYLocation);

        WriteLine();

                // gas only
        WriteBool(gasUseKlinkenburg);
        WriteBool(gasUseSimpleViscositySlope);
        WriteBool(gasFlowIsMass);
        WriteUnitReal(gasStdTemperature);
        WriteUnitReal(gasStdPressure);
        WriteLine();

                //  geometry
        WriteBool(compensateFlowDimensionGeometry);

                //  data entry options
        WriteBool(enterPermeabilityData);
        WriteBool(enterSpecificStorageData);
        WriteBool(enterDualAlpha);                  // added v2


                // test zone related
        WriteBool(testZoneVolumeVaries);
        WriteBool(testZoneCompressibilityVaries);
        WriteBool(testZoneTemperatureVaries);
        WriteUnitReal(defaultTestZoneTemperature);
        WriteLine();

                //  forward range options
        WriteBool(rangeFitIsChisq);
        WriteLine();

            // units control
        WriteDefaultUnitsToFile();
        WriteBool(NSUnitsBase::headFlag);
        WriteUnitReal(NSUnitsBase::headDensity);
        WriteLine();


        /*
         * ************* optimization control

      REMOVED in control file version 1

                //  static control variables basic
        WriteInt(int(optimizationType));
        WriteBool(calc95Limits);
        WriteBool(use2ndOrderCovar);
        WriteBool(useFixedDerivSpan);
        WriteLine();

                //  general control
        WriteDouble(parameterTolerance);
        WriteDouble(derivativeTolerance);
        WriteDouble(fixedDerivSpan);
                // multiple optimization control
        WriteBool(startMultipleFromOriginalEst);
        WriteBool(updateParameterEstimates);
        WriteLine();

                //  simplex control
        WriteDouble(simplexSpan);
        WriteLine();

                //  Lev-Mar control
        WriteDouble(lmDerivSpan);
        WriteDouble(lmLambdaFactor);
        WriteDouble(lmMinLambda);
        WriteDouble(lmRelChangeTolerance);
        WriteInt(maxOptRun);
        WriteLine();

                // Perturbation
        WriteBool(perturbFromStart);
        WriteInt(nPerturb);
        WriteDouble(perturbSpan);
        WriteLine();
    */


    #ifndef ADCONSOLEAPP
                // test description text
        WriteString(testDescription);
    #else
        WriteStringArray(testDescription);
    #endif
        WriteLine();

        // curve files
        WriteText(testZoneBCCurveFile.fileName);
        WriteText(frParameterCurveFile.fileName);
        WriteText(fPParameterCurveFile.fileName);

    }

    bool ControlStaticSupport::ReadFromFile()
    {
        ReadGroupHeader(groupHeader, majorVersion);

                //  general configuration
        simulationType = SimulationType(ReadInt());
        simulationSubType = SimulationSubType(ReadInt());
        VaryControlStatics::varyIsRange = (simulationSubType == sstRange);

        systemIsLiquid = ReadBool();
        hasSkin = ReadBool();
        externalBoundaryIsFixedP = ReadBool();

        if (currentObjReadMajorVersion > 3)
            useCurveObjects = ReadBool(); // added v4
        else
            useCurveObjects = false;

        if (currentObjReadMajorVersion > 4)
        {

            if (currentObjReadMajorVersion > 5)
            {
                simulationIs2D = ReadBool(); // added v6
                twoDisConfined = ReadBool();                // added v5, modified v6
                twoDisIsotropic = ReadBool();               // added v5, modified v6
                twoDisPartialPenetration = ReadBool();    // added v5, modified v6
                partialPenetrationType = PartialPenetrationType(ReadInt());  // added v5, modified v6

                if (currentObjReadMajorVersion > 6)
                {   // added v7, modified v9
                    if (currentObjReadMajorVersion < 9)
                    {
                        bool useSudickyWellModel = ReadBool();
                        if (useSudickyWellModel)
                            wellModelType = wmtAnalytic;
                        else
                            wellModelType = wmtMultiplier;
                    }
                    else
                    {
                        wellModelType = WellModelType(ReadInt());
                        if (currentObjReadMajorVersion > 9)
                        {
                            matrixSolver = MatrixSolver(ReadInt());
                            if (currentObjReadMajorVersion > 11)
                                reorder = ReadBool();
                            solverConvCriteria = SolverConvCriteria(ReadInt());
                        }
                        if (currentObjReadMajorVersion > 10)
                        {
                            solverPreconditioner = SolverPreconditioner(ReadInt());
                            dropTolerance = ReadDouble();
                        }
                    }
                }
            }
            else
            {
                simulationIs2D = ! ReadBool(); // was v5 is unconf - means is 2D
                if (simulationIs2D)
                    twoDisConfined = false;     // and unconfined if it was 2D

                twoDisIsotropic = ReadBool(); //added v5
                twoDisPartialPenetration = ReadBool();
                if (ReadBool())                     // v5 was bool for waterTable
                    partialPenetrationType = pptTop;
                else
                    partialPenetrationType = pptMiddle;
            }
        }
        else
        {
            simulationIs2D = false;
            twoDisConfined = true;
            twoDisIsotropic = true;
            twoDisPartialPenetration = false;
        }

        ReadLine();

                //  liquid only
        dualPorosity = ReadBool();
        dualGeometryIsPrismatic = ReadBool();
        leakageType = LeakageType(ReadInt());
        if (currentObjReadMajorVersion > 2)
            allowNegativePressure = ReadBool(); // added v3

        if (currentObjReadMajorVersion > 7)
        {
            horizontalAnisotropy = ReadBool();  // added v8
            ReadUnitReal(wellXLocation);
            ReadUnitReal(wellYLocation);
        }

        ReadLine();

                // gas only
        gasUseKlinkenburg = ReadBool();
        gasUseSimpleViscositySlope = ReadBool();
        gasFlowIsMass = ReadBool();
        ReadUnitReal(gasStdTemperature);
        ReadUnitReal(gasStdPressure);
        ReadLine();

                //  geometry
        compensateFlowDimensionGeometry = ReadBool();

                //  data entry options
        enterPermeabilityData = ReadBool();
        enterSpecificStorageData = ReadBool();
        if (currentObjReadMajorVersion > 1)
            enterDualAlpha = ReadBool();


                // test zone related
        testZoneVolumeVaries = ReadBool();
        testZoneCompressibilityVaries = ReadBool();
        testZoneTemperatureVaries = ReadBool();
        ReadUnitReal(defaultTestZoneTemperature);
        ReadLine();

                //  forward range options
        rangeFitIsChisq = ReadBool();
        ReadLine();

            // units control
        ReadDefaultUnitsFromFile();
        NSUnitsBase::headFlag = ReadBool();
        ReadUnitReal(NSUnitsBase::headDensity);
        NSUnitsBase::ResetHeadPressureConversions();

        if (currentObjReadMajorVersion == 0)
        {
        /*
         * ************* optimization control removed in version 0
         * Skip
         */

            ReadInt();
            ReadBool();
            ReadBool();
            ReadBool();

            ReadDouble();
            ReadDouble();
            ReadDouble();

            ReadBool();
            ReadBool();

            ReadDouble();


            ReadDouble();
            ReadDouble();
            ReadDouble();
            ReadDouble();
            ReadInt();

            ReadBool();
            ReadInt();
            ReadDouble();
        }


       #ifndef ADCONSOLEAPP
                // test description text
        ReadString(testDescription);
       #else
        ReadStringArray(testDescription);
       #endif
        ReadLine();

        ReadText(testZoneBCCurveFile.fileName, stdFileStrLen);
        ReadText(frParameterCurveFile.fileName, stdFileStrLen);
        ReadText(fPParameterCurveFile.fileName, stdFileStrLen);

        testZoneBCCurveFile.LoadCurvesFromFile();
        frParameterCurveFile.LoadCurvesFromFile();
        fPParameterCurveFile.LoadCurvesFromFile();

        CheckForInputFailure("Reading Control");

        return true;
    }

    void ControlStaticSupport::WriteDefaultUnitsToFile()
    {
        for (int i = 0; i < UnitsBase::GetAppMaxUnits(); i++ ) {
            WriteInt(UnitsBase::defaultUnitIndex[i]);
        }
        WriteLine();
    }

    void ControlStaticSupport::ReadDefaultUnitsFromFile()
    {
        for (int i = 0; i < UnitsBase::GetAppMaxUnits(); i++ ) {
            UnitsBase::defaultUnitIndex[i] = ReadInt();
        }
        ReadLine();
    }



};                                      // end namespace

