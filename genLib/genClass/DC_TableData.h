///////////////////////////////////////////////////////////////////////////////////
//
// DC_TableData.h
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
// DESCRIPTION: Platform independent and application independent base classes and
//              support classes.
//              Data Classes (DC) that define basic types of data used within the
//              framework: Tabular Data.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_TABLEDATA_H
#define DC_TABLEDATA_H


#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_StringArray.h>


class DC_TableData {
public:
  enum            {tableIDLen = 80};
  char            tableID[tableIDLen];

  SC_DoubleMatrix dataTable;
  SC_StringArray  columnDesc;
  SC_StringArray  rowDesc;

public:
  DC_TableData();
  DC_TableData(const DC_TableData& a);
  ~DC_TableData();

  DC_TableData&  operator=(const DC_TableData& a);


  bool        Alloc(int nCol, int nRow, int nChar = 20);  // allocates
  bool        Alloc(const DC_TableData& a);               // allocs to match

  void        SetSize(int nCol, int nRow);
  void        SetSizeToAlloc();

// allocates resizable dataTable
  bool        ResizableAlloc(int initialColAlloc, int initialRowAlloc,  int nChar = 20);

  void        SetTitle(int titleCol, const char* titleText);
  const char* GetTitle(int titleCol) const {return columnDesc[titleCol];}

  int         GetNCols() const {return columnDesc.Size();}
  int         GetNRows() const;
  int                     GetStringLen() const;

  void        ClearData();    // sets all columns to empty

  void        ClearAll();    // deallocs

  // description seta
  void        SetColumnDesc(const char* baseDesc, int offset = 0);
  void        SetRowDesc(const char* baseDesc, int offset = 0);

  // basic ops
  void        InsertRow(int beforeRow);
  void        DeleteRow(int delRow);
  void        InsertCol(int beforeCol);
  void        DeleteCol(int delCol);

  // ID related
  void        SetID(const char id[]);
  void        GetID(char id[], int  maxIDlen);
  const char* GetID() const {return tableID;}

  const SC_DoubleArray&   GetColData(int colIndex) const;  // ref
  SC_DoubleArray&         GetColData(int colIndex);  // ref
  void        GetColData(int colIndex,
                         SC_DoubleArray& colData) const; // copy
  void        GetRowData(int rowIndex,
                         SC_DoubleArray& rowData) const;  // copy

private:
  void        DeAlloc();      // deallocation used by destructor and alloc
};

#endif // DC_TABLEDATA_H

