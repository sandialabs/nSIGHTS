///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadTablePS.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/U_Str.h>

#include <genClassPS/PS_UIBase.h>

#include "DPO_ReadTablePS.h"


void DPO_ReadTablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ReadTablePS*>(inObj)->SetAllSensitive();
}

void DPO_ReadTablePS::SetAllSensitive()
{
    bool notTecplot = tableFileFormatUI.GetChoice() != 2;
    optionsFrame.SetSensitive(notTecplot);
    commentCharsUI.SetSensitive(notTecplot && ignoreCommentsUI.GetValue());
    readRowIDsUI.SetSensitive(tableFileFormatUI.GetChoice() < 2);

    bool basicOnly = tableFileFormatUI.GetChoice() < 2;

    averageMultipleColumnsUI.SetSensitive(basicOnly);
    multipleColumnCountUI.SetSensitive(basicOnly && averageMultipleColumnsUI.GetValue());

}

void DPO_ReadTablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.StdFormAttach();
    dataFile.InitFramedUI("Table File ", PPform.GetWidget(), args);

    args.StdTopObjectAttach(dataFile);
    nextRow.InitWidget(PPform.GetWidget(), args);

    static const char*  formatLabels[] = {"  Basic  ", "  CSV  ", "  Tecplot  ", " Multiple Row", "  Other  "};
    args.StdSplitSetup(25);
    tableFileFormatUI.PreInitSetUp(formatLabels, 5);
    tableFileFormatUI.InitFramedUI("File Format", nextRow.GetWidget(), args);
    tableFileFormatUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(75);
    optionsFrame.InitFramedUI("Options", nextRow.GetWidget(), args);

    args.StdToggleSetup(2);
    readColumnIDsUI.StdToggleInit("read col IDs", optionsFrame.GetForm(), args);

    args.StdToggleSetup(20);
    readRowIDsUI.StdToggleInit("read row IDs", optionsFrame.GetForm(), args);

    args.StdToggleSetup(40);
    setNullsToZeroUI.StdToggleInit("nulls to 0.0", optionsFrame.GetForm(), args);

    args.StdToggleSetup(60);
    ignoreCommentsUI.StdToggleInit("ignore comments", optionsFrame.GetForm(), args);
    ignoreCommentsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(80);
    averageMultipleColumnsUI.StdToggleInit("avg mult cols", optionsFrame.GetForm(), args);
    averageMultipleColumnsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(nextRow.GetWidget());
    secondRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    commentCharsUI.AttachObjectToFrame();
    commentCharsUI.InitFramedUI("Comment Chars", secondRow.GetWidget(), args);

    args.StdSplitMiddle(33, 66);
    multipleColumnCountUI.AttachObjectToFrame();
    multipleColumnCountUI.InitFramedUI("# cols to avg", secondRow.GetWidget(), args);

    args.StdSplitFinish(33);
    mpiFilesUI.nodeFileID += "table file";
    mpiFilesUI.StdInit(secondRow.GetWidget(), args);

    args.StdTopObjectAttach(secondRow.GetWidget());
    statusGroup.statusID += "# of columns";
    statusGroup.statusID += "# of rows";
    statusGroup.StdInit("Table Status", PPform.GetWidget(), args);
}

void  DPO_ReadTablePS::MenuClose()
{
    dataFile.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadTablePS::SetMenuData(DPO_ReadTable&    inData)
{
    MenuBaseC::SetMenuData(inData);

    dataFile.SetFieldString(inData.tableFname);
    tableFileFormatUI.SetChoice(int(inData.tableFileFormat));
    readColumnIDsUI.SetValue(inData.readColumnIDs);
    readRowIDsUI.SetValue(inData.readRowIDs);
    averageMultipleColumnsUI.SetValue(inData.averageMultipleColumns);
    multipleColumnCountUI.SetFieldValue(inData.multipleColumnCount);

    setNullsToZeroUI.SetValue(inData.setNullsToZero);
    ignoreCommentsUI.SetValue(inData.ignoreComments);

    commentCharsUI.SetFieldString(inData.commentChars);
    mpiFilesUI.SetNodeFileArray(inData.mpiFiles);

    statusGroup.SetStatus(inData, inData.tableData.GetNCols(),
                                  inData.tableData.GetNRows());

    SetAllSensitive();
}

bool DPO_ReadTablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataFile.FieldFileOK(allOK, "Table file", true, false);
    if (ignoreCommentsUI.GetValue())
    {
        char testStr[DPO_ReadTable::maxCommentChars];
        commentCharsUI.FieldStringOK(allOK, "Comment chars", testStr, DPO_ReadTable::maxCommentChars);
        if (allOK)
        {
            FullTrim(testStr);
            if (IsNull(testStr))
            {
                PropErrorMsg("Comment chars cannot be all blank");
                allOK = false;
            }
        }

    }

    if ((tableFileFormatUI.GetChoice() < 2) && averageMultipleColumnsUI.GetValue())
        multipleColumnCountUI.FieldValueOK(allOK, "# of cols to average", 2, 10000);

    return allOK;
}

void DPO_ReadTablePS::GetMenuData(DPO_ReadTable&    outData)
{
    MenuBaseC::GetMenuData(outData);
    dataFile.GetFieldFile(outData.tableFname);
    outData.tableFileFormat = DFO_ReadTable::TableFileFormat(tableFileFormatUI.GetChoice());
    mpiFilesUI.GetNodeFileArray(outData.mpiFiles);

    if (outData.tableFileFormat != DFO_ReadTable::tffTecplot)
    {
        outData.readColumnIDs = readColumnIDsUI.GetValue();

        if (outData.tableFileFormat != DFO_ReadTable::tffMultRows)
            outData.readRowIDs = readRowIDsUI.GetValue();

        outData.setNullsToZero = setNullsToZeroUI.GetValue();
        outData.ignoreComments = ignoreCommentsUI.GetValue();
        if (outData.ignoreComments)
            commentCharsUI.GetFieldString(outData.commentChars, DPO_ReadTable::maxCommentChars);

        if (outData.tableFileFormat < DFO_ReadTable::tffTecplot)
        {
            outData.averageMultipleColumns = averageMultipleColumnsUI.GetValue();
            if (outData.averageMultipleColumns)
                outData.multipleColumnCount = multipleColumnCountUI.GetFieldValue();
        }
    }
}

void DPO_ReadTablePS::MenuClear()
{
    dataFile.SetFieldString(NULL);
}

void DPO_ReadTablePS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    MenuClear();
    tableFileFormatUI.SetChoice(0);
    readColumnIDsUI.SetValue(true);
    readRowIDsUI.SetValue(false);
    SetAllSensitive();
}

