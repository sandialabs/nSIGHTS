///////////////////////////////////////////////////////////////////////////////////
//
// objPPOUtil.cpp
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

#include "ObjLibAfx.h"

#include <genClass/DO_ColorMap.h>
#include <objClass/DO_GridData.h>
#include <objClass/DO_CubeData.h>
#include <objClass/DO_CubeIndxArray.h>

#include "objPPOUtil.h"

namespace objPPOMenuUtilities {


   HorsetailUIC::HorsetailUIC() :
       colorMapDataObjRefUI(typeid(DO_ColorMap))
   {
   }

   void HorsetailUIC::SetSensitive()
   {
       bool useCM = plotWithColorMapUI.GetBoolChoice();
       fixedColorPenUI.SetSensitive(!useCM);
       colorMapDataObjRefUI.SetSensitive(useCM);

       COLORREF lineColor = RGB(0,0,0);
       if (!useCM)
           lineColor = fixedColorPenUI.GetSelectedColorValue();

       lineTypeUI.SetLineColorAndThick(lineColor, lineThickUI.GetLineThick());
   }

   void HorsetailUIC::SetSensitiveCB(void* inObj)
   {
       static_cast<HorsetailUIC*>(inObj)->SetSensitive();
   }

   void HorsetailUIC::StdInit(Widget       parent,
                              ArgListC&    attachments)
   {
       colorForm.InitWidget(parent, attachments);

       ArgListC args;

       args.StdSplitSetup(30);
       plotWithColorMapUI.rightOffset = 50;
       plotWithColorMapUI.PreInitSetUp("Pen", "Color Map", false);
       plotWithColorMapUI.InitFramedUI("Horsetail Color", colorForm.GetWidget(), args);
       plotWithColorMapUI.AddOtherObjectCallback(SetSensitiveCB, this);

       args.StdSplitMiddle(30, 60);
       fixedColorPenUI.InitFramedUI("Single Color Pen", colorForm.GetWidget(), args);
       fixedColorPenUI.AddOtherObjectCallback(SetSensitiveCB, this);

       args.StdSplitFinish(40);
       colorMapDataObjRefUI.InitFramedUI("Color Map", colorForm.GetWidget(), args);

       args.StdTopObjectAttach(colorForm.GetWidget());
       formatFrame.InitFramedUI("Horsetail Line Options", parent, args);

       args.StdSplitSetup(30);
       args.LeftOffset(20);
       lineThickUI.InitWidget(formatFrame.GetForm(), args);
       lineThickUI.AddOtherObjectCallback(SetSensitiveCB, this);

       args.StdSplitMiddle(45, 70);
       lineTypeUI.InitWidget(formatFrame.GetForm(), args);

       args.StdTopObjectAttach(formatFrame);
       legendDataUI.InitFramedUI("Legend", parent, args);
   }


   bool HorsetailUIC::UIdataOK(bool& allOK)
   {
       if (plotWithColorMapUI.GetBoolChoice())
           colorMapDataObjRefUI.FuncObjRefOK(allOK, "Color map");
       legendDataUI.UIdataOK(allOK);
       return allOK;
   }

   void HorsetailUIC::MenuOpen(PlotObjC&  inObj,
                               PSC_Horsetail& inData)
   {
       colorMapDataObjRefUI.StdMenuInit(inData.colorMapDataObjRef);
       fixedColorPenUI.ResetPens(inObj.objPenSet);
   }


   void HorsetailUIC::SetHorsetail(const PSC_Horsetail& inData)
   {
       plotWithColorMapUI.SetBoolChoice(inData.plotWithColorMap);
       fixedColorPenUI.SetPenSelection(inData.fixedColorPen);
       colorMapDataObjRefUI.SetFuncObjRef(inData.colorMapDataObjRef);

       lineThickUI.SetLineThick(inData.lineData.lineThickness);
       lineTypeUI.SetLineType(inData.lineData.lineType);
       legendDataUI.SetLegendOverride(inData.legendData);
       SetSensitive();
   }
   void HorsetailUIC::GetHorsetail(PSC_Horsetail& outData)
   {
       outData.plotWithColorMap = plotWithColorMapUI.GetBoolChoice();
       if (outData.plotWithColorMap)
           outData.colorMapDataObjRef = colorMapDataObjRefUI.GetFuncObjRef();
       else
           outData.fixedColorPen = fixedColorPenUI.GetPenSelection();

       outData.lineData.lineThickness = lineThickUI.GetLineThick();
       outData.lineData.lineType = lineTypeUI.GetLineType();
       legendDataUI.GetLegendOverride(outData.legendData);
   }


   GridDataUIC::GridDataUIC() :
       otherGridZObjRefUI(typeid(DO_GridData)),
       fixedZValueMSUI(typeid(DO_Real), false)
   {
   }

   void GridDataUIC::SetSensitive()
   {
       int zvalueSource = zvalueSourceUI.GetChoice();
       otherGridZObjRefUI.SetSensitive(zvalueSource == 1);
       fixedZValueMSUI.SetSensitive(zvalueSource == 2);
   }

   void GridDataUIC::SetSensitiveCB(void* inObj)
   {
       static_cast<GridDataUIC*>(inObj)->SetSensitive();
   }

   void GridDataUIC::StdInit(Widget       parent,
                             FuncObjRefUIC&  gridDataUI,
                             FormWidget&     topRowForm)
   {
       ArgListC    args;

       args.Clear();
       args.StdFormAttach();
       topRowForm.InitWidget(parent, args);

       args.StdSplitSetup(40);
       gridDataUI.InitFramedUI("Grid Data To Plot", topRowForm.GetWidget(), args);

       args.ResetPlace();
       args.LeftPosAttach(40);
       args.RightPosAttach(60);

       static const char*   zLabels[]  = {"Same", "Other Grid", "Constant"};
       zvalueSourceUI.PreInitSetUp(zLabels, 3);
       zvalueSourceUI.InitFramedUI("Z Value", topRowForm.GetWidget(), args);
       zvalueSourceUI.AddOtherObjectCallback(SetSensitiveCB, this);

       args.StdSplitFinish(40);
       otherGridZObjRefUI.InitFramedUI("Z Value Grid", topRowForm.GetWidget(), args);

       args.Clear();
       args.StdTopObjectAttach(topRowForm.GetWidget());
       fixedZValueMSUI.InitUI("Fixed Z Value", args, parent, true, true, SetSensitiveCB, this);
   }


   bool GridDataUIC::UIdataOK(bool& allOK)
   {
       int zvalueSource = zvalueSourceUI.GetChoice();

       if (zvalueSource == 1)
           otherGridZObjRefUI.FuncObjRefOK(allOK, "Z Grid");
       else
           if (zvalueSource == 2)
               fixedZValueMSUI.UIdataOK(allOK);

       return allOK;
   }

   void GridDataUIC::MenuOpen(PlotObjC&  inObj,
                               PSC_3DGridData& inData)
   {
       otherGridZObjRefUI.StdMenuInit(inData.otherGridZObjRef);
       fixedZValueMSUI.MenuInit(inData.fixedZValueMS);
   }


   void GridDataUIC::Set3DGridData(const PSC_3DGridData& inData)
   {
       zvalueSourceUI.SetChoice(int(inData.zvalueSource));
       otherGridZObjRefUI.SetFuncObjRef(inData.otherGridZObjRef);
       fixedZValueMSUI.SetValue(inData.fixedZValueMS);
       SetSensitive();
   }
   void GridDataUIC::Get3DGridData(PSC_3DGridData& outData)
   {
       outData.zvalueSource = PSC_3DGridData::ZValueSource(zvalueSourceUI.GetChoice());
       if (outData.zvalueSource == PSC_3DGridData::zvs_OtherGrid)
           outData.otherGridZObjRef = otherGridZObjRefUI.GetFuncObjRef();
       else if (outData.zvalueSource == PSC_3DGridData::zvs_FixedVal)
           fixedZValueMSUI.GetValue(outData.fixedZValueMS);

   }

   CubeDataUIC::CubeDataUIC() :
       cubeIndexesObjRefUI(typeid(DO_CubeIndxArray))
   {
       cubeLabels.Alloc(3);
       cubeLabels += " Cube X ";
       cubeLabels += " Cube Y ";
       cubeLabels += " Cube Z ";
       p_cubeDataUI = 0;
   }

   void CubeDataUIC::ResetLabelsCB(void* inObj)
   {
       static_cast<CubeDataUIC*>(inObj)->ResetLabels();
   }

   void CubeDataUIC::ResetLabels()
   {
       cubeLabels.SetEmpty();
       if (p_cubeDataUI && p_cubeDataUI->CheckDataObj())
       {
           DC_CubeData*  cubeInputDC = static_cast<DO_CubeData*>(p_cubeDataUI->GetDataObj())->cubeData;
           cubeLabels += cubeInputDC->xData.dataID;
           cubeLabels += cubeInputDC->yData.dataID;
           cubeLabels += cubeInputDC->zData.dataID;
       }
       else
       {
           cubeLabels += " Cube X ";
           cubeLabels += " Cube Y ";
           cubeLabels += " Cube Z ";
       }
   }


   void CubeDataUIC::SetSensitive()
   {
   }

   void CubeDataUIC::SetSensitiveCB(void* inObj)
   {
       static_cast<CubeDataUIC*>(inObj)->SetSensitive();
   }

   void CubeDataUIC::StdInit(Widget         parent,
                             FuncObjRefUIC& cubeDataUI,
                             FormWidget&    topRowForm,
                             bool           useIndx)
   {
       useIndexes = useIndx;

       ArgListC    args;

       args.Clear();
       args.StdFormAttach();
       topRowForm.InitWidget(parent, args);

       if (useIndexes)
           args.StdSplitSetup();

       cubeDataUI.InitFramedUI("Cube Data To Plot", topRowForm.GetWidget(), args);
       cubeDataUI.AddOtherObjectCallback(ResetLabelsCB, this);
       p_cubeDataUI = &cubeDataUI;

       if (useIndexes)
       {
           args.StdSplitFinish();
           cubeIndexesObjRefUI.InitFramedUI("Cube Indexes", topRowForm.GetWidget(), args);
       }
   }


   bool CubeDataUIC::UIdataOK(bool& allOK)
   {
       if (useIndexes)
           cubeIndexesObjRefUI.FuncObjRefOK(allOK, "Cube indexes");
       return allOK;
   }

   void CubeDataUIC::MenuOpen(PlotObjC&  inObj,
                               PSC_3DCubeData& inData)
   {
       if (useIndexes)
           cubeIndexesObjRefUI.StdMenuInit(inData.cubeIndexesObjRef);
   }


   void CubeDataUIC::Set3DCubeData(const PSC_3DCubeData& inData)
   {
       if (useIndexes)
           cubeIndexesObjRefUI.SetFuncObjRef(inData.cubeIndexesObjRef);
       SetSensitive();
   }
   void CubeDataUIC::Get3DCubeData(PSC_3DCubeData& outData)
   {
       if (useIndexes)
           outData.cubeIndexesObjRef = cubeIndexesObjRefUI.GetFuncObjRef();
   }

   void CoordMapUIC::StdInit(Widget       parent,
                             ArgListC&  attachments)
   {
       coordMapFrame.InitFramedUI("3D Coordinate Mapping", parent, attachments);

       ArgListC    args;
       args.StdSplitSetup(30);

       static const char*   mapLabels[]  = {"X Axis", "Y Axis", "Z Axis"};
       xMapUI.PreInitSetUp(mapLabels, 3);
       xMapUI.InitLabeledUI("Data X", coordMapFrame.GetForm(), args);

       args.ResetPlace();
       args.LeftPosAttach(35);
       args.RightPosAttach(65);
       yMapUI.PreInitSetUp(mapLabels, 3);
       yMapUI.InitLabeledUI("Data Y", coordMapFrame.GetForm(), args);

       args.StdSplitFinish(30);
       zMapUI.PreInitSetUp(mapLabels, 3);
       zMapUI.InitLabeledUI("Data Z", coordMapFrame.GetForm(), args);
   }

   void CoordMapUIC::SetLabels(SC_StringArray& cubeLab)
   {
       xMapUI.ChangeLabel(cubeLab[0]);
       yMapUI.ChangeLabel(cubeLab[1]);
       zMapUI.ChangeLabel(cubeLab[2]);
   }

   void CoordMapUIC::Set3DCoordMap(const PSC_3DCoordMap& inData)
   {
       xMapUI.SetChoice(int(inData.xMap));
       yMapUI.SetChoice(int(inData.yMap));
       zMapUI.SetChoice(int(inData.zMap));
   }
   void CoordMapUIC::Get3DCoordMap(PSC_3DCoordMap& outData)
   {
       outData.xMap = PSC_3DCoordMap::CoordMapType(xMapUI.GetChoice());
       outData.yMap = PSC_3DCoordMap::CoordMapType(yMapUI.GetChoice());
       outData.zMap = PSC_3DCoordMap::CoordMapType(zMapUI.GetChoice());
   }




};                                      // end namespace
