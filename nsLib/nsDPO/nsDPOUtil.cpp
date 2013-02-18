///////////////////////////////////////////////////////////////////////////////////
//
// nsDPOUtil.cpp
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

#include "nsLibAfx.h"

#include <nsClass/DO_SequenceTimes.h>

#include "nsDPOUtil.h"

namespace nsDPOMenuUtilities {


    TimeLimitsUIC::TimeLimitsUIC() :
        seqTimesObjRefUI(typeid(DO_SequenceTimes))
    {
    }

    void TimeLimitsUIC::SetAllSensitive()
    {
        bool useSeq  = (timeLimitSelectionUI.GetChoice() == 1);
        bool useUser  = (timeLimitSelectionUI.GetChoice() == 2);

        seqTimeFrame.SetSensitive(useSeq);
        if (useSeq)
        {
            startSeqUI.SetSensitive(seqListOK);
            endSeqUI.SetSensitive(seqListOK);
        }

        userTimeFrame.SetSensitive(useUser);
    }


    void TimeLimitsUIC::SetAllSensitiveCB(void* inObj)
    {
        static_cast<TimeLimitsUIC*>(inObj)->SetAllSensitive();
    }


    void TimeLimitsUIC::SetSequences()
    {
        SC_StringArray dataIDs;
        if (seqTimesObjRefUI.CheckDataObj())
        {
            DO_SequenceTimes* dataDO = static_cast<DO_SequenceTimes*>(seqTimesObjRefUI.GetDataObj());
            dataIDs	= dataDO->seqTimes->sequenceIDs;
        }

        seqListOK = !dataIDs.IsEmpty();
        if (seqListOK)
        {
            startSeqUI.SetListText(dataIDs);
            endSeqUI.SetListText(dataIDs);
        }
        else
        {
            startSeqUI.SetListNA();
            endSeqUI.SetListNA();
        }
        SetAllSensitive();
    }

    void TimeLimitsUIC::SetSequencesCB(void* inObj)
    {
        static_cast<TimeLimitsUIC*>(inObj)->SetSequences();
    }



    void TimeLimitsUIC::StdInit(Widget     parent,
                                ArgListC&  inArgs)
    {

        static const char* timeLimitSelectionLabels[]  = {"  All Input  "," Sequence Range ", "   Specified Range  "};
        timeLimitSelectionUI.PreInitSetUp(timeLimitSelectionLabels, 3, true);
        timeLimitSelectionUI.InitFramedUI("Time Data To Use", parent, inArgs);
        timeLimitSelectionUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

        ArgListC args;

        args.StdTopObjectAttach(timeLimitSelectionUI);
        mainForm.InitWidget(parent, args);

        args.StdSplitSetup();
        seqTimeFrame.InitFramedUI("Sequence Range", mainForm.GetWidget(), args);

        args.StdSplitFinish();
        userTimeFrame.InitFramedUI("Time Range", mainForm.GetWidget(), args);

        args.Clear();
        args.StdFormAttach();
        seqTimesObjRefUI.InitLabeledUI("Seq times", seqTimeFrame.GetForm(), args);
        timeLimitSelectionUI.AddOtherObjectCallback(SetSequencesCB, this);

        args.Clear();
        args.StdTopObjectAttach(seqTimesObjRefUI);
        startSeqUI.SetListWidth(20);
        startSeqUI.InitLabeledUI("Start", seqTimeFrame.GetForm(), args);

        args.Clear();
        args.StdTopObjectAttach(startSeqUI);
        endSeqUI.SetListWidth(20);
        endSeqUI.InitLabeledUI("End", seqTimeFrame.GetForm(), args);

        args.Clear();
        args.StdFormAttach();
        startValUI.InitLabeledUI("Minimum", userTimeFrame.GetForm(), args);

        args.Clear();
        args.StdTopObjectAttach(startValUI);
        endValUI.InitLabeledUI("Maximum", userTimeFrame.GetForm(), args);

    }

    bool TimeLimitsUIC::UIdataOK(bool& allOK)
    {
        if (timeLimitSelectionUI.GetChoice() == 1)
        {
            seqTimesObjRefUI.FuncObjRefOK(allOK, "Sequence times");
        }
        else if (timeLimitSelectionUI.GetChoice() == 2)
        {
            startValUI.FieldValueOK(allOK, "start");
            endValUI.FieldValueOK(allOK, "end");
        }
        return allOK;
    }

    void TimeLimitsUIC::StdMenuInit(DSC_TimeLimits& inData)
    {
        seqTimesObjRefUI.StdMenuInit(inData.seqTimesObjRef);
    }


    void TimeLimitsUIC::SetTimeLimits(const DSC_TimeLimits& inData)
    {
        timeLimitSelectionUI.SetChoice(inData.timeLimitSelection);

        seqTimesObjRefUI.SetFuncObjRef(inData.seqTimesObjRef);
        startSeqUI.SetSelection(inData.startSeq);
        endSeqUI.SetSelection(inData.endSeq);

        startValUI.SetFieldValue(inData.startVal);
        endValUI.SetFieldValue(inData.endVal);

        SetSequences();
    }

    void TimeLimitsUIC::GetTimeLimits(DSC_TimeLimits& outData)
    {
        outData.timeLimitSelection = DSC_TimeLimits::TimeLimitSelection(timeLimitSelectionUI.GetChoice());

        if (timeLimitSelectionUI.GetChoice() == 1)
        {
            outData.seqTimesObjRef = seqTimesObjRefUI.GetFuncObjRef();
            outData.startSeq = startSeqUI.GetSelection();
            outData.endSeq = endSeqUI.GetSelection();
        }
        else if (timeLimitSelectionUI.GetChoice() == 2)
        {
            outData.startVal = startValUI.GetFieldValue();
            outData.endVal = endValUI.GetFieldValue();
        }
    }


}; // end namespace

