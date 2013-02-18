///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadSequenceTimesPS.cpp
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

//
// Program      : mView
// Module       : DPO_ReadSequenceTimesPS
// Programmer   : John Avis,   INTERA Inc.
// Purpose      :

//
// Last Modification Date:
//
// Revision History:
//
#include "nsLibAfx.h"

#include <genClassPS/PS_UIBase.h>

#include "DPO_ReadSequenceTimesPS.h"


void DPO_ReadSequenceTimesPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.StdFormAttach();
    sequenceTimesFnameUI.InitFramedUI("Sequence Time Data File ", PPform.GetWidget(), args);
    sequenceTimesFnameUI.SetPromptAndMask("Sequence time file to read",
        "Seq Time Files (*.seqt)|*.seqt|All Files (*.*)|*.*|||");


    args.Clear();
    args.StdTopObjectAttach(sequenceTimesFnameUI);
    statusFrame.InitFramedUI("Data Status", PPform.GetWidget(), args);

    args.Clear();
    args.LeftOffset(10);
    args.RightOffset(10);
    args.StdFormAttach(5);
    nSequence.InitLabeledUI("# of sequences", statusFrame.GetForm(), args);
}

void  DPO_ReadSequenceTimesPS::MenuClose()
{
    sequenceTimesFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadSequenceTimesPS::SetMenuData(DPO_ReadSequenceTimes&    inData)
{
    sequenceTimesFnameUI.SetFieldString(inData.sequenceTimesFname);

    bool isOK = (inData.GetStatus() == os_OK);
    if (isOK)
    {
        nSequence.SetFieldValue(inData.sequenceTimesDC.Size());
    }
    else
    {
        nSequence.SetStatusNA();
    }
    statusFrame.SetSensitive(isOK);

    MenuBaseC::SetMenuData(inData);
}

bool DPO_ReadSequenceTimesPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    sequenceTimesFnameUI.FieldFileOK(allOK, "Sequence time file", true, false);
    return allOK;
}

void DPO_ReadSequenceTimesPS::GetMenuData(DPO_ReadSequenceTimes&    outData)
{
    MenuBaseC::GetMenuData(outData);
    sequenceTimesFnameUI.GetFieldFile(outData.sequenceTimesFname);
}

void DPO_ReadSequenceTimesPS::MenuClear()
{
    sequenceTimesFnameUI.SetFieldString(NULL);
}

