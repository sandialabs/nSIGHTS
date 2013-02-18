///////////////////////////////////////////////////////////////////////////////////
//
// C_MPISupport.h
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
// DESCRIPTION: Platform independent and application independent base claseess and
//              support classes.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_MPISUPPORT_H
#define C_MPISUPPORT_H

#include <genClass/T_SC_Array.h>

class FuncObjC;                         // forward decl


class NodeFile {
  // class contains specs of files to be processed for MPI node changes
public:
  bool            nodeBasedFile;
  bool            fileIsUsed;  // default true, override if necessary
  bool            fileIsInput; // if true file must exist, default true
  char*           fileName;
  const char*     nodeDesc;

  static int      mpiNodeRank;
  static bool     mpiDebugRun;

public:
  NodeFile();
  NodeFile(const NodeFile& a);
  NodeFile(char* inFile, const char* inDesc, bool isInputF = true);

  ~NodeFile(){};

  NodeFile&       operator= (const NodeFile& a);  // copy operator

  bool            CheckNodeFile(FuncObjC& inObj, bool isDirectory = false);  // checks status
  void            UpdateNodeFile(bool singleCheckOnly, bool isDirectory = false);  // replaces /00 /000 with /XXX
  bool            CheckExists(FuncObjC& inObj, bool isDirectory = false); // call after update
private:
  void            LocalCopy(const NodeFile& a);


};

class NodeFileArray : public T_SC_Array<NodeFile> {

public:
  NodeFileArray();

  // calls for each member
  void            SetUsed(int index, bool inUse) {(*this)[index].fileIsUsed = inUse;}

  bool            CheckNodeFile(FuncObjC& inObj);  // checks status

  void            UpdateNodeFile(bool singleCheckOnly = false);  // replaces /00 /000 with /XXX

  bool            StdCheckAndUpdate(FuncObjC& inObj);

  void            CopyFrom(const NodeFileArray& a);
};



#endif
