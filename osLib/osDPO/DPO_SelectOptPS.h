///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectOptPS.h
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
//      I/O and user interface for all osDFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DPO_SELECTOPTPS_H
#define DPO_SELECTOPTPS_H

#include <genApp/U_Menu.h>
#include <genDPO/DPO_SelectBasePS.h>

#include "DPO_SelectOpt.h"

class DPO_SelectOptPS : public DPO_SelectBasePS {

    friend class DPO_SelectOpt;

    private:
        FuncObjRefUIC       optSimResultsObjRefUI;
        DC_OptSimResults*   dataDC;
        int                 currRunIndex;
        int                 currCaseIndex;
        int                 currFitIndex;

    protected:
        bool            showFitComponents;
        bool            showOKonly;
        bool            multipleOK;
        bool            singleOK;

        //  multiple select UI components
        FormWidget      buttonForm;
        PushButton      clearAllUI;
        PushButton      selectAllUI;
        PushButton      selectSameRunUI;
        PushButton      selectSameCaseUI;
        PushButton      selectSameFitUI;

    public:
                        DPO_SelectOptPS();

                void    InitMenu(ArgListC& topAttach, bool snglOK, bool multOK);

                void    SetMenuData(DPO_SelectOpt&   inData);
                bool    UIdataOK();
                void    GetMenuData(DPO_SelectOpt&  outData);

                void    MenuOpen(DPO_SelectOpt&  inData);

    protected:
                virtual void SetAllSensitive();
                void    SetIndexList();

    private:
        bool            SetCurrentIndexes();
        void            SetMatchIndexes(int runMatch, int caseMatch, int fitMatch);

        static  void    SetIndexListCB(void* inObj);

        void            ClearAll();
        static  void    ClearAllCB(void* inObj);

        void            SelectAll();
        static  void    SelectAllCB(void* inObj);

        void            SelectRun();
        static  void    SelectRunCB(void* inObj);

        void            SelectCase();
        static  void    SelectCaseCB(void* inObj);

        void            SelectFit();
        static  void    SelectFitCB(void* inObj);

        static  void    SetAllSensitiveCB(void* inObj);


};

#endif // !DPO_SELECTOPTPS_H

