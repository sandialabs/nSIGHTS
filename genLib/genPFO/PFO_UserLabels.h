///////////////////////////////////////////////////////////////////////////////////
//
// PFO_UserLabels.h
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
//      basic user entered text for labels.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_USERLABELS_H
#define PFO_USERLABELS_H

#include <genPlotClass/C_AnnoObj.h>

class PFO_UserLabels : public AnnoObjC {  // doesnt use legendTitle
    public:

        class UserLabel {
            public:
                enum                    {userLabelLen = 160};

                bool                    doLabel;
                char                    userLabel[userLabelLen];
                HorizontalTextAlignment labAlign;

            public:
                                UserLabel() {doLabel = false; userLabel[0] = '\0'; labAlign = hta_Left;}
                                UserLabel(const UserLabel& a);
                UserLabel&      operator= (const UserLabel& a);
            private:
                void            LocalCopy(const UserLabel& a);
        };

        enum        {maxUserLabels = 10};

        //  input objects
        UserLabel           labelData[maxUserLabels];


    public:
                        PFO_UserLabels(PlotDefC& assPlt);
                        PFO_UserLabels(const PFO_UserLabels& a, PlotDefC& assPlt);
                        ~PFO_UserLabels();

        PFO_UserLabels& operator= (const PFO_UserLabels& a);

        void            SetDefault();

                        //  FuncObjC virtuals
        void            DoStatusChk();
                        //  PlotObjC  draw virtuals in derived PFO_UserLabelsGL

    protected:
        void            FullCopy(const PFO_UserLabels& a);  // called by copy in derived class

    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_UserLabels& a);  // common to copy constructor and = operator
};

#endif // !PFO_USERLABELS_H

