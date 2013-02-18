///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadSATable.h
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

#ifndef DFO_READSATABLE_H
#define DFO_READSATABLE_H

#include <genClass/C_FuncObj.h>

#include <genClass/DO_Label.h>
#include <saClass/DO_SATable.h>
#include <saClass/IO_SATable.h>

class DFO_ReadSATable : public FuncObjC {
    private:
        // output
        DO_Label                fileNameDO;
        DO_SATable              saDataDO;

    protected:
        DC_SATable              saDataDC;

        //  parameters
        char                    saFname[stdFileStrLen];
        IO_SATable::SAFileFormat saFileFormat;

        char                    goldsimDepVar[DC_SATable::varNameLen];
        bool                    translateIndepVarNames;
        bool                    selectOnlyNewNames;
        bool                    sortByNames;
        char                    varNameFname[stdFileStrLen];

public:
                            DFO_ReadSATable();
                            DFO_ReadSATable(const DFO_ReadSATable& a);
                            ~DFO_ReadSATable(){};

        DFO_ReadSATable&        operator= (const DFO_ReadSATable& a);

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);

    private:
        void                InitCommon();                        // common to normal and copy constructor
        void                LocalCopy(const DFO_ReadSATable& a);  // common to copy constructor and = operator
        bool                TranslateVariableNames();

};

#endif // !DFO_READSATABLE_H

