///////////////////////////////////////////////////////////////////////////////////
//
// PC_3DAxesLabel.h
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
//      Platform independent base classes for plot definitions and plot functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PC_3DAXESLABEL_H
#define PC_3DAXESLABEL_H

#include <genPlotClass/PSC_Font.h>

class PC_3DAxesLabel {

    public:
            bool        plotAxesLabel;
            bool        plotIncrementLabel;
            bool        autoPositionLabels;

                        //  labels
            enum        {maxLabelLen = 80};
            char        xaxesLabel[maxLabelLen];
            char        yaxesLabel[maxLabelLen];
            char        zaxesLabel[maxLabelLen];

                        //  fonts
            PSC_Font     labelFont;
            PSC_Font     incrementFont;

                        //  XY label format
            bool        xyLabIsVert;
            bool        xLabYPosIsMax;
            bool        yLabXPosIsMax;

                        //  Z label format
            enum ZLabOrientation {zloXZn, zloXZr, zloYZn, zloYZr};
            ZLabOrientation zLabOrientation;
            bool        zLabYPosIsMax;
            bool        zLabXPosIsMax;

    public:
                            PC_3DAxesLabel();
                            PC_3DAxesLabel(const PC_3DAxesLabel& a);
            PC_3DAxesLabel& operator= (const PC_3DAxesLabel& a);

            void            SetDefault();
    private:
            void            LocalCopy(const PC_3DAxesLabel& a);

};


#endif // !PC_3DAXESLABEL_H

