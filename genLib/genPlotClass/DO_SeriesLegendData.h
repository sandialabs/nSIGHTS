///////////////////////////////////////////////////////////////////////////////////
//
// DO_SeriesLegendData.h
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
//      Platform independent base classes for plot definitions and plot functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DO_SERIESLEGENDDATA_H
#define DO_SERIESLEGENDDATA_H

#include <genPlotClass/PSC_SeriesSpec.h>
#include <genPlotClass/PSC_ArrowSpec.h>

#include <genClass/SC_StringArray.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/C_DataObj.h>

class DO_SeriesLegendData : public DataObjC {
    private:
        static char*        typeDesc;
        enum                {maxDataNamesLen = 80};
        SC_StringArray      dataNames;
        PSC_SeriesArray     dataSeries;
        SC_BoolArray        specIsSeries;
        T_SC_Array<PSC_ArrowSpec> dataArrow;

    public:
                            DO_SeriesLegendData();
                            ~DO_SeriesLegendData(){};

            void            Alloc(int nAll);
            int             Size() const {return dataNames.Size();}
            void            Clear();
            void            AddSeries(const char            serName[],
                                      const PSC_SeriesSpec&  serData);
            void            AddArrow(const char             arrName[],
                                            int             arrowPen,
                                      const PSC_ArrowSpec&  arrowData);

            const char*     GetName(int indx) const {return dataNames[indx];}
            bool            IsSeries(int indx) const {return specIsSeries[indx];}

            const PSC_SeriesSpec& GetSeries(int indx) const;
            const PSC_ArrowSpec& GetArrow(int indx) const;
            int             GetPen(int indx) const {return dataSeries[indx].seriesPen;}

            char*           GetType() const {return typeDesc;}
            bool            DataOK();
};

#endif // !DO_SERIESLEGENDDATA_H

