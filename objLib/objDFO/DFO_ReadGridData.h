///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadGridData.h
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
//      Reads grid data from ASCII file in several formats
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_READGRIDDATA_H
#define DFO_READGRIDDATA_H

#include <genClass/C_FuncObj.h>

#include <genClass/DO_Label.h>
#include <objClass/DO_GridData.h>
#include <objClass/IO_GridData.h>

class DFO_ReadGridData : public FuncObjC {
    private:
        // output
        DO_Label            fileNameDO;
        DO_GridData         gridDataDO;

    protected:
        DC_GridData         gridData;

        //  parameters
        char                        gridDataFname[stdFileStrLen];
        IO_GridData::GridFileFormat gridFileFormat;

        bool                setXLog;
        bool                setYLog;
        char                setXID[DC_GridData::gridIDLen];
        char                setYID[DC_GridData::gridIDLen];

        int                 decimationModulus;

    public:
                            DFO_ReadGridData();
                            DFO_ReadGridData(const DFO_ReadGridData& a);
                            ~DFO_ReadGridData(){};

        DFO_ReadGridData&       operator= (const DFO_ReadGridData& a);

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);

    private:
        void                InitCommon();                        // common to normal and copy constructor
        void                LocalCopy(const DFO_ReadGridData& a);  // common to copy constructor and = operator
        void                CheckLogSet(DC_DataCoord& inCoord);

};
#endif //DFO_READGRIDDATA_H

