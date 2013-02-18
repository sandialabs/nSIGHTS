///////////////////////////////////////////////////////////////////////////////////
//
// UFO_WriteTableCF.cpp
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
//      Basic utility classes for writing data.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>

#include <genClass/C_TxtFile.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/DC_TableData.h>
#include <genClass/DO_TableData.h>

#include <objUFO/UFO_WriteTable.h>

void UFO_WriteTable::WriteTableFile()
{
    const int minColWidth = 16;

    const DC_TableData& tmpDC = *(static_cast<DO_TableData*>(GetInPortData(tableObjRef)))->tableData;


    int colWidth = 0;
    if (writeRowDescs)
    {
        for (int i = 0; i < tmpDC.GetNRows(); i++)
        {
            int descLen = StringLength(tmpDC.rowDesc[i]);
            if (descLen > colWidth)
                colWidth = descLen;
        }
    }

    if (writeColHeaders)
    {
        for (int i = 0; i < tmpDC.GetNCols(); i++)
        {
            int descLen = StringLength(tmpDC.columnDesc[i]);
            if (descLen > colWidth)
                colWidth = descLen;
        }
    }

    colWidth++;
    if (colWidth < minColWidth)
        colWidth = minColWidth;

    try
    {
        TxtFileC outFile(outFname, false);

        char tempStr[80];
        if (writeColHeaders)
        {
            if (writeRowDescs)
            {
                CopyString(tempStr, "Desc", 80);
                LeftJustify(tempStr, colWidth);
                outFile.WriteSolidText(tempStr);
            }

            for (int i = 0; i < tmpDC.GetNCols(); i++)
            {
                CopyString(tempStr, tmpDC.columnDesc[i], 80);
                FillBlanks(tempStr);
                RightJustify(tempStr, colWidth);
                outFile.WriteSolidText(tempStr);
            }
            outFile.WriteLine();
        }

        SC_RealFormat tableFormat(ncf_Scientific, 9);

        for (int i = 0; i < tmpDC.GetNRows(); i++)
        {
            if (writeRowDescs)
            {
                CopyString(tempStr, tmpDC.rowDesc[i], 80);
                LeftJustify(tempStr, colWidth);
                outFile.WriteSolidText(tempStr);
            }

            for (int j = 0; j < tmpDC.GetNCols(); j++)
            {
                char tempStr[80];
                tableFormat.RealToString(tmpDC.dataTable[j][i], tempStr, 80);
                RightJustify(tempStr, colWidth);
                outFile.WriteSolidText(tempStr);
            }
            outFile.WriteLine();
        }

        outFile.CloseFile();
    }
    catch (TextC::TextError& re) {
        GenAppErrorMsg("WriteTable", re.errMsg);
        return;
    }

    GenAppInfoMsg("WriteTable", "file written successfully");
}

