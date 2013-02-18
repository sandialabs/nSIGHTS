///////////////////////////////////////////////////////////////////////////////////
//
// PPD_Base.cpp
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
//      components common to all plots (window size)
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genPlotClass/C_PlotDef.h>
#include <genPlotClassPS/PC_PlatformPS.h>

#include <genApp/AppConfigFile.h>

#include <genApp/C_MenuPage.h>

#include "PPD_Base.h"


PPD_Base :: PPD_Base(PlotDefC* bPD, const char* longDesc) : MenuObjC(bPD, longDesc), basePD(*bPD)
{
    objDeleteable = false;
    objCopyable = false;
    resizeCBset = false;
}


void  PPD_Base::ResizeCB(void* inObj)
{
    static_cast<PPD_Base*>(inObj)->PropUpdate();
}


void  PPD_Base::CheckResizeCB()
{
    if (resizeCBset || (! basePD.platformData->initComplete))
        return;

    static_cast<PC_PlatformPS*>(basePD.platformData)->AddResizeCallback(ResizeCB, this);
    resizeCBset = true;
}

void PPD_Base::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    CheckResizeCB();
}

void PPD_Base::PropCloseMenu()
{
    MenuObjC::PropCloseMenu();
    MenuPageC::currPageObj->UpdatePage();
/*
    if (resizeCBset)
    {
        BaseWidget& plotWidget = static_cast<PC_PlatformPS*>(basePD.platformData)->oglObject;
        plotWidget.RemoveCallback(XmNresizeCallback, ResizeCB, this);
        resizeCBset = false;
    }
 */
}

void PPD_Base::PropApply()
{
    MenuObjC::PropApply();
    MenuPageC::currPageObj->UpdatePage();
    CheckResizeCB();
}

bool  PPD_Base::ReadFromFile()
{
    basePDReadMaj = ReadInt();
    basePDReadMin = ReadInt();

    ReadFuncObjRef(basePD.plotPenSetObjRef);
    basePD.plotBackgroundPen = ReadInt();
    basePD.plotUwidth        = ReadInt();
    basePD.plotVheight       = ReadInt();

    if (basePDReadMaj > 0)
    {
        basePD.rightMargin  = ReadInt();
        basePD.leftMargin   = ReadInt();
        basePD.topMargin    = ReadInt();
        basePD.bottomMargin = ReadInt();
    }

    //  moved to 2D only in ver 4
    if ((basePDReadMaj > 1) && (basePDReadMaj < 4))
        bool dummyshowReportArea = ReadBool();

    if (basePDReadMaj > 2)
    {
        ReadFileName(basePD.tgaOutput.tgaRootFile);

        if (basePDReadMaj > 9)
            basePD.tgaOutput.dumpFormat = PC_DumpTGA::DumpFormat(ReadInt());

        basePD.tgaOutput.dumpMethod    = PC_DumpTGA::TGADumpMethod(ReadInt());

        // we rarely ever really want auto on read ...
        if (basePD.tgaOutput.dumpMethod == PC_DumpTGA::dtga_FullAuto)
            basePD.tgaOutput.dumpMethod = PC_DumpTGA::dtga_SemiAuto;

        basePD.tgaOutput.nextIncrement = ReadInt();
        if (basePDReadMaj > 6)
            basePD.tgaOutput.dumpFrameCount = ReadInt();
    }

    if (basePDReadMaj > 4)
    {
        ReadFileName(basePD.psOutput.outputFile);
        basePD.psOutput.outputFormat     = PC_PSOutData::OutputFormat(ReadInt());
        basePD.psOutput.outputIsPortrait = ReadBool();

        basePD.psOutput.pageWidth    = ReadDouble();
        basePD.psOutput.pageHeight   = ReadDouble();
        basePD.psOutput.rightMargin  = ReadDouble();
        basePD.psOutput.leftMargin   = ReadDouble();
        basePD.psOutput.topMargin    = ReadDouble();
        basePD.psOutput.bottomMargin = ReadDouble();

        if (basePDReadMaj > 5)
        {
            basePD.psOutput.lineWidthMultiplier  = ReadDouble();
            basePD.psOutput.gammaCorrection      = ReadDouble();
            basePD.psOutput.zBufferMultiplier    = ReadDouble();
            basePD.psOutput.textLengthMultiplier = ReadDouble();
        }
        if (basePDReadMaj > 8)
            basePD.psOutput.arialIsHelvetica  = ReadBool();
        if (basePDReadMaj > 10)
            basePD.psOutput.nextPSIncrement  = ReadInt();

    }

    if (basePDReadMaj > 7)
    {
        basePD.plotDefIsActive = ReadBool();
        basePD.axesHaveBeenSet = ReadBool();
    }

    return true;
}


void PPD_Base::WriteToFile()
{
    // ver 0
    // ver 1 - added margins
    // ver 2 - showReportArea
    // ver 3 - TGA Dump stuff
    // ver 4 - 2D/3D restructuring -- move report Area to 2D only
    // ver 5 - GLpr postscript output
    // ver 6 - revisions to GLpr postscript output
    // ver 7 - revisions to TGA -- add dumpFrameCount
    // ver 8 - added pointer status and axes set status
    // ver 9 - added postscript arialIsHelvetica
    // ver 10 - added bitmap file format - was default jpeg
    // ver 11 - added postscript auto increment
    const int objMajVer = 11;
    const int objMinVer = 0;

    WriteInt(objMajVer);
    WriteInt(objMinVer);
    WriteLine();

    WriteFuncObjRef(basePD.plotPenSetObjRef);

    WriteInt(basePD.plotBackgroundPen);
    WriteInt(basePD.plotUwidth       );
    WriteInt(basePD.plotVheight      );
    WriteLine();

    // added ver 1
    WriteInt(basePD.rightMargin );
    WriteInt(basePD.leftMargin  );
    WriteInt(basePD.topMargin   );
    WriteInt(basePD.bottomMargin);
    WriteLine();

    //  ver 2 obsolete

    // added ver 3
    WriteText(basePD.tgaOutput.tgaRootFile);
    WriteInt(int(basePD.tgaOutput.dumpFormat)); // added v10
    WriteInt(int(basePD.tgaOutput.dumpMethod));
    WriteInt(basePD.tgaOutput.nextIncrement);
    WriteInt(basePD.tgaOutput.dumpFrameCount);  // added v7
    WriteLine();

    // added ver 5
    WriteText(basePD.psOutput.outputFile);
    WriteInt(int(basePD.psOutput.outputFormat));
    WriteBool(basePD.psOutput.outputIsPortrait);
    WriteLine();

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
    WriteDouble(basePD.psOutput.pageWidth);
    WriteDouble(basePD.psOutput.pageHeight  );
    WriteDouble(basePD.psOutput.rightMargin );
    WriteDouble(basePD.psOutput.leftMargin  );
    WriteDouble(basePD.psOutput.topMargin   );
    WriteDouble(basePD.psOutput.bottomMargin);
    WriteLine();

    //  added v6
    WriteDouble(basePD.psOutput.lineWidthMultiplier );
    WriteDouble(basePD.psOutput.gammaCorrection     );
    WriteDouble(basePD.psOutput.zBufferMultiplier   );
    WriteDouble(basePD.psOutput.textLengthMultiplier);
    WriteBool(basePD.psOutput.arialIsHelvetica); // added v9
    WriteInt(basePD.psOutput.nextPSIncrement); // added v11
    WriteLine();

    //  added v8
    WriteBool(basePD.plotDefIsActive);
    WriteBool(basePD.axesHaveBeenSet);
    WriteLine();

}

