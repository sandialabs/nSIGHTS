///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SAObjBase.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <saClass/DO_SATable.h>

#include <saDFO/DFO_SAObjBase.h>

DFO_SAObjBase :: DFO_SAObjBase(const char* inID) : FuncObjC(inID)
{
    InitCommon();
}

DFO_SAObjBase::DFO_SAObjBase(const DFO_SAObjBase& a) : FuncObjC(a)
{
    InitCommon();
    LocalCopy(a);
}

DFO_SAObjBase :: ~DFO_SAObjBase ()
{
}

void DFO_SAObjBase::InitCommon()
{
    AddInPort(indepVarObjRef, typeid(DO_SATable));
    AddInPort(depVarObjRef, typeid(DO_SATable));
    progListingDO.listData = &listingData;
    AddOutPort(progListingDO);
}


DFO_SAObjBase& DFO_SAObjBase::operator= (const DFO_SAObjBase& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void  DFO_SAObjBase::LocalCopy(const DFO_SAObjBase& a)
{
    CopyString(title, a.title, titleLen);
}

void DFO_SAObjBase::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(indepVarObjRef, "Independent variables");
    CheckInputFO(depVarObjRef, "Dependent variables");
    if (StatusNotOK())
        return;

    const DC_SATable& indVar = GetIndepVar();
    if (indVar.GetNStep() > 1)
        SetObjErrMsg("independent variable can have only 1 step");

    const DC_SATable& depVar = GetDepVar();
    if (indVar.GetNVector() != depVar.GetNVector())
        SetObjErrMsg("independent and dependent variables must have the same # of observations");
}


const DC_SATable& DFO_SAObjBase::GetDepVar()
{
    return static_cast<DO_SATable*>(GetInPortData(depVarObjRef))->GetSATableRef();
}

const DC_SATable& DFO_SAObjBase::GetIndepVar()
{
    return static_cast<DO_SATable*>(GetInPortData(indepVarObjRef))->GetSATableRef();
}

void DFO_SAObjBase::FullVariableList()
{
    TextStdTitle("Independent Variable Input");
    ListOneVar(GetIndepVar());

    TextStdTitle("Dependent Variable Input");
    ListOneVar(GetDepVar());
}

void DFO_SAObjBase::ListOneVar(const DC_SATable&    var)
{
    if (IsNotNull(var.originalFile) || var.processingSteps.IsNotEmpty())
    {
        char outStr[256];

        if (IsNotNull(var.originalFile))
        {
            TextSubTitle("Source File");
            AddLine(var.originalFile);
        }

        if (var.processingSteps.IsNotEmpty())
        {
            AddBlankLine();
            TextSubTitle("Processing:");
            for (int i = 0; i < var.processingSteps.Size(); i++)
            {
                MakeString(outStr, "! ", var.processingSteps[i], 256);
                AddLine(outStr);
            }
        }
        AddBlankLine();
    }

    if (var.GetNStep() > 1)
        AddStdInt("Number of steps", var.GetNStep());
    AddStdInt("Number of variables", var.GetNVariable());
    AddStdInt("Number of vectors", var.GetNVector());
    AddBlankLine();

    const int maxCol = 5;
    SC_IntArray colWidths(maxCol, 12);
    TableSetup(colWidths, 2);
    tableLeftJustify.FillToAlloc(true);

    TextSubTitle("Variables");
    int colNum = 0;
    for (int i = 0; i < var.GetNVariable(); i++)
    {
        SetTableCol(colNum++, var.GetVariableName(i));
        if (colNum == maxCol)
        {
            AddNextLine();
            colNum = 0;
        }
    }
    if (colNum > 0)
        AddNextLine();
    AddBlankLine();

    if (var.GetNStep() > 1)
    {
        TextSubTitle("Steps");
        tableLeftJustify.FillToAlloc(false);
        colNum = 0;
        for (int i = 0; i < var.GetNStep(); i++)
        {
            SetTableReal(colNum++, var.stepValues[i], SC_DecFormat(3));
            if (colNum == maxCol)
            {
                AddNextLine();
                colNum = 0;
            }
        }
        if (colNum > 0)
            AddNextLine();
        AddBlankLine();
    }
    AddBlankLine();
}


void DFO_SAObjBase::VariablesNameList(int skipIDVar)
{
    AddConditionalBlank();
    {
        const int cWid[] = {30, 30, -1};
        const bool lJust[] = {true, true, };
        TableSetup(cWid, lJust);

        SetTableCol("Independent Variables", "Dependent Variables");
        SetTableCol("---------------------", "-------------------");
    }

    {
        const int cWid[] = {8, 18, 8, 18, -1};
        const bool lJust[] = {false, true, false, true};
        TableSetup(cWid, lJust, 2, 2);
    }

    const DC_SATable& depVar = GetDepVar();
    const DC_SATable& indepVar = GetIndepVar();

    int maxRow = depVar.GetNVariable();
    int niv = indepVar.GetNVariable();
    if (skipIDVar >= 0)
        niv--;
    maxRow = IMax(maxRow, niv);

    int nextIV = 0;
    for (int i = 0; i < maxRow; i++)
    {
        bool rowSet = false;
        if (nextIV < indepVar.GetNVariable())
        {
            if (nextIV != skipIDVar)
            {
                SetTableInt(0, indepVar.variableIndexes[nextIV] + 1);
                SetTableCol(1, indepVar.GetVariableName(nextIV));
                rowSet = true;
            }
            nextIV++;
        }
        if (i < depVar.GetNVariable())
        {
            SetTableInt(2, depVar.variableIndexes[i] + 1);
            SetTableCol(3, depVar.GetVariableName(i));
            rowSet = true;
        }
        if (rowSet)
            AddNextLine();
    }
    AddBlankLine();
}

void DFO_SAObjBase::ListSymmetricMatrix(const SC_DoubleMatrix& a,
                                        const SC_StringArray& rcIDs,
                                        const SC_RealFormat&  rForm,
                                                int           minColWidth,
                                                SymFormat     symFormat,
                                                int           maxCol,
                                        const char*           colType)
{
    int baseColWidth = IMax(minColWidth, rcIDs.GetMaxStringLen() + 1);
    SC_IntArray colWidths(maxCol + 5);
    colWidths.FillToAlloc(baseColWidth);
    TableSetup(colWidths, 1);

    int ndata = a.Size();
    int stCol = 0;
    int lastCol = ndata;
    int stRow = 0;
    int lastRow = ndata;
    if (symFormat == sfUTri)
    {
        stCol++;
        lastRow--;
    }
    else if (symFormat == sfLTri)
    {
        stRow++;
        lastCol--;
    }

    int nrc = ndata;
    if (symFormat != sfAll)
        nrc--;

    int ntables = nrc / maxCol;
    if ((nrc  % maxCol) > 0)
        ntables++;

    for (int i = 0; i < ntables; i++)
    {
        int endCol = IMin(stCol + maxCol, lastCol);

        int nextCol = 1;
        if (symFormat != sfLTri)
        {
            SetTableCol(0, colType);
            for (int k = stCol; k < endCol; k++)
            {
                SetTableCol(nextCol, rcIDs[k]);
                nextCol++;
            }
            AddNextLine();
        }

        // data
        bool doRow = true;
        for (int j = stRow; j < lastRow; j++)
        {
            if (symFormat == sfUTri)
                doRow = j < (endCol - 1);
            else
                if (symFormat == sfLTri)
                    doRow = j > stCol;
            if (!doRow)
                continue;

            SetTableCol(0, rcIDs[j]);
            nextCol = 1;
            bool doCol = true;
            for (int k = stCol; k < endCol; k++)
            {
                if (symFormat == sfUTri)
                    doCol = k > j;
                else
                    if (symFormat == sfLTri)
                        doCol = k < j;

                if (doCol)
                    SetTableReal(nextCol, a[k][j], rForm);

                nextCol++;
            }
            AddNextLine();
        }

        if (symFormat == sfLTri)
        {
            nextCol = 1;
            SetTableCol(0, colType);
            for (int k = stCol; k < endCol; k++)
            {
                SetTableCol(nextCol, rcIDs[k]);
                nextCol++;
            }
            AddNextLine();
        }


        AddBlankLine();
        stCol += maxCol;
    }
    AddBlankLine();

}

void DFO_SAObjBase::RankArray(SC_DoubleArray& x)
{
    int ndata = x.Size();

    SC_IntArray indx;
    indx.AllocAndIndex(ndata);
    x.Sort(true, indx);

    SC_DoubleArray ranks;

    SetRanks(x, ranks);

    // replace inData with correct order ranks
    for (int i = 0; i < ndata; i++)
        x[indx[i]] = ranks[i];

}

void DFO_SAObjBase::SetRanks(const SC_DoubleArray& x,
                                   SC_DoubleArray& ranks)
{
    int ndata = x.Size();

    ranks.AllocAndSetSize(ndata);
    for (int i = 0; i < ndata; i++)
        ranks[i] = double(i + 1);

    // look for ties
    int ii = 0;
    while (ii < (ndata - 1))
    {
        bool tieFound = false;
        for (int i = ii; i < (ndata - 1); i++)
        {
            if (x[i] == x[i+1])
            {
                tieFound = true;
                ii = i;
                break;
            }
        }

        // no remaining ties? - break
        if (!tieFound)
            break;

        // count ties
        int nties = 2;  // already know ii and ii+1
        for (int i = ii + 2; i < ndata; i++)
        {
            if (x[i] != x[ii])
                break;
            nties++;
        }

        // calc avg rank of all ties
        double avg = 0.0;
        for (int i = 0; i < nties; i++)
            avg += ranks[ii + i];
        avg /= double(nties);

        // set ranks to avg
        for (int i = 0; i < nties; i++)
            ranks[ii + i] = avg;

        // start at next non-tie
        ii += nties;
    }

}

void DFO_SAObjBase::StandardizeArray(SC_DoubleArray& x)
// standardizes array - mean 0; variance 1
// based on STEPWISE routine STD01
{
//  --Calculate mean
    double xsum = 0.0;
    int nx = x.Size();
    for (int i = 0; i < nx; i++)
        xsum += x[i];

    double xmean = xsum / double(nx);

//  --Calculate standard deviation
    double xss = 0.0;
    for (int i = 0; i < nx; i++)
        xss += Sqr(x[i] - xmean);

    double xvar = xss / double(nx - 1);
    double xstdev = sqrt(xvar);

//  --Standardize

    if (IsPositive(xstdev))
    {
        for (int i = 0; i < nx; i++)
            x[i] = (x[i] - xmean) / xstdev;
    }
    else
    {
        for (int i = 0; i < nx; i++)
            x[i] -= xmean;
    }
}



double  DFO_SAObjBase::CorrelateXY(const SC_DoubleArray& x,
                                    const SC_DoubleArray& y,
                                          bool            ranked)
{
    double prc1, prc2, prc3;
    int nobs = x.Size();
    double robs = double(nobs);

    // find means
    if (ranked)
    {
        prc1 = - (robs * Sqr((robs + 1.0) / 2.0));
        prc2 = prc1;
        prc3 = prc1;
    }
    else
    {
        double sx = 0.0;
        double sy = 0.0;
        for (int i = 0; i < nobs; i++)
        {
            sx += x[i];
            sy += y[i];
        }
        prc1 = - sx * sx / robs;
        prc2 = - sy * sy / robs;
        prc3 = - sx * sy / robs;
    }

    // sum of squares and cross products
    double sxy = 0.0;
    double sx2 = 0.0;
    double sy2 = 0.0;

    for (int i = 0; i < nobs; i++)
    {
        sxy += x[i] * y[i];
        sx2 += x[i] * x[i];
        sy2 += y[i] * y[i];
    }

    // check for constant first var in input values
    double v = sxy + prc3;
    double v1 = sx2 + prc1;
    double v2 = sy2 + prc2;

    if ((v1 != 0.0) && (v2 != 0.0))
        return v / sqrt(v1 * v2);

    return 0.0;
}



int DFO_SAObjBase::InvertSymmetricMatrix(     SC_DoubleMatrix&  a,
                                        const SC_StringArray&   rcIDs,
                                              bool              listErrors)
{
    int inverr = 0;
    int n = a.Size();

    /*  INVERR = 0

      DO 120 IV1 = 1, N
         TOL = ABS (0.01 * A(IXSYM(IV1,IV1)))
         DO 110 IV2 = IV1, N
            DSUM = 0.0
            DO 100 IV3 = IV1-1, 1, -1
               DSUM = DSUM + A(IXSYM(IV1,IV3))*A(IXSYM(IV2,IV3))
  100       CONTINUE
            DSUM = A(IXSYM(IV2,IV1)) - DSUM */

    double dpiv;
    char errStr[80];
    char rowStr[40];
    for (int i = 0; i < n; i++)
    {
        double tol = fabs(0.01 * a[i][i]);
        bool tolError = false;
        if (tol == 0.0)
        {
            inverr = 2;
            tol = 1.0E-20;
            tolError = true;
            if (listErrors)
            {
                IntToString(i + 1, rowStr, 40);
                MakeString(errStr, "** Error: Zero diagonal ", rowStr, " var ", rcIDs[i], 80);
                AddLine(errStr);
            }
        }

        for (int j = i; j < n; j++)
        {
            double dsum = 0.0;
            for (int k = i - 1; k >= 0; k--)
                dsum += a[i][k] * a[j][k];
            dsum = a[j][i] - dsum;


/*            IF (IV2 .EQ. IV1) THEN
               IF (DSUM .LE. TOL) THEN
                  IF (DSUM .LE. 0.0) THEN
                     IROW = IV1
                     WRITE (TXTMSG, 10000, IOSTAT=IDUM)
     &                  'Matrix is singular at row', IROW,
     &                  LABELS(IROW)(:ISTRLEN(LABELS(IROW)))
10000                 FORMAT (A, I5, '(', A, ')')
                     CALL QAMESSAG (-1, 'ERROR', TXTMSG)
                     DSUM = TOL / 1000.0
                     INVERR = 1
                  ELSE
                     IROW = IV1 - 1
                     WRITE (TXTMSG, 10000, IOSTAT=IDUM)
     &                  'Matrix inversion rounding error at row', IROW,
     &                  LABELS(IROW)(:ISTRLEN(LABELS(IROW)))
                     CALL QAMESSAG (-1, 'WARNING', TXTMSG)
                     IF (INVERR .EQ. 0) INVERR = -1
                  END IF
               END IF
               DPIV = SQRT (DSUM)
               A(IXSYM(IV2,IV1)) = DPIV
               DPIV = 1.0 / DPIV
            ELSE
               A(IXSYM(IV2,IV1)) = DSUM * DPIV
            END IF */

            if (j == i)
            {
                if (dsum <= tol)
                {
                    if (dsum <= 0.0)
                    {
                        if (listErrors)
                        {
                            IntToString(i + 1, rowStr, 40);
                            MakeString(errStr, "** Error: Matrix singular at row ", rowStr, " var ", rcIDs[i], 80);
                        }
                        dsum = tol / 1000.0;
                        if (inverr < 2)
                            inverr = 1;
                    }
                    else
                    {
                        if (listErrors)
                        {
                            IntToString(i, rowStr, 40);
                            MakeString(errStr, "** Warning: Matrix inversion rounding error at row ", rowStr, " var: ", rcIDs[i-1], 80);
                        }
                        if (inverr == 0)
                            inverr = -1;
                    }
                    if (listErrors && (!tolError))
                        AddLine(errStr);
                }
                dpiv = sqrt(dsum);
                a[j][i] = dpiv;
                dpiv = 1.0 / dpiv;
            }
            else
            {
                a[j][i] = dpiv * dsum;
            }
        }
    }

/*      DO 120 IV1 = N, 1, -1
         DIN = 1.0 / A(IXSYM(IV1,IV1))
         A(IXSYM(IV1,IV1)) = DIN
         DO 110 IV2 = N, IV1+1, -1
            WORK = 0.0
            DO 100 IV3 = IV1+1, IV2
               WORK = WORK + A(IXSYM(IV2,IV3))*A(IXSYM(IV3,IV1))
  100       CONTINUE
            A(IXSYM(IV2,IV1)) = -WORK * DIN
  110    CONTINUE
  120 CONTINUE */

    for (int i = n-1; i >= 0; i--)
    {
        double din = 1.0 / a[i][i];
        a[i][i] = din;
        for (int j = n - 1; j > i; j--)
        {
            double work = 0.0;
            for (int k = i + 1; k <= j; k++)
                work += a[j][k] * a[k][i];
            a[j][i] = -work * din;
        }
    }

/*      DO 150 IV1 = 1, N
         DO 140 IV2 = IV1, N
            WORK = 0.0
            DO 130 IV3 = IV2, N
               WORK = WORK + A(IXSYM(IV3,IV2))*A(IXSYM(IV3,IV1))
  130       CONTINUE
            A(IXSYM(IV2,IV1)) = WORK
  140    CONTINUE
  150 CONTINUE */

    for (int i = 0; i < n; i++)
        for (int j = i; j < n; j++)
        {
            double work = 0.0;
            for (int k = j; k < n; k++)
                work += a[k][j] * a[k][i];
            a[j][i] = work;
            a[i][j] = work;
        }


    return inverr;
}

