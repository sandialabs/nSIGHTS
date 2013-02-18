///////////////////////////////////////////////////////////////////////////////////
//
// G_DataCapture.cpp
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
//      Data capture specification vars and associated file I/O, error checking and
//      listing support.
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <iostream>

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osMain/G_OptRange.h>

#include <Var/G_PreConfigFile.h>

#include <Var/G_Control.h>
#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>
#include <Var/G_Parameter.h>

namespace nsDataCapture  {

    DataCaptureArray    dataCaptureData;
    UnitIndex           productionRestartUnits(uTime);
    SC_DoubleArray      productionRestartTimes;
    SC_DoubleMatrix     capturedData;
    DC_XYDataArray      capturedDataXY;
    DataCaptureOutput   capturedDataFO;
    bool                dataCaptureUIChange = false;


    DataCaptureErrorListing  dataCaptureErrorListing;
    DataCaptureStaticSupport dataCapture;


    CaptureOutputFO::CaptureOutputFO() : FuncObjC("f(t)Output")
    {
        DataCaptureStaticSupport::capturedObj.AddTo(this);
        dcIndex = 0;
        AddOutPort(xyDataDO);
        AddInPort(xyArrayObjRef, typeid(DO_XYDataArray));
        xyArrayObjRef = FuncObjRef(capturedDataFO);
    }


    CaptureOutputFO::~CaptureOutputFO()
    {
        DataCaptureStaticSupport::capturedObj.DeleteFrom(this);
    }

    void CaptureOutputFO::DoStatusChk()
    {
        xyDataDO.xyData = 0;
        FuncObjC::DoStatusChk();
        if (!CheckInputFO(xyArrayObjRef, "Input array"))
            return;

        DO_XYDataArray* dataDO = static_cast<DO_XYDataArray*>(GetInPortData(xyArrayObjRef));
        if (dataDO->xyDataArray->IsEmpty())
        {
            SetObjErrMsg("no entries in input array ??");
            return;
        }

        if (dcIndex > dataDO->xyDataArray->UpperBound())
            dcIndex = 0;

        xyDataDO.xyData = &(*dataDO->xyDataArray)[dcIndex];
    }

    void CaptureOutputFO::CalcOutput(FOcalcType  calcType)
    {
        DoStatusChk();
        capturedDataFO.InitStaticPressure();  // FB376 fix
    }




    DataCaptureArray::~DataCaptureArray()
    {
        // causes memory leak on exit, but required to avoid crash caused
        // by capturedObj.DeleteFrom after capturedObj is gone..
        tListData = 0;
    }

    DataCaptureOutput::DataCaptureOutput() :
        GlobalFunc("f(t)Table")
    {
        xyDataArrayDO.xyDataArray = &capturedDataXY;
        AddOutPort(xyDataArrayDO);
        AddOutPort(staticPressureDO);
        staticPressureDO.SetTypeLabel("Static pressure");
        staticPressureDO.InitLabelAndValue(0.0);
        DoStatusChk();
    }
    void DataCaptureOutput::DoStatusChk()
    {
        FuncObjC::DoStatusChk();
        if (!xyDataArrayDO.DataOK())
            SetObjErrMsg("No data in output table");
    }

    void DataCaptureOutput::CalcOutput(FOcalcType  calcType)
    {
        DoStatusChk();
        if (!StatusOK())
            return;
        for (int i = 0; i < capturedDataXY.Size(); i++)
            capturedDataXY[i].CreateFrom(capturedData[0], capturedData[i + 1]);
    }

    void DataCaptureOutput::InitStaticPressure()
    {
        double staticPressure = 0.0;

        if (control.IsConfined())
        {
            Parameter& staticPar = *(allParam[pF_Pf]);
            SC_SetupErr dummy;
            if (staticPar.SetupOK(dummy))
            {
                UnitIndex pConv(uPress);
                staticPressure = pConv.MetricToUser(staticPar.GetParValue());
            }
        }
        capturedDataFO.staticPressureDO.InitLabelAndValue(staticPressure);
    //  std::cout << "** Static P init " << staticPressure << std::endl;
    }

    DataCaptureListing::DataCaptureListing() 
        :OSListObj("nPreDataCapture")
    {
    }

    void DataCaptureListing::ListSuperComponent(const SuperComponent& outSuper)
    {
        ClearEnumStrs();
        AddEnumStr("+ Pressure",    SuperComponent::soAddP);
        AddEnumStr("- Pressure",    SuperComponent::soSubP);
        AddEnumStr("+ Delta P",     SuperComponent::soAddDP);
        AddEnumStr("- Delta P",     SuperComponent::soSubDP);

        char tempStr[80];
        CopyString(tempStr, "   ", 80);
        ConcatString(tempStr, outSuper.GetID(), 80);
        ConcatString(tempStr, " operation", 80);
        AddStdEnum(tempStr, outSuper.superOp);

        ClearEnumStrs();
        AddEnumStr("Constant",  SuperComponent::stConstant);
        AddEnumStr("Optimized", SuperComponent::stOptimized);
        AddEnumStr("Sampled",   SuperComponent::stSampled);
        AddStdEnum("    Type",  outSuper.superType);

        switch (outSuper.superType)
        {
            case SuperComponent::stConstant : {
                UnitReal temp(outSuper.superRadUnits, outSuper.constantRad);
                AddUnitReal("    Fixed radius", temp);
                break;
            }
            case SuperComponent::stOptimized : {
                AddOptVar("    Optimized radius", outSuper.optRadVal);
                break;
            }
            case SuperComponent::stSampled : {
                AddSampVar("    Sampled radius", outSuper.sampRadVal);
                break;
            }
            case SuperComponent::stVary : {
                AddVaryVar("    Varied radius", outSuper.varyRadVal);
                break;
            }
        }
    }


    void DataCaptureListing::ListDataCapture(const DataCaptureSpec& outDC)
    {
        AddConditionalBlank();
        AddStdText("Output ID", outDC.dataDesig);


        ClearEnumStrs();
        AddEnumStr("Pressure", DataCaptureSpec::ctPressure);
        AddEnumStr("Flow Rate", DataCaptureSpec::ctFlow);
        AddEnumStr("Cumulative Production", DataCaptureSpec::ctProduction);
        AddEnumStr("Test Zone Property", DataCaptureSpec::ctTestZone);
        AddEnumStr("Water Table", DataCaptureSpec::ctWaterTable);
        AddStdEnum("  Output type", outDC.captureType);

        ClearEnumStrs();
        switch (outDC.captureType)
        {
            case DataCaptureSpec::ctPressure  : {
                AddEnumStr("Test Zone", DataCaptureSpec::pctTestZone);
                AddEnumStr("Observation Well", DataCaptureSpec::pctRadius);
                AddEnumStr("Superposition", DataCaptureSpec::pctSuperposition);
                AddStdEnum("  Pressure capture type", outDC.pressureCapType);
                switch (outDC.pressureCapType)
                {
                    case DataCaptureSpec::pctTestZone : {
                        break;
                    }
                    case DataCaptureSpec::pctRadius : {
                        if (control.IsHorizontalAnisotropic())
                        {
                            AddUnitReal("  Observation well X location", outDC.obsXLocation);
                            AddUnitReal("  Observation well Y location", outDC.obsYLocation);
                            AddStdReal("  Observation well angle (degrees)", Degrees(outDC.GetObsAngle()), SC_DecFormat(3));
                        }
                        AddUnitReal("  Observation well radius", outDC.radiusData);
                        if (control.Is2DRadial())
                        {
                            AddBoolText("  Vertical position", outDC.zvalueIsRatio, "Actual", "Ratio");
                            if (outDC.zvalueIsRatio)
                                AddStdReal("  Vertical ratio", outDC.normalizedZ, SC_DecFormat(3));
                            else
                                AddUnitReal("  Vertical offset", outDC.actualZ);
                        }
                        break;
                    }
                    case DataCaptureSpec::pctSuperposition : {
                        for (int i = 0; i < outDC.superData.Size(); i++)
                            if (outDC.superData.IsValid(i))
                                ListSuperComponent(outDC.superData.GetRef(i));
                        break;
                    }
                }
                break;
            }
            case DataCaptureSpec::ctFlow  : {
                AddEnumStr("Well",              DataCaptureSpec::fctWell);
                AddEnumStr("Formation",         DataCaptureSpec::fctFormation);
                AddEnumStr("Test Zone",         DataCaptureSpec::fctTestZone);
                AddEnumStr("Wellbore Storage",  DataCaptureSpec::fctWellboreStorage);
                AddStdEnum("  Flow rate output type", outDC.flowCapType);
                break;
            }
            case DataCaptureSpec::ctProduction  : {
                AddEnumStr("Well",              DataCaptureSpec::prctWell);
                AddEnumStr("Formation",         DataCaptureSpec::prctFormation);
                AddEnumStr("Test Zone",         DataCaptureSpec::prctTestZone);
                AddStdEnum("  Production output type", outDC.flowCapType);
                break;
            }

            case DataCaptureSpec::ctTestZone : {
                AddEnumStr("Temperature",       DataCaptureSpec::tzctTemperature);
                AddEnumStr("Compressibility",    DataCaptureSpec::tzctCompressibility);
                AddEnumStr("Volume",            DataCaptureSpec::tzctVolume);
                AddStdEnum("  Test zone property output type", outDC.flowCapType);
                break;
            }

            case DataCaptureSpec::ctWaterTable : {
                AddUnitReal("  Water table radius", outDC.radiusData);
                break;
            }
        }

        AddUnitIndex("  Output units", outDC.outputUnits);
    }


    void DataCaptureListing::CreateListing()
    {
        StdTitle("Simulation Results Setup");
        for (int i = 0; i < dataCaptureData.Size(); i++)
            if (dataCaptureData.IsValid(i))
            {
                dataCaptureData.GetRef(i).SetIDs();
                ListDataCapture(dataCaptureData.GetRef(i));
            }

        if (productionRestartTimes.Size() > 0)
        {
            SubTitle("Production Restart Times");
            for (int i = 0; i < productionRestartTimes.Size(); i++)
            {
                char tempStr[40];
                CopyString(tempStr, "Time#", 40);
                char intStr[40];
                IntToString(i + 1, intStr, 40);
                ConcatString(tempStr, intStr, 40);
                UnitReal temp(productionRestartUnits, productionRestartTimes[i]);
                AddUnitReal(tempStr, temp);
            }
        }

    }

    DataCaptureErrorListing::DataCaptureErrorListing() 
        :ErrorListObjC("DataCapture")
    {
    }

    void DataCaptureErrorListing::CreateListing()
    {
        StdTitle("Simulation Results Setup Errors");

        SC_SetupErr dcErr;

        if (dataCaptureData.IsEmpty())
            AddError("no output data specified");


        for (int i = 0; i < dataCaptureData.Size(); i++)
            if (dataCaptureData.IsNotValid(i))
            {
                AddError("Null capture spec found");
            }
            else
            {
                if (!dataCaptureData.GetRef(i).SetupOK(dcErr))
                {
                    AddError(dcErr);
                }
            }

        if (productionRestartTimes.Size() > 0)
        {
            double prev = productionRestartTimes[0];
            for (int i = 1; i < productionRestartTimes.Size(); i++)
            {
                double next = productionRestartTimes[i];
                if (RealIsNull(prev) || RealIsNull(next))
                {
                    AddError("Null production restart time found");
                    break;
                }
                if (prev >= next)
                {
                    AddError("Production restart times must be ascending");
                    break;
                }
            }
        }

    }


    DataCaptureStaticSupport::DataCaptureStaticSupport()
    {
        dataCaptureData.SetResizable();
        productionRestartTimes.SetResizable();
    }

    void DataCaptureStaticSupport::SetToInitial()
    {
        dataCaptureData.DeAlloc();
        dataCaptureData.Alloc(10);
        dataCaptureData.CreateIfNecessary(0);
        dataCaptureData.CreateIfNecessary(1);

        productionRestartTimes.DeAlloc();
        productionRestartTimes.Alloc(10);

        // start with a default TZ pressure
        {
            DataCaptureSpec& currRef = dataCaptureData.GetRef(0);
            currRef.SetUserDesig("DAT");
        }

        // and flows
        {
            DataCaptureSpec& currRef = dataCaptureData.GetRef(1);
            currRef.SetUserDesig("DAT");
            currRef.captureType = DataCaptureSpec::ctFlow;
            currRef.flowCapType = DataCaptureSpec::fctWell;
            currRef.SetOutputUnits();
        }
        dataCaptureData.SetSize(2);

        capturedDataXY.DeAlloc();
        capturedDataXY.AllocAndSetSize(2);
        capturedDataXY[0].SetEmpty();
        capturedDataXY[0].SetID("sPDAT");
        capturedDataXY[1].SetEmpty();
        capturedDataXY[1].SetID("sQDAT");

        capturedData.DeAlloc();
        capturedData.AllocAndSetSize(3);
        capturedData[0].SetResizable();
        capturedData[1].SetResizable();
        capturedData[2].SetResizable();

        capturedDataFO.DoStatusChk();
        dataCaptureData[0]->outputFO.DoStatusChk();
        dataCaptureData[1]->outputFO.DoStatusChk();
    }

    void DataCaptureStaticSupport::SetForFlags()
    {
        for (int i = 0; i < dataCaptureData.Size(); i++)
            if (dataCaptureData.IsValid(i))
                dataCaptureData[i]->SetForFlags();
    }

    bool DataCaptureStaticSupport::DataCaptureOK()
    {
        if (dataCaptureData.IsEmpty())
            return false;

        if (!dataCaptureErrorListing.ErrorCheckOK())
            return false;

        // check to see if alloc needs changing
        if (capturedDataXY.Size() != dataCaptureData.Size())
        {
            capturedDataXY.AllocAndSetSize(dataCaptureData.Size());
            // and the table
            capturedData.AllocAndSetSize(dataCaptureData.Size() + 1);
            for (int i = 0; i <= dataCaptureData.Size(); i++)
                capturedData[i].SetResizable();
        }


        // reset the IDs, FOs, and clear
        for (int i = 0; i < dataCaptureData.Size(); i++)
        {
            DataCaptureSpecGlob& currSpec = *(dataCaptureData[i]);

            char foDesig[80];
            CopyString(foDesig, "s", 80);
            if (currSpec.IsWell())
            {
                if (currSpec.IsPressure())
                    ConcatString(foDesig, "P", 80);
                if (currSpec.IsFlow())
                    ConcatString(foDesig, "Q", 80);
            }
            // clear if already has correct designator
            if (strstr(currSpec.dataDesig, foDesig) == currSpec.dataDesig)
                SetToNull(foDesig);

            ConcatString(foDesig, currSpec.dataDesig, 80);

            capturedDataXY[i].SetID(foDesig);

            currSpec.outputFO.SetID(foDesig);
            currSpec.outputFO.dcIndex = i;
            currSpec.outputFO.DoStatusChk();
    //      capturedDataXY[i].SetEmpty();
        }


        // check for alloc size
        int minTS = sequence.GetTotalNTimeStep();
        bool reAlloc = false;
        for (int i = 0; i < dataCaptureData.Size(); i++)
            if (capturedData[i].AllocSize() < minTS)
            {
                reAlloc = true;
                break;
            }

        if (reAlloc)
        {
            for (int i = 0; i < dataCaptureData.Size(); i++)
                capturedData[i].Alloc(minTS);
        }

        capturedDataFO.InitStaticPressure();
        capturedDataFO.PreCalcSetup();

        return true;
    }

    void DataCaptureStaticSupport::DataCaptureCleanup()
    {
        dataCaptureData.Cleanup();
    }

    bool DataCaptureStaticSupport::SetDataCaptureDesigs()
    {
        int nextP_TZ = 1; int nextP_OW = 1; int nextP_S = 1;
        int nextQ_W = 1; int nextQ_F = 1; int nextQ_TZ = 1; int nextQ_WS = 1;
        int nextPr_W = 1; int nextPr_F = 1; int nextPr_TZ = 1;
        int nextO_T = 1; int nextO_C = 1; int nextO_V = 1;
        int nextWT = 1;

        bool retVal = false;
        for (int i = 0; i < dataCaptureData.Size(); i++)
            if (dataCaptureData.IsValid(i))
            {
                int nextVal;
                char nextStr[20];
                DataCaptureSpec& currDC = dataCaptureData.GetRef(i);
                switch (currDC.captureType)
                {
                    case DataCaptureSpec::ctPressure  : {
                        switch (currDC.pressureCapType)
                        {
                            case DataCaptureSpec::pctTestZone : {
                                nextVal = nextP_TZ++;
                                CopyString(nextStr, "P_TZ_", 20);
                                break;
                            }
                            case DataCaptureSpec::pctRadius : {
                                nextVal = nextP_OW++;
                                CopyString(nextStr, "P_OW_", 20);
                                break;
                            }
                            case DataCaptureSpec::pctSuperposition: {
                                nextVal = nextP_S++;
                                CopyString(nextStr, "P_S_", 20);
                                break;
                            }
                        }
                        break;
                    }
                    case DataCaptureSpec::ctFlow  : {
                        switch (currDC.flowCapType)
                        {
                            case DataCaptureSpec::fctWell: {
                                nextVal = nextQ_W++;
                                CopyString(nextStr, "Q_W_", 20);
                                break;
                            }
                            case DataCaptureSpec::fctFormation : {
                                nextVal = nextQ_F++;
                                CopyString(nextStr, "Q_F_", 20);
                                break;
                            }
                            case DataCaptureSpec::fctTestZone: {
                                nextVal = nextQ_TZ++;
                                CopyString(nextStr, "Q_TZ_", 20);
                                break;
                            }
                            case DataCaptureSpec::fctWellboreStorage: {
                                nextVal = nextQ_WS++;
                                CopyString(nextStr, "Q_WS_", 20);
                                break;
                            }
                        }
                        break;
                    }

                    case DataCaptureSpec::ctProduction  : {
                        switch (currDC.productionCapType)
                        {
                            case DataCaptureSpec::prctWell: {
                                nextVal = nextPr_W++;
                                CopyString(nextStr, "Pr_W_", 20);
                                break;
                            }
                            case DataCaptureSpec::prctFormation : {
                                nextVal = nextPr_F++;
                                CopyString(nextStr, "Pr_F_", 20);
                                break;
                            }
                            case DataCaptureSpec::prctTestZone: {
                                nextVal = nextPr_TZ++;
                                CopyString(nextStr, "Pr_TZ_", 20);
                                break;
                            }
                        }
                        break;
                    }
                    case DataCaptureSpec::ctTestZone : {
                        switch (currDC.testZoneCapType)
                        {
                            case DataCaptureSpec::tzctTemperature : {
                                nextVal = nextO_T++;
                                CopyString(nextStr, "O_T_", 20);
                                break;
                            }
                            case DataCaptureSpec::tzctCompressibility : {
                                nextVal = nextO_C++;
                                CopyString(nextStr, "O_C_", 20);
                                break;
                            }
                            case DataCaptureSpec::tzctVolume : {
                                nextVal = nextO_V++;
                                CopyString(nextStr, "O_V_", 20);
                                break;
                            }
                        }
                        break;
                    }
                    case DataCaptureSpec::ctWaterTable : {
                        nextVal = nextWT++;
                        CopyString(nextStr, "WT_OW_", 20);
                        break;
                    }
                }


                if (currDC.makeDefaultDataDesig)
                {
                    char nextIStr[10];
                    IntToString(nextVal, nextIStr, 2, 10);
                    char newDesig[40];
                    CopyString(newDesig, nextStr, 40);
                    ConcatString(newDesig, nextIStr, 40);
                    retVal = retVal || (strcmp(newDesig, currDC.dataDesig) != 0);
                    CopyString(currDC.dataDesig, newDesig, 40);
                }
            }

        return retVal;
    }


    void DataCaptureStaticSupport::CheckAndAllocDataCapture(int dcIndex)
    {
        dataCaptureData.CreateIfNecessary(dcIndex);
    /*  if (dataCaptureData.CreateIfNecessary(dcIndex))
        {
            IntToString(dcIndex, dataCaptureData[dcIndex]->dataDesig, 20);
        }
    */
    }


    bool DataCaptureStaticSupport::GetWellIDs(SC_StringArray& wellIDs)
    {
        wellIDs.SetStringLen(40);
        wellIDs.Alloc(dataCaptureData.Size());
        for (int i = 0; i < dataCaptureData.Size(); i++)
        {
            DataCaptureSpecGlob& currSpec = *(dataCaptureData[i]);
            if (currSpec.IsWell() && currSpec.IsPressure())
                wellIDs += currSpec.dataDesig;
        }
        return wellIDs.IsNotEmpty();
    }

    FuncObjC* DataCaptureStaticSupport::GetWellPFO(const char* wellID)
    {
        for (int i = 0; i < dataCaptureData.Size(); i++)
        {
            DataCaptureSpecGlob& currSpec = *(dataCaptureData[i]);
            if (SameString(wellID, currSpec.dataDesig) && currSpec.IsWell() && currSpec.IsPressure())
                return currSpec.GetObjRef();
        }
        return 0;
    }
    FuncObjC* DataCaptureStaticSupport::GetWellQFO(const char* wellID)
    {
        for (int i = 0; i < dataCaptureData.Size(); i++)
        {
            DataCaptureSpecGlob& currSpec = *(dataCaptureData[i]);
            if (SameString(wellID, currSpec.dataDesig) && currSpec.IsWell() && currSpec.IsFlow())
                return currSpec.GetObjRef();
        }
        return 0;
    }

    bool DataCaptureStaticSupport::GetSuperpositionCapture(DataCapturePtrArray& superpositionCapture)
    {
        superpositionCapture.Alloc(dataCaptureData.Size());
        for (int i = 0; i < dataCaptureData.Size(); i++)
            if (dataCaptureData.IsValid(i) && dataCaptureData.GetRef(i).IsSuperposition())
                superpositionCapture += dataCaptureData[i];

        return !superpositionCapture.IsEmpty();
    }

    appFuncObjGlobals::AppFuncArray DataCaptureStaticSupport::capturedObj("f(t)Output");

    void DataCaptureStaticSupport::InitAppGlobals()
    {
        capturedObj.AddTo(&capturedDataFO);
        capturedObj.AddTo(&optRangeOutputFO);
        appFuncObjGlobals::appFuncData += &capturedObj;
    }


    const char* DataCaptureStaticSupport::groupHeader = "DataCapture";
    //const int  DataCaptureStaticSupport::majorVersion = 0;
    //const int  DataCaptureStaticSupport::majorVersion = 1; // added vary par
    //const int  DataCaptureStaticSupport::majorVersion = 2; // added global objects
    //const int  DataCaptureStaticSupport::majorVersion = 3; // added unconfined support
    const int  DataCaptureStaticSupport::majorVersion = 4; // added horizontal anisotropy
    const int  DataCaptureStaticSupport::minorVersion = 0;

    void DataCaptureStaticSupport::WriteSuperComponent(SuperComponent& outSuper)
    {
        WriteInt(outSuper.superOp);
        WriteInt(outSuper.superType);
        WriteDouble(outSuper.constantRad);
        WriteSampVar(outSuper.sampRadVal);
        WriteOptVar(outSuper.optRadVal);
        WriteVaryVar(outSuper.varyRadVal);
    }

    void DataCaptureStaticSupport::WriteDataCapture(DataCaptureSpecGlob& outDC)
    {
        WriteText(outDC.dataDesig);
        WriteBool(outDC.makeDefaultDataDesig);
        WriteInt(outDC.captureType);
        WriteInt(outDC.pressureCapType);
        WriteInt(outDC.flowCapType);
        WriteInt(outDC.productionCapType);
        WriteInt(outDC.testZoneCapType);
        WriteUnitReal(outDC.radiusData);
        WriteBool(outDC.zvalueIsRatio);     // added v3
        WriteUnitReal(outDC.actualZ);       // added v3
        WriteDouble(outDC.normalizedZ);     // added v3
        WriteUnitReal(outDC.obsXLocation); // addec v4
        WriteUnitReal(outDC.obsYLocation); // addec v4

        WriteUnitIndex(outDC.outputUnits);
        WriteGlobalFunc(outDC.outputFO);
        WriteLine();

        WriteInt(outDC.superData.ValidSize());
        WriteLine();
        for (int i = 0; i < outDC.superData.Size(); i++)
            if (outDC.superData.IsValid(i))
                WriteSuperComponent(outDC.superData.GetRef(i));
    }

    void DataCaptureStaticSupport::ReadSuperComponent(SuperComponent& inSuper)
    {
        inSuper.superOp = SuperComponent::SuperOp(ReadInt());
        inSuper.superType = SuperComponent::SuperType(ReadInt());
        inSuper.constantRad = ReadDouble();
        ReadSampVar(inSuper.sampRadVal);
        ReadOptVar(inSuper.optRadVal);
        if (currentObjReadMajorVersion > 0)
            ReadVaryVar(inSuper.varyRadVal);

        inSuper.isOpt = (inSuper.superType == SuperComponent::stOptimized);
        inSuper.isSampled = (inSuper.superType == SuperComponent::stSampled);
        inSuper.isVary = (inSuper.superType == SuperComponent::stVary);

    }

    void DataCaptureStaticSupport::ReadDataCapture(DataCaptureSpecGlob& inDC)
    {
        ReadText(inDC.dataDesig, DataCaptureSpec::dataDesigLen);
        inDC.makeDefaultDataDesig = ReadBool();
        inDC.captureType       = DataCaptureSpec::CaptureType(ReadInt());
        inDC.pressureCapType   = DataCaptureSpec::PressureCapType(ReadInt());
        inDC.flowCapType       = DataCaptureSpec::FlowCapType(ReadInt());
        inDC.productionCapType = DataCaptureSpec::ProductionCapType(ReadInt());
        inDC.testZoneCapType   = DataCaptureSpec::TestZoneCapType(ReadInt());

        ReadUnitReal(inDC.radiusData);
        if (currentObjReadMajorVersion > 2)
        {
            inDC.zvalueIsRatio = ReadBool();        // added v3
            ReadUnitReal(inDC.actualZ);     // added v3
            inDC.normalizedZ = ReadDouble();        // added v3
        }

        if (currentObjReadMajorVersion > 3)
        {
            ReadUnitReal(inDC.obsXLocation);        // added v4
            ReadUnitReal(inDC.obsYLocation);        // added v4
        }

        ReadUnitIndex(inDC.outputUnits);

        if (currentObjReadMajorVersion > 1)
            ReadGlobalFunc(inDC.outputFO);

        inDC.superData.AllocAndCreate(ReadInt());
        for (int i = 0; i < inDC.superData.Size(); i++)
            ReadSuperComponent(inDC.superData.GetRef(i));
        inDC.SetIDs();

    }


    void DataCaptureStaticSupport::WriteToFile()
    {
        WriteGroupHeader(groupHeader, majorVersion, minorVersion);

        // global objects
        WriteGlobalFunc(capturedDataFO);
        WriteGlobalFunc(optRangeOutputFO);

        WriteInt(dataCaptureData.ValidSize());
        WriteLine();
        for (int i = 0; i < dataCaptureData.Size(); i++)
            if (dataCaptureData.IsValid(i))
                WriteDataCapture(dataCaptureData.GetRef(i));

        WriteUnitIndex(productionRestartUnits);
        WriteDoubleArray(productionRestartTimes, true);
        WriteLine();
    }

    bool DataCaptureStaticSupport::ReadFromFile()
    {
        ReadGroupHeader(groupHeader, majorVersion);
        if (currentObjReadMajorVersion > 1)
        {
            ReadGlobalFunc(capturedDataFO);
            ReadGlobalFunc(optRangeOutputFO);
        }

        dataCaptureData.AllocAndCreate(ReadInt());
        for (int i = 0; i < dataCaptureData.Size(); i++)
            ReadDataCapture(dataCaptureData.GetRef(i));

        ReadUnitIndex(productionRestartUnits);
        ReadDoubleArray(productionRestartTimes, true);

        return true;
    }



};                                      // end namespace

