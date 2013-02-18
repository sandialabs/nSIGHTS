///////////////////////////////////////////////////////////////////////////////////
//
// LFO_XYDataArray.h
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

#ifndef LFO_XYDATAARRAY_H
#define LFO_XYDATAARRAY_H

#include <genClass/SC_RealFormat.h>

#include <genListClass/C_ListObj.h>

class LFO_XYDataArray : public ListObjC  {
  protected:
      FuncObjRef          xyDataObjRef;
      bool                listXYDataID;
      bool                listHeaders;
      SC_RealFormat       xyDataFormat;
      int                 xyDataColWidth;

  public:
                      LFO_XYDataArray(ListDefC& assLst);
                      LFO_XYDataArray(const LFO_XYDataArray& a, ListDefC& assLst);
                      ~LFO_XYDataArray();

      LFO_XYDataArray& operator= (const LFO_XYDataArray& a);

                      //  FuncObjC virtuals
      virtual void    DoStatusChk();

                      // C_Listing virtuals
      void            CreateListing();

  private:
      void            InitCommon();                             // common to normal and copy constructor
      void            LocalCopy(const LFO_XYDataArray& a);  // common to copy constructor and = operator
};

#endif // !LFO_XYDATAARRAY_H
