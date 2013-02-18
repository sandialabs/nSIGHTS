///////////////////////////////////////////////////////////////////////////////////
//
// A_Main.cpp
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
//      Implements auto setup feature.
//
///////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <genApp/C_AllocObj.h>
#include <genApp/C_AppMenu.h>
#include <genApp/C_MenuRoot.h>
#include <genApp/C_PlotPage.h>
#include <genApp/C_DataPage.h>
#include <genApp/DefaultObjects.h>

#include <genPlotClass/PD_2DXY.h>
#include <genPlotClassPS/PC_PlatformPS.h>

#include <genPFO/PFO_DataLabels.h>
#include <genPFO/PFO_UserLabels.h>
#include <objPFO/PFO_2DXYData.h>

#include <osMain/G_Optimize.h>
#include <Var/G_DataCapture.h>

#include <Auto/A_Main.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace nsAutoSetup  {

    //  declarations and initial values

    MenuRootC* autoRoot;
    MenuPageC* autoPage;
    SC_StringArray  currentWells;

    bool processTestZonePressureData = true;
    bool processTestZoneFlowData = true;
    bool processObsWellPressureData = true;

    MenuObjC* AddNewObject(const char* objAlloc, const char* objID, bool execute)
    {
        MenuObjC* newObj = AllocObjC::CreateMenuObject(objAlloc, autoPage);
        newObj->SetID(objID);
        autoPage->AddObject(newObj);
        autoRoot->AddObjectLeafToRoot(autoPage, newObj);

        if (execute)
        {
            newObj->objFunction->Execute();
        }

        return newObj;
    }


    MenuObjC*   CopyNewObject(MenuObjC* objToCopy, const char* objID, bool execute)
    {
        MenuObjC* copyObj = AllocObjC::CopyMenuObject(*objToCopy, autoPage);
        copyObj->SetID(objID);
        autoPage->AddObject(copyObj);
        autoRoot->AddObjectLeafToRoot(autoPage, copyObj);

        if (execute)
        {
            copyObj->objFunction->Execute();
        }

        return copyObj;
    }

    void RecalcPage(MenuPageC* currPage)
    {
        if (currPage->IsPlotPage())
        {
            // slight kluge .....
            PC_PlatformPS* currPlatform = static_cast<PC_PlatformPS*>(static_cast<PlotPageC*>(currPage)->GetPlot()->platformData);
            currPlatform->ResetView();
        }
        else
        {
            FuncObjArray recalcFO;
            recalcFO.SetResizable(10);

            for (int i = 0; i < currPage->pageObjects.Size(); i++)
            {
                MenuObjC* currObj = currPage->pageObjects[i];
                if (currObj->objFunction)
                    recalcFO += currObj->objFunction;
            }

            if (!recalcFO.IsEmpty())
            {
                FuncObjC::CalcSelectedObjects(foc_Apply, recalcFO);
                PlotDefC::ReplotAfterCalc();
            }
        }

    }
    void RecalcPage()
    {
        RecalcPage(autoPage);
    }


    void RecalcAll()
    {
        //  recalc all objects to make sure links are OK
        FuncObjC::CalcAllObjects(foc_PortOnly);
        PlotDefC::ReplotAfterCalc();
    }


    void AddNewDataPage(const char*      pageID,
                              MenuPageC* dataParent)
    {
        autoPage = AppMenuC::CreateNewMenuPage(DataPageAlloc::PageTypeName());

        autoPage->parentPage = dataParent;
        if (dataParent != NULL)
            dataParent->childPages += autoPage;

        autoPage->pageExpanded = true;
        autoPage->pageObjects[0]->SetID(pageID);
        autoRoot->AddPageLeafToRoot(autoPage);

        autoPage->UpdatePage();
    }

    void AddNewXYPage(const char* rootID, const char* pageID, bool linX, bool linY)
    {
        autoPage = AppMenuC::CreateNewMenuPage(Plot2DXYPageAlloc::PageTypeName());

        PD_2DXY* currPlot = static_cast<PD_2DXY*>(static_cast<PlotPageC*>(autoPage)->GetPlot());
        currPlot->preID = rootID;
        currPlot->xAxis.axesAreLinear = linX;
        currPlot->yAxis.axesAreLinear = linY;
        PlotDefC::ExecuteAndReplot(*currPlot);

        autoPage->pageExpanded = true;
        autoPage->pageObjects[0]->SetID(pageID);
        autoRoot->AddPageLeafToRoot(autoPage);

        MenuObjC* annoObj = AddNewObject("PPO_DataLabels", "VersionInfo");
        PFO_DataLabels& annoFO = *(static_cast<PFO_DataLabels*>(annoObj->objFunction));
        
        PFO_DataLabels::DataLabel& vLab = annoFO.labelData[0];
        vLab.doLabel = true;
        vLab.labelObjRef = FuncObjRef(appDefaultObjects::sysInfoObj, 0);
        vLab.overrideMainLabel = true;

        PFO_DataLabels::DataLabel& tLab = annoFO.labelData[1];
        tLab.doLabel = true;
        tLab.labelObjRef = FuncObjRef(appDefaultObjects::sysInfoObj, 2);
        tLab.overrideMainLabel = true;
        annoFO.ulLocation = Point2D(90.0, 95.0);

        annoObj = AddNewObject("PPO_DataLabels", "OptInfo");
        PFO_DataLabels& optFO = *(static_cast<PFO_DataLabels*>(annoObj->objFunction));
        PFO_DataLabels::DataLabel& oLab = optFO.labelData[0];
        oLab.doLabel = true;
        oLab.labelObjRef = FuncObjRef(optProgressOutput, 0);
        optFO.doPlot = false;
        optFO.ulLocation = Point2D(60.0, 95.0);

        annoObj = AddNewObject("PPO_UserLabels", "Title");
        PFO_UserLabels& labFO = *(static_cast<PFO_UserLabels*>(annoObj->objFunction));
        PFO_UserLabels::UserLabel& titLab = labFO.labelData[0];
        titLab.doLabel = true;
        CopyString(titLab.userLabel, "Enter Title", PFO_UserLabels::UserLabel::userLabelLen);
        labFO.legendFont.fontSize = 12;
        labFO.ulLocation = Point2D(25.0, 95.0);

        autoPage->UpdatePage();
    }

    PFO_2DXYData*   AddXYDataPFO(const char* objID)
    {
        MenuObjC* plotObj = AddNewObject("PPO_2DXYData", objID);
        return static_cast<PFO_2DXYData*>(plotObj->objFunction);
    }


    void IncrementID(const char* baseID,
                           char* nextID,
                           int&  idNum)
    {
        CopyString(nextID, baseID, idLen);
        ConcatInt(idNum++, nextID, 3, idLen);
    }


    void SetXYDisplay(          PFO_2DXYData&   objPtr,
                                int             index,
                        const   FuncObjRef&     inRef,
                                bool            isSymbol,
                                PlotSymbol      symType,
                        const   char*           objDesc)
    {
        PFO_2DXYData::XYSeries& currSpec = objPtr.xySeries[index];
        currSpec.doSeries = true;
        currSpec.xyDataObjRef = inRef;
        if (isSymbol)
        {
            currSpec.seriesPlotType = psl_SymbolOnly;
            currSpec.symbolType = symType;
        }
        else
            currSpec.seriesPlotType = psl_LineOnly;

        if (objDesc != NULL)
        {
            currSpec.overrideDefaultLabel = true;
            CopyString(currSpec.overrideLabel, objDesc, PSC_LegendOverride::labelLen);
        }
    }

    void SetXYDisplay(          PFO_2DXYData&   objPtr,
                                int             index,
                        const   FuncObjRef&     inRef,
                                PlotSymbol      symType,
                        const   char*           objDesc)
    {
        SetXYDisplay(objPtr, index, inRef, true, symType, objDesc);
    }

    void SetXYDisplay(          PFO_2DXYData&   objPtr,
                                int             index,
                        const   FuncObjRef&     inRef,
                        const   char*           objDesc)
    {
        SetXYDisplay(objPtr, index, inRef, false, ps_X, objDesc);
    }


    static SC_StringArray     listObjDesc;
    FuncObjRefArray           autoObjects;
    static bool               objRefOK;

    void SetObjRef(const type_info&     objType)
    {
        MenuObjC::currMenuObj = 0;
        autoObjects.SetEmpty();
        objRefOK = AppMenuC::GetFuncObjList(objType, listObjDesc, autoObjects, "::");
    }

    FuncObjRef  GetMatchingObj(const char* idToMatch)
    {
        if (objRefOK)
            for (int i = 0; i < autoObjects.Size(); i++)
                if (autoObjects[i].objRef && SameString(idToMatch, autoObjects[i].objRef->GetID()))
                    return autoObjects[i];

        return FuncObjRef();
    }


    char seqID[idLen];
    char fieldID[idLen];
    char simID[idLen];
    const char* wellID;


    void MakeObjID(char* outID, const char* suffix1, const char* suffix2)
    {
        MakeString(outID, seqID, suffix1, suffix2, idLen);
    }

    void MakeObjID(char* outID, const char* suffix1, const char* suffix2, const char* suffix3)
    {
        MakeString(outID, seqID, suffix1, suffix2, suffix3, idLen);
    }

    void MakeObjID(char* outID, const char* suffix)
    {
        MakeString(outID, seqID, suffix, idLen);
    }

    void MakeObjID(const char* suffix)
    {
        MakeObjID(fieldID, "_f", suffix);
        MakeObjID(simID, "_s", suffix);
    }

    MenuObjC* lastObject;

    FuncObjC* AddFieldObject(const char* objID)
    {
        lastObject = AddNewObject(objID, fieldID);
        return lastObject->objFunction;
    }

    FuncObjC* AddSimObject()
    {
        lastObject = CopyNewObject(lastObject, simID);
        return lastObject->objFunction;
    }

    bool MultiWellIDS()
    {
        currentWells.SetEmpty();
        bool multiWells = DataCaptureStaticSupport::GetWellIDs(currentWells) 
                        && (currentWells.Size() > 1) && processObsWellPressureData;

        if ((!multiWells) && currentWells.IsEmpty())
        {
            currentWells.Alloc(1);
            currentWells += "DAT";
        }

        return multiWells;
    }

}; // end NS

