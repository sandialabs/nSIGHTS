///////////////////////////////////////////////////////////////////////////////////
//
// objPPOConfigFile.cpp
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

#include "objPPOConfigFile.h"

namespace objPPOConfigFile {


   void  ReadHorsetail(PSC_Horsetail& htData)
   {
       int readVer = ReadInt();

       ReadFuncObjRef(htData.colorMapDataObjRef);

       htData.plotWithColorMap = ReadBool();
       htData.fixedColorPen    = ReadInt();

       ReadSeriesSpec(htData.lineData);
       ReadLegendOverride(htData.legendData);
   }


   void  WriteHorsetail(const PSC_Horsetail& htData)
   {
       static const int htVer = 0;
       WriteInt(htVer);

       WriteFuncObjRef(htData.colorMapDataObjRef);

       WriteBool(htData.plotWithColorMap);
       WriteInt(htData.fixedColorPen);
       WriteLine();

       WriteSeriesSpec(htData.lineData);
       WriteLegendOverride(htData.legendData);
   }

   void  Read3DGridData(PSC_3DGridData& gData)
   {
       int readVer = ReadInt();

       gData.zvalueSource = PSC_3DGridData::ZValueSource(ReadInt());

       ReadFuncObjRef(gData.otherGridZObjRef);
       ReadRealMSSpec(gData.fixedZValueMS);
       Read3DCoordMap(gData.coordMap);
   }


   void  Write3DGridData(const PSC_3DGridData& gData)
   {
       static const int gVer = 0;
       WriteInt(gVer);

       WriteInt(int(gData.zvalueSource));

       WriteFuncObjRef(gData.otherGridZObjRef);
       WriteRealMSSpec(gData.fixedZValueMS);
       Write3DCoordMap(gData.coordMap);

   }

   void  Read3DCubeData(PSC_3DCubeData& cData)
   {
       int readVer = ReadInt();

       ReadFuncObjRef(cData.cubeIndexesObjRef);
       Read3DCoordMap(cData.coordMap);
   }


   void  Write3DCubeData(const PSC_3DCubeData& cData)
   {
       static const int cVer = 0;
       WriteInt(cVer);

       WriteFuncObjRef(cData.cubeIndexesObjRef);
       Write3DCoordMap(cData.coordMap);

   }


   void  Read3DCoordMap(PSC_3DCoordMap& cmData)
   {
       int readVer = ReadInt();

       cmData.xMap  = PSC_3DCoordMap::CoordMapType(ReadInt());
       cmData.yMap  = PSC_3DCoordMap::CoordMapType(ReadInt());
       cmData.zMap  = PSC_3DCoordMap::CoordMapType(ReadInt());
   }



   void  Write3DCoordMap(const PSC_3DCoordMap& cmData)
   {
       static const int cmVer = 0;
       WriteInt(cmVer);

       WriteInt(int(cmData.xMap));
       WriteInt(int(cmData.yMap));
       WriteInt(int(cmData.zMap));
       WriteLine();

   }


};

