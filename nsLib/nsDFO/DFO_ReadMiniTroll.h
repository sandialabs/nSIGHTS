///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadMiniTroll.h
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
//      Reads results from MiniTroll formatted file in raw text or post-processed
//  CSV format.  Calculates time as elapsed calendar time
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_READMINITROLL_H
#define DFO_READMINITROLL_H

#include <genClass/C_FuncObj.h>

#include <genClass/DO_Label.h>
#include <genClass/DO_TableData.h>
#include <genClass/DC_DateTime.h>
#include <genClass/DO_DateTimeSpec.h>
#include <genClass/DO_XYData.h>
#include <objClass/DO_XYDataArray.h>

class DFO_ReadMiniTroll : public FuncObjC {

    friend class MiniTrollFile;
    public:
        enum                        FileType {mtRawText, mtCSV, dasCSV, mtRawText07, wbMOSDAQ, mtRawText08};

    protected:
        DC_DateTimeSpec             timeSpecDC;

        FileType                    fileType;
        bool                        readAllColumns;
        int                         yColumn;

        bool                        dasReadID;
        bool                        dasReadKey;

        //  parameters
        char                        mtFname[stdFileStrLen];

        // output - protected for status
        DC_TableData                tableData;

        double                      minimumDeltaT;

    private:
        // output
        DO_Label            fileNameDO;
        DO_TableData        tableDO;
        DO_XYDataArray      xyDataArrayDO;
        DC_XYDataArray      xyTableData;
        DO_XYData           singleXYDO;
        DO_DateTimeSpec     timeSpecDO;

        // Westbay specific
        DC_TableData        westBayPressureTimeDC;
        DO_TableData        westBayPressureTimeDO;

        DC_TableData        westBayPressureDepthDC;
        DO_TableData        westBayPressureDepthDO;
        SC_DoubleArray      westbayDepths;

    public:
                            DFO_ReadMiniTroll();
                            DFO_ReadMiniTroll(const DFO_ReadMiniTroll& a);
                            ~DFO_ReadMiniTroll(){};

        DFO_ReadMiniTroll&  operator= (const DFO_ReadMiniTroll& a);

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);

    private:
        void                InitCommon();                           // common to normal and copy constructor
        void                LocalCopy(const DFO_ReadMiniTroll& a);  // common to copy constructor and = operator
        bool                ReadMiniTroll();                        // in DFO_ReadMiniTrollCF.cpp
        void                SetWestbayTables();

};

#endif //DFO_READMINITROLL_H

