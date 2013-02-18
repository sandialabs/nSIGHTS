///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PenSetPS.h
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              User interface for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DPO_PENSETPS_H
#define DPO_PENSETPS_H

#include <genApp/U_Menu.h>
#include <genApp/C_MenuBase.h>

#include "DPO_PenSet.h"

class DPO_PenSetPS;

class PenSetSliderUIC : public EntrySliderUIC {
public:
    void            InitLabeledUI(char             label[],
                                  Widget           parent,
                                  const ArgListC&  inArgs,// attachment resources + others
                                  DPO_PenSetPS*    penSetPS);
};

class PenSetButton : public DrawnButton {
public:
    int                 buttonIndx;
    DPO_PenSetPS*       m_pMenu;

public:
    void        DoInit(int              indx,
                       LayoutManager&   parent,
                       ArgListC&        args);

    void         Select();
private:
    static void  SelectCB(void* inObj);


};

class PenSetScrolledForm : public ScrolledForm {
protected:

    virtual CSize SetCalculateFormSize();
};


class DPO_PenSetPS : public MenuBaseC {

    friend class PenSetButton;
    friend class PenSetSliderUIC;

protected:

    BoolRadioBoxUIC     useHSV;

    BaseUIC             sliderForm;
    BaseUIC             rgbForm;
    PenSetButton        penButtons[DC_PenSet::maxPenSet];

    TextEntryWidget     penDescs[DC_PenSet::maxPenSet];

    PenSetSliderUIC     redHueSlider;
    PenSetSliderUIC     greenSatSlider;
    PenSetSliderUIC     blueValSlider;

    SC_ColorSpec        penColors[DC_PenSet::maxPenSet];

    PenSetScrolledForm  scrollArea;
    FormWidget          penForms[DC_PenSet::maxPenSet];

    int                 currPenButton;

public:
    DPO_PenSetPS();

    void    InitMenu();

    void    SetMenuData(DPO_PenSet&     inData);
    bool    UIdataOK();
    void    GetMenuData(DPO_PenSet&  outData);

    void    MenuDefault();

    void    SetSliderVals();
    void    SetRowLabels();
    void    SetButtonColor(int buttonIndx);

private:
    static void UseHSVCB(void* inObj);
    static void SliderCB(void* inObj);

};

#endif // DPO_PENSETPS_H

