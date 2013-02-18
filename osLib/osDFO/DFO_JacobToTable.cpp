///////////////////////////////////////////////////////////////////////////////////
//
// DFO_JacobToTable.cpp
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
//      extracts data from Jacobian in DC_TableData form.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osClass/DO_Jacobian.h>

#include <osDFO/DFO_JacobToTable.h>


DFO_JacobToTable :: DFO_JacobToTable()
    : FuncObjC("JacobianToTable")
{
    parJacobian  = true;
    fitCompIndex = -1;
    parCompIndex = -1;
    xareValues   = true;
    takeAbs      = false;

    InitCommon();
}

DFO_JacobToTable::DFO_JacobToTable(const DFO_JacobToTable& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_JacobToTable :: ~DFO_JacobToTable ()
{
}

void DFO_JacobToTable::InitCommon()
{
    AddInPort(jacobObjRef, typeid(DO_Jacobian));

    jacobTableDO.tableData = &jacobTable;
    AddOutPort(jacobTableDO);

    DoStatusChk();
}

DFO_JacobToTable& DFO_JacobToTable::operator= (const DFO_JacobToTable& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_JacobToTable::LocalCopy(const DFO_JacobToTable& a)
{
    jacobObjRef  = a.jacobObjRef;
    parJacobian  = a.parJacobian;
    fitCompIndex = a.fitCompIndex;
    parCompIndex = a.parCompIndex;
    xareValues   = a.xareValues;
    takeAbs      = a.takeAbs;
}


void  DFO_JacobToTable:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    CheckInputFO(jacobObjRef, "Jacobian");

}

void DFO_JacobToTable:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    DC_Jacobian& jacData = *(static_cast<DO_Jacobian*>(GetInPortData(jacobObjRef))->jacobianData);

    int nPar = jacData.parDesc.Size();
    int nFit = jacData.fitDesc.fitComponentDescription.Size();

    if (fitCompIndex >= nFit)
        fitCompIndex = nFit - 1;

    int stFit = fitCompIndex;
    int endFit = fitCompIndex + 1;
    if (stFit < 0)
    {
        stFit = 0;
        endFit = jacData.fitDesc.fitComponentDescription.Size();
    }

    int nRows = 0;
    int i;
    for (i = stFit; i < endFit; i++)
        nRows += jacData.fitDesc.fitComponentDescription[i].nfitPoints;

    if (!jacobTable.Alloc(nPar + 1, nRows, 80))
    {
        SetObjErrMsg("table allocation??");
        return;
    }

    if (xareValues)
        jacobTable.SetTitle(0, "XValue");
    else
        jacobTable.SetTitle(0, "Index");
    for (i = 0; i < nPar; i++)
        jacobTable.SetTitle(i + 1, jacData.parDesc[i].varDesc);
    jacobTable.dataTable.MatrixSetSize(nPar + 1, nRows);

    int nextRow = 0;
    for (i = stFit; i < endFit; i++)
    {
        const OptFitComponentDescription& currFit = jacData.fitDesc.fitComponentDescription[i];

        for ( int j = 0; j < currFit.nfitPoints; j++)
        {
            double xVal = currFit.fitResidJacobX[j];
            if (!xareValues)
                xVal = double(nextRow);
            jacobTable.dataTable[0][nextRow] = xVal;

            for ( int k = 0; k < nPar; k++)
            {
                double dP = jacData.parDesc[k].parSigma / jacData.derivSpan;
                double dR = jacData.jacobData[i][k][j] / currFit.fitChisqSigma;
                double dpdr = dR * dP;
                if (takeAbs)
                    dpdr = fabs(dpdr);

                jacobTable.dataTable[k + 1][nextRow] = dpdr;
            }

            nextRow ++;
        }

    }

}


