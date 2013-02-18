///////////////////////////////////////////////////////////////////////////////////
//
// DPO_MergeColorMapPS.cpp
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

#include "GenLibAfx.h"

#include <genClass/C_Common.h>

#include <genClass/U_Str.h>
#include <genClassPS/PS_Color.h>

#include "DPO_MergeColorMap.h"
#include "DPO_MergeColorMapPS.h"

//  draws current color map in drawing area

void DPO_MergeColorMapPS::RedrawColorMapCB(void* inObj)
{
    static_cast<DPO_MergeColorMapPS*>(inObj)->RedrawColorMap();
}

void DPO_MergeColorMapPS::RedrawColorMap()
{
    if (!(dataASource.CheckDataObj() && dataBSource.CheckDataObj()))
        return;

    DO_ColorMap* dataADO = (DO_ColorMap*) dataASource.GetDataObj();
    DO_ColorMap* dataBDO = (DO_ColorMap*) dataBSource.GetDataObj();

    DC_ColorMap newMap;
    if (!newMap.AddColorMaps(*(dataADO->colorMapData), *(dataBDO->colorMapData)))
        return;

    colorMapDraw.DrawColorMap(newMap);
}

DPO_MergeColorMapPS::DPO_MergeColorMapPS() :
    dataASource(typeid(DO_ColorMap)),
    dataBSource(typeid(DO_ColorMap))
{
}

void DPO_MergeColorMapPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC   args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    args.LeftFormAttach();
    args.RightPosAttach(50);
    dataASource.InitFramedUI("Map A", topRowForm.GetWidget(), args);
    dataASource.AddOtherObjectCallback(RedrawColorMapCB, this);

    args.ResetPlace();
    args.LeftUICAttach(dataASource);
    args.RightFormAttach();
    dataBSource.InitFramedUI("Map B", topRowForm.GetWidget(), args);
    dataBSource.AddOtherObjectCallback(RedrawColorMapCB, this);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    args.BottomFormAttach();
    colorMapDraw.InitWidget(PPform.GetWidget(), args);
    //    colorMapDraw.AddOtherObjectCallback(RedrawColorMapCB, this);
    //    colorMapDraw.SetDisplayData();
}

void DPO_MergeColorMapPS::SetMenuData(DPO_MergeColorMap&    inData)
{
    bool isOK = (inData.GetStatus() == os_OK);

    dataASource.SetFuncObjRef(inData.input1ObjRef);
    dataBSource.SetFuncObjRef(inData.input2ObjRef);

    MenuBaseC::SetMenuData(inData);

    RedrawColorMap();
}


void DPO_MergeColorMapPS::MenuOpen(DPO_MergeColorMap& inData)
{
    MenuBaseC::MenuOpen(false, false);
    dataASource.StdMenuInit(inData.input1ObjRef);
    dataBSource.StdMenuInit(inData.input2ObjRef);
}


bool DPO_MergeColorMapPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataASource.FuncObjRefOK(allOK, "Map A source");
    dataBSource.FuncObjRefOK(allOK, "Map B source");
    return allOK;
}

void DPO_MergeColorMapPS::GetMenuData(DPO_MergeColorMap&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.input1ObjRef  = dataASource.GetFuncObjRef();
    outData.input2ObjRef  = dataBSource.GetFuncObjRef();
}

