///////////////////////////////////////////////////////////////////////////////////
//
// PPD_Base.h
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
//      components common to all plots (window size)
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPD_BASE_H
#define PPD_BASE_H

#include <genApp/C_MenuObj.h>

class PlotDefC;
class PPD_BasePS;

class PPD_Base : public MenuObjC {
    public:
        PlotDefC&   basePD;

        friend class PPD_BasePS;

    private:
        bool        resizeCBset;        //  CB is used to update menu on resize

    protected:                      // used by PPD_2D and 3D -- didnt start until maj ver 4
        int         basePDReadMaj;
        int         basePDReadMin;

    public:
                    PPD_Base(PlotDefC* bPD, const char* longDesc);
                    ~PPD_Base(){};

        void        PropOpenMenu();
        void        PropCloseMenu();
        void        PropApply();

        void        WriteToFile();
        bool        ReadFromFile();

    private:
        void        CheckResizeCB();
        static void ResizeCB(void* inObj);

};


#endif // !PPD_BASE_H

