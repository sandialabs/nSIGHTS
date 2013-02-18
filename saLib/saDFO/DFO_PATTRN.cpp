///////////////////////////////////////////////////////////////////////////////////
//
// DFO_PATTRN.cpp
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
#include <genClass/U_Real.h>

#include <genClass/DC_XYCDF.h>

#include <genDataClass/DSC_Parser.h>


#include <saDFO/DFO_PATTRN.h>

DFO_PATTRN :: DFO_PATTRN() :
DFO_SAObjBase("PATTRN"), ran3(SC_Random::raWichmannHill)
{
    CopyString(obsStr, "ALL", inpStrLen);   // OBSERVATIONS
    nreplicates     = 1;                        // NUMBER OF REPLICATES
    randSeed        = -132453;                  // SEED
    nxintInp        = 5;                // NXINT
    nyintInp        = 5;                // NYINT
    nrandomPatterns = 5;    // NUMBER OF RANDOM PATTERNS
    logarithms      = false;            // LOGARITHMS
    alphaCutoff     = 1.0;      // ALPHA CUTOFF
    coeffFormat     = SC_DecFormat(5);
    mtest1          = 0;

    tests.AllocAndFill(ptLastTest, false);
    tests[ptCC] = true;

    verbose         = false;
    verboseMC       = false;
    verboseCV       = false;
    coefficients    = false;
    rankCoefficients = false;
    CopyString(mcRepStr, "NONE", inpStrLen);    // OBSERVATIONS

    CopyString(title, "PATTRN - mView", 80);


    InitCommon();
}

DFO_PATTRN::DFO_PATTRN(const DFO_PATTRN& a)
: DFO_SAObjBase(a), ran3(a.ran3)
{
    LocalCopy(a);
    InitCommon();
}


const char* DFO_PATTRN::testStrs[] = {
        "CC",   "CCMC",     "CIQ",  "CIQMC",
        "CL",   "CLMC",     "CMD",  "CMDMC",
        "CMN",  "CMNMC",    "CV",   "CVMC",
        "DR",   "DRMC",     "FR",   "FRMC",
        "PCC",  "PRCC",     "RCC",  "RCCMC",
        "SI",   "SIMC",     "SRC",  "SRRC"
    };


void DFO_PATTRN::InitCommon()
{
    fullTableDO.tableData = &fullTableDC;
    AddOutPort(fullTableDO);

    xgridTableDO.tableData = &xgridTableDC;
    AddOutPort(xgridTableDO);

    ySIgridTableDO.tableData = &ySIgridTableDC;
    AddOutPort(ySIgridTableDO);

    yMDgridTableDO.tableData = &yMDgridTableDC;
    AddOutPort(yMDgridTableDO);

    yIQgridTableDO.tableData = &yIQgridTableDC;
    AddOutPort(yIQgridTableDO);

    dvTableDO.tableData = &dvTableDC;
    AddOutPort(dvTableDO);

    ivTableDO.tableData = &ivTableDC;
    AddOutPort(ivTableDO);

    dvCDFDO.xyDataArray = &dvCDFDC;
    AddOutPort(dvCDFDO);

    ivCDFDO.xyDataArray = &ivCDFDC;
    AddOutPort(ivCDFDO);

    DoStatusChk();
}

DFO_PATTRN& DFO_PATTRN::operator= (const DFO_PATTRN& a)
{
    if (&a != this)
    {
        DFO_SAObjBase::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_PATTRN::LocalCopy(const DFO_PATTRN& a)
{
    CopyString(obsStr, a.obsStr, inpStrLen);
    CopyString(mcRepStr, a.mcRepStr, inpStrLen);

    nreplicates     = a.nreplicates;
    randSeed        = a.randSeed;
    nxintInp        = a.nxintInp;
    nyintInp        = a.nyintInp;
    nrandomPatterns = a.nrandomPatterns;
    logarithms      = a.logarithms;
    alphaCutoff     = a.alphaCutoff;
    mtest1          = a.mtest1;
    coeffFormat     = a.coeffFormat;

    tests           = a.tests;

    verbose         = a.verbose;
    verboseMC       = a.verboseMC;
    verboseCV       = a.verboseCV;
    coefficients    = a.coefficients;
    rankCoefficients = a.rankCoefficients;

}

void  DFO_PATTRN::DoStatusChk()
{
    DFO_SAObjBase::DoStatusChk();
    if (StatusNotOK())
        return;

    if ((GetDepVar().GetNStep() > 1) || (GetIndepVar().GetNStep() > 1))
        SetObjErrMsg("IV/DV can have only 1 step");

    if (GetDepVar().GetNVector() < 2)
        SetObjErrMsg("must be more than 2 observations");


    bool repTestFound = false;

    // force sorting test
    tests[mtest1] = true;

    for (int i = ptCC; i < ptLastTest; i++)
    {
        // tests using replicates
        if (tests[i] &&
            ((i == ptDR) || (i == ptDRMC) || (i == ptFR) || (i == ptFRMC)))
            repTestFound = true;
    }

    SC_SetupErr err;
    if (!ProcessObsStr(err))
        SetObjErrMsg(err.GetErrorText());

    if ((tests[ptFR]  || tests[ptFRMC]) && (nreplicates < 2))
        SetObjErrMsg("at least 2 replicates must be specified for FR or FRMC tests");

    if ((nobs > 0) && ((nobs % nreplicates) != 0))
        SetObjErrMsg("# of observations must be integer multiple of # of replicates");

    nrep = nreplicates;

    if (verboseMC && MonteCarloTest())
    {
        if (!ProcessMCRepStr(err))
            SetObjErrMsg(err.GetErrorText());
    }

    nyint = nyintInp;
    nxint = nxintInp;
}


void DFO_PATTRN:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    InputListing();

    CSR();

    // set cb stat
    svgsc.AllocAndSetSize(niv);
    stat.MatrixAllocAndSetSize(ptLastTest, niv);
    statpv.MatrixAllocAndSetSize(ptLastTest, niv);

    Savage();

    if (MonteCarloTest())
    {
        ran3.SetupRandom(randSeed);
        double dummy = ran3.Random(); // call to sync up with FORTRAN version for QA tests

        xivrp.MatrixAllocAndSetSize(nrandomPatterns, nobs);
        xrivrp.MatrixAllocAndSetSize(nrandomPatterns, nobs);
        ydvrp.MatrixAllocAndSetSize(nrandomPatterns, nobs);
        yrdvrp.MatrixAllocAndSetSize(nrandomPatterns, nobs);
    }

    nxgrd.AllocAndSetSize(niv);
    xgrdl.AllocAndSetSize(niv);
    ySIgrdl.MatrixAllocAndSetSize(ndv, nyint);
    yMDgrdl.MatrixAllocAndSetSize(ndv, 1);
    yIQgrdl.MatrixAllocAndSetSize(ndv, 2);

    // plot output selectors

    ivAlphaOK.AllocAndFill(niv, false);
    dvAlphaOK.AllocAndFill(ndv, false);

    for (int j = 0; j < ndv; j++)
    {
        // clear
        statpv.MatrixFill(0.0);
        stat.MatrixFill(0.0);

        if (ysdv[j][0] == ysdv[j][nobs - 1])
        {
            AddLine("*** Error: degenerate DV - all values identical *** ");
            AddStdReal(GetDepVar().GetVariableName(j), ysdv[j][0], SC_SciFormat(6));
            AddBlankLine();
            continue;
        }

        for (int i = 0; i < niv; i++)
        {
            if (xsiv[i][0] == xsiv[i][nobs - 1])
            {
                AddLine("*** Error: degenerate IV - all values identical *** ");
                AddStdReal(GetIndepVar().GetVariableName(i), xsiv[i][0], SC_SciFormat(6));
                AddBlankLine();
                continue;
            }

            int nxsav = nxint;  // can be changed by grid
            int nysav = nyint;

            SetGrid(i, j);

            if (MonteCarloTest())
                CalcMC(i, j);

            if (tests[ptCL] || tests[ptCLMC])
                TestCL(i, j);

            if (tests[ptCC] || tests[ptCCMC] || tests[ptRCC] || tests[ptRCCMC] ||
                coefficients || rankCoefficients)
                TestCRC(i, j);

            if (tests[ptCMD] || tests[ptCMDMC])
                TestCMD(i, j);

            if (tests[ptCMN] || tests[ptCMNMC])
                TestCMN(i, j);

            if (tests[ptCIQ] || tests[ptCIQMC])
                TestCIQ(i, j);

            if (tests[ptCV] || tests[ptCVMC])
                TestCV(i, j);

            if (tests[ptDR] || tests[ptDRMC])
                TestDR(i, j);

            if (tests[ptFR] || tests[ptFRMC])
                TestFR(i, j);

            if (tests[ptSI] || tests[ptSIMC])
                TestSI(i, j);

            nxint = nxsav;
            nyint = nysav;
        }

        if (tests[ptPCC] || tests[ptPRCC] ||
            tests[ptSRC] || tests[ptSRRC] ||
            coefficients || rankCoefficients)
            TestPCSR(j);

        SetIVDVOK(j);

        WriteResults(j);
    }

    // set all output
    int nivt = ivAlphaOK.GetSetCount();
    if (nivt == 0)
    {
        fullTableDC.ClearAll();
        dvTableDC.ClearAll();
        ivTableDC.ClearAll();
        dvCDFDC.DeAlloc();
        ivCDFDC.DeAlloc();
    }
    else
    {
        int ndvt = dvAlphaOK.GetSetCount();

        fullTableDC.Alloc(nivt + ndvt, nobs);
        dvTableDC.Alloc(ndvt, nobs);
        ivTableDC.Alloc(nivt, nobs);
        fullTableDC.SetID("AlphaOKVar");
        fullTableDC.rowDesc.SetIndexed("Obs", 1);
        dvTableDC.SetID("DepVar");
        dvTableDC.rowDesc = fullTableDC.rowDesc;
        ivTableDC.SetID("InDepVar");
        ivTableDC.rowDesc = fullTableDC.rowDesc;

        int maxXGrid = 0;
        SC_IntArray orderedIV;
        ivAlphaOK.MakeForwardMap(orderedIV);

        for (int i = 0; i < nivt; i++)
            maxXGrid = IMax(maxXGrid, xgrdl[orderedIV[i]].Size());

        xgridTableDC.Alloc(nivt, maxXGrid);
        xgridTableDC.rowDesc.SetIndexed("XGrid", 1);
        xgridTableDC.SetID("XGridLines");

        ySIgridTableDC.Alloc(ndvt, nyint);
        ySIgridTableDC.rowDesc.SetIndexed("SIGrid", 1);
        ySIgridTableDC.SetID("SI_GridLines");

        yMDgridTableDC.Alloc(ndvt, 1);
        yMDgridTableDC.rowDesc += "Y50";
        yMDgridTableDC.SetID("MD_GridLines");

        yIQgridTableDC.Alloc(ndvt, 2);
        yIQgridTableDC.rowDesc += "Y25";
        yIQgridTableDC.rowDesc += "Y75";
        yIQgridTableDC.SetID("IQ_GridLines");

        dvCDFDC.Alloc(ndvt * nrep);
        ivCDFDC.Alloc(nivt * nrep);

        // order IV by test statistic for last DV
        {
            SC_DoubleArray temp(nivt);
            for (int i = 0; i < nivt; i++)
            {
                temp[i] = statpv[mtest1][orderedIV[i]];
                if (PCCSRCTest(mtest1))
                    temp[i] = -fabs(temp[i]);
            }
            temp.Sort(true, orderedIV);
        }

        int nfull = 0;
        int nivCDF = 0;
        for (int i = 0; i < nivt; i++)
        {
            int currIV = orderedIV[i];

            const char* varID = GetIndepVar().GetVariableName(currIV);
            fullTableDC.columnDesc += varID;
            ivTableDC.columnDesc += varID;
            xgridTableDC.columnDesc += varID;

            const SC_DoubleArray& ivVar = GetIndepVar().saData[0][currIV];

            fullTableDC.dataTable[nfull++] = ivVar;
            ivTableDC.dataTable[i] = ivVar;

            int nxgi = xgrdl[currIV].Size();
            xgridTableDC.dataTable[i] = xgrdl[currIV];
            for (int j = nxgi; j < maxXGrid; j++)
                xgridTableDC.dataTable[i][j] = nullReal;

            for (int j = 0; j < nrep; j++)
            {
                DC_XYData& nextXY = ivCDFDC[nivCDF++];
                char idStr[40];
                if (nrep > 1)
                {
                    char repStr[40];
                    IntToString(j + 1, repStr, 40);
                    MakeString(idStr, varID, "CDF/Rep", repStr, 40);
                }
                else
                {
                    MakeString(idStr, varID, "CDF", 40);
                }
                nextXY.SetID(idStr);
                SetCDF(ivVar, j, nextXY);
            }
        }

        ndvt = 0;
        int ndvCDF = 0;
        for (int i = 0; i < ndv; i++)
            if (dvAlphaOK[i])
            {
                const char* varID = GetDepVar().GetVariableName(i);
                fullTableDC.columnDesc += varID;
                ySIgridTableDC.columnDesc += varID;
                yMDgridTableDC.columnDesc += varID;
                yIQgridTableDC.columnDesc += varID;
                dvTableDC.columnDesc += varID;

                const SC_DoubleArray& dvVar = GetDepVar().saData[0][i];

                fullTableDC.dataTable[nfull++] = dvVar;

                ySIgridTableDC.dataTable[ndvt] = ySIgrdl[i];
                yMDgridTableDC.dataTable[ndvt] = yMDgrdl[i];
                yIQgridTableDC.dataTable[ndvt] = yIQgrdl[i];

                dvTableDC.dataTable[ndvt++] = dvVar;

                for (int j = 0; j < nrep; j++)
                {
                    DC_XYData& nextXY = dvCDFDC[ndvCDF++];
                    char idStr[40];
                    if (nrep > 1)
                    {
                        char repStr[40];
                        IntToString(j + 1, repStr, 40);
                        MakeString(idStr, varID, "CDF/Rep", repStr, 40);
                    }
                    else
                    {
                        MakeString(idStr, varID, "CDF", 40);
                    }
                    nextXY.SetID(idStr);
                    SetCDF(dvVar, j, nextXY);
                }

            }

        fullTableDC.SetSize(nfull, nobs);
        fullTableDO.nDV = ndvt;
        fullTableDO.nIV = nivt;

        xgridTableDC.SetSize(nivt, maxXGrid);
        xgridTableDO.nIV = nivt;

        ySIgridTableDC.SetSize(ndvt, nyint);
        ySIgridTableDO.nDV = ndvt;

        yMDgridTableDC.SetSize(ndvt, 1);
        yMDgridTableDO.nDV = ndvt;

        yIQgridTableDC.SetSize(ndvt, 2);
        yIQgridTableDO.nDV = ndvt;

        ivTableDC.SetSize(nivt, nobs);
        dvTableDC.SetSize(ndvt, nobs);

        dvCDFDC.SetID("DV CDF");
        dvCDFDC.SetSize(ndvCDF);

        ivCDFDC.SetID("IV CDF");
        ivCDFDC.SetSize(nivCDF);
    }

    CloseListingData();
}


void DFO_PATTRN::InputListing()
{
    InitListingData();

    TextBigTitle(title);
    FullVariableList();

    const DC_SATable& depVar = GetDepVar();
    const DC_SATable& indepVar = GetIndepVar();

    TextStdTitle("PATTRN Standard Listing");
    AddStdInt("Number of input independent variables", indepVar.GetNVariable());
    AddStdInt("Number of input dependent variables", depVar.GetNVariable());
    AddStdInt("Number of input observations", depVar.GetNVector());
    AddStdInt("Number of selected observations", nobs);
    if (MonteCarloTest())
        AddStdInt("Number of random patterns for MC", nrandomPatterns);
    if (nrep > 1)
        AddStdInt("Number of replicates", nrep);

    AddBlankLine();

    VariablesNameList();
}

bool DFO_PATTRN::ProcessObsStr(SC_SetupErr& err)
{
    SC_BoolArray keepObs;

    if (!ProcessNumStr(obsStr, keepObs, GetIndepVar().GetNVector(), err))
        return false;

    if (keepObs.GetSetCount() == 0)
    {
        err.SetConstantError("no observations selected");
        return false;
    }

    keepObs.MakeForwardMap(mobs);
    nobs = mobs.Size();
    return true;
}

bool DFO_PATTRN::ProcessMCRepStr(SC_SetupErr& err)
{
    if (SameUCString(mcRepStr, "NONE"))
    {
        mcReport.AllocAndFill(nrandomPatterns, false);
        return true;
    }

    if (!ProcessNumStr(mcRepStr, mcReport, nrandomPatterns, err))
        return false;

    return true;
}


bool DFO_PATTRN::ProcessNumStr(const char*          inStr,
                                     SC_BoolArray&  selIndx,
                                     int            maxIndx,
                                     SC_SetupErr&   err)
{
    if (SameUCString(inStr, "ALL"))
    {
        selIndx.AllocAndFill(maxIndx, true);
        return true;
    }

    selIndx.AllocAndFill(maxIndx, false);

    DSC_Parser strParser;
    if (!strParser.GetFields(inStr, err))
        return false;

    int lastIndex = -1;
    bool rangePending = false;
    for (int i = 0; i < strParser.fields.Size(); i++)
    {
        const DSC_Parser::Field& currField = strParser.fields[i];
        if (currField.IsNumber())
        {
            if ((currField.fVal < 1.0) || (currField.fVal > double(maxIndx)))
            {
                err.SetConstantError("number out of range");
                return false;
            }
            int nextIndex = int(currField.fVal) - 1;
            selIndx[nextIndex] = true;

            if (rangePending)
            {
                for (int j = lastIndex; j < nextIndex; j++)
                    selIndx[j] = true;

                rangePending = false;
            }

            lastIndex = nextIndex;
        }
        else if (currField.IsCharacter() && (currField.fChar == '-'))
        {
            rangePending = (lastIndex >= 0);
        }
        else
        {
            err.SetConstantError("unrecognized text in string - must be number or -");
            return false;
        }
    }

    return true;
}

bool DFO_PATTRN::MonteCarloTest()
{
    return  tests[ptCCMC] ||
            tests[ptCIQMC] ||
            tests[ptCLMC] ||
            tests[ptCMDMC] ||
            tests[ptCMNMC] ||
            tests[ptCVMC] ||
            tests[ptDRMC] ||
            tests[ptFRMC] ||
            tests[ptRCCMC] ||
            tests[ptSIMC];
}

bool DFO_PATTRN::MonteCarloTest(int tIndx)
{
    return  (tIndx == ptCCMC) ||
            (tIndx == ptCIQMC) ||
            (tIndx == ptCLMC) ||
            (tIndx == ptCMDMC) ||
            (tIndx == ptCMNMC) ||
            (tIndx == ptCVMC) ||
            (tIndx == ptDRMC) ||
            (tIndx == ptFRMC) ||
            (tIndx == ptRCCMC) ||
            (tIndx == ptSIMC);
}

void DFO_PATTRN::CSR()
{
    DoCSR(GetIndepVar(), xiv, xsiv, xriv, niv);
    DoCSR(GetDepVar(), ydv, ysdv, yrdv, ndv);
}

void DFO_PATTRN::DoCSR(const DC_SATable& saTbl,
                                            SC_DoubleMatrix& v,
                                            SC_DoubleMatrix& sv,
                                            SC_DoubleMatrix& rv,
                                            int&             nv)
{
    nv = saTbl.GetNVariable();

    v.MatrixAllocAndSetSize(nv, nobs);
    sv.MatrixAllocAndSetSize(nv, nobs);
    rv.MatrixAllocAndSetSize(nv, nobs);
    for (int i = 0; i < nv; i++)
    {
        const SC_DoubleArray& inV = saTbl.saData[0][i];
        SC_DoubleArray& outV = v[i];
        for (int j = 0; j < nobs; j++)
            outV[j] = inV[mobs[j]];

        sv[i] = outV;
        sv[i].Sort(true);
        rv[i] = outV;
        RankArray(rv[i]);
    }
}



void DFO_PATTRN::Savage()
{
    for (int i = 0; i < niv; i++)
    {
        svgsc[i] = 0.0;
        for (int j = i; j < niv; j++)
            svgsc[i] += 1.0 / double(j + 1);
    }
}

void DFO_PATTRN::CalcMC(int ivIndx, int dvIndx)
{
    int nsampleObs = nobs / nrep;

    const SC_DoubleArray& ivX = xiv[ivIndx];
    const SC_DoubleArray& dvY = ydv[dvIndx];

    SC_DoubleArray xwork, ywork;
    xwork.AllocAndSetSize(nsampleObs);
    ywork.AllocAndSetSize(nsampleObs);

    SC_BoolArray obsSel(nsampleObs);

    for (int i = 0; i < nrandomPatterns; i++)
    {
        int repStart = 0;
        for (int j = 0; j < nrep; j++)
        {
            for (int k = 0; k < nsampleObs; k++)
            {
                xwork[k] = ivX[repStart + k];
                ywork[k] = dvY[repStart + k];
            }

            obsSel.FillToAlloc(false);

            if (verboseMC && mcReport[i])
            {
                char pairStr[40];
                IntToString(i + 1, pairStr, 40);
                char titleString[80];
                if (nrep == 1)
                {
                    MakeString(titleString, "MC Summary: Random Pairing #", pairStr, 80);
                }
                else
                {
                    char repStr[40];
                    IntToString(j + 1, repStr, 40);
                    MakeString(titleString, "MC Summary: Random Pairing #", pairStr, " Replicate #", repStr, 80);
                }
                TextSubTitle(titleString);
                TableSetup(15, 5, 5, 2);
                SetTableCol(GetIndepVar().GetVariableName(ivIndx), GetDepVar().GetVariableName(dvIndx));
            }

            // GENERATE RANDOM PAIRINGS FROM THE MARGINAL DISTRIBUTIONS
            // BY SELECTING THE INDEPENDENT VARIABLE VALUES AND PAIRING
            // THEM WITH A DEPENDENT VARIABLE VALUE CHOSEN AT RANDOM
            // (WITHOUT REPLACEMENT) FOR THE CURRENT REPLICATE

            for (int k = 0; k < nsampleObs; k++)
            {
                int iobsy;
                do {
                    iobsy = IMin(int(floor(double(nsampleObs) * ran3.Random())), nsampleObs - 1);
                } while (obsSel[iobsy]);
                obsSel[iobsy] = true;

                xivrp[i][repStart + k] = xwork[k];
                ydvrp[i][repStart + k] = ywork[iobsy];

                if (verboseMC && mcReport[i])
                {
                    SetTableReal(0, xwork[k], SC_SciFormat(6));
                    SetTableReal(1, ywork[iobsy], SC_SciFormat(6));
                    AddNextLine();
                }
            }
            repStart += nsampleObs;

            if (verboseMC && mcReport[i])
                AddBlankLine();
        }

        // copy & rank
        xrivrp[i] = xivrp[i];
        yrdvrp[i] = ydvrp[i];
        RankArray(xrivrp[i]);
        RankArray(yrdvrp[i]);
    }

}

void DFO_PATTRN::SetGrid(int ivIndx, int dvIndx)
{

//  DETERMINE IF INDEPENDENT VARIABLE IS CONTINUOUS OR DISCRETE
    SC_IntArray mxlvl;
    CalcDV(xsiv[ivIndx], mxlvl);
    bool ivContinous = mxlvl.IsEmpty();
    if (!ivContinous)
        nxint = mxlvl.Size();
    nxgrd[ivIndx] = nxint;

    // allocate
    egrid.MatrixAllocAndSetSize(nxint, nyint);
    xgrid.AllocAndSetSize(nxint + 1);
    ygrid.AllocAndSetSize(nyint + 1);

    xgrdl[ivIndx].AllocAndSetSize(nxint - 1);

    //  DETERMINE CELL PROBABILITY AND CELL EXPECTED VALUE
    if (ivContinous)
        egrid.MatrixFill(double(nobs) / double(nxint * nyint));
    else
    {
        for (int j = 0; j < nyint; j++)
            for (int i = 0; i < nxint; i++)
            {
                egrid[i][j] = double(mxlvl[i]) / double(nyint);
            }
    }

    // DETERMINE CELL BOUNDARIES AND CELL GRID LINES
    xgrid[0] = 0.0;
    int iglx = 0;
    for (int i = 0; i < nxint; i++)
    {
        if (ivContinous)
            xgrid[i + 1] = double(i + 1) / double(nxint);
        else
            xgrid[i + 1] = xgrid[i + 1] + double(mxlvl[i]) / double(nobs);

        if (i < (nxint - 1))
        {
            if (ivContinous)
                iglx = int(floor(double(nobs) * xgrid[i + 1]));
            else
                iglx += mxlvl[i];
            xgrdl[ivIndx][i] = 0.5 * (xsiv[ivIndx][iglx - 1] + xsiv[ivIndx][iglx]);
        }

    }

    ygrid[0] = 0.0;
    int jygl = -1;
    for (int j = 0; j < nyint; j++)
    {
        ygrid[j + 1] = double(j + 1) / double(nyint);
        jygl += nobs / nyint;
        ySIgrdl[dvIndx][j] = 0.5 * (ysdv[dvIndx][jygl - 1] + ysdv[dvIndx][jygl]);
    }

    int jmd = int(0.5 * double(nobs));
    yMDgrdl[dvIndx][0] = 0.5 * (ysdv[dvIndx][jmd] + ysdv[dvIndx][jmd + 1]);

    int j25 = int(0.25 * double(nobs));
    yIQgrdl[dvIndx][0] = 0.5 * (ysdv[dvIndx][j25] + ysdv[dvIndx][j25 + 1]);

    int j75 = int(0.75 * double(nobs));
    yIQgrdl[dvIndx][1] = 0.5 * (ysdv[dvIndx][j75] + ysdv[dvIndx][j75 + 1]);
}

void DFO_PATTRN::CalcDV(const SC_DoubleArray& xin, // sorted
                              SC_IntArray&   mlev) // levels
// SUBROUTINE CALCDV DETERMINES IF A VARIABLE IS A CONTINUOUS VARIABLE
// OR A DISCRETE VARIABLE AND FINDS THE DIFFERENT LEVELS OF A DISCRETE
// VARIABLE
{
    int n = xin.Size();
    int imax = n - 1;
    mlev.Alloc(n);
    int nlvl = 0;
    int i = 0;
    while (i < imax)
    {
        if (xin[i] != xin[i+1])
        {
            i++;
            continue;
        }

        int ntv = 2;
        while ((i + ntv) < n)
        {
            if (xin[i] != xin[i + ntv])
                break;
            ntv++;
        }

        mlev += ntv;

        i += ntv;
    }

    int lsum = 0;
    for (int i = 0; i < mlev.Size(); i++)
        lsum += mlev[i];

    if (lsum != n)
        mlev.SetEmpty();
}





int DFO_PATTRN::Search(const double& val,
                       const SC_DoubleArray& edf,
                       const SC_DoubleArray& grid)
{
    double prob = 0.0;
    for (int i = 0; i < edf.Size(); i++)
        if (val == edf[i])
        {
            prob = double(i + 1) / double(edf.Size());
            break;
        }

    for (int i = 0; i < grid.UpperBound(); i++)
        if ((prob > grid[i]) && (prob <= grid[i + 1]))
            return i;

//  GenAppInternalError("DFO_PATTRN::Search");
    return grid.UpperBound();
}


double DFO_PATTRN::CalcProb(      PattrnTests       pTest,
                                  int               ivIndx,
                                  int               dvIndx,
                            const double&           statV,
                                  SC_DoubleArray&   ccdf)
{
    if (verboseMC)
    {
        SC_IntArray mrpwk;
        mrpwk.AllocAndIndex(nrandomPatterns);
        ccdf.Sort(true, mrpwk);

        char titleString[80];
        MakeString(titleString, "MC Summary for Test: ", testStrs[pTest], 80);

        TextSubTitle(titleString);
        AddStdText("Independent variable", GetIndepVar().GetVariableName(ivIndx));
        AddStdText("Dependent variable", GetDepVar().GetVariableName(dvIndx));
        AddBlankLine();

        TableSetup(20, 5, 5, 2);

        SetTableCol("Random Pattern Stat", "Random Pattern #");
        AddNextLine();

        for (int i = 0; i < nrandomPatterns; i++)
        {
            SetTableReal(0, ccdf[i], SC_SciFormat(6));
            SetTableInt(1, mrpwk[i] + 1);
            AddNextLine();
        }
    }
    else
        ccdf.Sort(true);

    double rnrp = 1.0 / double(nrandomPatterns);
    int irp = 0;
    int i;
    for (i = nrandomPatterns; i >= 1; i--)
        if (statV >= ccdf[i - 1])
        {
            irp = i;
            break;
        }

    double prob = 1.0 - double(irp) * rnrp;
    if (pTest == ptDRMC)
        prob = 1.0 - prob;


    if (verboseMC)
    {
        double ccdfl, ccdfu, probl, probu;
        if (irp == 0)
        {
            ccdfl = ccdf[0];
            ccdfu = ccdfl;
            probl = prob;
            probu = prob;
        }
        else if (irp == nrandomPatterns)
        {
            ccdfl = ccdf.LastIndex();
            ccdfu = ccdfl;
            probl = prob;
            probu = prob;
        }
        else
        {
            ccdfl = ccdf[i - 1];
            ccdfu = ccdf[i];
            probl = prob;
            probu = prob - rnrp;
        }

        const SC_SciFormat sci6(6);
        AddStdReal("Test statistic", statV, sci6);
        AddStdReal("Lower bracketing statistic", ccdfl, sci6);
        AddStdReal("Upper bracketing statistic", ccdfu, sci6);
        AddStdReal("Lower bracketing prob.", probl, sci6);
        AddStdReal("Upper bracketing prob.", probu, sci6);
        AddStdReal("Selected P-value", prob, sci6);
        AddBlankLine();
    }

    return prob;
}

void  DFO_PATTRN::CalcCO(const SC_DoubleArray& statRes,
                         SC_IntArray&    maco)
{
    for (int i = 0; i < statRes.Size(); i++)
        if (statRes[i] <= alphaCutoff)
            maco.AddIfNotFound(i);
}

bool DFO_PATTRN::PCCSRCTest(int tIndex)
{
    return (tIndex == ptSRC) || (tIndex == ptPCC) ||
           (tIndex ==  ptPRCC) || (tIndex ==  ptSRRC);
}

/* SUBROUTINE SIT PLACES ORDERED PAIRS OF INDEPENDENT VARIABLES AND
C DEPENDENT VARIABLES ON A GRID THAT CONTAINS AREAS OF EQUAL
C PROBABILITY BASED ON THE DISTRIBUTIONS OF THE INDEPENDENT VARIABLES
C AND THE DEPENDENT VARIABLES.  CALCULATIONS ARE PERFORMED TO DETERMINE
C IF THE RESULTING PATTERN IS SIGNIFICANTLY DIFFERENT FROM A RANDOM
C PATTERN. */

double DFO_PATTRN::SITest(const SC_DoubleArray& ivVar,
                           const SC_DoubleArray& dvVar,
                                 int             ivIndx,
                                 int             dvIndx)
{
    // DETERMINE WHERE ORDERED PAIRS OF INDEPENDENT VARIABLES AND
    // DEPENDENT VARIABLES FALL IN THE GRID AND RECORD
    SC_IntMatrix ngrid;
    ngrid.MatrixAllocAndSetSize(nxint, nyint);
    ngrid.MatrixFill(0);

    for (int i = 0; i < nobs; i++)
    {
        int igx = Search(ivVar[i], xsiv[ivIndx], xgrid);
        int igy = Search(dvVar[i], ysdv[dvIndx], ygrid);
        ngrid[igx][igy]++;
    }

    // CALCULATE AND STORE STATISTICAL INDEPENDENCE TEST STATISTICS

    double tsi = 0.0;
    for (int j = 0; j < nyint; j++)
        for (int i = 0; i < nxint; i++)
        {
            const double& e = egrid[i][j];
            double term = Sqr(double(ngrid[i][j]) - e);
            tsi += term / e;
        }

    return tsi;
}


void DFO_PATTRN::TestSI(int ivIndx, int dvIndx)
{
    const PattrnTests currTest = ptSI;
    const PattrnTests currTestMC = ptSIMC;

    double tsi = SITest(xiv[ivIndx], ydv[dvIndx], ivIndx, dvIndx);

    stat[currTest][ivIndx] = tsi;

    // CALCULATE TEST STATISITIC P-VALUE
    double df = double((nxint - 1) * (nyint - 1));
    statpv[currTest][ivIndx] = GammaQ(0.5 * df, 0.5 * tsi);

    // Monte-Carlo ?
    if (!tests[currTestMC])
        return;

    SC_DoubleArray tccdf(nrandomPatterns);

    for (int k = 0; k < nrandomPatterns; k++)
    {
        tccdf[k] = SITest(xivrp[k], ydvrp[k], ivIndx, dvIndx);
    }

    statpv[currTestMC][ivIndx] = CalcProb(currTestMC, ivIndx, dvIndx, tsi, tccdf);
}


// SUBROUTINE CLT PERFORMS THE COMMON LOCATIONS TEST

double DFO_PATTRN::CLTest(const SC_DoubleArray& ivVar,
                          const SC_DoubleArray& dvVar,
                                int             ivIndx)
{
    double fobs = double(nobs);
    double cnst = fobs * Sqr(fobs + 1.0) / 4.0;

    SC_DoubleArray rsum(nxint, 0.0);
    SC_IntArray mi(nxint, 0);

    double rssq = 0.0;
    for (int i = 0; i < nobs; i++)
    {
        rssq += Sqr(dvVar[i]);
        int igx = Search(ivVar[i], xsiv[ivIndx], xgrid);
        rsum[igx] += dvVar[i];
        mi[igx]++;
    }

//C CALCULATE AND STORE COMMON LOCATIONS TEST STATISTIC

    double tclsum = 0.0;
    for (int i = 0; i < nxint; i++)
        if (mi[i] > 0)
            tclsum += Sqr(rsum[i]) / double(mi[i]);

    double s2  = (rssq - cnst) / (fobs - 1.0);
    return (tclsum - cnst) / s2;
}


void DFO_PATTRN::TestCL(int ivIndx, int dvIndx)
{
    const PattrnTests currTest = ptCL;
    const PattrnTests currTestMC = ptCLMC;

    double tcl = CLTest(xiv[ivIndx], yrdv[dvIndx], ivIndx);

    stat[currTest][ivIndx] = tcl;

    // CALCULATE TEST STATISITIC P-VALUE
    double df = double(nxint - 1);
    statpv[currTest][ivIndx] = GammaQ(0.5 * df, 0.5 * tcl);

    // Monte-Carlo ?
    if (!tests[currTestMC])
        return;

    SC_DoubleArray ccdf(nrandomPatterns);

    for (int k = 0; k < nrandomPatterns; k++)
    {
        ccdf[k] = CLTest(xivrp[k], yrdvrp[k], ivIndx);
    }

    statpv[currTestMC][ivIndx] = CalcProb(currTestMC, ivIndx, dvIndx, tcl, ccdf);

}

double  DFO_PATTRN::CalcCC(const SC_DoubleArray& x,
                           const SC_DoubleArray& y)
{
    int n = x.Size();

    double sx = 0.0;
    double sy = 0.0;
    double sxx = 0.0;
    double syy = 0.0;
    double sxy = 0.0;
    for (int i = 0; i < n; i++)
    {
        sx += x[i];
        sy += y[i];
        sxx += Sqr(x[i]);
        syy += Sqr(y[i]);
        sxy += x[i] * y[i];
    }

    double fn = double(n);
    double snum = sxy - sx * sy / fn;
    double sden = (sxx - sx * sx / fn) * (syy - sy * sy / fn);
    if (sden <= 0.0)
        return 0.0;

    return snum / sqrt(sden);
}



void DFO_PATTRN::TestCRC(int ivIndx, int dvIndx)
{
    const PattrnTests currTest1 = ptCC;
    const PattrnTests currTest2 = ptRCC;
    const PattrnTests currTest1MC = ptCCMC;
    const PattrnTests currTest2MC = ptRCCMC;

    double srobs = sqrt(double(nobs));
    double srobs1 = sqrt(double(nobs - 1));

    double ccrp = CalcCC(xiv[ivIndx], ydv[dvIndx]) * srobs;
    double rccrp = CalcCC(xriv[ivIndx], yrdv[dvIndx]) * srobs1;

    stat[currTest1][ivIndx] = ccrp;
    stat[currTest2][ivIndx] = rccrp;

    // CALCULATE TEST STATISITIC P-VALUE
    statpv[currTest1][ivIndx] = Erfc(fabs(ccrp) / sqrt(2.0));
    statpv[currTest2][ivIndx] = Erfc(fabs(rccrp) / sqrt(2.0));

    // Monte-Carlo ?
    if (!(tests[currTest1MC] || tests[currTest2MC]))
        return;

    SC_DoubleArray ccdf(nrandomPatterns);
    SC_DoubleArray rccdf(nrandomPatterns);

    for (int k = 0; k < nrandomPatterns; k++)
    {
        ccdf[k] = fabs(CalcCC(xivrp[k], ydvrp[k])) * srobs;
        rccdf[k] = fabs(CalcCC(xrivrp[k], yrdvrp[k])) * srobs1;
    }

    statpv[currTest1MC][ivIndx] = CalcProb(currTest1MC, ivIndx, dvIndx, fabs(ccrp), ccdf);
    statpv[currTest2MC][ivIndx] = CalcProb(currTest2MC, ivIndx, dvIndx, fabs(rccrp), rccdf);

}


// SUBROUTINE CM PERFORMS THE COMMON MEDIAN TEST

double DFO_PATTRN::CMDTest(const SC_DoubleArray& ivVar,
                          const SC_DoubleArray& dvVar,
                          const double&         medVal,
                                int             ivIndx,
                                bool&           testOK)
{
    testOK = true;

    SC_IntArray ngrid1(nxint, 0);
    SC_IntArray ngrid2(nxint, 0);

    SC_IntArray midot(nxint, 0);
    int mdot1 = 0;
    int mdot2 = 0;
    for (int i = 0; i < nobs; i++)
    {
        int igx = Search(ivVar[i], xsiv[ivIndx], xgrid);
        midot[igx]++;
        if (dvVar[i] <= medVal)
        {
            mdot1++;
            ngrid1[igx]++;
        }
        else
        {
            mdot2++;
            ngrid2[igx]++;
        }
    }

// CALCULATE COMMON MEDIANS TEST STATISTIC
    double tcmd = 0.0;
    for (int i = 0; i < nxint; i++)
    {
        if ((midot[i] == 0) || (mdot1 == 0) || (mdot2 == 0))
        {
            testOK = false;
            return 0.0;
        }

        double term = double(midot[i]) / double(nobs);
        double ei1   = double(mdot1) * term;
        double ei2   = double(mdot2) * term;
        double term1 = Sqr(double(ngrid1[i]) - ei1) / ei1;
        double term2 = Sqr(double(ngrid2[i]) - ei2) / ei2;
        tcmd  += term1 + term2;
    }
    return tcmd;
}


void DFO_PATTRN::TestCMD(int ivIndx, int dvIndx)
{
    const PattrnTests currTest = ptCMD;
    const PattrnTests currTestMC = ptCMDMC;

    int imd = int(double(nobs) / 2.0);
    double ymed = ysdv[dvIndx][imd - 1];

    bool testOK = true;
    double tcmd = CMDTest(xiv[ivIndx], ydv[dvIndx], ymed, ivIndx, testOK);
    if (!testOK)
    {
        AddTestError("CMD", "mdot = 0", ivIndx);
        return;
    }

    stat[currTest][ivIndx] = tcmd;

    // CALCULATE TEST STATISITIC P-VALUE
    double df = double(nxint - 1);
    statpv[currTest][ivIndx] = GammaQ(0.5 * df, 0.5 * tcmd);

    // Monte-Carlo ?
    if (!tests[currTestMC])
        return;

    SC_DoubleArray ccdf(nrandomPatterns);

    for (int k = 0; k < nrandomPatterns; k++)
    {
        ccdf[k] = CMDTest(xivrp[k], ydvrp[k], ymed, ivIndx, testOK);
    }

    statpv[currTestMC][ivIndx] = CalcProb(currTestMC, ivIndx, dvIndx, tcmd, ccdf);
}

// SUBROUTINE CM PERFORMS THE COMMON MEANS TEST

double DFO_PATTRN::CMNTest(const SC_DoubleArray& ivVar,
                          const SC_DoubleArray& dvVar,
                                int             ivIndx,
                                bool&           testOK)
{
    testOK = true;

    SC_IntArray mi(nxint, 0);
    SC_DoubleArray ycol(nxint, 0.0);
    double ysum = 0.0;
    double yssq = 0.0;
    for (int i = 0; i < nobs; i++)
    {
        double yval = dvVar[i];
        if (logarithms)
            if (yval <= 0.0)
                yval = 0.0;
            else
                yval = log(yval);

        ysum += yval;
        yssq += Sqr(yval);

        int igx = Search(ivVar[i], xsiv[ivIndx], xgrid);
        mi[igx]++;
        ycol[igx] += yval;
    }

    double colssq = 0.0;
    for (int i = 0; i < nxint; i++)
    {
        if (mi[i] == 0)
        {
            testOK = false;
            return 0.0;
        }
        colssq += Sqr(ycol[i]) / double(mi[i]);
    }
    double df1 = double(nxint - 1);
    double df2 = double (nobs - nxint);
    double fnum = (colssq - (Sqr(ysum) / double(nobs))) / df1;
    double fden = (yssq - colssq) / df2;
    if (fden == 0.0)
    {
        testOK = false;
        return 0.0;
    }


    double fcmn = fnum / fden;
    if (fcmn < 0.0)
        fcmn = 0.0;
    return fcmn;
}


void DFO_PATTRN::TestCMN(int ivIndx, int dvIndx)
{
    if (nxint < 2)
    {
        AddTestError("CMN", "NXINT < 2", ivIndx);
        return;
    };

    const PattrnTests currTest = ptCMN;
    const PattrnTests currTestMC = ptCMNMC;

    bool testOK = true;
    double fcmn = CMNTest(xiv[ivIndx], ydv[dvIndx], ivIndx, testOK);
    if (!testOK)
    {
        AddTestError("CMN", "mi[i] = 0 || yssq = colssq", ivIndx);
        return;
    }

    stat[currTest][ivIndx] = fcmn;

    // CALCULATE TEST STATISITIC P-VALUE
    double df1 = double(nxint - 1);
    double df2 = double (nobs - nxint);

    statpv[currTest][ivIndx] = BetaI(0.5 * df2, 0.5 * df1, df2 / (df2 + df1 * fcmn));


    // Monte-Carlo ?
    if (!tests[currTestMC])
        return;

    SC_DoubleArray ccdf(nrandomPatterns);

    for (int k = 0; k < nrandomPatterns; k++)
    {
        ccdf[k] = CMNTest(xivrp[k], ydvrp[k], ivIndx, testOK);
    }

    statpv[currTestMC][ivIndx] = CalcProb(currTestMC, ivIndx, dvIndx, fcmn, ccdf);
}


double DFO_PATTRN::CIQTest(const SC_DoubleArray& ivVar,
                          const SC_DoubleArray& dvVar,
                             const double&   ydv25,
                             const double&   ydv75,
                                int          ivIndx,
                             bool&           testOK)
{
    testOK = true;
    SC_IntArray ngrid1(nxint, 0);
    SC_IntArray ngrid2(nxint, 0);

    SC_IntArray midot(nxint, 0);
    int mdot1 = 0;
    int mdot2 = 0;
    for (int i = 0; i < nobs; i++)
    {
        int igx = Search(ivVar[i], xsiv[ivIndx], xgrid);
        midot[igx]++;
        if ((dvVar[i] > ydv25) && (dvVar[i] <= ydv75))
        {
            mdot1++;
            ngrid1[igx]++;
        }
        else
        {
            mdot2++;
            ngrid2[igx]++;
        }
    }

// CALCULATE COMMON INTERQUARTILE TEST STATISTIC
    double tciq = 0.0;
    for (int i = 0; i < nxint; i++)
    {
        if ((midot[i] == 0) || (mdot1 == 0) || (mdot2 == 0))
        {
            testOK = false;
            return 0.0;
        }

        double term = double(midot[i]) / double(nobs);
        double ei1   = double(mdot1) * term;
        double ei2   = double(mdot2) * term;
        double term1 = Sqr(double(ngrid1[i]) - ei1) / ei1;
        double term2 = Sqr(double(ngrid2[i]) - ei2) / ei2;
        tciq  += term1 + term2;
    }
    return tciq;
}


void DFO_PATTRN::TestCIQ(int ivIndx, int dvIndx)
{
    const PattrnTests currTest = ptCIQ;
    const PattrnTests currTestMC = ptCIQMC;

    int i25 = int(0.25 * double(nobs));
    double y25 = ysdv[dvIndx][i25 - 1];

    int i75 = int(0.75 * double(nobs));
    double y75 = ysdv[dvIndx][i75 - 1];

    bool testOK = true;
    double tciq = CIQTest(xiv[ivIndx], ydv[dvIndx], y25, y75, ivIndx, testOK);
    if (!testOK)
    {
        AddTestError("CIQ", "midot[i] = 0 || mdot1 = 0 || mdot2  = 0", ivIndx);
        return;
    }

    stat[currTest][ivIndx] = tciq;

    // CALCULATE TEST STATISITIC P-VALUE
    double df = double(nxint - 1);
    statpv[currTest][ivIndx] = GammaQ(0.5 * df, 0.5 * tciq);

    // Monte-Carlo ?
    if (!tests[currTestMC])
        return;

    SC_DoubleArray ccdf(nrandomPatterns);

    for (int k = 0; k < nrandomPatterns; k++)
    {
        ccdf[k] = CIQTest(xivrp[k], ydvrp[k], y25, y75, ivIndx, testOK);
    }

    statpv[currTestMC][ivIndx] = CalcProb(currTestMC, ivIndx, dvIndx, tciq, ccdf);
}


double DFO_PATTRN::CVTest(const SC_DoubleArray& ivVar,
                           const SC_DoubleArray& dvVar,
                                 int             ivIndx,
                                 bool&           testOK,
                                 int             dvIndx, // only used in verbose output
                                 bool            writeCV)
{
    testOK = true;
    SC_IntArray mi(nxint, 0);
    SC_DoubleArray pcol(nxint, 0.0);
    SC_DoubleMatrix ycol(nxint, nobs);
    ycol.MatrixFill(0);

    for (int i = 0; i < nobs; i++)
    {
        int igx = Search(ivVar[i], xsiv[ivIndx], xgrid);
        ycol[igx][mi[igx]] = dvVar[i];
        mi[igx]++;
    }

    // write header
    if (writeCV)
    {
        char titStr[80];
        MakeString(titStr, "CV Test Summary for IV: ", GetIndepVar().GetVariableName(ivIndx),
            " and DV: ", GetDepVar().GetVariableName(dvIndx), 80);
        TextSubTitle(titStr);
    }
    double sums, sump, ssqs, ssqp; // for write only
    double psum = 0.0;
    double pssq = 0.0;
    for (int i = 0; i < nxint; i++)
    {
        if (writeCV)
        {
            sums = 0.0;
            sump = 0.0;
            ssqs = 0.0;
            ssqp = 0.0;
            AddBlankLine();
            AddStdInt("Intermediate results for column", i + 1);

            TableSetup(15, 2, 5, 3);
            SetTableCol("W/O Obs", "ColVar", "Pseudo-ColVar");
        }

        int nmi = mi[i];
        if (nmi < 3)
            continue;

        double fmi = double(nmi);

        // estimate variance for each column

        double ysum = 0.0;
        double yssq = 0.0;
        for (int j = 0; j < nmi; j++)
        {
            ysum += ycol[i][j];
            yssq += Sqr(ycol[i][j]);
        }
        double sisq = (yssq - (ysum * ysum / fmi)) / (fmi - 1.0);

        // compute new variance estimators for each column
        for (int j = 0; j < nmi; j++)
        {
            ysum = 0.0;
            yssq = 0.0;
            for (int k = 0; k < nmi; k++)
                if (j != k)
                {
                    ysum += ycol[i][k];
                    yssq += Sqr(ycol[i][k]);
                }

            double sigsq = (yssq - (ysum * ysum / (fmi - 1.0))) / (fmi - 2.0);

            //  compute pseudo-values for column variances
            //  also compute sums and sums of squares of pseudo variances
            double pig;

            if (logarithms)
                pig  = (fmi * log(sisq)) - ((fmi - 1.0) * log(sigsq));
            else
                pig  = (fmi * (sisq)) - ((fmi - 1.0) * (sigsq));

            if (writeCV)
            {
                sums += sigsq;
                sump += pig;
                ssqs += sigsq * sigsq;
                ssqp += pig * pig;

                SetTableInt(0, j + 1);
                SetTableReal(1, sigsq, SC_DecFormat(4));
                SetTableReal(2, pig, SC_DecFormat(4));
                AddNextLine();
            }

            psum += pig;
            pssq += pig * pig;
            pcol[i] += pig;
        }

        if (writeCV)
        {
            double smn = sums / fmi;
            double pmn = sump / fmi;
            double svr = (ssqs - (sums * sums / fmi)) / (fmi - 1.0);
            double pvr = (ssqp - (sump * sump / fmi)) / (fmi - 1.0);
            SC_DecFormat rForm(4);
            AddBlankLine();
            AddStdReal("Mean of column var", smn, rForm);
            AddStdReal("Mean of pseudo-column var", pmn, rForm);
            AddStdReal("Var of column var", svr, rForm);
            AddStdReal("Var of pseudo-column var", pvr, rForm);
            AddStdInt("# of obs in column", nmi);
            AddStdReal("Column var", sisq, rForm);
        }
    }

    // CALCULATE AND STORE COMMON VARIANCES TEST STATISTIC

    double colssq = 0.0;
    for (int i = 0; i < nxint; i++)
        if (mi[i] > 0)
            colssq += Sqr(pcol[i]) / double(mi[i]);

    double df1  = double(nxint - 1);
    double df2  = double(nobs - nxint);
    double fnum = (colssq - (psum * psum / double(nobs))) / df1;
    double fden = (pssq - colssq) / df2;
    double fcv  = fnum / fden;
    if (fcv < 0.0)
        fcv = 0.0;

    if (writeCV)
    {
        SC_DecFormat rForm(4);
        AddBlankLine();

        AddLine("CVT Intermediate Results");
        AddStdInt("# of observations", nobs);
        AddStdInt("Numerator degrees of freedom", nxint - 1);
        AddStdInt("Denominator degrees of freedom", nobs - nxint);
        AddStdReal("Column sum of squares", colssq, rForm);
        AddStdReal("Sum of pseudo-var", psum, rForm);
        AddStdReal("Sum of squared pseudo-var", pssq, rForm);
        AddStdReal("Test statistic numerator", fnum, rForm);
        AddStdReal("Test statistic denominator", fden, rForm);
        AddStdReal("Test statistic", fcv, rForm);

        double pval = BetaI(0.5 * df2, 0.5 * df1, df2 / (df2 + df1 * fcv));
        AddStdReal("Test statistic P-value", pval, rForm);
        AddBlankLine();
    }
    return fcv;
}


void DFO_PATTRN::TestCV(int ivIndx, int dvIndx)
{
    if (nxint < 2)
    {
        AddTestError("CV", "NXINT < 2",ivIndx);
        return;
    };

    const PattrnTests currTest = ptCV;
    const PattrnTests currTestMC = ptCVMC;

    bool testOK = true;
    double fcv= CVTest(xiv[ivIndx], ydv[dvIndx], ivIndx, testOK, dvIndx, verboseCV);

    stat[currTest][ivIndx] = fcv;

    // CALCULATE TEST STATISITIC P-VALUE
    double df1 = double(nxint - 1);
    double df2 = double (nobs - nxint);
    double pval = BetaI(0.5 * df2, 0.5 * df1, df2 / (df2 + df1 * fcv));

    statpv[currTest][ivIndx] = pval;

    // Monte-Carlo ?
    if (!tests[currTestMC])
        return;

    SC_DoubleArray ccdf(nrandomPatterns);

    for (int k = 0; k < nrandomPatterns; k++)
    {
        ccdf[k] = CVTest(xivrp[k], ydvrp[k], ivIndx, testOK);
    }

    statpv[currTestMC][ivIndx] = CalcProb(currTestMC, ivIndx, dvIndx, fcv, ccdf);
}



double DFO_PATTRN::DRTest(const SC_DoubleArray& ivVar,
                          const SC_DoubleArray& dvVar,
                                int             ivIndx)
{
    int nsampleObs = nobs / nrep;

    SC_DoubleArray xwork(nsampleObs), ywork(nsampleObs);
    SC_DoubleMatrix rydv(nrep, nsampleObs);
    SC_IntArray indexes(nsampleObs);

    int repStart = 0;
    for (int i = 0; i < nrep; i++)
    {
        for (int j = 0; j < nsampleObs; j++)
        {
            xwork[j] = ivVar[repStart + j];
            ywork[j] = dvVar[repStart + j];
        }
        RankArray(ywork);
        indexes.AllocAndIndex(nsampleObs);
        xwork.Sort(true, indexes);
        for (int j = 0; j < nsampleObs; j++)
            rydv[i][j] = ywork[indexes[j]];

        repStart += nsampleObs;
    }

    //  calculate and store difference of ranks test statistic
    double tdr = 0.0;
    for (int i = 0; i < nrep; i++)
        for (int j = 1; j < nsampleObs; j++)
            tdr += Sqr(rydv[i][j] - rydv[i][j-1]);

    double frep   = double(nrep);
    double fobs   = double(nsampleObs);

    // calculate mean
    double fmu = fobs * (fobs * fobs - 1.0) / 6.0;

    // calculate variance
    double sigman = 0.0;
    if (nsampleObs > 40)
        sigman = (1.0 / 6.0);
    else if (nsampleObs > 10)
        sigman = (1.0 / 6.0) - (0.136 / fobs);
    else if (nsampleObs == 10)
        sigman = 0.152;
    else if (nsampleObs == 9)
        sigman = 0.150;
    else if (nsampleObs == 8)
        sigman = 0.147;
    else if (nsampleObs == 7)
        sigman = 0.143;
    else if (nsampleObs == 6)
        sigman = 0.138;
    else if (nsampleObs == 5)
        sigman = 0.130;
    else if (nsampleObs == 4)
        sigman = 0.117;
    else
        sigman = 0.091;

    double v = pow(fobs, 5.0) * sigman * sigman ;

    return (tdr - frep * fmu) / sqrt(frep * v);
}




void DFO_PATTRN::TestDR(int ivIndx, int dvIndx)
{
    const PattrnTests currTest = ptDR;
    const PattrnTests currTestMC = ptDRMC;

    double z = DRTest(xiv[ivIndx], ydv[dvIndx], ivIndx);

    stat[currTest][ivIndx] = z;

    // SIGN(0.5, Z)
    double sgnZ = 0.5;
    if (z < 0.0)
        sgnZ = -0.5;

    statpv[currTest][ivIndx] =  0.5 + sgnZ * GammaP(0.5, 0.5 * z * z);

    // Monte-Carlo ?
    if (!tests[currTestMC])
        return;

    SC_DoubleArray ccdf(nrandomPatterns);

    for (int k = 0; k < nrandomPatterns; k++)
    {
        ccdf[k] = DRTest(xivrp[k], ydvrp[k], ivIndx);
    }

    statpv[currTestMC][ivIndx] = CalcProb(currTestMC, ivIndx, dvIndx, z, ccdf);
}

double DFO_PATTRN::FRTest(const SC_DoubleArray& ivVar,
                          const SC_DoubleArray& dvVar,
                                int             ivIndx)
{
    int nsampleObs = nobs / nrep;

    SC_DoubleArray xwork(nsampleObs), ywork(nsampleObs);
    SC_DoubleMatrix rydv(nrep, nsampleObs);
    SC_IntArray indexes(nsampleObs);

    int repStart = 0;
    for (int i = 0; i < nrep; i++)
    {
        for (int j = 0; j < nsampleObs; j++)
        {
            xwork[j] = ivVar[repStart + j];
            ywork[j] = dvVar[repStart + j];
        }
        RankArray(ywork);
        indexes.AllocAndIndex(nsampleObs);
        xwork.Sort(true, indexes);
        for (int j = 0; j < nsampleObs; j++)
            rydv[i][j] = ywork[indexes[j]];

        repStart += nsampleObs;
    }

    // loop over number of observations
    double a2 = 0.0;
    double b2 = 0.0;
    for (int i = 0; i < nsampleObs; i++)
    {
        //  find sum of ranks and sum of squares of ranks
        double rsum = 0.0;
        for (int j = 0; j < nrep; j++)
        {
            const double& rVal = rydv[j][i];
            rsum += rVal;
            a2   += Sqr(rVal);
        }
        b2 += rsum * rsum;
    }

    double frep = double(nrep);
    b2 /= frep;

    // calculate and store friedman test statistic
    double fobs   = double(nsampleObs);
    double fnum = (frep - 1.0) * (b2 - frep * fobs * Sqr(fobs + 1.0) / 4.0);

    double t2 = 1.0e6;
    if (a2 != b2)
        t2 = fnum / (a2 - b2);

    if (t2 < 0.0)
        t2 = 0.0;

    return t2;
}


void DFO_PATTRN::TestFR(int ivIndx, int dvIndx)
{
    const PattrnTests currTest = ptFR;
    const PattrnTests currTestMC = ptFRMC;

    double t2 = FRTest(xiv[ivIndx], ydv[dvIndx], ivIndx);

    stat[currTest][ivIndx] = t2;

    double df1 = double(nobs / nrep - 1);
    double df2 = df1 * double(nrep - 1);

    statpv[currTest][ivIndx] =  BetaI(0.5 * df2, 0.5 * df1, df2 / (df2 + df1 * t2));

    // Monte-Carlo ?
    if (!tests[currTestMC])
        return;

    SC_DoubleArray ccdf(nrandomPatterns);

    for (int k = 0; k < nrandomPatterns; k++)
    {
        ccdf[k] = FRTest(xivrp[k], ydvrp[k], ivIndx);
    }

    statpv[currTestMC][ivIndx] = CalcProb(currTestMC, ivIndx, dvIndx, t2, ccdf);
}

void DFO_PATTRN::TestPCSR(int dvIndx)
{
    int nmat = niv + 1;
    SC_DoubleMatrix corMat(nmat, nmat);
    SC_DoubleMatrix rcorMat(nmat, nmat);
    corMat.MatrixFill(1.0);
    rcorMat.MatrixFill(1.0);

    for (int i = 0; i < (niv - 1); i++)
        for (int j = i + 1; j < niv; j++)
        {
            corMat[i][j] = CalcCC(xiv[i], xiv[j]);
            rcorMat[i][j] = CalcCC(xriv[i], xriv[j]);

            corMat[j][i]  = corMat[i][j];
            rcorMat[j][i]  = rcorMat[i][j];
        }

    for (int i = 0; i < niv; i++)
    {
        corMat[niv][i] = CalcCC(xiv[i], ydv[dvIndx]);
        rcorMat[niv][i] = CalcCC(xriv[i], yrdv[dvIndx]);

        corMat[i][niv] = corMat[niv][i];
        rcorMat[i][niv] = rcorMat[niv][i];
    }

    SC_StringArray dummy;
    bool matErr = false;
    if (InvertSymmetricMatrix(corMat, dummy, false) != 0)
    {
        AddLine("*** ERROR - inverting correlation matrix");
        matErr = true;
    }
    if (InvertSymmetricMatrix(rcorMat, dummy, false) != 0)
    {
        AddLine("*** ERROR - inverting rank correlation matrix");
        matErr = true;
    }
    if (matErr)
    {
        AddStdText("Dependent variable", GetDepVar().GetVariableName(dvIndx));
        AddBlankLine();
    }

    int ptPCCi = ptPCC;
    for (int i = 0; i < niv; i++)
    {
        statpv[ptPCC][i] = -corMat[i][niv] / sqrt(corMat[i][i] * corMat[niv][niv]);
        statpv[ptSRC][i] = -corMat[i][niv] / corMat[niv][niv];
        statpv[ptPRCC][i] = -rcorMat[i][niv] / sqrt(rcorMat[i][i] * rcorMat[niv][niv]);
        statpv[ptSRRC][i] = -rcorMat[i][niv] / rcorMat[niv][niv];
    }
}


void DFO_PATTRN::SetIVDVOK(int dvIndx)
{
    SC_IntArray maco(niv);
    maco.SetEmpty();
    for (int i = 0; i < ptLastTest; i++)
        if (tests[i] && (!PCCSRCTest(i)))
        {
            CalcCO(statpv[i], maco);
        }

    if (maco.IsNotEmpty())
    {
        ivAlphaOK.SetIndexes(maco, true);
        dvAlphaOK[dvIndx] = true;
    }
}


void DFO_PATTRN::WriteResults(int dvIndx)
{

//  DETERMINE WHICH VARIABLES' P-VALUES EXCEED ALPHA CUTOFF

    SC_IntArray maco(niv);
    maco.SetEmpty();
    for (int i = 0; i < ptLastTest; i++)
        if (tests[i] && (!PCCSRCTest(i)))
        {
            CalcCO(statpv[i], maco);
        }

    if (maco.IsEmpty())
    {
        AddStdReal("*** no IV exceed alpha cutoff ***", alphaCutoff, SC_DecFormat(4));
        return;
    }


//  CALCULATE IMPORTANCE ORDERING FROM P-VALUES

    SC_DoubleMatrix rio;
    rio.MatrixAllocAndSetSize(ptLastTest, niv);

    for (int i = 0; i < ptLastTest; i++)
    {
        if (!tests[i])
            continue;

        rio[i] = statpv[i];
        if (PCCSRCTest(i))
        {
            for (int j = 0; j < niv; j++)
                rio[i][j] = -fabs(rio[i][j]);
        }
        RankArray(rio[i]);
    }


    // COPY AND SORT USER SELECTED TEST STATISTIC
    int naco = maco.Size();
    SC_DoubleArray temp(naco);
    for (int i = 0; i < naco; i++)
    {
        temp[i] = statpv[mtest1][maco[i]];
        if (PCCSRCTest(mtest1))
            temp[i] = -fabs(temp[i]);
    }

    temp.Sort(true, maco);

//  WRITE IMPORTANCE ORDERINGS AND P-VALUES (OR VALUES) FOR
//  THOSE INDEPENDENT VARIABLES THAT EXCEED THE ALPHA CUTOFF

    char titStr[80];
    MakeString(titStr, "Results for dependent variable: ", GetDepVar().GetVariableName(dvIndx), 80);
    TextStdTitle(titStr);
    AddBlankLine();

    // set globals
    tests.MakeForwardMap(selTests);
    ntests = selTests.Size();

    // main response importance table
    ImportanceTable(maco, rio);

    // importance orders
    ImportanceOrdering(maco, rio);

    if (ntests > 1)
    {
        SC_DoubleMatrix ssio(ptLastTest, naco);
        rio.MatrixSetSize(ptLastTest, naco);

        SC_StringArray selTestNames;
        selTestNames.Alloc(ntests);

        for (int i = 0; i < ptLastTest; i++)
        {
            if (!tests[i])
                continue;

            selTestNames += testStrs[i];

            for (int j = 0; j < naco; j++)
            {
                double& currRI = rio[i][j];
                currRI = statpv[i][maco[j]];
                if (PCCSRCTest(i))
                    currRI = -fabs(currRI);
            }
            RankArray(rio[i]);
            CalcSS(rio[i], ssio[i]);
        }

        // calculate top-down correlation matrix and
        // calculate top-down correlation matrix p-values

        SC_DoubleMatrix tdcm(ntests, ntests);
        SC_DoubleMatrix tdcmpv(ntests, ntests);
        tdcm.MatrixFill(1.0);
        tdcmpv.MatrixFill(1.0);

        double faco = sqrt(double(naco - 1));
        for (int i = 1; i < ntests; i++)
        {
            for (int j = 0; j < i; j++)
            {
                double t = CalcCC(ssio[selTests[i]], ssio[selTests[j]]);
                tdcm[i][j] = t;
                tdcm[j][i] = t;

                double z = faco * t;
                double zsign = 0.5;
                if (z < 0.0)
                    zsign = -0.5;

                double tpv = 0.5 - zsign * GammaP(0.5, 0.5 * z * z);
                tdcmpv[i][j] = tpv;
                tdcmpv[j][i] = tpv;
            }
        }

        TextSubTitle("Top-Down Correlation Matrix");
        ListSymmetricMatrix(tdcm, selTestNames, coeffFormat, 10, sfLTri, 10, "Test");

        TextSubTitle("Top-Down Correlation Matrix P-Values");
        ListSymmetricMatrix(tdcmpv, selTestNames, coeffFormat, 10, sfLTri, 10, "Test");
    }

    if (verbose && (ntests > 1))
        WriteDetailedSummary();

    if (coefficients)
        WriteCoeffSummary(false);

    if (rankCoefficients)
        WriteCoeffSummary(true);
}



void DFO_PATTRN::ImportanceTable(const SC_IntArray& maco,
                                 const SC_DoubleMatrix& rio)

{
    int ntbl = ntests / 4;
    if ((ntests % 4) != 0)
        ntbl++;

    const int headerWidth[] = {15, 12, 18, 18, 18, -1};
    const int dataWidth[] = {15, 9, 9, 9, 9, 9, 9, 9, 9, -1};
    const bool lJust[] = {true, false, false , false, false, false, false , false, false, false};

    int naco = maco.Size();
    int tStart = 0;
    for (int i = 0; i < ntbl; i++)
    {
        TableSetup(headerWidth, lJust);
        SetTableCol(0, "Test:");
        int tIndx = tStart;
        for (int j = 0; (j < 4) && (tIndx < ntests); j++, tIndx++)
            SetTableCol(j + 1, testStrs[selTests[tIndx]]);
        AddNextLine();

        TableSetup(dataWidth, lJust);
        SetTableCol(0, "IndVar Name");
        tIndx = tStart;
        int colIndx = 1;
        for (int j = 0; (j < 4) && (tIndx < ntests); j++, tIndx++)
        {
            int tNum = selTests[tIndx];
            SetTableCol(colIndx++, "RANK");
            if (PCCSRCTest(tNum))
                SetTableCol(colIndx++, "VALUE");
            else
                SetTableCol(colIndx++, "P-VAL");
        }
        AddNextLine();

        for (int k = 0; k < naco; k++)
        {
            int ivIndx = maco[k];
            SetTableCol(0, GetIndepVar().GetVariableName(ivIndx));

            tIndx = tStart;
            int colIndx = 1;
            for (int j = 0; (j < 4) && (tIndx < ntests); j++, tIndx++)
            {
                int tNum = selTests[tIndx];
                SetTableReal(colIndx++, rio[tNum][ivIndx], SC_DecFormat(1));
                SetTableReal(colIndx++, statpv[tNum][ivIndx], coeffFormat);
            }
            AddNextLine();
        }

        AddBlankLine();
        tStart += 4;
    }
}


void DFO_PATTRN::ImportanceOrdering(const SC_IntArray& maco,
                                    const SC_DoubleMatrix& rio)
{
    int naco = maco.Size();
    {
        int ntbl = naco / 10;
        if ((naco % 10) > 0)
            ntbl++;

        SC_IntArray colWidth(11, 7);
        TableSetup(colWidth, 0);
        int ivStart = 0;
        for (int i = 0; i < ntbl; i++)
        {
            SetTableCol(0, "Test");
            int ivIndx = ivStart;
            for (int j = 0; (j < 10) && (ivIndx < naco); j++, ivIndx++)
                SetTableInt(j + 1, maco[ivIndx] + 1);
            AddNextLine();

            for (int k = 0; k < ntests; k++)
            {
                int tIndx = selTests[k];
                SetTableCol(0, testStrs[tIndx]);
                ivIndx = ivStart;
                for (int j = 0; (j < 10) && (ivIndx < naco); j++, ivIndx++)
                    SetTableReal(j + 1, rio[tIndx][maco[ivIndx]], SC_DecFormat(1));
                AddNextLine();
            }

            AddBlankLine();
            ivStart += 10;
        }

    }

    {
        // legend for table
        AddLine("Table Legend");
        SC_IntArray colWidth(2, 15);
        colWidth[1] = 5;
        TableSetup(colWidth, 0);
        SetTableCol("Indep Var", "Index");
        for (int i = 0; i < naco; i++)
        {
            int ivIndx = maco[i];
            SetTableCol(0, GetIndepVar().GetVariableName(ivIndx));
            SetTableInt(1, ivIndx + 1);
            AddNextLine();
        }
        AddBlankLine();
    }
}


void DFO_PATTRN::CalcSS(const SC_DoubleArray& rio,
                           const SC_DoubleArray& ssa)
{
    int n = rio.Size();
    SC_DoubleArray rwk(rio);
    SC_IntArray iwk;
    iwk.AllocAndIndex(n);

    // sort importance orderings while carrying along pointers
    rwk.Sort(true, iwk);

    // find ties among importance orderings or
    // assign savage scores to non-tied importance orderings
    int iio = -1;
    while (true) {

        iio++;

        // last importance ordering so assign savage score and return
        if (iio == (n - 1))
        {
            ssa[iwk[iio]] = svgsc[iio];
            return;
        }

        // if no tie then assign savage score and continue search for ties
        if (rwk[iio] != rwk[iio + 1])
        {
            ssa[iwk[iio]] = svgsc[iio];
            continue;
        }

        // tie detected so count ties among importance orderings
        int ntio = 2;
        while ((iio + ntio) < n)
        {
            if (rwk[iio] != rwk[iio + ntio])
                break;
            ntio++;
        }

        // calculate average savage score from tied importance orderings
        double avg = 0.0;
        for (int i = 0; i < ntio; i++)
            avg += svgsc[iio + i];

        avg /= double(ntio);

        // store average savage score
        iio--;
        for (int i = 0; i < ntio; i++)
        {
            iio++;
            ssa[iwk[iio]] = avg;
        }

        if (iio >= (n - 1))
            return;
    }

    return;
}


void DFO_PATTRN::WriteDetailedSummary()
{
    SC_IntArray maco(niv);
    SC_DoubleArray rio(niv);

    for (int i = 0; i < ptLastTest; i++)
    {
        if ((!tests[i]) || PCCSRCTest(i) || MonteCarloTest(i))
            continue;

        maco.SetEmpty();
        CalcCO(statpv[i], maco);

        if (maco.IsEmpty())
        {
            char outStr[80];
            char rStr[40];
            RealToString(SC_DecFormat(3), alphaCutoff, rStr, 40);
            MakeString(outStr, "*** Test: ", testStrs[i], " -- no results meeting Alpha cutoff: ", rStr, 80);
            AddLine(outStr);
            continue;
        }
        int naco = maco.Size();

        // calculate importance orderings
        rio = statpv[i];
        RankArray(rio);

        // copy and sort test statistic
        SC_DoubleArray rwk(naco);
        for (int j = 0; j < naco; j++)
            rwk[j] = statpv[i][maco[j]];
        rwk.Sort(true, maco);

        // write the detailed test summary
        char outStr[80];
        MakeString(outStr, "Detailed Test Summary for Test: ", testStrs[i], 80);
        TextSubTitle(outStr);

        SC_IntArray colWidths(6, 5);
        colWidths[0] = IMax(12, GetIndepVar().variableNames.GetMaxStringLen() + 1);
        colWidths[2] = 12;
        colWidths[3] = 12;
        TableSetup(colWidths, 0);
        SetTableCol(0, "Variable");
        SetTableCol(1, "Rank");
        SetTableCol(2, "Statistic");
        SetTableCol(3, "P-Value");

        // 5, 7, 17
        if ((i == ptCC) || (i == ptRCC) || (i == ptDR))
        {
        }
        // 1 3 9 13
        else if ((i == ptSI) ||(i == ptCL) || (i == ptCMD) || (i == ptCIQ))
        {
            SetTableCol(4, "DF");
        }
        // 11 15 19
        else if ((i == ptCMN) || (i == ptCV) || (i == ptFR))
        {
            SetTableCol(4, "DFN");
            SetTableCol(5, "DFD");
        }
        AddNextLine();

        for (int j = 0; j < naco; j++)
        {
            int ivIndx = maco[j];
            SetTableCol(0, GetIndepVar().GetVariableName(ivIndx));
            SetTableReal(1, rio[ivIndx], SC_DecFormat(1));
            SetTableReal(2, stat[i][ivIndx], coeffFormat);
            SetTableReal(3, statpv[i][ivIndx], coeffFormat);

            // 1
            int ndf = (nxgrd[ivIndx] - 1);
            if (i == ptSI)
            {
                ndf *= (nyint - 1);
                SetTableInt(4, ndf);
            }
            // 3 9 13
            else if ((i == ptCL) || (i == ptCMD) || (i == ptCIQ))
            {
                SetTableInt(4, ndf);
            }
            // 11 15 19
            else if ((i == ptCMN) || (i == ptCV) || (i == ptFR))
            {
                int ndfd;
                if (i == ptFR)
                {
                    ndf = (nobs / nrep) - 1;
                    ndfd = ndf * (nrep - 1);
                }
                else
                {
                    ndfd = nobs - nxgrd[ivIndx];
                }
                SetTableInt(4, ndf);
                SetTableInt(5, ndfd);
            }

            AddNextLine();
        }
        AddBlankLine();
    }

}


void DFO_PATTRN::WriteCoeffSummary(bool rankSummary)
{
    // select summary vars
    PattrnTests corrT = ptCC;
    PattrnTests coeffT = ptPCC;
    PattrnTests coeffT1 = ptSRC;
    if (rankSummary)
    {
        corrT = ptRCC;
        coeffT = ptPRCC;
        coeffT1 = ptSRRC;
    }

    SC_IntArray maco(niv);
    maco.SetEmpty();
    CalcCO(statpv[corrT], maco);

    if (maco.IsEmpty())
    {
        char outStr[80];
        char rStr[40];
        RealToString(SC_DecFormat(3), alphaCutoff, rStr, 40);
        const char* eStr = "";
        if (rankSummary)
            eStr = "Rank ";
        MakeString(outStr, "*** For the ", eStr, " Coefficient summary there are no IV meeting AC ", rStr, " ***", 80);
        AddLine(outStr);
        return;
    }

    if (rankSummary)
        TextSubTitle("Rank Coefficients Summary");
    else
        TextSubTitle("Coefficients Summary");

    SC_DoubleMatrix rio(3, niv);

    rio[0] = statpv[corrT];
    rio[1] = statpv[coeffT];
    rio[2] = statpv[coeffT1];
    for (int i = 0; i < niv; i++)
    {
        rio[1][i] = -fabs(rio[1][i]);
        rio[2][i] = -fabs(rio[2][i]);
    }

    RankArray(rio[0]);
    RankArray(rio[1]);
    RankArray(rio[2]);

    // copy and sort test statistic
    int naco = maco.Size();
    SC_DoubleArray rwk(naco);
    for (int j = 0; j < naco; j++)
        rwk[j] = statpv[corrT][maco[j]];
    rwk.Sort(true, maco);

    // write summary
    SC_IntArray headerColWidths(4);
    headerColWidths[0] = 15;
    headerColWidths[1] = 15;
    headerColWidths[2] = 25;
    headerColWidths[3] = 20;
    TableSetup(headerColWidths, 0);
    SetTableCol("Indep", testStrs[corrT], testStrs[coeffT],  testStrs[coeffT1]);

    SC_IntArray colWidths(8, 10);
    colWidths[0] = 15;
    colWidths[2] = 5;
    TableSetup(colWidths, 0);
    SetTableCol(0, "Var ID");
    SetTableCol(1, "P-Val");
    SetTableCol(2, "Rank");
    SetTableCol(3, "Value");
    SetTableCol(4, "Rank");
    SetTableCol(5, "Value");
    SetTableCol(6, "Rank");
    SetTableCol(7, "Value");
    AddNextLine();

    for (int i = 0; i < naco; i++)
    {
        int ivIndx = maco[i];
        SetTableCol(0, GetIndepVar().GetVariableName(ivIndx));
        SetTableReal(1, statpv[corrT][ivIndx], coeffFormat);
        SetTableReal(2, rio[0][ivIndx], SC_DecFormat(1));
        SetTableReal(3, stat[corrT][ivIndx], coeffFormat);
        SetTableReal(4, rio[1][ivIndx], SC_DecFormat(1));
        SetTableReal(5, statpv[coeffT][ivIndx], coeffFormat);
        SetTableReal(6, rio[2][ivIndx], SC_DecFormat(1));
        SetTableReal(7, statpv[coeffT1][ivIndx], coeffFormat);
        AddNextLine();
    }

    AddBlankLine();
}


void DFO_PATTRN::SetCDF(const SC_DoubleArray& varVal,
                                             int             repIndx,
                                         DC_XYData&      varCDF)
{
    int nsampleObs = nobs / nrep;
    SC_DoubleArray ywork(nsampleObs);
    int repStart = repIndx * nsampleObs;
    for (int j = 0; j < nsampleObs; j++)
    {
        ywork[j] = varVal[repStart + j];
    }
    ywork.Sort(true);

    DC_XYCDF tempCDF;
    if (!tempCDF.CreateFrom(ywork, ywork))
    {
        AddStdText("**** Error - allocating CDF ?? ***", varCDF.GetID());
        return;
    }

    tempCDF.SetCDFQuantile(DC_XYCDF::qtExcel, 0.0);
    tempCDF.SetLinear();

    SC_SetupErr cdfErr;
    if (!tempCDF.SetupOK(cdfErr))
    {
        AddStdText("**** Error - calculating CDF ***", varCDF.GetID());
        return;
    }

    varCDF = tempCDF;

}


void DFO_PATTRN::AddTestError(const char* test, const char* errCond, int ivIndx)
{
    char errStr[80];
    MakeString(errStr, "*** ", test, " test error: ", errCond, " ***", 80);
    AddLine(errStr);
    AddStdText("** Bad variable:", GetIndepVar().GetVariableName(ivIndx));
    AddBlankLine();
}

