///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ScaleTransformPS.h
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

#ifndef DPO_SCALETRANSFORMPS_H
#define DPO_SCALETRANSFORMPS_H

#include "DPO_ScaleTransform.h"
#include <genApp/C_MenuBase.h>
#include <genApp/U_MenuDPO.h>

class DPO_ScaleTransform;

class DPO_ScaleTransformPS : public MenuBaseC {

    friend class DPO_ScaleTransform;

protected:
    FormWidget          topRowForm;
    BoolOptionUIC       scaleFirstUI;
    OptionUIC           scaleOpUI;
    OptionUIC           tranOpUI;

    RealMSSpecUI        scaleValueMSUI;
    RealMSSpecUI        offsetValueMSUI;

    BaseUIC             nullFrame;
    RealEntryUIC        setNullValueUI;
    ToggleBoxWidget     processNullsUI;

public:
    DPO_ScaleTransformPS();

    void        InitMenu();
    void        InitSTCommon(ArgListC& attachments);

    void        SetMenuData(DPO_ScaleTransform& inData);
    bool        UIdataOK();
    void        GetMenuData(DPO_ScaleTransform& outData);
    void        MenuOpen(DPO_ScaleTransform& inData);
    void        MenuDefault();

private:
    static void     SetAllSensitiveCB(void* inObj);
    void            SetAllSensitive();

    static void     ToggleExposedScaleValueCB(void* inObj);
    void            ToggleExposedScaleValue();
    static void     ToggleExposedOffsetValueCB(void* inObj);
    void            ToggleExposedOffsetValue();
};

#endif // DPO_SCALETRANSFORMSPS_H

