///////////////////////////////////////////////////////////////////////////////////
//
// PPD_2DXY.h
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
//      derived from PPD_2D. Adds components for 2DXY plots
//
//      derived from PPD_Base.  Adds components for all 2D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPD_2DXY_H
#define PPD_2DXY_H

#include <genApp/C_MenuObj.h>

#include "PPD_2D.h"
#include <genPlotClassPS/PD_2DXYPS.h>


class PPD_2DXYPS;

class PPD_2DXY : public PPD_2D, public PD_2DXYPS {
    private:
        static const char*  objLongDesc;
        static const char*  allocID;

    public:
                        PPD_2DXY();
                        PPD_2DXY(const PPD_2DXY& a);
                        ~PPD_2DXY();

        PPD_2DXY& operator= (const PPD_2DXY& a);

        void            PropOpenMenu();
        void            PropUpdate();
        void            PropApply();

        void            WriteToFile();
        bool            ReadFromFile();

    private:
        PPD_2DXYPS&     MenuRef();

};


#endif // !PPD_2DXY_H

