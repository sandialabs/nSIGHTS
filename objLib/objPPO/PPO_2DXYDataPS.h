///////////////////////////////////////////////////////////////////////////////////
//
// PPO_2DXYDataPS.h
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

#ifndef PPO_2DXYDATAPS_H
#define PPO_2DXYDATAPS_H

#include <genPPO/PPO_BasePS.h>

#include "PPO_2DXYData.h"


class PPO_2DXYDataPS : public PPO_BasePS {
    private:

        class XYDataUIC {
        private:
            FormWidget                  topRow;
            FormWidget                  nextRow;
            ToggleBoxWidget             doSeriesUI;
            PSC_LegendOverrideUIC        legendOverrideUI;

        public:
            FuncObjRefUIC               xyDataObjRefUI;
            SeriesDataUIC               seriesSpecUI;

            FormWidget                  outsideForm;
            HorizontalSeparator         hSep;

                    XYDataUIC();

            void  InitSeriesData();  // parent is outsideForm
            void  SetSeriesData(const PFO_2DXYData::XYSeries& seriesData);
            bool  SeriesDataOK(bool& prevOK);
            void  GetSeriesData(PFO_2DXYData::XYSeries& seriesData);
            void  SetAllSensitive();

        private:
                static  void SetAllSensitiveCB(void* inObj);

        };

        class XYDataScrolledArea : public ScrolledForm {
            protected:
                CSize       SetCalculateFormSize();
        };

    private:

            XYDataUIC               xySeriesUI[PFO_2DXYData::maxXYSeries];
            XYDataScrolledArea      scrollArea;

    public:
                        PPO_2DXYDataPS();
                        ~PPO_2DXYDataPS(){};

                void    InitMenu(const PPO_2DXYData&    inData);

                void    MenuOpen(PPO_2DXYData&  inData);

                void    SetMenuData(PPO_2DXYData&   inData);
                bool    UIdataOK();
                void    GetMenuData(PPO_2DXYData&   outData);


    private:
                void    SetAllSensitive();
};

#endif // !PPO_2DXYDATAPS_H

