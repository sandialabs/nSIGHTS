///////////////////////////////////////////////////////////////////////////////////
//
// PPO_2DTableHistogramPS.h
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPO_2DTableHistogramPS_H
#define PPO_2DTableHistogramPS_H

#include <genPPO/PPO_BasePS.h>

#include "PPO_2DTableHistogram.h"


class PPO_2DTableHistogramPS : public PPO_BasePS {

        class TableHistogramUIC {

            friend class PPO_2DTableHistogramPS;

            private:
                FormWidget                  topRow;
                FormWidget                  nextRow;

                ToggleBoxWidget             doHistogram;
                ComboBoxSelectionWidget     seriesDataColumn;
                SeriesDataUIC               seriesSpec;
                PSC_LegendOverrideUIC        legendOverrideUI;

            public:
                FormWidget                  outsideForm;
                HorizontalSeparator         hSep;

                void  InitSeriesData();

                void  SetSeriesData(const PFO_2DTableHistogram::TableHistogram& seriesData);
                void  GetSeriesData(PFO_2DTableHistogram::TableHistogram& seriesData);
                void  SetAllSensitive(bool inSens);

            private:
                static void SetAllSensitiveCB(void* inObj);

        };

        class TableScrolledArea : public ScrolledForm {
            protected:
                CSize       SetCalculateFormSize();
        };

    private:
            FuncObjRefUIC           dataSource;
            ComboBoxSelectionUIC    xColumn;
            TableHistogramUIC          seriesDataUI[PFO_2DTableHistogram::maxTableHistogram];

            bool                    tableDataOK;

            TableScrolledArea       scrollArea;
            FormWidget              topRowForm;

            BaseUIC                 optionsFrame;
            ToggleBoxWidget         plotAllRowsUI;
            IntEntryUIC             lastNRowsUI;


    public:
                        PPO_2DTableHistogramPS();
                        ~PPO_2DTableHistogramPS(){};

                void    InitMenu(const PPO_2DTableHistogram&    inData);

                void    MenuOpen(PPO_2DTableHistogram&  inData);

                void    SetMenuData(PPO_2DTableHistogram&   inData);
                bool    UIdataOK();
                void    GetMenuData(PPO_2DTableHistogram&          outData);

    private:
        static  void    SetTableColumnsCB(void* inObj);
                void    SetTableColumns();
        static  void    SetAllSensitiveCB(void* inObj);
                void    SetAllSensitive();
};

#endif // !PPO_2DTableHistogramPS_H

