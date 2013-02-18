///////////////////////////////////////////////////////////////////////////////////
//
// osConfigFile.cpp
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
//      Main optimizer/sampler code and global variables and other support classes used by optimizer and application.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <osMain/osConfigFile.h>

#include <osMain/G_Sample.h>            // for


namespace osConfigFile {

    bool osUseVersionControl = true;

    void WriteOptVar(const OptVar& inOpt)
    {
        const int optVer = 1;
        WriteInt(optVer);
        WriteBool(inOpt.wasActive);

        WriteNullDouble(inOpt.optMinLim);
        WriteNullDouble(inOpt.optMaxLim);
        WriteNullDouble(inOpt.optParEstimate);
        WriteDouble(inOpt.optParSigma);
        WriteBool(inOpt.optIsLinear);
        WriteLine();
    }

    void ReadOptVar(OptVar& outOpt)
    {
        int readVer = 0;
        if (osUseVersionControl)
            readVer = ReadInt();

        if (readVer > 0)
            outOpt.wasActive = ReadBool();

        outOpt.optMinLim = ReadNullDouble();
        outOpt.optMaxLim = ReadNullDouble();
        outOpt.optParEstimate = ReadNullDouble();
        outOpt.optParSigma = ReadDouble();
        outOpt.optIsLinear = ReadBool();
    }


    static SampVarArray   oldVals;   // for updating correlation s
    static SampVarArray   newVals;

    void SampVarPreReadSetup()
    {
        oldVals.DeAlloc();
        oldVals.SetResizable();
        oldVals.Alloc(50);

        newVals.DeAlloc();
        newVals.SetResizable();
        newVals.Alloc(50);
    }

    void  SampVarPostReadFixup()
    {
        //  update correlation data
        for (SampVar* o = SampVar::GetFirst(); o; o = o->GetNext())
        {
            SVCArray& corrs = o->correlations;
            for (int i = 0; i < corrs.Size(); i++)
            {
                SampVar* currCorr = corrs[i].correlatedVar;
                corrs[i].correlatedVar = 0;  // in case no match
                for (int j = 0; j < oldVals.Size(); j++)
                    if (currCorr == oldVals[j])
                    {
                        corrs[i].correlatedVar = newVals[j];
                        break;
                    }
            }
        }
    }


    void WriteSampVar(SampVar& inSamp)
    {
        // *************************************
        //  if you are making changes here check objLib/DPO_TableSample
        //  as well
        // ***************************************

        const int sampVer = 7; // student t added
    //  const int sampVer = 6; // truncated normal/lognormal added
    //  const int sampVer = 5; // exponential added
    //  const int sampVer = 4; // table lookup added
    //  const int sampVer = 3; // user CDF added
    //  const int sampVer = 2; // Poisson added
    //  const int sampVer = 1;
        WriteInt(sampVer);
        WriteBool(inSamp.wasActive);

        // added cfg file v1 with rest of sample support
        WritePointer(&inSamp);

        WriteInt(inSamp.svDistribution);        // changed v5
        WriteNullDouble(inSamp.distMean);
        WriteNullDouble(inSamp.distStdDev);
        WriteNullDouble(inSamp.distLogStdDev);
        WriteNullDouble(inSamp.distPeak);
        WriteNullDouble(inSamp.distMin);
        WriteNullDouble(inSamp.distMax);
        WriteNullDouble(inSamp.poissonLambda); // added v2
        WriteNullDouble(inSamp.weibullShape); // added v2
        WriteNullDouble(inSamp.weibullScale); // added v2
        WriteNullDouble(inSamp.betaP); // added v2
        WriteNullDouble(inSamp.betaQ); // added v2

        WriteFuncObjRef(inSamp.userCDFInputObjRef); // added v3

        WriteFuncObjRef(inSamp.tableLookupObjRef); // added v4
        WriteInt(inSamp.tableLookupColumn);

        inSamp.correlations.Cleanup();
        WriteInt(inSamp.correlations.Size());
        WriteLine();

        for (int i = 0; i < inSamp.correlations.Size(); i++)
        {
            WritePointer(inSamp.correlations[i].correlatedVar);
            WriteDouble(inSamp.correlations[i].correlationValue);
            WriteLine();
        }
    }
    void ReadSampVar(SampVar& outSamp)
    {
        int readVer = 0;
        if (osUseVersionControl)
            readVer = ReadInt();

        if (readVer > 0)
            outSamp.wasActive = ReadBool();

        // an old kluge since made irrelevant by the new kluge
        if ((readVer > 0) || (currentCfgFileReadMajorVersion > 1))
        {
            oldVals += static_cast<SampVar*>(ReadPointer());
            newVals += &outSamp;
        }

        outSamp.svDistribution = SampVar::SVDistribution(ReadInt());

        // *************************************
        //  if you are making changes here check objLib/DPO_TableSample
        //  as well
        // ***************************************

        if (readVer <  5)
        // exponential added in middle of enum
        {
            if (outSamp.svDistribution >= SampVar::svdExponential)
                outSamp.svDistribution = SampVar::SVDistribution(int(outSamp.svDistribution) + 1);
        }
        if (readVer <= 5)
        // truncated normal and lognormal distributions added in middle of enum
        {
            if (outSamp.svDistribution >= SampVar::svdTruncNormal)
                outSamp.svDistribution = SampVar::SVDistribution(int(outSamp.svDistribution) + 1);
            if (outSamp.svDistribution >= SampVar::svdTruncLogNormal)
                outSamp.svDistribution = SampVar::SVDistribution(int(outSamp.svDistribution) + 1);
        }

        if (readVer < 7)
        {
            if (outSamp.svDistribution >= SampVar::svdStudentT)
                outSamp.svDistribution = SampVar::SVDistribution(int(outSamp.svDistribution) + 1);

        }

    /*            enum SVDistribution  {svdNormal, svdTruncNormal,
                                      svdLogNormal, svdTruncLogNormal,
                                      svdUniform, svdLogUniform,
                                      svdTriangular, svdLogTriangular,
                                      svdExponential, svdPoisson, svdWeibull,   // added for paCalc
                                      svdBeta, svdStudentT,                     // added for paCalc, student T for nSights
                                      svdUserCDF, svdTableLookup, };            // added for paCalc
    */

        outSamp.distMean = ReadNullDouble();
        outSamp.distStdDev = ReadNullDouble();
        outSamp.distLogStdDev = ReadNullDouble();
        outSamp.distPeak = ReadNullDouble();
        outSamp.distMin  = ReadNullDouble();
        outSamp.distMax  = ReadNullDouble();

        if (readVer > 1)
        {
            outSamp.poissonLambda   = ReadNullDouble();
            outSamp.weibullShape    = ReadNullDouble();
            outSamp.weibullScale    = ReadNullDouble();
            outSamp.betaP           = ReadNullDouble();
            outSamp.betaQ           = ReadNullDouble();

            if (readVer > 2)
            {
                ReadFuncObjRef(outSamp.userCDFInputObjRef); // added v3
                if (readVer > 3)
                {
                    ReadFuncObjRef(outSamp.tableLookupObjRef); // added v4
                    outSamp.tableLookupColumn = ReadInt();
                }
            }
        }

        outSamp.correlations.SetEmpty();
        if ((readVer > 0) || (currentCfgFileReadMajorVersion > 1))
        {
            int ncorr = ReadInt();
            SVCorrelation addVar;
            for (int i = 0; i < ncorr; i++)
            {
                addVar.correlatedVar   = static_cast<SampVar*>(ReadPointer());
                addVar.correlationValue = ReadDouble();
                outSamp.correlations += addVar;
            }
        }
    }


    void WriteVaryVar(VaryVar& inVary)
    {
        const int varyVer = 1;
        WriteInt(varyVer);
        WriteBool(inVary.wasActive);


        WriteNullDouble(inVary.rangeMinLim);
        WriteNullDouble(inVary.rangeMaxLim);
        WriteBool(inVary.rangeIsLinear);
        WriteInt(inVary.rangeNStep);
        WriteLine();
        WriteInt(inVary.suiteValues.Size());
        for (int i = 0; i < inVary.suiteValues.Size(); i++)
            WriteNullDouble(inVary.suiteValues[i]);
        WriteLine();
        WriteInt(inVary.varyOrder);
        WriteLine();
    }

    void ReadVaryVar(VaryVar& outVary)
    {
        int readVer = 0;
        if (osUseVersionControl)
            readVer = ReadInt();

        if (readVer > 0)
            outVary.wasActive = ReadBool();

        outVary.rangeMinLim = ReadNullDouble();
        outVary.rangeMaxLim = ReadNullDouble();
        outVary.rangeIsLinear = ReadBool();
        outVary.rangeNStep = ReadInt();
        int nRead = ReadInt();
        outVary.suiteValues.SetEmpty();
        for (int i = 0; i < nRead; i++)
             outVary.suiteValues += ReadNullDouble();

        outVary.varyOrder = ReadInt();
    }

};                                      // end namespace

