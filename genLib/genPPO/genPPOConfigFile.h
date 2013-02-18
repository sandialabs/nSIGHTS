///////////////////////////////////////////////////////////////////////////////////
//
// genPPOConfigFile.h
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

#ifndef GENPPOCONFIGFILE_H
#define GENPPOCONFIGFILE_H

#include <genClass/DC_DateTime.h>

#include <genPlotClass/PSC_EdgeSpec.h>
#include <genPlotClass/PSC_ExtrusionSpec.h>
#include <genPlotClass/PSC_SeriesSpec.h>
#include <genPlotClass/PSC_SymbolSpec.h>
#include <genPlotClass/PSC_StippleSpec.h>
#include <genPlotClass/PSC_LineSpec.h>
#include <genPlotClass/PSC_3DSeriesSpec.h>
#include <genPlotClass/PSC_LegendOverride.h>
#include <genPlotClass/PSC_ColorBase.h>
#include <genPlotClass/PSC_ContourSpec.h>
#include <genPlotClass/PSC_GridLineLabelFormat.h>
#include <genPlotClass/PSC_XYLabelSpec.h>
#include <genPlotClass/PSC_XYZLabelSpec.h>
#include <genPlotClass/PSC_AnnoSpec.h>
#include <genPlotClass/PSC_ColorLegendSpec.h>
#include <genPlotClass/PSC_ArrowSpec.h>
#include <genPlotClass/PSC_Histogram.h>

#include <genApp/AppConfigFile.h>


namespace genPPOConfigFile {

        extern void         WriteEdgeSpec(const PSC_EdgeSpec& edgeSpec);
        extern void         ReadEdgeSpec(PSC_EdgeSpec& edgeSpec);
        extern void         WriteTubeExtrusionSpec(const PSC_TubeExtrusionSpec& extrusionSpec);
        extern void         ReadTubeExtrusionSpec(PSC_TubeExtrusionSpec& extrusionSpec);
        extern void         WritePolygonExtrusionSpec(const PSC_PolygonExtrusionSpec& extrusionSpec);
        extern void         ReadPolygonExtrusionSpec(PSC_PolygonExtrusionSpec& extrusionSpec);
        extern void         WriteSeriesSpec(const PSC_SeriesSpec& seriesSpec);

                            // kluge for 3DSeriesSpec - returns version number
        extern int          ReadSeriesSpec(PSC_SeriesSpec& seriesSpec);

        extern void         WriteSymbolSpec(const PSC_SymbolSpec& symbolSpec);
        extern void         ReadSymbolSpec(PSC_SymbolSpec& symbolSpec);
        extern void         WriteStippleSpec(const PSC_StippleSpec& stippleSpec);
        extern void         ReadStippleSpec(PSC_StippleSpec& stippleSpec);
        extern void         WriteLineSpec(const PSC_LineSpec& lineSpec);
        extern void         ReadLineSpec(PSC_LineSpec& lineSpec);
        extern void         Write3DSeriesSpec(const PSC_3DSeriesSpec& seriesSpec);
        extern void         Read3DSeriesSpec(PSC_3DSeriesSpec& seriesSpec);
        extern void         WriteLegendOverride(const PSC_LegendOverride& legendOverride);
        extern void         ReadLegendOverride(PSC_LegendOverride& legendOverride);
        extern void         WriteColorBase(const PSC_ColorBase& colorBase);
        extern void         ReadColorBase(PSC_ColorBase& colorBase);
        extern void         WriteContourSpec(const PSC_ContourSpec& contourSpec);
        extern void         ReadContourSpec(PSC_ContourSpec& contourSpec);
        extern void         WriteGridLineLabelFormat(const PSC_GridLineLabelFormat& labelFormat);
        extern void         ReadGridLineLabelFormat(PSC_GridLineLabelFormat& labelFormat);
        extern void         WriteXYLabelSpec(const PSC_XYLabelSpec& labelSpec);
        extern void         ReadXYLabelSpec(PSC_XYLabelSpec& labelSpec, int ver = 3);
        extern void         WriteXYZLabelSpec(const PSC_XYZLabelSpec& labelSpec);
        extern void         ReadXYZLabelSpec(PSC_XYZLabelSpec& labelSpec, bool oldVer = false);
        extern void         WriteAnnoSpec(const PSC_AnnoSpec& annoSpec);
        extern void         ReadAnnoSpec(PSC_AnnoSpec& annoSpec);
        extern void         WriteColorLegendSpec(const PSC_ColorLegendSpec& legendSpec);
        extern void         ReadColorLegendSpec(PSC_ColorLegendSpec& legendSpec);

        extern void         WriteArrowSpec(const PSC_ArrowSpec& arrowSpec);
        extern void         ReadArrowSpec(PSC_ArrowSpec& arrowSpec);

        extern void         WriteHistogram(const PSC_Histogram& histogram);
        extern void         ReadHistogram(PSC_Histogram& histogram);

        extern void         WriteDateFormat(const DC_DateFormat& df);
        extern void         ReadDateFormat(DC_DateFormat& df);

};

using namespace genPPOConfigFile;

#endif // !GENPPOCONFIGFILE_H

