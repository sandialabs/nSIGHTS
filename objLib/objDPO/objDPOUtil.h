///////////////////////////////////////////////////////////////////////////////////
//
// objDPOUtil.h
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

#ifndef OBJDPOUTIL_H
#define OBJDPOUTIL_H

#include <genApp/U_Menu.h>
#include <objClass/C_Curve.h>
#include <objDataClass/DSC_InterpPoints.h>
#include <objDataClass/DSC_TableColSel.h>
#include <objDataClass/DSC_XYColSel.h>
#include <objDataClass/DSC_GridInput.h>
#include <objDataClass/DSC_CubeInput.h>


namespace objDPOMenuUtilities {

  class TableColSelUIC  {
      public:
          FuncObjRefUIC           tableInputObjRefUI;
          ComboBoxSelectionUIC    selectedColUI;
          bool                    tableDataOK;

      public:
                          TableColSelUIC();

          void            StdInit(Widget     objRefUIparent,
                                  Widget     selColUIparent);

          void            StdInit(const char* objRefTitle,
                                  Widget     objRefUIparent,
                                  const char* selColTitle,
                                  Widget     selColUIparent);

          void            ObjRefInit(Widget     objRefUIparent,
                                     ArgListC&  args);

          void            ObjRefInit(const char* objRefTitle,
                                     Widget     objRefUIparent,
                                     ArgListC&  args);

          void            SelColInit(Widget     selColUIparent,
                                     ArgListC&  args);

          bool            UIdataOK(bool& allOK);

          void            MenuOpen(DSC_TableColSel& inData);
          void            SetTableColSel(const DSC_TableColSel& inData);
          void            GetTableColSel(DSC_TableColSel& outData);

          void            SetSensitive();
          void            SetTableColumns(ListBaseWidget&  selUI);
      private:
          void            SetSelectedColTableColumns();
          static  void    SetSelectedColTableColumnsCB(void* inObj);
  };

  class XYColSelUIC  {
      public:
          FuncObjRefUIC           xyInputObjRefUI;
          BoolRadioBoxUIC         operateOnXUI;

      public:
                          XYColSelUIC();

          void            StdInit(Widget     objRefUIparent,
                                  Widget     selColUIparent);

          void            ObjRefInit(Widget     objRefUIparent,
                                     ArgListC&  args);

          void            SelColInit(Widget     selColUIparent,
                                     ArgListC&  args);

          bool            UIdataOK(bool& allOK);

          void            MenuOpen(DSC_XYColSel& inData);
          void            SetXYColSel(const DSC_XYColSel& inData);
          void            GetXYColSel(DSC_XYColSel& outData);
  };

  class GridInputUIC  {
      public:
          FuncObjRefUIC           gridInputObjRefUI;
          SC_StringArray          gridLabels;

      public:
                          GridInputUIC();

          void            StdInit(Widget     objRefUIparent);

          void            ObjRefInit(Widget     objRefUIparent,
                                     ArgListC&  args);


          bool            UIdataOK(bool& allOK);

          void            MenuOpen(DSC_GridInput& inData);
          void            SetGridInput(const DSC_GridInput& inData);
          void            GetGridInput(DSC_GridInput& outData);

      private:
          static  void    ResetLabelsCB(void* inObj);
                  void    ResetLabels();

  };

  class DataCoordStatusUIC  {
      public:
          BaseUIC             coordFrame;
          StatusWidget        coordID;
          StatusWidget        coordType;
          IntStatusWidget     coordN;
          RealStatusWidget    coordMin;
          RealStatusWidget    coordMax;

      public:
                          DataCoordStatusUIC();

          void            StdInit(const char* title,
                                  Widget     parent,
                                  ArgListC&  args);

          void            SetOKStatus(const DC_DataCoord& inData);
          void            SetBadStatus();


  };



  class CubeInputUIC  {
      public:
          FuncObjRefUIC           cubeInputObjRefUI;
          SC_StringArray          cubeLabels;

      public:
                          CubeInputUIC();

          void            StdInit(Widget     objRefUIparent);

          void            ObjRefInit(Widget     objRefUIparent,
                                     ArgListC&  args);


          bool            UIdataOK(bool& allOK);

          void            MenuOpen(DSC_CubeInput& inData);
          void            SetCubeInput(const DSC_CubeInput& inData);
          void            GetCubeInput(DSC_CubeInput& outData);

      private:
          static  void    ResetLabelsCB(void* inObj);
                  void    ResetLabels();
  };


  class CurveUIC  {
      public:
          SplitFormUIC            mainForm;
          OptionUIC               curveTypeUI;

          // spline parameters
          BaseUIC                 splineForm;
          BoolRadioBoxUIC         splineNaturalSlopeUI;
          RealEntryUIC            fixedSplineStartSlopeUI;
          RealEntryUIC            fixedSplineEndSlopeUI;
          RealEntryUIC            splineTensionUI;

          // polynomial parameters
          IntEntryUIC             polyOrderUI;

      public:
                          CurveUIC();


          void            StdInit(Widget     parent,
                                  ArgListC&  inArgs);


          bool            UIdataOK(bool& allOK);

          void            SetCurve(const CurveDescription& inData);
          void            GetCurve(CurveDescription& outData);

          void            SetSensitive();

      private:
          static  void    SetSensitiveCB(void* inObj);


  };

  class InterpPointsUIC  {
      public:
          OptionUIC               interpPointsOpUI;
          SplitFormUIC            mainForm;

          BaseUIC                 minMaxFrame;
          BoolRadioBoxUIC         useDataLimitsUI;
          RealEntryUIC            minUserLimitUI;
          RealEntryUIC            maxUserLimitUI;
          RealEntryUIC            logRelativestartUI;
          IntEntryUIC             numInterpPointsUI;

      public:
                          InterpPointsUIC();


          void            StdInit(Widget     parent,
                                  ArgListC&  inArgs);


          bool            UIdataOK(bool& allOK);

          void            SetInterp(const DSC_InterpPoints& inData);
          void            GetInterp(DSC_InterpPoints& outData);

          bool            AtInputX();

          void            SetSensitive();

          void            SetDefault();

      private:
          static  void    SetSensitiveCB(void* inObj);


  };


};

using namespace objDPOMenuUtilities;


#endif // !OBJDPOUTIL_H

