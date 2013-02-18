///////////////////////////////////////////////////////////////////////////////////
//
// objDPOUtil.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/DO_TableData.h>
#include <genClass/DO_XYData.h>

#include <objClass/DO_GridData.h>
#include <objClass/DO_CubeData.h>

#include "objDPOUtil.h"

namespace objDPOMenuUtilities {


   TableColSelUIC::TableColSelUIC() :
       tableInputObjRefUI(typeid(DO_TableData))
   {
   }

   void TableColSelUIC::SetSensitive()
   {
       selectedColUI.SetSensitive(tableDataOK);
   }

   void  TableColSelUIC::SetSelectedColTableColumns()
   {
       SetTableColumns(selectedColUI);
   }

   void TableColSelUIC::SetSelectedColTableColumnsCB(void* inObj)
   {
       static_cast<TableColSelUIC*>(inObj)->SetSelectedColTableColumns();
   }

   void TableColSelUIC::SetTableColumns(ListBaseWidget&  selUI)
   {
       tableDataOK = tableInputObjRefUI.CheckDataObj();
       if (tableDataOK)
       {
           const SC_StringArray& colDesc = static_cast<DO_TableData*>(tableInputObjRefUI.GetDataObj())->tableData->columnDesc;;
           selUI.SetListText(colDesc);
       }
       else
       {
           selUI.SetListNA();
       }
       SetSensitive();
   }

   void TableColSelUIC::ObjRefInit(Widget     objRefUIparent,
                                                        ArgListC&  args)
   {
       tableInputObjRefUI.InitFramedUI("Table Data To Use", objRefUIparent, args);
       tableInputObjRefUI.AddOtherObjectCallback(SetSelectedColTableColumnsCB, this);
   }

   void TableColSelUIC::ObjRefInit(const char* objRefTitle,
                                   Widget     objRefUIparent,
                                                        ArgListC&  args)
   {
       tableInputObjRefUI.InitFramedUI(const_cast<char*>(objRefTitle), objRefUIparent, args);
       tableInputObjRefUI.AddOtherObjectCallback(SetSelectedColTableColumnsCB, this);
   }

   void TableColSelUIC::StdInit(const char* objRefTitle,
                                Widget     objRefUIparent,
                               const char* selColTitle,
                               Widget     selColUIparent)
   {
       ArgListC args;
       args.AllFormAttach();
       tableInputObjRefUI.InitFramedUI(const_cast<char*>(objRefTitle), objRefUIparent, args);
       tableInputObjRefUI.AddOtherObjectCallback(SetSelectedColTableColumnsCB, this);

       args.StdFormAttach();
       selectedColUI.topOffset = 0;
       selectedColUI.listWidth = 20;
       selectedColUI.InitFramedUI(selColTitle, selColUIparent, args);
   }

   void TableColSelUIC::SelColInit(Widget     selColUIparent,
                                                        ArgListC&  args)
   {
       selectedColUI.InitFramedUI("Table Column", selColUIparent, args);
   }

   void TableColSelUIC::StdInit(Widget objRefUIparent,
                                                    Widget selColUIparent)
   {
       ArgListC args;
       args.AllFormAttach();
       ObjRefInit(objRefUIparent, args);

       args.StdFormAttach();
       selectedColUI.topOffset = 0;
       selectedColUI.listWidth = 20;
       SelColInit(selColUIparent, args);
   }


   bool TableColSelUIC::UIdataOK(bool& allOK)
   {
       tableInputObjRefUI.FuncObjRefOK(allOK, "Table data source");
       return allOK;
   }

   void TableColSelUIC::MenuOpen(DSC_TableColSel& inData)
   {
       tableInputObjRefUI.StdMenuInit(inData.tableInputObjRef);
   }
   void TableColSelUIC::SetTableColSel(const DSC_TableColSel& inData)
   {
       tableInputObjRefUI.SetFuncObjRef(inData.tableInputObjRef);
       selectedColUI.SetSelection(inData.selectedCol);
       SetSelectedColTableColumns();
   }
   void TableColSelUIC::GetTableColSel(DSC_TableColSel& outData)
   {
       outData.tableInputObjRef = tableInputObjRefUI.GetFuncObjRef();
       outData.selectedCol = selectedColUI.GetSelection();
   }



   XYColSelUIC::XYColSelUIC() :
       xyInputObjRefUI(typeid(DO_XYData))
   {
   }

   void XYColSelUIC::ObjRefInit(Widget     objRefUIparent,
                                                        ArgListC&  args)
   {
       xyInputObjRefUI.InitFramedUI("XY Input Data", objRefUIparent, args);
   }


   void XYColSelUIC::SelColInit(Widget     selColUIparent,
                                                        ArgListC&  args)
   {
       operateOnXUI.PreInitSetUp("   X Data   ", "   Y Data   ");
       operateOnXUI.InitFramedUI("Data Component to Operate On", selColUIparent, args);
   }

   void XYColSelUIC::StdInit(Widget objRefUIparent,
                                                  Widget selColUIparent)
   {
       ArgListC args;
       args.AllFormAttach();
       ObjRefInit(objRefUIparent, args);

       args.Clear();
       args.StdFormAttach();
       operateOnXUI.rightOffset = 50;
       SelColInit(selColUIparent, args);
   }


   bool XYColSelUIC::UIdataOK(bool& allOK)
   {
       xyInputObjRefUI.FuncObjRefOK(allOK, "XY data source");
       return allOK;
   }

   void XYColSelUIC::MenuOpen(DSC_XYColSel& inData)
   {
       xyInputObjRefUI.StdMenuInit(inData.xyInputObjRef);
   }
   void XYColSelUIC::SetXYColSel(const DSC_XYColSel& inData)
   {
       xyInputObjRefUI.SetFuncObjRef(inData.xyInputObjRef);
       operateOnXUI.SetBoolChoice(!inData.operateOnX);
   }
   void XYColSelUIC::GetXYColSel(DSC_XYColSel& outData)
   {
       outData.xyInputObjRef = xyInputObjRefUI.GetFuncObjRef();
       outData.operateOnX   = ! operateOnXUI.GetBoolChoice();
   }

   GridInputUIC::GridInputUIC() :
       gridInputObjRefUI(typeid(DO_GridData))
   {
       gridLabels.Alloc(3);
       gridLabels += " Grid X ";
       gridLabels += " Grid Y ";
   }

   void GridInputUIC::ResetLabelsCB(void* inObj)
   {
       static_cast<GridInputUIC*>(inObj)->ResetLabels();
   }

   void GridInputUIC::ResetLabels()
   {
       gridLabels.SetEmpty();
       if (gridInputObjRefUI.CheckDataObj())
       {
           const DC_GridData*  gridInputDC = static_cast<DO_GridData*>(gridInputObjRefUI.GetDataObj())->gridData;
           gridLabels += gridInputDC->xData.dataID;
           gridLabels += gridInputDC->yData.dataID;
       }
       else
       {
           gridLabels += " Grid X ";
           gridLabels += " Grid Y ";
      }
   }


   void GridInputUIC::ObjRefInit(Widget     objRefUIparent,
                                 ArgListC&  args)
   {
       gridInputObjRefUI.InitFramedUI("Grid Input Data", objRefUIparent, args);
       gridInputObjRefUI.AddOtherObjectCallback(ResetLabelsCB, this);
   }

   void GridInputUIC::StdInit(Widget objRefUIparent)
   {
       ArgListC args;
       args.AllFormAttach();
       ObjRefInit(objRefUIparent, args);
   }


   bool GridInputUIC::UIdataOK(bool& allOK)
   {
       gridInputObjRefUI.FuncObjRefOK(allOK, "Grid input data source");
       return allOK;
   }

   void GridInputUIC::MenuOpen(DSC_GridInput& inData)
   {
       gridInputObjRefUI.StdMenuInit(inData.gridInputObjRef);
   }
   void GridInputUIC::SetGridInput(const DSC_GridInput& inData)
   {
       gridInputObjRefUI.SetFuncObjRef(inData.gridInputObjRef);
       ResetLabels();
   }
   void GridInputUIC::GetGridInput(DSC_GridInput& outData)
   {
       outData.gridInputObjRef = gridInputObjRefUI.GetFuncObjRef();
   }

   CubeInputUIC::CubeInputUIC() :
       cubeInputObjRefUI(typeid(DO_CubeData))
   {
       cubeLabels.Alloc(3);
       cubeLabels += " Cube X ";
       cubeLabels += " Cube Y ";
       cubeLabels += " Cube Z ";
   }

   void CubeInputUIC::ResetLabelsCB(void* inObj)
   {
       static_cast<CubeInputUIC*>(inObj)->ResetLabels();
   }

   void CubeInputUIC::ResetLabels()
   {
       cubeLabels.SetEmpty();
       if (cubeInputObjRefUI.CheckDataObj())
       {
           DC_CubeData*  cubeInputDC = static_cast<DO_CubeData*>(cubeInputObjRefUI.GetDataObj())->cubeData;
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

   void CubeInputUIC::ObjRefInit(Widget     objRefUIparent,
                                 ArgListC&  args)
   {
       cubeInputObjRefUI.InitFramedUI("Cube Input Data", objRefUIparent, args);
       cubeInputObjRefUI.AddOtherObjectCallback(ResetLabelsCB, this);
   }

   void CubeInputUIC::StdInit(Widget objRefUIparent)
   {
       ArgListC args;
       args.AllFormAttach();
       ObjRefInit(objRefUIparent, args);
   }


   bool CubeInputUIC::UIdataOK(bool& allOK)
   {
       cubeInputObjRefUI.FuncObjRefOK(allOK, "Cube input data source");
       return allOK;
   }

   void CubeInputUIC::MenuOpen(DSC_CubeInput& inData)
   {
       cubeInputObjRefUI.StdMenuInit(inData.cubeInputObjRef);
   }
   void CubeInputUIC::SetCubeInput(const DSC_CubeInput& inData)
   {
       cubeInputObjRefUI.SetFuncObjRef(inData.cubeInputObjRef);
       ResetLabels();
   }
   void CubeInputUIC::GetCubeInput(DSC_CubeInput& outData)
   {
       outData.cubeInputObjRef = cubeInputObjRefUI.GetFuncObjRef();
   }



   DataCoordStatusUIC::DataCoordStatusUIC()
   {
   }

   void DataCoordStatusUIC::StdInit(const char* title,
                                       Widget     parent,
                                          ArgListC&  args)

   {
       coordFrame.InitFramedUI(title, parent, args);

       args.StdSplitSetup(20);
       coordID.InitWidget(coordFrame.GetForm(), args);

       args.StdSplitMiddle(25, 35);
       coordType.InitWidget(coordFrame.GetForm(), args);

       args.StdSplitMiddle(40, 48);
       coordN.InitWidget(coordFrame.GetForm(), args);

       args.StdSplitMiddle(52, 73);
       coordMin.InitWidget(coordFrame.GetForm(), args);

       args.StdSplitFinish(23);
       coordMax.InitWidget(coordFrame.GetForm(), args);
   }


   void DataCoordStatusUIC::SetOKStatus(const DC_DataCoord& inData)
   {
       coordFrame.SetSensitive(true);
       coordID.SetFieldString(inData.dataID);
       if (inData.dataIsLog)
       {
           coordType.SetFieldString("log");
           coordMin.SetFieldFormat(SC_SciFormat(6));
           coordMax.SetFieldFormat(SC_SciFormat(6));
       }
       else
       {
           coordType.SetFieldString("lin");
           coordMin.SetFieldFormat(SC_RealFormat());
           coordMax.SetFieldFormat(SC_RealFormat());
       }

       coordN.SetFieldValue(inData.Size());
       coordMin.SetFieldValue(inData[0]);
       coordMax.SetFieldValue(inData.LastIndex());
   }

   void DataCoordStatusUIC::SetBadStatus()
   {
       coordFrame.SetSensitive(false);
       coordID.SetStatusNA();
       coordType.SetStatusNA();
       coordN.SetStatusNA();
       coordMin.SetStatusNA();
       coordMax.SetStatusNA();
   }

   CurveUIC::CurveUIC()
   {
   }

   void CurveUIC::SetSensitive()
   {
       bool isSpline     = (curveTypeUI.GetChoice() == 0);
       bool isPolynomial = (curveTypeUI.GetChoice()  == 1);
       bool isUserSlope  =  !splineNaturalSlopeUI.GetBoolChoice();

       splineForm.SetSensitive(isSpline);
       splineNaturalSlopeUI.SetSensitive(isSpline);
       fixedSplineStartSlopeUI.SetSensitive(isSpline && isUserSlope);
       fixedSplineEndSlopeUI.SetSensitive(isSpline && isUserSlope);
       splineTensionUI.SetSensitive(isSpline);

       polyOrderUI.SetSensitive(isPolynomial);
   }

   void CurveUIC::SetSensitiveCB(void* inObj)
   {
       static_cast<CurveUIC*>(inObj)->SetSensitive();
   }

   void CurveUIC::StdInit(Widget     parent,
                          ArgListC&  inArgs)
   {
       mainForm.InitFormUI(parent, inArgs);

       ArgListC  args;

       args.Clear();
       args.StdFormAttach();
       static const char* typeLabels[]  = {"CubicSpline", "Polynomial", "Linear", "StepMid", "StepFull"};

       curveTypeUI.PreInitSetUp(typeLabels, 5);
       curveTypeUI.InitFramedUI("CurveType", mainForm.GetLeft(), args);
       curveTypeUI.AddOtherObjectCallback(SetSensitiveCB, this);

       args.Clear();
       args.StdTopObjectAttach(curveTypeUI);
   //  args.BottomFormAttach();  // TOFIX Layout bug: added frame dissapears
       polyOrderUI.SetFieldLen(5);
       polyOrderUI.InitFramedUI("Polynomial Order", mainForm.GetLeft(), args);

       args.Clear();
       args.AllFormAttach();
       splineForm.InitFramedUI("Spline Settings", mainForm.GetRight(), args);

       args.Clear();
       args.StdFormAttach();
       splineNaturalSlopeUI.PreInitSetUp("User", "Natural");
       splineNaturalSlopeUI.InitLabeledUI("Slope", splineForm.GetForm(), args);
       splineNaturalSlopeUI.AddOtherObjectCallback(SetSensitiveCB, this);

       args.Clear();
       args.StdTopObjectAttach(splineNaturalSlopeUI);
       fixedSplineStartSlopeUI.SetFieldLen(8);
       fixedSplineStartSlopeUI.InitLabeledUI("Start Slope", splineForm.GetForm(), args);

       args.Clear();
       args.StdTopObjectAttach(fixedSplineStartSlopeUI);
       fixedSplineEndSlopeUI.SetFieldLen(8);
       fixedSplineEndSlopeUI.InitLabeledUI("End Slope", splineForm.GetForm(), args);

       args.Clear();
       args.StdTopObjectAttach(fixedSplineEndSlopeUI);
       splineTensionUI.SetFieldLen(8);
       splineTensionUI.InitLabeledUI("Tension", splineForm.GetForm(), args);
   }


   bool CurveUIC::UIdataOK(bool& allOK)
   {
       if(curveTypeUI.GetChoice() == 0)
       {
           if (!splineNaturalSlopeUI.GetBoolChoice())
           {
               fixedSplineStartSlopeUI.FieldValueOK(allOK, "Spline Start Slope");
               fixedSplineEndSlopeUI.FieldValueOK(allOK, "Spline End Slope");
           }
           splineTensionUI.FieldValueOK(allOK, "Spline Tension", 1.0E-10, 100.0);
       }

       if (curveTypeUI.GetChoice() == 1)
           polyOrderUI.FieldValueOK(allOK, "Polynomial Order", 1, 12);

       return allOK;
   }

   void CurveUIC::SetCurve(const CurveDescription& inData)
   {
       curveTypeUI.SetChoice(int(inData.curveType));
       splineNaturalSlopeUI.SetBoolChoice(inData.splineNaturalSlope);
       fixedSplineStartSlopeUI.SetFieldValue(inData.fixedSplineStartSlope);
       fixedSplineEndSlopeUI.SetFieldValue(inData.fixedSplineEndSlope);
       splineTensionUI.SetFieldValue(inData.splineTension);
       polyOrderUI.SetFieldValue(inData.polyOrder);

       SetSensitive();
   }
   void CurveUIC::GetCurve(CurveDescription& outData)
   {
       outData.curveType = Curve::CurveType(curveTypeUI.GetChoice());

       if (outData.curveType == Curve::ctCubicSpline)
       {
           outData.splineNaturalSlope = splineNaturalSlopeUI.GetBoolChoice();

           if (!outData.splineNaturalSlope)
           {
               outData.fixedSplineStartSlope = fixedSplineStartSlopeUI.GetFieldValue();
               outData.fixedSplineEndSlope = fixedSplineEndSlopeUI.GetFieldValue();
           }

           outData.splineTension = splineTensionUI.GetFieldValue();
       }

       if (outData.curveType == Curve::ctPolynomial)
           outData.polyOrder = polyOrderUI.GetFieldValue();

   }


   InterpPointsUIC::InterpPointsUIC()
   {
   }

   void InterpPointsUIC::SetSensitive()
   {
       bool useSettings = !AtInputX();

       useDataLimitsUI.SetSensitive(useSettings);
       numInterpPointsUI.SetSensitive(useSettings);
       minMaxFrame.SetSensitive(useSettings);

       bool usesMinMax   = useSettings && (!useDataLimitsUI.GetBoolChoice());
       minUserLimitUI.SetSensitive(usesMinMax);
       maxUserLimitUI.SetSensitive(usesMinMax);

       bool logRel       =  (interpPointsOpUI.GetChoice() == 3);
       logRelativestartUI.SetSensitive(logRel);
   }

   void InterpPointsUIC::SetSensitiveCB(void* inObj)
   {
       static_cast<InterpPointsUIC*>(inObj)->SetSensitive();
   }

   void InterpPointsUIC::StdInit(Widget     parent,
                          ArgListC&  inArgs)
   {
       mainForm.InitFormUI(parent, inArgs);

       ArgListC  args;
       args.StdFormAttach();

       static const char* typeLabels[]  = {"Input X", "Linear", "Log (Absolute)", "Log (Relative)"};

       interpPointsOpUI.PreInitSetUp(typeLabels, 4);
       interpPointsOpUI.InitFramedUI("Interpolation Method", mainForm.GetLeft(), args);
       interpPointsOpUI.AddOtherObjectCallback(SetSensitiveCB, this);

       args.Clear();
       args.StdTopObjectAttach(interpPointsOpUI);
       numInterpPointsUI.SetFieldLen(5);
       numInterpPointsUI.InitFramedUI("Number of points", mainForm.GetLeft(), args);

       args.Clear();
       args.StdTopObjectAttach(numInterpPointsUI);
       logRelativestartUI.SetFieldLen(8);
       logRelativestartUI.InitFramedUI("Log relative start", mainForm.GetLeft(), args);

       args.Clear();
       args.AllFormAttach();
       minMaxFrame.InitFramedUI("Limit Settings", mainForm.GetRight(), args);

       args.Clear();
       args.StdFormAttach();
       useDataLimitsUI.PreInitSetUp("Specified", "From Input");
       useDataLimitsUI.InitLabeledUI("Limits", minMaxFrame.GetForm(), args);
       useDataLimitsUI.AddOtherObjectCallback(SetSensitiveCB, this);

       args.Clear();
       args.StdTopObjectAttach(useDataLimitsUI);
       minUserLimitUI.SetFieldLen(8);
       minUserLimitUI.InitLabeledUI("Minimum", minMaxFrame.GetForm(), args);

       args.Clear();
       args.StdTopObjectAttach(minUserLimitUI);
       maxUserLimitUI.SetFieldLen(8);
       maxUserLimitUI.InitLabeledUI("Maximum", minMaxFrame.GetForm(), args);
   }

   bool InterpPointsUIC::AtInputX()
   {
       return interpPointsOpUI.GetChoice() == 0;
   }


   bool InterpPointsUIC::UIdataOK(bool& allOK)
   {
       if(!AtInputX())
       {
           if (!useDataLimitsUI.GetBoolChoice())
           {
               minUserLimitUI.FieldValueOK(allOK, "Minimum");
               maxUserLimitUI.FieldValueOK(allOK, "Maximum");
           }

           if(interpPointsOpUI.GetChoice() == 3)
               logRelativestartUI.FieldValueOK(allOK, "Log start", 1.0E-10, 10000.0);

           numInterpPointsUI.FieldValueOK(allOK, "# of points", 5, 100000);
       }


       return allOK;
   }

   void InterpPointsUIC::SetInterp(const DSC_InterpPoints& inData)
   {
       interpPointsOpUI.SetChoice(int(inData.interpPointsOp));

       useDataLimitsUI.SetBoolChoice(inData.useDataLimits);
       minUserLimitUI.SetFieldValue(inData.minUserLimit);
       maxUserLimitUI.SetFieldValue(inData.maxUserLimit);

       logRelativestartUI.SetFieldValue(inData.logRelativestart);
       numInterpPointsUI.SetFieldValue(inData.numInterpPoints);

       SetSensitive();
   }

   void InterpPointsUIC::GetInterp(DSC_InterpPoints& outData)
   {
       outData.interpPointsOp = DSC_InterpPoints::InterpPointsOp(interpPointsOpUI.GetChoice());

       if(!AtInputX())
       {
           outData.useDataLimits = useDataLimitsUI.GetBoolChoice();

           if (!outData.useDataLimits)
           {
               outData.minUserLimit = minUserLimitUI.GetFieldValue();
               outData.maxUserLimit = maxUserLimitUI.GetFieldValue();
           }

           if(interpPointsOpUI.GetChoice() == 3)
               outData.logRelativestart = logRelativestartUI.GetFieldValue();

           outData.numInterpPoints = numInterpPointsUI.GetFieldValue();
       }


   }

   void InterpPointsUIC::SetDefault()
   {
       SetInterp(DSC_InterpPoints());
   }


};                                      // end namespace

