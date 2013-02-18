///////////////////////////////////////////////////////////////////////////////////
//
// A_Field.cpp
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
//      Implements auto setup procedures.
//
///////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <genClass/U_Str.h>

#include <objDFO/DFO_ReadTable.h>
#include <objDFO/DFO_SelectXY.h>
#include <objDFO/DFO_XYRemoveDuplicate.h>
#include <objDFO/DFO_XYDualScaleTransform.h>
#include <objDFO/DFO_XYReduction.h>
#include <objPFO/PFO_2DXYData.h>
#include <nsDFO/DFO_ReadMiniTroll.h>

#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>

#include <App/nPreRoot.h>

#include <Auto/A_Main.h>
#include <Auto/A_Field.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace nsAutoSetup  {

    //  declarations and initial values
    InputDataFormat inputDataFormat = idfXY;
    bool            bothInSameFile = true;

    bool processDups        = false;
    bool processReduce      = false;
    bool processST          = false;

    bool createPressurePlot = true;
    bool createFlowPlot     = false;

    static void FieldProcessing(FuncObjC*& inputXYFO)
    {
        char baseID[idLen];
        CopyString(baseID, inputXYFO->GetID(), 80);

        int idNum = 0;
        char incID[idLen];
        IncrementID(baseID, incID, idNum);

        inputXYFO->SetID(incID);

        if (processDups)
        {
            IncrementID(baseID, incID, idNum);
            MenuObjC* xyObj = AddNewObject("DPO_XYRemoveDuplicate", incID);
            DFO_XYRemoveDuplicate* currXY = static_cast<DFO_XYRemoveDuplicate*>(xyObj->objFunction);

            currXY->xyInputObjRef = FuncObjRef(*inputXYFO);

            inputXYFO = currXY;
        }

        if (processReduce)
        {
            IncrementID(baseID, incID, idNum);
            MenuObjC* xyObj = AddNewObject("DPO_XYReduction", incID);
            DFO_XYReduction* currXY = static_cast<DFO_XYReduction*>(xyObj->objFunction);

            currXY->xyInputObjRef = FuncObjRef(*inputXYFO);

            inputXYFO = currXY;
        }

        if (processST)
        {
            IncrementID(baseID, incID, idNum);
            MenuObjC* xyObj = AddNewObject("DPO_XYDualScaleTransform", incID);
            DFO_XYDualScaleTransform* currXY = static_cast<DFO_XYDualScaleTransform*>(xyObj->objFunction);

            currXY->xyInputObjRef = FuncObjRef(*inputXYFO);

            inputXYFO = currXY;
        }

        inputXYFO->SetID(baseID);
    }


    static void DeleteFieldPage(bool inSpec, const char* inID, MenuPageC* parent)
    {
        if (inSpec)
        {
            fieldDataRoot.DeletePage(inID, parent);
        }
    }
    static int FieldPageExists(bool inSpec, const char* pageID, MenuPageC* parent)
    {
        if (inSpec && fieldDataRoot.PageExists(pageID, parent))
            return 1;

        return 0;
    }


    static void DeleteFieldPages(MenuPageC* parent)
    {
        DeleteFieldPage(processTestZonePressureData, "P_Process", parent);
        DeleteFieldPage(processTestZoneFlowData, "Q_Process", parent);
        DeleteFieldPage(processTestZonePressureData, "P_Input", parent);
        DeleteFieldPage(processTestZoneFlowData, "Q_Input", parent);
    }


    static void AddFieldPages(MenuPageC*    parent,
                              bool          isMainWell,
                              FuncObjArray& fieldPFO,
                              FuncObjArray& fieldQFO)
    {
        FuncObjC* pressXYFO = 0;
        FuncObjC* flowXYFO = 0;

        char pressureID[80];
        MakeString(pressureID, "fP", wellID, 80);

        char flowID[80];
        MakeString(flowID, "fQ", wellID, 80);


        if ((inputDataFormat == idfTable) ||  (inputDataFormat == idfMiniTroll))
        {
            MenuObjC* pressTblObj = 0;
            MenuObjC* flowTblObj = 0;
            MenuPageC* flowPage = 0;
            MenuPageC* pressPage = 0;

            const char* readObj = "DPO_ReadTable";
            if (inputDataFormat == idfMiniTroll)
                readObj = "DPO_ReadMiniTroll";

            if (isMainWell && processTestZonePressureData && processTestZoneFlowData && bothInSameFile)
            {
                DeleteFieldPage(true, "PQ_Input", parent);
                AddNewDataPage("PQ_Input", parent);

                pressTblObj = AddNewObject(readObj, "PQ_Input");
                flowTblObj  = pressTblObj;
                flowPage    = autoPage;
                pressPage   = autoPage;
            }
            else
            {
                // seperate pages
                if (processTestZonePressureData)
                {
                    AddNewDataPage("P_Input", parent);
                    pressTblObj = AddNewObject(readObj, "P_Input");
                    pressPage = autoPage;
                }

                if (isMainWell && processTestZoneFlowData)
                {
                    AddNewDataPage("Q_Input", parent);
                    flowTblObj = AddNewObject(readObj, "Q_Input");
                    flowPage = autoPage;
                }
            }

            if (processTestZonePressureData)
            {
                pressPage->pageSelected = true;
                pressTblObj->objSelected = true;
            }
            else if (isMainWell)
            {
                flowPage->pageSelected = true;
                flowTblObj->objSelected = true;
            }

            if (processTestZonePressureData)
            {
                autoPage = pressPage;
                MenuObjC* pressXYObj = AddNewObject("DPO_SelectXY", pressureID);
                DFO_SelectXY* currXY = static_cast<DFO_SelectXY*>(pressXYObj->objFunction);
                currXY->xyDataArrayObjRef = FuncObjRef(*(pressTblObj->objFunction));
                pressXYFO = currXY;
            }

            if (isMainWell && processTestZoneFlowData)
            {
                autoPage = flowPage;
                MenuObjC* flowXYObj = AddNewObject("DPO_SelectXY", flowID);
                DFO_SelectXY* currXY = static_cast<DFO_SelectXY*>(flowXYObj->objFunction);
                currXY->xyDataArrayObjRef = FuncObjRef(*(flowTblObj->objFunction));
                flowXYFO = currXY;
            }

        }
        else
        {
            if (processTestZonePressureData)
            {
                AddNewDataPage("P_Input", parent);
                MenuObjC* pressObj = AddNewObject("DPO_ReadXY", pressureID);
                autoPage->pageSelected = true;
                pressObj->objSelected = true;
                pressXYFO = pressObj->objFunction;
            }
            if (isMainWell && processTestZoneFlowData)
            {
                AddNewDataPage("Q_Input", parent);
                MenuObjC* flowObj = AddNewObject("DPO_ReadXY", flowID);
                if (!processTestZonePressureData)
                {
                    autoPage->pageSelected = true;
                    flowObj->objSelected = true;
                }
                flowXYFO = flowObj->objFunction;
            }
        }


        // check for post processing
        if (processDups || processReduce || processST)
        {

            if (processTestZonePressureData)
            {
                DeleteFieldPage(true, "P_Process", parent);
                AddNewDataPage("P_Process", parent);
                FieldProcessing(pressXYFO);
            }

            if (isMainWell && processTestZoneFlowData)
            {
                DeleteFieldPage(true, "Q_Process", parent);
                AddNewDataPage("Q_Process",parent);
                FieldProcessing(flowXYFO);
            }
        }

        fieldPFO += pressXYFO;
        fieldQFO += flowXYFO;
    }


    void AutoFieldSetup()
    {
        MenuRootC::currTreeObj = NULL;
        fieldDataRoot.CloseUI();

        bool multiWells = MultiWellIDS() && processTestZonePressureData;

        autoRoot = &fieldDataRoot;

        // just for effect
        fieldDataRoot.pageLeafs[0]->pageObjects[0]->SetID("Pens&ColorMaps");
        fieldDataRoot.pageLeafs[0]->pageSelected = false;
        fieldDataRoot.pageLeafs[0]->pageExpanded = false;
        fieldDataRoot.pageLeafs[0]->UpdatePage();

        // delete downstream pages
        DeleteFieldPage(processTestZonePressureData, "P_Cartesian", 0);
        DeleteFieldPage(processTestZoneFlowData, "Q_Cartesian", 0);

        //
        FuncObjArray fieldPFO, fieldQFO, simPFO, simQFO;
        if (multiWells)
        {
            for (int i = 0; i < currentWells.Size(); i++)
            {
                MenuPageC* currWell = fieldDataRoot.FindPage(currentWells[i], 0);
                if (currWell)
                {
                    DeleteFieldPages(currWell);
                    if (currWell->childPages.IsEmpty())
                        DeleteFieldPage(true, currentWells[i], 0);
                }
            }

            for (int i = 0; i < currentWells.Size(); i++)
            {
                wellID = currentWells[i];
                MenuPageC* currWell = fieldDataRoot.FindPage(wellID, 0);
                if (!currWell)
                {
                    AddNewDataPage(wellID, 0);
                    currWell = autoPage;
                }

                AddFieldPages(autoPage, i == 0, fieldPFO, fieldQFO);
                simPFO += DataCaptureStaticSupport::GetWellPFO(wellID);
                simQFO += DataCaptureStaticSupport::GetWellQFO(wellID);
            }

        }
        else
        {
            DeleteFieldPages(0);
            wellID = currentWells[0];
            AddFieldPages(0, true, fieldPFO, fieldQFO);
            simPFO += DataCaptureStaticSupport::GetWellPFO(wellID);
            simQFO += DataCaptureStaticSupport::GetWellQFO(wellID);
        }


        // set data capture
    //  dataCapture.SetToInitial();   // sets s_PDAT & s_QDAT

        FuncObjC* pressSimFO = 0;
        FuncObjC* flowSimFO = 0;
        if (dataCapture.DataCaptureOK())
        {
            pressSimFO = dataCaptureData[0]->GetObjRef();
            if (processTestZoneFlowData)
                flowSimFO = dataCaptureData[1]->GetObjRef();
        }

        if (createPressurePlot && processTestZonePressureData)
        {
            AddNewXYPage("Fld", "P_Cartesian", true, true);

            if (multiWells)
            {
                for (int i = 0; i < currentWells.Size(); i++)
                {
                    char objID[80];
                    MakeString(objID, currentWells[i], "_Pressure", 80);
                    PFO_2DXYData* currXY = AddXYDataPFO(objID);

                    MakeString(objID, currentWells[i], " measured pressure", 80);
                    SetXYDisplay(*currXY, 0, FuncObjRef(fieldPFO[i], 0), ps_FCircle, objID);
                    currXY->xySeries[0].seriesPen = i + 1;

                    MakeString(objID, currentWells[i], " simulated pressure", 80);
                    SetXYDisplay(*currXY, 1, FuncObjRef(simPFO[i], 0), "Simulated pressure");
                    currXY->xySeries[1].seriesPen = i + 1;
                }
            }
            else
            {
                PFO_2DXYData* currXY = AddXYDataPFO("Pressure");
                SetXYDisplay(*currXY, 0, FuncObjRef(fieldPFO[0], 0), ps_FCircle, "Measured pressure");
                SetXYDisplay(*currXY, 1, FuncObjRef(simPFO[0], 0), "Simulated pressure");
            }

            AddNewObject("PPO_SequenceGridLines", "Sequences", true);
            RecalcPage();
        }

        if (createFlowPlot && processTestZoneFlowData)
        {
            AddNewXYPage("Fld", "Q_Cartesian", true, true);
            PFO_2DXYData* currXY = AddXYDataPFO("Flow");
            SetXYDisplay(*currXY, 0, FuncObjRef(fieldQFO[0], 0), ps_FCircle, "Field flow rate");
            SetXYDisplay(*currXY, 1, FuncObjRef(simQFO[0], 0), "Simulated flow rate");
            AddNewObject("PPO_SequenceGridLines", "Sequences", true);
            RecalcPage();
        }

        RecalcAll();
    }


    static int DelFieldPagesCount(MenuPageC* parent)
    {
        int delCount = 0;
        if (processTestZonePressureData && processTestZoneFlowData &&
            (inputDataFormat != idfXY) && bothInSameFile)
            delCount += FieldPageExists(true, "PQ_Input", parent);

        delCount += FieldPageExists(processTestZonePressureData, "P_Input", parent);
        delCount += FieldPageExists(processTestZoneFlowData, "Q_Input", parent);

        delCount += FieldPageExists(processTestZonePressureData, "P_Process", parent);
        delCount += FieldPageExists(processTestZoneFlowData, "Q_Process", parent);

        return delCount;
    }

    int AutoFieldWillDeletePages()
    {
        int delCount = 0;
        if (MultiWellIDS() && processTestZonePressureData)
        {
            for (int i = 0; i < currentWells.Size(); i++)
            {
                MenuPageC* currWell = fieldDataRoot.FindPage(currentWells[i], 0);
                if (currWell)
                    delCount += DelFieldPagesCount(currWell);
            }
        }
        else
            delCount += DelFieldPagesCount(0);

        delCount += FieldPageExists(processTestZonePressureData, "P_Cartesian", 0);
        delCount += FieldPageExists(processTestZoneFlowData, "Q_Cartesian", 0);

        return delCount;
    }

}; // end NS

