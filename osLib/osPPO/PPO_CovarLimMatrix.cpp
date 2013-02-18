///////////////////////////////////////////////////////////////////////////////////
//
// PPO_CovarLimMatrix.cpp
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
//      File I/O and user interface for all osPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genPPO/genPPOConfigFile.h>

#include "PPO_CovarLimMatrixPS.h"
#include "PPO_CovarLimMatrix.h"

const char* PPO_CovarLimMatrix::objLongDesc = "Covariance Matrix Confidence Limits - Multiple Dual Parameter";

PPO_CovarLimMatrix :: PPO_CovarLimMatrix(PlotDefC* assPlt)
                   : PFO_CovarLimMatrixGL(*assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_CovarLimMatrix :: PPO_CovarLimMatrix(PlotDefC* assPlt, const PPO_CovarLimMatrix& a)
                            : PFO_CovarLimMatrixGL (a, *assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_CovarLimMatrix& PPO_CovarLimMatrix::operator= (const PPO_CovarLimMatrix& a)
{
    if (&a != this)
       *((PFO_CovarLimMatrix*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_CovarLimMatrixPS& PPO_CovarLimMatrix::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_CovarLimMatrixPS&>(*assocMenu);
}


void PPO_CovarLimMatrix::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_CovarLimMatrix::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_CovarLimMatrixPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_CovarLimMatrix::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


bool  PPO_CovarLimMatrix::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    if (currentObjReadMajorVersion > 0)
        ReadColorBase(*this);

    ReadFuncObjRef(penSetObjRef);

    if (currentObjReadMajorVersion == 0)
        ReadFuncObjRef(inputDataObjRef);
    cLimits = DC_Covar::ConfidenceLimits(ReadInt());

    plotEllipse = ReadBool();
    plotBestEst = ReadBool();
    plotBadCov = ReadBool();
    plotAreaLim = ReadBool();

    if (currentObjReadMajorVersion > 0)
        matrixPlotOption = MatrixPlotOption(ReadInt());

    covarEllipseType = CovarEllipseType(ReadInt());
    ndualPts = ReadInt();
    covarLineThk = ReadInt();
    centerEllipse = ReadBool();

    ReadSymbolSpec(bestEstSymbol);

    plotWithColorMap = ReadBool();
    covarPen = ReadInt();
    if (currentObjReadMajorVersion == 0)
        ReadFuncObjRef(colorMapObjRef);

    colorAttribute = ColorAttribute(ReadInt());
    areaPct = ReadDouble();

    badCovXPen = ReadInt();
    badCovXThk = ReadInt();
    plotAreaLimPen = ReadInt();
    plotAreaLimThk = ReadInt();

    ReadLegendOverride(legendOverride);

    CheckForInputFailure("Reading PPO_CovarLimMatrix");
    return true;
}

void PPO_CovarLimMatrix::WriteToFile()
{
    const int objMajVer = 1;  //added matrix plot options and color base
    //const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();
    WriteColorBase(*this);

    WriteFuncObjRef(penSetObjRef);

    //WriteFuncObjRef(covarArrayObjRef);
    WriteInt(int(cLimits));
    WriteLine();

    WriteBool(plotEllipse);
    WriteBool(plotBestEst);
    WriteBool(plotBadCov);
    WriteBool(plotAreaLim);
    WriteLine();

    WriteInt(int(matrixPlotOption));
    WriteLine();

    WriteInt(int(covarEllipseType));
    WriteInt(ndualPts);
    WriteInt(covarLineThk);
    WriteBool(centerEllipse);
    WriteLine();

    WriteSymbolSpec(bestEstSymbol);

    WriteBool(plotWithColorMap);
    WriteInt(covarPen);
    //WriteFuncObjRef(colorMapDataObjRef);

    WriteInt(int(colorAttribute));
    WriteDouble(areaPct);
    WriteLine();

    WriteInt(badCovXPen);
    WriteInt(badCovXThk);
    WriteInt(plotAreaLimPen);
    WriteInt(plotAreaLimThk);
    WriteLine();

    WriteLegendOverride(legendOverride);

}

