///////////////////////////////////////////////////////////////////////////////////
//
// PS_Import.h
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
// DESCRIPTION: Platform specific but application independent code.
//              Base class for reading data from the Windows clipboard
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PS_IMPORT_H
#define PS_IMPORT_H

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_StringArray.h>

#include <genClassPS/PS_TxtClipboard.h>

//
//
// Programmer   : Radim Krampol,    DE&S
// Purpose      : Sequential reading of text file/ cliboard in text format
//
// Last Modification Date:
//
// Revision History:
//      09/20/2000 Initial version
//      RK code was refactored and moved up to PS_TxtClipboard
//

class PS_Import : public PS_TxtClipboard
{
private:
    enum    {maxImportChars = 16384};
    char    nextLine[maxImportChars];

public:
    PS_Import();
    virtual ~PS_Import();

    bool    OpenClipboardImport();

    bool    GetLineStrings(SC_StringArray& lineData);   // reads import line, parses into strings

    bool    GetLineDoubles(SC_DoubleArray& lineData);   // reads import line, parses into doubles
                                                        // returns false only if ReadLineImport is false
    void    CloseImport();

private:
    bool    ImportNextLine();

};


#endif // PS_IMPORT_H
