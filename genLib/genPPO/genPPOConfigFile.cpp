///////////////////////////////////////////////////////////////////////////////////
//
// genPPOConfigFile.cpp
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include "genPPOConfigFile.h"


namespace genPPOConfigFile {

void WriteEdgeSpec(const PSC_EdgeSpec& edgeSpec)
{
    static const int edgeVer = 0;
    WriteInt(edgeVer);

    WriteBool(edgeSpec.plotEdges);
    WriteInt(edgeSpec.edgePen       );
    WriteInt(edgeSpec.edgeThk       );
    WriteDouble(edgeSpec.edgePolyOffset, SC_RealFormat(ncf_Scientific, 2));
    WriteLine();
}

void ReadEdgeSpec(PSC_EdgeSpec& edgeSpec)
{
    int readVer = ReadInt();

    edgeSpec.plotEdges       = ReadBool();
    edgeSpec.edgePen         = ReadInt();
    edgeSpec.edgeThk         = ReadInt();
    edgeSpec.edgePolyOffset  = ReadDouble();
}

void WriteTubeExtrusionSpec(const PSC_TubeExtrusionSpec& extrusionSpec)
{
    static const int extrudeVer = 0;
    WriteInt(extrudeVer);

    WriteInt(extrusionSpec.nextrusionPoints);
    WriteBool(extrusionSpec.usePixelSizing);
    WriteInt(extrusionSpec.extrusionPixelSize);
    WriteDouble(extrusionSpec.extrusionRealSize, SC_RealFormat(ncf_Scientific, 5));
    WriteBool(extrusionSpec.capEnds);
    WriteBool(extrusionSpec.adjustForScaling);
    WriteLine();
}

void ReadTubeExtrusionSpec(PSC_TubeExtrusionSpec& extrusionSpec)
{
    int readVer = ReadInt();

    extrusionSpec.nextrusionPoints   = ReadInt();
    extrusionSpec.usePixelSizing     = ReadBool();
    extrusionSpec.extrusionPixelSize = ReadInt();
    extrusionSpec.extrusionRealSize  = ReadDouble();
    extrusionSpec.capEnds            = ReadBool();
    extrusionSpec.adjustForScaling   = ReadBool();
}

void WritePolygonExtrusionSpec(const PSC_PolygonExtrusionSpec& extrusionSpec)
{
    static const int extrudeVer = 0;
    WriteInt(extrudeVer);

    WriteBool(extrusionSpec.capEnds);
    WriteBool(extrusionSpec.adjustForScaling);
    WriteLine();
}

void ReadPolygonExtrusionSpec(PSC_PolygonExtrusionSpec& extrusionSpec)
{
    int readVer = ReadInt();

    extrusionSpec.capEnds            = ReadBool();
    extrusionSpec.adjustForScaling   = ReadBool();
}


void WriteSeriesSpec(const PSC_SeriesSpec& seriesSpec)
{
    static const int seriesVer = 1;  // kluge for Read3DSeries which doesn't have ver#
                                     // added line mitre
//    static const int seriesVer = 0;
    WriteInt(seriesVer);

    WriteInt(seriesSpec.seriesPlotType);
    WriteInt(seriesSpec.seriesPen     );
    WriteInt(seriesSpec.symbolType    );
    WriteInt(seriesSpec.symbolSize    );
    WriteInt(seriesSpec.lineThickness );
    WriteInt(seriesSpec.lineType      );
    WriteBool(seriesSpec.mitreLines );
    WriteLine();
}


int ReadSeriesSpec(PSC_SeriesSpec& seriesSpec)
{
    int readVer = ReadInt();

    seriesSpec.seriesPlotType   = PlotSymLine(ReadInt());
    seriesSpec.seriesPen        = ReadInt();
    seriesSpec.symbolType       = PlotSymbol(ReadInt());
    seriesSpec.symbolSize       = ReadInt();
    seriesSpec.lineThickness    = ReadInt();
    seriesSpec.lineType         = PlotLineType(ReadInt());

    if (readVer > 0)
        seriesSpec.mitreLines = ReadBool();

    return readVer;
}

void WriteSymbolSpec(const PSC_SymbolSpec& symbolSpec)
{
    static const int edgeVer = 0;
    WriteInt(edgeVer);

    WriteInt(symbolSpec.symbolPen     );
    WriteInt(symbolSpec.symbolType    );
    WriteInt(symbolSpec.symbolSize    );
    WriteLine();
}


void ReadSymbolSpec(PSC_SymbolSpec& symbolSpec)
{
    int readVer = ReadInt();

    symbolSpec.symbolPen        = ReadInt();
    symbolSpec.symbolType       = PlotSymbol(ReadInt());
    symbolSpec.symbolSize       = ReadInt();

}


void WriteStippleSpec(const PSC_StippleSpec& stippleSpec)
{
    static const int stippVer = 0;
    WriteInt(stippVer);

    WriteInt(stippleSpec.stippleType);
    WriteInt(stippleSpec.stipplePen);
    WriteInt(stippleSpec.stippleSize);
    WriteInt(stippleSpec.stippleLineThickness);
    WriteLine();
}


void ReadStippleSpec(PSC_StippleSpec& stippleSpec)
{
    int readVer = ReadInt();

    stippleSpec.stippleType = ReadInt();
    stippleSpec.stipplePen = ReadInt();
    stippleSpec.stippleSize = ReadInt();
    stippleSpec.stippleLineThickness = ReadInt();
}




void WriteLineSpec(const PSC_LineSpec& lineSpec)
{
    static const int writeVer = 1; // added mitreLine
//    static const int writeVer = 0;
    WriteInt(writeVer);

    WriteInt(lineSpec.linePen     );
    WriteInt(lineSpec.lineType    );
    WriteInt(lineSpec.lineThickness );
    WriteBool(lineSpec.mitreLines);
    WriteLine();
}


void ReadLineSpec(PSC_LineSpec& lineSpec)
{
    int readVer = ReadInt();

    lineSpec.linePen        = ReadInt();
    lineSpec.lineType       = PlotLineType(ReadInt());
    lineSpec.lineThickness  = ReadInt();
    if (readVer > 0)
        lineSpec.mitreLines = ReadBool();
}

void Write3DSeriesSpec(const PSC_3DSeriesSpec& seriesSpec)
{
    WriteSeriesSpec(seriesSpec);
    WriteEdgeSpec(seriesSpec);
    WriteTubeExtrusionSpec(seriesSpec);

//    WriteInt(int(seriesSpec.plotLineType));  // removed v1
    WriteBool(seriesSpec.plotLineAsTube);  // added v1
    WriteBool(seriesSpec.plotSymEdges);
    WriteBool(seriesSpec.plotTubeEdges);
    WriteLine();
}

void Read3DSeriesSpec(PSC_3DSeriesSpec& seriesSpec)
{
    int readVer = ReadSeriesSpec(seriesSpec);
    ReadEdgeSpec(seriesSpec);
    ReadTubeExtrusionSpec(seriesSpec);

    if (readVer == 0)
        // seriesSpec.plotLineType = PSC_3DSeriesSpec::PSC_3DLineType(ReadInt());
        seriesSpec.plotLineAsTube = (ReadInt() == 3); // was 4 the enum
    else
        seriesSpec.plotLineAsTube = ReadBool(); // added v1
    seriesSpec.plotSymEdges = ReadBool();
    seriesSpec.plotTubeEdges = ReadBool();
}

void WriteLegendOverride(const PSC_LegendOverride& legendOverride)
{
    WriteBool(legendOverride.overrideDefaultLabel);
    WriteLine();
    WriteText(legendOverride.overrideLabel);
}

void ReadLegendOverride(PSC_LegendOverride& legendOverride)
{
    legendOverride.overrideDefaultLabel = ReadBool();
    ReadText(legendOverride.overrideLabel, PSC_LegendOverride::labelLen);
}



void WriteColorBase(const PSC_ColorBase& colorBase)
{
    const int scalarVer = 0;
    WriteInt(scalarVer);
    WriteLine();

    WriteFuncObjRef(colorBase.inputDataObjRef);
    WriteFuncObjRef(colorBase.colorMapObjRef);
    WriteFuncObjRef(colorBase.limitMasterObjRef);
    WriteBool(colorBase.limitIsMaster);
    WriteLine();

    SetRealConvFormat(SC_RealFormat(ncf_Scientific, 8));

    WriteBool(colorBase.rangeLimit.logRange);
    WriteDouble(colorBase.rangeLimit.minLimit);
    WriteDouble(colorBase.rangeLimit.maxLimit);

    WriteBool(colorBase.autoLimits);
    WriteInt(colorBase.baseAutoDataSource);
    WriteBool(colorBase.clipToRange);
    WriteLine();
}

void ReadColorBase(PSC_ColorBase& colorBase)
{
    int scalarReadVer = ReadInt();

    ReadFuncObjRef(colorBase.inputDataObjRef);
    ReadFuncObjRef(colorBase.colorMapObjRef);
    ReadFuncObjRef(colorBase.limitMasterObjRef);

    colorBase.limitIsMaster = ReadBool();

    colorBase.rangeLimit.logRange = ReadBool();
    colorBase.rangeLimit.minLimit = ReadNullDouble();
    colorBase.rangeLimit.maxLimit = ReadNullDouble();

    colorBase.autoLimits         = ReadBool();
    colorBase.baseAutoDataSource     = ReadInt();
    colorBase.clipToRange        = ReadBool();

    CheckForInputFailure("Reading PSC_ColorBase");
}

void WriteContourSpec(const PSC_ContourSpec& contourSpec)
{
    const int scalarVer = 2; // added mitreLine
//    const int scalarVer = 1; // added master override
//    const int scalarVer = 0;
    WriteInt(scalarVer);
    WriteLine();

    WriteFuncObjRef(contourSpec.dataObjRef);
    WriteFuncObjRef(contourSpec.specMasterObjRef);

    WriteBool(contourSpec.specIsMaster);
    WriteBool(contourSpec.calcIncVal);
    WriteBool(contourSpec.doLogInc);

    SetRealConvFormat(SC_RealFormat(ncf_Scientific, 8));
    WriteDouble(contourSpec.startVal);
    WriteDouble(contourSpec.endVal);
    WriteDouble(contourSpec.incVal);
    WriteInt(contourSpec.nIncs);

    WriteInt(contourSpec.contourLineSpec.linePen);
    WriteInt(contourSpec.contourLineSpec.lineThickness);
    WriteInt(contourSpec.contourLineSpec.lineType);
    WriteBool(contourSpec.contourLineSpec.mitreLines); // v2
    WriteBool(contourSpec.overrideMasterLineSpec);

    WriteLegendOverride(contourSpec.legendGC);
}

void ReadContourSpec(PSC_ContourSpec& contourSpec)
{
    int scalarReadVer = ReadInt();

    ReadFuncObjRef(contourSpec.dataObjRef);
    ReadFuncObjRef(contourSpec.specMasterObjRef);

    contourSpec.specIsMaster = ReadBool();
    contourSpec.calcIncVal = ReadBool();
    contourSpec.doLogInc = ReadBool();
    contourSpec.startVal = ReadDouble();
    contourSpec.endVal = ReadDouble();
    contourSpec.incVal = ReadDouble();
    contourSpec.nIncs = ReadInt();

    contourSpec.contourLineSpec.linePen = ReadInt();
    contourSpec.contourLineSpec.lineThickness= ReadInt();
    contourSpec.contourLineSpec.lineType = PlotLineType(ReadInt());
    if (scalarReadVer > 1)
        contourSpec.contourLineSpec.mitreLines = ReadBool();

    if (scalarReadVer > 0)
        contourSpec.overrideMasterLineSpec = ReadBool();

    ReadLegendOverride(contourSpec.legendGC);
}


void WriteGridLineLabelFormat(const PSC_GridLineLabelFormat& labelFormat)
{
    const int labelVer = 1;  // changed doLabel to labelType
//    const int labelVer = 0;
    WriteInt(labelVer);
    WriteLine();

//    WriteBool(labelFormat.doLabel);
    WriteInt(int(labelFormat.labelType));  // added v1
    WriteDouble(labelFormat.labelPosition);
    WriteInt(labelFormat.labelOffset);
    WriteInt(int(labelFormat.halign));
    WriteInt(int(labelFormat.valign));
    WriteBool(labelFormat.blankLabel);
    WriteLine();

}
void ReadGridLineLabelFormat(PSC_GridLineLabelFormat& labelFormat)
{
    int labelReadVer = ReadInt();

    if (labelReadVer == 0)
    {
        bool doLabel = ReadBool();
        if (doLabel)
            labelFormat.labelType = PSC_GridLineLabelFormat::ltText;
        else
            labelFormat.labelType = PSC_GridLineLabelFormat::ltNone;
    }
    else
        labelFormat.labelType = PSC_GridLineLabelFormat::LabelType(ReadInt());

    labelFormat.labelPosition = ReadDouble();
    labelFormat.labelOffset = ReadInt();

    labelFormat.halign = HorizontalTextAlignment(ReadInt());
    labelFormat.valign = VerticalTextAlignment(ReadInt());
    labelFormat.blankLabel = ReadBool();
}


void WriteXYLabelSpec(const PSC_XYLabelSpec& labelSpec)
{
    const int specVer = 3;  // start at 3 for conmpatibility
    WriteInt(specVer);
    WriteLine();

    WriteInt(labelSpec.labelPen);
    WriteFont(labelSpec.labelFont);

    WriteInt(int(labelSpec.horizontalPos));
    WriteInt(int(labelSpec.verticalPos));

    WriteInt(labelSpec.pixelOffset);
    WriteBool(labelSpec.clipToAxes);
    WriteLine();

}

void ReadXYLabelSpec(PSC_XYLabelSpec& labelSpec, int ver)
{
    if (ver > 2)        // start at 2
        ver = ReadInt();

    labelSpec.labelPen = ReadInt();
    ReadFont(labelSpec.labelFont);
    if (ver == 0)
        labelSpec.labelFont.fontRotation = PSC_Font::FontRotation(ReadInt());

    labelSpec.horizontalPos = HorizontalTextAlignment(ReadInt());
    labelSpec.verticalPos   = VerticalTextAlignment(ReadInt());

    if (ver > 1)
    {
        labelSpec.pixelOffset = ReadInt();
        labelSpec.clipToAxes = ReadBool();
    }
}


void WriteXYZLabelSpec(const PSC_XYZLabelSpec& labelSpec)
{
    const int specVer = 1; // add clip to axes
//    const int specVer = 0;
    WriteInt(specVer);
    WriteLine();

    WriteInt(labelSpec.labelPen);
    WriteFont(labelSpec.labelFont);

    WriteInt(int(labelSpec.labelAttitude));
    WriteBool(labelSpec.autoLabelRotation);
    WriteDouble(labelSpec.rotationAngle);

    WriteInt(int(labelSpec.horizontalPos));
    WriteInt(int(labelSpec.verticalPos));
    WriteBool(labelSpec.clipToAxes);
    WriteLine();
}

void ReadXYZLabelSpec(PSC_XYZLabelSpec& labelSpec, bool oldVer)
{
    int ver = 0;
    if (!oldVer)
        ver = ReadInt();

    labelSpec.labelPen = ReadInt();
    ReadFont(labelSpec.labelFont);

    labelSpec.labelAttitude     = PSC_XYZLabelSpec::LabelAttitude(ReadInt());
    labelSpec.autoLabelRotation = ReadBool();
    labelSpec.rotationAngle     = ReadDouble();

    labelSpec.horizontalPos = HorizontalTextAlignment(ReadInt());
    labelSpec.verticalPos   = VerticalTextAlignment(ReadInt());
    if (ver > 0)
        labelSpec.clipToAxes = ReadBool();
}


void WriteColorLegendSpec(const PSC_ColorLegendSpec& legendSpec)
{
    const int specVer = 0;
    WriteInt(specVer);
    WriteLine();

    WriteFuncObjRef(legendSpec.colorLegendObjRef);

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
    WriteDouble(legendSpec.barLength    );
    WriteDouble(legendSpec.barAspect    );
    WriteLine();

    WriteInt(int(legendSpec.orientation));
    WriteBool(legendSpec.ticsAtIncs );
    WriteBool(legendSpec.dataTypeLabel );
    WriteLine();

    WriteBool(legendSpec.dataLabelAboveLeft);
    WriteBool(legendSpec.incLabelAboveLeft);
    WriteBool(legendSpec.overrideLabel);
    WriteLine();

    WriteText(legendSpec.newLabel);
}

void  ReadColorLegendSpec(PSC_ColorLegendSpec& legendSpec)
{
    int ver = ReadInt();

    ReadFuncObjRef(legendSpec.colorLegendObjRef);
    legendSpec.barLength    = ReadDouble();
    legendSpec.barAspect    = ReadDouble();

    legendSpec.orientation = PSC_Font::FontRotation(ReadInt());
    legendSpec.ticsAtIncs  = ReadBool();
    legendSpec.dataTypeLabel = ReadBool();

    legendSpec.dataLabelAboveLeft = ReadBool();
    legendSpec.incLabelAboveLeft = ReadBool();
    legendSpec.overrideLabel = ReadBool();

    ReadText(legendSpec.newLabel, stdMsgStrLen);
}

void WriteAnnoSpec(const PSC_AnnoSpec& annoSpec)
{
    const int specVer = 0;
    WriteInt(specVer);
    WriteLine();

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
    WriteDouble(annoSpec.ulLocation.pX);
    WriteDouble(annoSpec.ulLocation.pY);
    WriteLine();

    WriteText(annoSpec.legendTitle);

    WriteBool(annoSpec.frameLegend);
    WriteInt (annoSpec.frameThick);
    WriteBool(annoSpec.opaqueBackground);
    WriteInt (annoSpec.legendPen);

    WriteFont(annoSpec.legendFont);
}

void ReadAnnoSpec(PSC_AnnoSpec& annoSpec)
{
    int ver = ReadInt();

    annoSpec.ulLocation.pX = ReadDouble();
    annoSpec.ulLocation.pY = ReadDouble();

    ReadText(annoSpec.legendTitle, PSC_AnnoSpec::legendTitleLen);

    annoSpec.frameLegend        = ReadBool();
    annoSpec.frameThick         = ReadInt ();
    annoSpec.opaqueBackground   = ReadBool();
    annoSpec.legendPen          = ReadInt ();

    ReadFont(annoSpec.legendFont);
}


void WriteArrowSpec(const PSC_ArrowSpec& arrowSpec)
{
    const int specVer = 0;
    WriteInt(specVer);
    WriteLine();
    WriteBool(arrowSpec.plotHeads);
    WriteBool(arrowSpec.fillHeads);
    WriteInt(arrowSpec.headSize);
    WriteInt(arrowSpec.lineThk);
    WriteLine();
}


void ReadArrowSpec(PSC_ArrowSpec& arrowSpec)
{
    int ver = ReadInt();
    arrowSpec.plotHeads = ReadBool();
    arrowSpec.fillHeads = ReadBool();
    arrowSpec.headSize = ReadInt();
    arrowSpec.lineThk = ReadInt();
}

void WriteHistogram(const PSC_Histogram& histogram)
{
    WriteEdgeSpec(histogram);
    const int specVer = 1;   // added indexOffset
//    const int specVer = 0;
    WriteInt(specVer);
    WriteLine();
    WriteBool(histogram.orientationX);
    WriteBool(histogram.baselineIsValue);
    WriteDouble(histogram.baselineValue);
    WriteBool(histogram.positionIsValue);
    WriteInt(int(histogram.widthSpec));
    WriteInt(histogram.widthLineThk);
    WriteDouble(histogram.widthSpecValue);
    WriteInt(histogram.widthSpecPixel);
    WriteDouble(histogram.widthSpecPct);
    WriteDouble(histogram.indexOffset);
    WriteLine();
}


void ReadHistogram(PSC_Histogram& histogram)
{
    ReadEdgeSpec(histogram);
    int ver = ReadInt();
    histogram.orientationX = ReadBool();
    histogram.baselineIsValue = ReadBool();
    histogram.baselineValue = ReadDouble();
    histogram.positionIsValue = ReadBool();
    histogram.widthSpec = PSC_Histogram::WidthSpec(ReadInt());
    histogram.widthLineThk = ReadInt();
    histogram.widthSpecValue = ReadDouble();
    histogram.widthSpecPixel = ReadInt();
    histogram.widthSpecPct = ReadDouble();
    if (ver > 0)
        histogram.indexOffset = ReadDouble();
}


void WriteDateFormat(const DC_DateFormat& df)
{
    const int specVer = 0;
    WriteInt(specVer);
    WriteLine();

    WriteInt(int(df.dateOrder));
    WriteInt(int(df.monthFormat));
    WriteBool(df.dayHasLeadingZero);
    WriteBool(df.yearIsFull);
    WriteLine();

    WriteTextWithTrailingBlanks(df.delim1);
    WriteTextWithTrailingBlanks(df.delim2);
    WriteLine();
}
void ReadDateFormat(DC_DateFormat& df)
{
    int ver = ReadInt();
    df.dateOrder = DC_DateFormat::DateOrder(ReadInt());
    df.monthFormat = DC_DateFormat::MonthFormat(ReadInt());
    df.dayHasLeadingZero = ReadBool();
    df.yearIsFull = ReadBool();
    ReadTextWithTrailingBlanks(df.delim1, DC_DateFormat::delimStrLen);
    ReadTextWithTrailingBlanks(df.delim2, DC_DateFormat::delimStrLen);
}

}; // end namespace

