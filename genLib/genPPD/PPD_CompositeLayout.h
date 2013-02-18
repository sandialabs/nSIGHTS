///////////////////////////////////////////////////////////////////////////////////
//
// PPD_CompositeLayout.h
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
//      describes layout of sub-plots on composite plot.
//
//      adds components for composite plots.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPD_COMPOSITELAYOUT_H
#define PPD_COMPOSITELAYOUT_H

#include <genApp/C_MenuObj.h>

#include <genPlotClass/PD_Composite.h>

class PPD_CompositeLayoutPS;

class PPD_CompositeLayout : public MenuObjC {

    private:
        PD_Composite&       compPD;

        //  no FO so needs to supply own ID and Type
        char                objID[FuncObjC::objectIDLen];
        static const char*  objType;
        static const char*  objLongDesc;
        static const char*  allocID;

    public:
                    PPD_CompositeLayout(PD_Composite& inPD);
                    ~PPD_CompositeLayout(){};

        void        PropOpenMenu();
        void        PropUpdate();
        void        PropApply();
        void        PropClear();

        void        WriteToFile();
        bool        ReadFromFile();

    private:
        PPD_CompositeLayoutPS&  MenuRef();

};


#endif // !PPD_COMPOSITELAYOUT_H

