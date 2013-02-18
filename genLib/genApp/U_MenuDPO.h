///////////////////////////////////////////////////////////////////////////////////
//
// U_MenuDPO.h
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              basic UI support classes used primarily by DPO_XXXPS functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef U_MENUDPO_H
#define U_MENUDPO_H

#include <genClass/C_MPISupport.h>
#include <genClass/DC_ColorMap.h>
#include <genDataClass/DSC_TimeBase.h>
#include <genDataClass/DSC_Threshold.h>
#include <genDataClass/DSC_RealMSSpec.h>
#include <genDataClass/DSC_IndexMSSpec.h>

#include <genApp/ExposedReal.h>
#include <genApp/ExposedList.h>
#include <genApp/U_Menu.h>

#include <genClassPS/PS_UIClass.h>


using namespace genClassPS;

namespace genAppMenuUtilities {


    extern void PropSetColorLabels(genClassPS::BaseUIC* labeledUIC[],  // assumed 3
                                   bool                 useHSV);


    class ColorMapDisplay : public DrawingAreaWidget {
    public:
        DC_ColorMap colorMap;

    public:
        ColorMapDisplay();
        void        DrawColorMap(const DC_ColorMap& newMap);

    protected:
        virtual void OnDraw(CDC* pDC);
    };



    class TimeBaseUIC : public BaseUIC {
    public:
        OptionUIC        inputTBU;
        OptionUIC        outputTBU;

    public:
        void            StdInit(Widget           parent,
                                const ArgListC&  inArgs);

        void            SetTimeBase(const DSC_TimeBase& inTB);
        void            GetTimeBase(DSC_TimeBase&       outTB);
    };

    class ThresholdUIC : public BaseUIC {
    public:
        BaseUIC              minThresholdFrame;
        OptionChooserWidget  minThresholdUI;
        RealEntryWidget      minValUI;
        BaseUIC              maxThresholdFrame;
        OptionChooserWidget  maxThresholdUI;
        RealEntryWidget      maxValUI;

    public:
        void            StdInit(      Widget           parent,
                                      const ArgListC&  inArgs,
                                      bool             horizontal);

        bool            UIdataOK(bool& allOK);
        void            SetThreshold(const DSC_Threshold& inTh);
        void            GetThreshold(DSC_Threshold& outTh);
        void            SetInternalSensitive(); //used in CB - must be hooked to min/maxThresholdUI
    };



    class RealMSSpecUI : public BaseUIC {
    public:
        MasterSlaveUIC       realValueMS;
        RealEntryUIC         realValueUI;

    public:
        RealMSSpecUI(const type_info& inType);
        RealMSSpecUI(const type_info& inType, bool expOK);

        void                MenuInit(DSC_RealMSSpec& iSpec);

        void                SetSensitive(bool inSens);               // whole UI
        void                SetAllSensitive();                       // internal sensitive
        void                SetValue(const DSC_RealMSSpec& iSpec,    // w expose
                                     ExposedReal* exposedSpec);
        void                SetValue(const DSC_RealMSSpec& iSpec,    // w expose
                                     ExposedRealWrapper& exposedSpec);
        void                SetValue(const DSC_RealMSSpec& iSpec);   // w/o expose

        void                GetValue(DSC_RealMSSpec& oSpec);

        bool                TurnExposedOff() {return realValueMS.TurnExposedOff();}
        bool                                UIdataOK(bool& allOK);
        bool                                UIdataOK(          bool& allOK,
                                                               const double& minVal,
                                                               const double& maxVal);

        void                InitUI(char*        specID,
                                   ArgListC&    topAttachments,
                                   Widget       parent,
                                   bool         horizontal,
                                   bool         sameRow,
                                   void         (*ToggleCB)(void* inOb),
                                   void*        otherObj);

        // basic init or no expose init
        void                InitUI(char*        specID,
                                   ArgListC&    topAttachments,
                                   Widget       parent,
                                   bool         horizontal,
                                   bool         sameRow);
    };


    class IndexMSSpecUI : public BaseUIC {
    public:
        MasterSlaveUIC      indexValueMS;
        ScrolledListWidget  indexListUI;
        ToggleBoxWidget     isMultiple;
    private:
        bool                snglmultOK;
        const DSC_IndexMSSpec*  currMS;
    public:
        IndexMSSpecUI(const type_info& inType);
        IndexMSSpecUI(const type_info& inType, bool expOK);

        void                MenuInit(DSC_IndexMSSpec& iSpec);

        void                SetSensitive(bool inSens);                              // whole UI
        void                SetAllSensitive();                                              // internal sensitive
        void                SetValue(const DSC_IndexMSSpec& iSpec,    // w expose
                                     ExposedListWrapper& exposedSpec);
        void                SetValue(const DSC_IndexMSSpec& iSpec);   // w/o expose

        void                UpdateList(const DSC_IndexMSSpec& iSpec);

        void                GetValue(DSC_IndexMSSpec& oSpec);

        bool                TurnExposedOff() {return indexValueMS.TurnExposedOff();}
        bool                                UIdataOK(bool& allOK);

        // most common -- single only
        void                InitUI(const char*  specID,
                                   ArgListC&    topAttachments,
                                   Widget       parent,
                                   void         (*ToggleCB)(void* inOb),
                                   void*        otherObj);

        // most common -- single and/or multiple
        void                InitUI(const char*        specID,
                                   ArgListC&        topAttachments,
                                   Widget       parent,
                                   void         (*ToggleCB)(void* inOb),
                                   void*         otherObj,
                                   bool          snglOK,
                                   bool          multOK);

        void                SetIndexSensitive();
    private:
        // basic init
        void                InitUI(const char*   specID,
                                   ArgListC&     topAttachments,
                                   Widget        parent,
                                   bool          snglOK,
                                   bool          multOK);

        static void         SetIndexSensitiveCB(void* inObj);
        void                SetIndexSelection();
        static void         SetIndexSelectionCB(void* inObj);

        void                SetSnglMultValue(const DSC_IndexMSSpec& iSpec);

        void                SetExposeSinglMult();   // kluge to set MS spec correctly before expose
        static void         SetExposeSinglMultCB(void* inObj);



    };

    class StatusGroupUIC : public BaseUIC {
    private:
        enum               {maxStatus = 4};
        IntStatusUIC       statusUI[maxStatus];
        int                ninit;

    public:
        SC_StringArray          statusID;

    public:
        StatusGroupUIC();

        void            StdInit(const char*       title,
                                Widget            parent,
                                const ArgListC&   inArgs);

        void            SetStatus(const FuncObjC& inObj,
                                  int             stat1);
        void            SetStatus(const FuncObjC& inObj,
                                  int             stat1,
                                  int             stat2);
        void            SetStatus(const FuncObjC& inObj,
                                  int             stat1,
                                  int             stat2,
                                  int             stat3);
        void            SetStatus(const FuncObjC& inObj,
                                  int             stat1,
                                  int             stat2,
                                  int             stat3,
                                  int             stat4);
    };


    class NodeFileArrayUIC : public BaseUIC {
    private:
        enum            {maxNodeFile = 8};
        int             ninit;

    public:
        ToggleBoxWidget nodeFileUI[maxNodeFile];
        SC_StringArray  nodeFileID;

    public:
        NodeFileArrayUIC();

        void            StdInit(Widget                parent,
                                const ArgListC&       inArgs);

        void            SetNodeFileArray(const NodeFileArray& inData);
        void            GetNodeFileArray(NodeFileArray& inData);
    };



};    // end namespace

#endif // U_MENU_H

