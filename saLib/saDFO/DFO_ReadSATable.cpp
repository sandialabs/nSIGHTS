///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadSATable.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/C_MessageLog.h>
#include <saClass/IO_SATable.h>

#include <saDFO/DFO_ReadSATable.h>

DFO_ReadSATable :: DFO_ReadSATable() :
    FuncObjC("Read SA Table")
{
    saFileFormat = IO_SATable::saffSandiaWIPP;
    SetToNull(saFname);

    CopyString(goldsimDepVar, "GSDepVar", DC_SATable::varNameLen);
    translateIndepVarNames = false;
    selectOnlyNewNames = false;
    sortByNames = false;

    SetToNull(varNameFname);

    InitCommon();
}

DFO_ReadSATable::DFO_ReadSATable(const DFO_ReadSATable& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadSATable::InitCommon()
{
    saDataDO.saTable = &saDataDC;
    fileNameDO.SetTypeLabel("SA Table file");
    AddOutPort(saDataDO);
    AddOutPort(fileNameDO);
    DoStatusChk();
}

DFO_ReadSATable& DFO_ReadSATable::operator= (const DFO_ReadSATable& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadSATable::LocalCopy(const DFO_ReadSATable& a)
{
    CopyString(saFname, a.saFname, stdFileStrLen);
    CopyString(goldsimDepVar, a.goldsimDepVar, DC_SATable::varNameLen);
    CopyString(varNameFname, a.varNameFname, stdFileStrLen);

    saFileFormat            = a.saFileFormat;
    translateIndepVarNames  = a.translateIndepVarNames;
    selectOnlyNewNames      = a.selectOnlyNewNames;
    sortByNames             = a.sortByNames;
}

void  DFO_ReadSATable:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (IsNull(saFname))
        SetObjErrMsg("file name not set");

    if (!saDataDO.DataOK())
        SetObjErrMsg("SA table file not read");
}

void DFO_ReadSATable::CalcOutput(FOcalcType  calcType)
{
    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        if (!CheckFileExists(saFname))
        {
            SetObjErrMsg("file does not exist");
            return;
        }

        objStatus = os_OK;
        IO_SATable inFile;
        if (!inFile.ReadSATable(saFname, saFileFormat, saDataDC))
        {
            SetObjErrMsg(MessageLogC::GetLastMessage());
            return;
        }

        switch (saFileFormat)
        {
            case IO_SATable::saffSandiaWIPP : {
                break;
            }
            case IO_SATable::saffGoldsimDepVar : {
                saDataDC.variableNames.SetString(goldsimDepVar, 0);
                break;
            }
            case IO_SATable::saffGoldsimIndepVar : {
                if (translateIndepVarNames && (!TranslateVariableNames()))
                {
                    SetObjErrMsg(MessageLogC::GetLastMessage());
                    return;
                }
                break;
            }
            default: {
                break;
            }
        }

        saDataDC.SetID("Data read");
        fileNameDO.SetFileValueLabel(saFname);
        SetDefaultFileID(saFname);
        saDataDC.SetID(GetID());
    }

    DoStatusChk();
}


bool DFO_ReadSATable::TranslateVariableNames()
{
    try
    {
        ConvFileC varFile(varNameFname);
        varFile.CFsetMsgStuff("ReadVar", "VarNameFile", 100);

        char oldVarStr[DC_SATable::varNameLen];
        char newVarStr[DC_SATable::varNameLen];

        SC_StringArray& varStrs = saDataDC.variableNames;
        int ncurrVar = saDataDC.GetNVariable();
        SC_IntArray namedVars(ncurrVar);
        namedVars.SetEmpty();
        while((!varFile.CFeof()) && varFile.CFreadText())
        {
            varFile.CFgetNextStr(oldVarStr, DC_SATable::varNameLen);
            varFile.CFgetNextStr(newVarStr, DC_SATable::varNameLen);
            int varIndex = varStrs.SearchForKey(oldVarStr);
            if (varIndex >= 0)
            {
                namedVars.AddIfNotFound(varIndex);
                varStrs.SetString(newVarStr, varIndex);
            }
            else
            {
                char badMatch[80];
                MakeString(badMatch, "cannot find match for ", oldVarStr, 80);
                varFile.CFtextError(badMatch);
            }
        }

        int nnewVar = namedVars.Size();
        if (selectOnlyNewNames && namedVars.IsNotEmpty() && (ncurrVar > nnewVar))
        {
            namedVars.Sort(true);
            int nextVar = 0;
            SC_IntArray& varIndexes = saDataDC.variableIndexes;
            for (int i = 0; i < nnewVar; i++)
            {
                int newVar = namedVars[i];
                if (newVar > nextVar)
                {
                    for (int j = 0; j < saDataDC.GetNStep(); j++)
                    {
                        SC_DoubleMatrix& varData = saDataDC.saData[j];
                        varData[nextVar] = varData[newVar];
                    }
                    varStrs.SetString(varStrs[newVar], nextVar);
                    varIndexes[nextVar] = varIndexes[newVar];
                }
                nextVar++;
            }
            varStrs.SetSize(nextVar);
            varIndexes.SetSize(nextVar);
            for (int j = 0; j < saDataDC.GetNStep(); j++)
                saDataDC.saData[j].SetSize(nextVar);
        }

        if (sortByNames)
        {
            // copy
            DC_SATable temp(saDataDC);

            int nvar = temp.GetNVariable();
            SC_IntArray sortIndexes;
            sortIndexes.AllocAndIndex(nvar);
            saDataDC.variableNames.Sort(sortIndexes);

            for (int i = 0; i < nvar; i++)
            {
                int sIndx = sortIndexes[i];
                for (int j = 0; j < saDataDC.GetNStep(); j++)
                {
                    saDataDC.saData[j][i] = temp.saData[j][sIndx];
                }
                saDataDC.variableIndexes[i] = temp.variableIndexes[sIndx];
            }

        }

    }
    catch (ConvFileC::CFReadError re) {
        GenAppErrorMsg("VarFile", re.errMsg);
        return false;
    }

    return true;
}

