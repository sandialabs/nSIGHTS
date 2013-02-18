///////////////////////////////////////////////////////////////////////////////////
//
// G_Vary.cpp
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
//      more support for multiple value runs.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genApp/AppConfigFile.h>

#include <osMain/osConfigFile.h>

#include <osMain/G_Vary.h>

namespace osVary  {

    VaryErrorListing    varyErrorListing;

    VaryStaticSupport   vary;
    VaryVarArray        orderedVary;


    VaryListing::VaryListing()
        :ListObjC("Vary")
    {
    }

    void VaryListing::CreateListing()
    {
        StdTitle("Suite/Range Setup");

        VaryVarArray currVary;

        AddStdInt("# of suite/range variables", VaryVar::GetnActive());

        SC_SetupErr tempErr;
        for (int i = 0; i < currVary.Size(); i++)
            AddStatus(currVary[i]->GetLongID(), currVary[i]->VarySetupOK(tempErr));
    }

    VaryErrorListing::VaryErrorListing()
        :ErrorListObjC("Vary")
    {
    }

    void VaryErrorListing::CreateListing()
    {
        StdTitle("Range/Suite Setup Errors");

        SC_SetupErr varyErr;

        VaryVarArray currVary;
        int nVary = currVary.Size();
        if (nVary > maxVary)
        {
            AddError("maximum of 3 variables can be suite/range");
        }

        if (IsRange() && (nVary  < 2))
        {
            AddError("at least 2 range variables must be set");
        }


        AddStdInt("# of suite/range variables", VaryVar::GetnActive());

        for (int i = 0; i < nVary; i++)
            if (!currVary[i]->VarySetupOK(varyErr))
                AddError(varyErr);

    }



    VaryStaticSupport::VaryStaticSupport()
    {
    }

    void VaryStaticSupport::SetToInitial()
    {
        varyIsRange = false;
        orderedVary.Alloc(maxVary);
    }


    void VaryStaticSupport::SetForFlags()
    {
        SetupOrdered();
    }

    int VaryStaticSupport::GetnVaryCombinations()
    {
        SetupOrdered();
        int nCombo = 1;
        for (int i = 0; i < orderedVary.Size(); i++)
            nCombo *= orderedVary[i]->GetnVary();
        return nCombo;
    }

    void VaryStaticSupport::SetVaryIndexes(int comboNum)
    {
        VaryVar::SetVaryControlIndex(comboNum);
        for (int i = orderedVary.UpperBound(); i >= 0; i--)
        {
            VaryVar& currVar = *(orderedVary[i]);
            currVar.varyIndex = comboNum % currVar.GetnVary();
            comboNum /= currVar.GetnVary();
        }
    }


    void VaryStaticSupport::GetCaseID(char* idStr, int maxLen)
    {
        if (orderedVary.IsEmpty())
        {
            CopyString(idStr, "OnlyCase", maxLen);
            return;
        }

        int padLen = 1;
        int i;
        for (i = 0; i < orderedVary.Size(); i++)
        {
            int nSel = orderedVary[i]->GetnVary();
            if (nSel > 99)
                padLen = 3;
            else if ((nSel > 9) && (padLen < 2))
                padLen = 2;
        };

        SetToNull(idStr);
        for (i = 0; i < orderedVary.Size(); i++)
        {
            char tempStr[20];
            IntToString(orderedVary[i]->varyIndex + 1, tempStr, padLen, 20);
            ConcatString(idStr, tempStr, maxLen);
        }

    }


    void VaryStaticSupport::GetCaseValues(SC_DoubleArray& caseVals)
    {
        caseVals.Alloc(orderedVary.Size());
        for (int i = 0; i < orderedVary.Size(); i++)
            caseVals += orderedVary[i]->GetFinalVaryVal();
    }

    void VaryStaticSupport::SetupOrdered()
    {
        VaryVar::GetActiveArray(orderedVary);

        // sort by vary order
        for (int i = 0; i < orderedVary.Size() - 1; i++)
            for (int j = i + 1; j < orderedVary.Size(); j++)
                if (orderedVary[i]->varyOrder > orderedVary[j]->varyOrder)
                    orderedVary.Swap(i, j);

    }

    bool VaryStaticSupport::VaryOK()
    {
        bool retVal = false;
        if (varyErrorListing.ErrorCheckOK())
        {
            retVal = true;
            SetupOrdered();
        }
        return retVal;
    }


    const char* VaryStaticSupport::groupHeader = "VaryingPars";
    const int  VaryStaticSupport::majorVersion = 0;
    const int  VaryStaticSupport::minorVersion = 0;

    void VaryStaticSupport::WriteToFile()
    {
        WriteGroupHeader(groupHeader, majorVersion, minorVersion);

        //  static control variables basic
        WriteBool(varyIsRange);
        WriteLine();
    }


    bool VaryStaticSupport::ReadFromFile()
    {
        ReadGroupHeader(groupHeader, majorVersion);

        //  static control variables basic
        varyIsRange  = ReadBool();


        return true;
    }


};                                      // end namespace


