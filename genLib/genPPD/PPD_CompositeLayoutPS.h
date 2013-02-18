///////////////////////////////////////////////////////////////////////////////////
//
// PPD_CompositeLayoutPS.h
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
//      describes layout of sub-plots on composite plot.
//
//      adds components for composite plots.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPD_COMPOSITELAYOUTPS_H
#define PPD_COMPOSITELAYOUTPS_H

#include <genClass/T_SC_AllocPtrArray.h>

#include <genPlotClass/PD_Composite.h>

#include <genApp/C_MenuBase.h>
#include <genApp/U_MenuPPO.h>

class PPD_CompositeLayoutPS : public MenuBaseC {

    private:
        // for free form

        class PlotLayoutWidget {
            public:
                ToggleBoxWidget     usePlotLayoutUI;
                RealEntryUIC        xOffsetUI;
                RealEntryUIC        yOffsetUI;
                RealEntryUIC        xWidthUI;
                RealEntryUIC        yWidthUI;

                FormWidget          outsideForm;

                                    PlotLayoutWidget();

                void                InitPlotLayout();
                void                SetPlotLayout(          bool            useLayout,
                                                    const   PSC_PlotLayout& layoutData);

                bool                UIdataOK();
                void                GetPlotLayout(bool&         useLayout,
                                                  PSC_PlotLayout& layoutData);
                void                SetSensitive();

            private:
                static  void        SetSensitiveCB(void* inObj);
        };

        class PlotLayoutScrolledForm : public ScrolledForm {
            protected:
                virtual CSize SetCalculateFormSize();
        };

        PlotLayoutScrolledForm  scrollArea;
        PlotLayoutWidget        layoutData[PD_Composite::maxCustomLayout];

        bool                    doingFreeForm;

        // for row column
        IntEntryUIC             nlayoutRowsUI;
        IntEntryUIC             nlayoutColsUI;

        enum                    {maxRowCol = PD_Composite::maxLayoutRowCol};
        BaseUIC                 rowLayoutFrame;
        FormWidget              rowForms[3];  // assume 4 per row
        T_SC_AllocPtrArray<RealEntryUIC>    rowSpacingUI;

        BaseUIC                 colLayoutFrame;
        FormWidget              colForms[3];  // assume 4 per row
        T_SC_AllocPtrArray<RealEntryUIC>    colSpacingUI;

        enum                    {rowColLabelLen = 10};
        char                    rowIDs[maxRowCol][rowColLabelLen];
        char                    colIDs[maxRowCol][rowColLabelLen];
    public:
                            PPD_CompositeLayoutPS();
                            ~PPD_CompositeLayoutPS();

        void                InitMenu(const PD_Composite&    inData);
        void                MenuOpen(PD_Composite&  inPD);
        void                SetMenuData(const PD_Composite& inData,
                                                MenuObjC& inObj);
        bool                UIdataOK();
        void                GetMenuData(PD_Composite& outData,
                                        MenuObjC&       outObj);

        void                MenuDefault();
        void                MenuClear();

    private:
        void                InitFreeFormMenu();
        void                InitRowColumnMenu();

        void                GetFreeFormMenuData(PD_Composite&   inData);
        void                GetRowColumnMenuData(PD_Composite& inData);

        void                SetAllSensitive();
        static  void        SetAllSensitiveCB(void* inObj);

        void                GetNRowsCols(int& nrows, int& ncols);

};

#endif // !PPD_COMPOSITELAYOUTPS_H

