///////////////////////////////////////////////////////////////////////////////////
//
// objDPOConfigFile.cpp
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

#include "objDPOConfigFile.h"

namespace objDPOConfigFile {


   void  ReadTableColSel(DSC_TableColSel& colSel,
                         bool            oldVer)
   {
       int readVer = 0;
       if (!oldVer)
           readVer = ReadInt();

       ReadFuncObjRef(colSel.tableInputObjRef);
       colSel.selectedCol = ReadInt();
   }


   void  WriteTableColSel(const DSC_TableColSel& colSel)
   {
       static const int tcsVer = 0;
       WriteInt(tcsVer);

       WriteFuncObjRef(colSel.tableInputObjRef);
       WriteInt(colSel.selectedCol);
       WriteLine();
   }



   void  ReadCurveSpec(CurveDescription& inCurve,
                       bool              oldVer)
   {
       int readVer = 0;
       if (!oldVer)
           readVer = ReadInt();

       inCurve.curveType               = Curve::CurveType(ReadInt());
       inCurve.splineNaturalSlope      = ReadBool();
       inCurve.fixedSplineStartSlope   = ReadDouble();
       inCurve.fixedSplineEndSlope     = ReadDouble();
       inCurve.splineTension           = ReadDouble();
       inCurve.polyOrder               = ReadInt();

   }

   void  WriteCurveSpec(const CurveDescription& outCurve)
   {
       static const int crvVer = 0;
       WriteInt(crvVer);

       WriteInt(int(outCurve.curveType));
       WriteBool(outCurve.splineNaturalSlope);
       WriteDouble(outCurve.fixedSplineStartSlope);
       WriteDouble(outCurve.fixedSplineEndSlope);
       WriteDouble(outCurve.splineTension);
       WriteLine();

       WriteInt(outCurve.polyOrder);
       WriteLine();
   }

   void ReadInterpPoints(DSC_InterpPoints& inInterp)
   {
       int readVer = ReadInt();
       inInterp.interpPointsOp   = DSC_InterpPoints::InterpPointsOp(ReadInt());
       inInterp.useDataLimits    = ReadBool();
       inInterp.minUserLimit     = ReadDouble();
       inInterp.maxUserLimit     = ReadDouble();
       inInterp.logRelativestart = ReadDouble();
       inInterp.numInterpPoints  = ReadInt();
   }

   void WriteInterpPoints(const DSC_InterpPoints& outInterp)
   {
       static const int intVer = 0;
       WriteInt(intVer);

       WriteInt(int(outInterp.interpPointsOp));
       WriteBool(outInterp.useDataLimits);
       WriteDouble(outInterp.minUserLimit);
       WriteDouble(outInterp.maxUserLimit);
       WriteDouble(outInterp.logRelativestart);
       WriteInt(outInterp.numInterpPoints);
       WriteLine();
   }

};

