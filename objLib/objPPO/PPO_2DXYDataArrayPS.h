///////////////////////////////////////////////////////////////////////////////////
//
// PPO_2DXYDataArrayPS.h
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

#ifndef PPO_2DXYDATAARRAYPS_H
#define PPO_2DXYDATAARRAYPS_H

#include <genPPO/PPO_BasePS.h>

#include "PPO_2DXYDataArray.h"


class PPO_2DXYDataArrayPS : public PPO_BasePS {

        class XYSeriesUIC {

            friend class PPO_2DXYDataArrayPS;

            private:
                FormWidget                  topRow;
                FormWidget                  nextRow;

                ToggleBoxWidget             doSeries;
                ComboBoxSelectionWidget     xyArrayIndxUI;
                SeriesDataUIC               seriesSpec;
                PSC_LegendOverrideUIC       legendOverrideUI;

            public:
                FormWidget                  outsideForm;
                HorizontalSeparator         hSep;

                void  InitSeriesData();

                void  SetSeriesData(const PFO_2DXYDataArray::XYSeries& seriesData);
                void  GetSeriesData(PFO_2DXYDataArray::XYSeries& seriesData);
                void  SetAllSensitive(bool inSens);

            private:
                static void SetAllSensitiveCB(void* inObj);

        };

        class XYScrolledArea : public ScrolledForm {
            protected:
                CSize       SetCalculateFormSize();
        };

    private:
            FuncObjRefUIC           dataSource;
            XYSeriesUIC             seriesDataUI[PFO_2DXYDataArray::maxXYSeries];

            bool                    xyDataOK;
            XYScrolledArea          scrollArea;


    public:
                        PPO_2DXYDataArrayPS();
                        ~PPO_2DXYDataArrayPS(){};

                void    InitMenu(const PPO_2DXYDataArray&   inData);

                void    MenuOpen(PPO_2DXYDataArray& inData);

                void    SetMenuData(PPO_2DXYDataArray&  inData);
                bool    UIdataOK();
                void    GetMenuData(PPO_2DXYDataArray&          outData);

    private:
        static  void    SetXYIndexesCB(void* inObj);
                void    SetXYIndexes();
        static  void    SetAllSensitiveCB(void* inObj);
                void    SetAllSensitive();
};

#endif // !PPO_2DXYDATAARRAYPS_H

