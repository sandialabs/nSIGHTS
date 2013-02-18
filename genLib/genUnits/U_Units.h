///////////////////////////////////////////////////////////////////////////////////
//
// U_Units.h
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
//      unit controls (unit selection combined real/units) for use in form managed UI
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef U_UNITS_H
#define U_UNITS_H

#include <genClass/SC_RealFormat.h>
#include <genClassPS/PS_UIClass.h>

#include <genUnits/C_GenUnits.h>
#include <genUnits/UnitTableCol.h>

using namespace genClassPS;

namespace genUnitsMenuUtilities {


    class UnitsTypeOptionWidget : public OptionChooserWidget, public UnitsBase  {
        public:
                            UnitsTypeOptionWidget();

            void            PreInitSetUp();
            void            SetUnitType(UnitType uType);
            int             GetUnitType();

    };

    class UnitsTypeUIC : public OptionBaseUIC, public UnitsTypeOptionWidget  {

        public:
                          UnitsTypeUIC(): OptionBaseUIC(this) {}
                  void    SetSensitive(bool isSensitive) {OptionBaseUIC::SetSensitive(isSensitive);}
    };


    class UnitsConvOptionWidget : public OptionChooserWidget, public UnitsBase    {
        private:
            UnitType        initType;

        public:
                            UnitsConvOptionWidget();
                            UnitsConvOptionWidget(UnitType uType);

            void            ChangeUnitType(UnitType uType);
            int             GetUnitType() const {return initType;}

            void            SetUnitConv(int  uConv);
            void            SetUnitConv(const UnitIndex& inConv);
            int             GetUnitConv();
            void            GetUnitConv(UnitIndex& outConv);

            void            SetSensitive(bool inSens);
    };

    class UnitsConvUIC : public OptionBaseUIC, public UnitsConvOptionWidget  {

        public:
                          UnitsConvUIC(): OptionBaseUIC(this) {}
                          UnitsConvUIC(UnitType uType)
                              : OptionBaseUIC(this),UnitsConvOptionWidget(uType) {}
                  void    SetSensitive(bool isSensitive);
    };


    class UnitRealUIC : public BaseUIC {
        public:
            RealEntryWidget         realValueUI;
        protected:
            UnitIndex               currConv;

        public:

            void            InitFramedUI(      char       frameTitle[],
                                               Widget     parent,
                                         const ArgListC&  inArgs);

            void            InitLabeledUI(      char      labText[],
                                               Widget     parent,
                                         const ArgListC&  inArgs);

            void            SetUnitReal(const UnitReal&     inReal);
            void            SetUnitReal(const double&       inMetricVal,
                                        const UnitIndex&    inConv);

            UnitType        GetCurrUnitType() {return currConv.unitType;}
            UnitIndex       GetCurrUnitIndex() {return currConv;}


            void            ChangeUnitType(int  uType);
            void            ChangeConvIndex(const UnitIndex& newConv);

            bool            UIdataOK(bool& allOK);
            bool            UIdataOK(bool& allOK,
                                     const double& metricMin,
                                     const double& metricMax);


            void            GetUnitReal(UnitReal& outReal);
            void            GetUnitReal(double&     outMetricVal,
                                        UnitIndex&  outConv);
            void            GetUnitReal(double&     outMetricVal);

            void            UnitConvChange(const UnitIndex& newConv);

            void            SetSensitive(bool inSens);

        private:
                    void        InitUI();

    };

    class UnitRealAndConvUIC : public UnitRealUIC {
        public:
            UnitsConvOptionWidget   convIndexUI;

        public:

            void            InitFramedUI(      char       frameTitle[],
                                               Widget     parent,
                                         const ArgListC&  inArgs,
                                               UnitType   inUnits);

            void            InitLabeledUI(      char      labText[],
                                               Widget     parent,
                                         const ArgListC&  inArgs,
                                               UnitType   inUnits);

            void            ChangeUnitType(int  uType);
            void            ChangeConvIndex(const UnitIndex& newConv);

            void            SetUnitReal(const UnitReal&     inReal);
            void            SetUnitReal(const double&       inMetricVal,
                                        const UnitIndex&    inConv);

            void            SetSensitive(bool inSens);

        private:
                    void        InitUI(int      valPos,
                                       UnitType inUnits);

                    void        UnitConvChange();
            static  void        UnitConvChangeCB(void* inObj);

    };


    class UnitRealStatusUIC : public RealStatusUIC {
        public:
            void            SetUnitReal(const double&       inMetricVal,
                                        const UnitIndex&    inConv);
    };


    class UnitTableColUIC {
        public:
            BaseUIC                 frame;
            UnitsConvUIC            tableUnitsUI;
            ComboBoxSelectionUIC    tableColumnUI;

        public:
                    UnitTableColUIC() {};
                    ~UnitTableColUIC() {};

            void    InitUI(const char* frameTitle,
                           Widget parent, const ArgListC& args);

            void    SetTableColumns(const DC_TableData& inTbl);
            void    SetTableNA();

            void    SetData(const UnitTableCol& inData);

            void    GetData(UnitTableCol& outData);

            void    SetSensitive(bool inSens);
    };



};                                      // end namespace

using namespace genUnitsMenuUtilities;

#endif // !U_UNITS_H


