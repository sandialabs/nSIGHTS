///////////////////////////////////////////////////////////////////////////////////
//
// G_FileNames.h
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
// DESCRIPTION:Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef G_FILENAMES_H
#define G_FILENAMES_H

#include <genClass/T_SC_Array.h>
#include <genClass/C_FuncObj.h>
#include <genClass/SC_StringArray.h>


namespace appFileNameGlobals {

    struct FNUpdateRec {
    public:
        char*   fName;
        FuncObjC* assocFO;
    public:
        FNUpdateRec() : fName(0), assocFO(0) {};
        FNUpdateRec(char* inFN, FuncObjC* inFO) : fName(inFN), assocFO(inFO) {};
    };

    class FileNameArray : public T_SC_Array<FNUpdateRec> {

    public:
        FileNameArray();
        void               AddTo(char* newFname);
        void               AddTo(char* newFname,
                                 FuncObjC* newFO);
        void               DeleteFrom(const char* delFname);

        static void        GetCurrentPaths(SC_StringArray& currPaths);
        static bool        UpdatePaths(const char* existingPath,
                                       const char* newPaths,
                                       bool executeAfterUpdate);
        static void        ReturnPaths();

    private:
        FileNameArray(const FileNameArray& a) {};
        FileNameArray&   operator=(const FileNameArray& a) {return *this;}
    };

    extern FileNameArray appFileNameData;
    extern SC_StringArray oldFileNames;
    extern bool          writeLinux;
    extern bool          changePaths;
};  // end namespace


using namespace appFileNameGlobals;


#endif // G_FILENAMES_H

