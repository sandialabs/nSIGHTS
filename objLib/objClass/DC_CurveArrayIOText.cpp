///////////////////////////////////////////////////////////////////////////////////
//
// DC_CurveArrayIOText.cpp
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

#include <objClass/DC_CurveArrayIOText.h>

const int    DC_CurveArrayIOText::fileWriteVer = 0;

static void InfoMsg(const char* msgStr)
{
    GenAppInfoMsg("CurveText", msgStr);
}


bool DC_CurveArrayIOText::ReadFromFile(const char fName[])
{
    try
    {
        InfoMsg(" Opening file .. ");

        OpenFile(fName, true);
        int dummy;                      // don't care about minor version
        ReadFileHeader(fileHeader, readFileVer, dummy, "nSIGHTS curve file");

        if (fileWriteVer != readFileVer)
            TextError("Old or bad version number");

        DeAlloc();

        int nCurve = ReadInt();
        AllocAndSetSize(nCurve);
        for ( int i = 0; i < nCurve; i++)
        {
            ReadOneCurve((*this)[i]);
            CheckTextError("Reading curve ");
        }

    }
    catch (TextC::TextError re) {
        CloseFile();
        GenAppErrorMsg("ReadCurve", re.errMsg);
        return false;
    }

    CloseFile();

    InfoMsg(" File opened successfully ");

    return true;
}


void DC_CurveArrayIOText::WriteToFile(const char fName[])
{
    try
    {
        OpenFile(fName, false);
        WriteFileHeader(fileHeader, fileWriteVer, 0);

        doubleConvFormat = SC_RealFormat(ncf_Scientific, 10);

        //  write size stuff
        WriteInt(Size());
        WriteLine();

        for (int i = 0; i < Size(); i++)
            WriteOneCurve((*this)[i]);

        CloseFile();
    }
    catch (TextC::TextError re) {
        CloseFile();
        GenAppErrorMsg("WriteCurve", re.errMsg);
        return;
    }


    InfoMsg(" File written successfully ");
}


void DC_CurveArrayIOText::ReadOneCurve(DC_CurveWithID& crv)
{
    // write ID
    ReadText(crv.curveID, DC_CurveWithID::curveIDlen);

    //  curve description stuff
    crv.curveType               = Curve::CurveType(ReadInt());
    crv.splineNaturalSlope      = ReadBool();
    crv.fixedSplineStartSlope   = ReadDouble();
    crv.fixedSplineEndSlope     = ReadDouble();
    crv.splineTension           = ReadDouble();
    crv.polyOrder               = ReadInt();
    CheckTextError("Reading curve description");

    // DC_Curve scalar data
    crv.curveIsCreated      = ReadBool();
    crv.inputXmin           = ReadNullDouble();
    crv.inputXmax           = ReadNullDouble();
    crv.normalizedTension   = ReadDouble();

    // DC Curve arrays
    int ncrv = ReadInt();
    crv.curveData.Alloc(ncrv);
    crv.curveData.xData.SetSize(ncrv);
    crv.curveData.yData.SetSize(ncrv);
    if (crv.curveData.Size() > 0)
    {
        ReadDoubleArray(crv.curveData.xData, false);
        ReadDoubleArray(crv.curveData.yData, false);
    }

    ncrv = ReadInt();
    crv.splineDeriv.AllocAndSetSize(ncrv);
    if (ncrv > 0)
    {
        ReadDoubleArray(crv.splineDeriv, false);
    }

    ncrv = ReadInt();
    crv.polyCoefficients.AllocAndSetSize(ncrv);
    if (ncrv > 0)
    {
        ReadDoubleArray(crv.polyCoefficients, false);
    }

    CheckTextError("Reading curve arrays");
}
void DC_CurveArrayIOText::WriteOneCurve(const DC_CurveWithID& crv)
{
    // write ID
    WriteText(crv.curveID);

    //  curve description stuff
    WriteInt(int(crv.curveType));
    WriteBool(crv.splineNaturalSlope);
    WriteDouble(crv.fixedSplineStartSlope);
    WriteDouble(crv.fixedSplineEndSlope);
    WriteDouble(crv.splineTension);
    WriteInt(crv.polyOrder);
    WriteLine();

    // DC_Curve scalar data
    WriteBool(crv.curveIsCreated);
    WriteNullDouble(crv.inputXmin);
    WriteNullDouble(crv.inputXmax);
    WriteDouble(crv.normalizedTension);
    WriteLine();

    // DC Curve arrays
    WriteInt(crv.curveData.Size());
    WriteLine();
    if (crv.curveData.Size() > 0)
    {
        WriteDoubleArray(crv.curveData.xData, false);
        WriteDoubleArray(crv.curveData.yData, false);
        WriteLine();
    }

    WriteInt(crv.splineDeriv.Size());
    WriteLine();
    if (crv.splineDeriv.Size() > 0)
    {
        WriteDoubleArray(crv.splineDeriv, false);
        WriteLine();
    }

    WriteInt(crv.polyCoefficients.Size());
    WriteLine();
    if (crv.polyCoefficients.Size() > 0)
    {
        WriteDoubleArray(crv.polyCoefficients, false);
        WriteLine();
    }
}


