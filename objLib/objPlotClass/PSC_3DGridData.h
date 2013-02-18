///////////////////////////////////////////////////////////////////////////////////
//
// PSC_3DGridData.h
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PSC_3DGRIDDATA_H
#define PSC_3DGRIDDATA_H

#include <genClass/C_Common.h>
#include <genClass/SC_CoordArray.h>

#include <genDataClass/DSC_RealMSSpecBase.h>
#include <genPlotClass/C_PlotObj.h>

#include <objClass/DC_GridData.h>
#include <objPlotClass/PSC_3DCoordMap.h>

// mix in with mult inheritance
class PSC_3DGridData {
  public:
      enum ZValueSource       {   zvs_Same,         // same as data source
                                  zvs_OtherGrid,    //  another grid
                                  zvs_FixedVal};    //  fixed value

      ZValueSource            zvalueSource;         // set source
      FuncObjRef              otherGridZObjRef;      // for zvs_OtherGrid
      DSC_RealMSSpecBase      fixedZValueMS;         // for zvs_FixedVal

      PSC_3DCoordMap          coordMap;

  protected:
      const DC_GridData*      otherGridData;
      double                  fixedZ;
      bool                    isOther;
      bool                    isFixed;

  private:
      bool*               otherGridCheckRef;
      bool*               fixedZCheckRef;

  public:
                          PSC_3DGridData();
                          PSC_3DGridData(const PSC_3DGridData& a);
                          ~PSC_3DGridData();

      PSC_3DGridData&     operator= (const PSC_3DGridData& a);

      void                StatusChkSetup(PlotObjC&  inPO);  // sets flags prior to status check call

      void                DoStatusChk(PlotObjC&    inPO,
                                      const DC_GridData& currGridData);


  protected:
      void                InitCommon(FuncObjC& inFO);     // common to normal and copy constructor in derived class

      inline void         MapCoords(Coord3D& inCoord) {coordMap.MapCoords(inCoord);}
      inline void         MapCoords(SC_CoordArray& inCoords) {coordMap.MapCoords(inCoords);}

      double              GetZVal(int               indx,
                                  const DC_GridData& currGridData);


  private:
      void                LocalCopy(const PSC_3DGridData& a);  // common to copy constructor and = operator

};
#endif // !PSC_3DGRIDDATA_H

