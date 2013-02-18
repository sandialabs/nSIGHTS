///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableToGrid.h
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

#ifndef DFO_TABLETOGRID_H
#define DFO_TABLETOGRID_H

#include <genClass/C_FuncObj.h>

#include <genClass/DO_TableData.h>
#include <objClass/DC_GridData.h>
#include <objClass/DO_GridData.h>

#include <objDataClass/DSC_TableColSel.h>

class DFO_TableToGrid : public FuncObjC, public DSC_TableColSel  {
    public:
        enum                        ConvertFormat {cfGrid, cfXYZList, cfOther};
    protected:
        ConvertFormat               convertFormat;

        // grid conversion
        bool                        columnValueIsHeader;
        bool                        useColumnIndex;
        bool                        gridXIsColumns;

        // XYZ List conversion - x sel is default table sel
        int                         ycolumnIndex;
        int                         dataColumnIndex;

        // all
        char                        xdataID[DC_DataCoord::dataIDLen];
        char                        ydataID[DC_DataCoord::dataIDLen];
        int                         tableRowModulus;
        int                         tableColModulus;
        bool                        includeLastRow;  // only if skipped by modulus setting
        bool                        includeLastCol;

    protected:
        //  output data
        DC_GridData                 outputGrid;

    private:
        //  input/output data
        DO_GridData                 outputGridDO;
        const DC_TableData*         inputTableDC;
        const SC_DoubleArray*       yDataCol;
        const SC_DoubleArray*       xDataCol;
        const SC_DoubleArray*       gridDataCol;

        int                         ncol;
        int                         nrow;
        int                         navailCol;
        int                         navailRow;

    public:
                        DFO_TableToGrid();
                        DFO_TableToGrid(const DFO_TableToGrid& a);
                        ~DFO_TableToGrid();

        DFO_TableToGrid& operator= (const DFO_TableToGrid& a);

        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

        private:
            void            InitCommon();                        // common to normal and copy constructor
            void            LocalCopy(const DFO_TableToGrid& a);  // common to copy constructor and = operator

            void            GridConvert();
            void            XYZListConvert();

            bool            DataCoordOK(const DC_DataCoord& inCoord);

            bool            ColOK(int colIndex);
            bool            RowOK(int rowIndex);

            void            AddNewEntry(      SC_DoubleArray& inGrid,
                                        const double&         newVal);

            void            ProcessModulus(SC_DoubleArray& inGrid,
                                           int             modulus,
                                           bool            keepLast);

            int             GetEntryIndex(const SC_DoubleArray& inGrid,
                                          const double&         newVal);

    };

#endif //DFO_TABLETOGRID_H

