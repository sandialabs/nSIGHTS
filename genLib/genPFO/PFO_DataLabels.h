///////////////////////////////////////////////////////////////////////////////////
//
// PFO_DataLabels.h
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
//      labels that are linked to values produced by other objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_DATALABELS_H
#define PFO_DATALABELS_H

#include <genClass/SC_RealFormat.h>
#include <genClass/DO_Label.h>

#include <genPlotClass/C_AnnoObj.h>

class PFO_DataLabels : public AnnoObjC {
    public:

        class DataLabel {
            public:
                FuncObjRef              labelObjRef;
                bool                    doLabel;
                HorizontalTextAlignment labAlign;

                bool                    overrideFormat;  // only for DO_Real derived
                SC_RealFormat           labelFormat;

                bool                    overrideMainLabel;
                char                    mainLabel[DO_Label::maxLabelLen];

                char                    unitsLabel[DO_Label::maxLabelLen];

                DO_Label*               dataSource;

            public:
                                DataLabel();
                                DataLabel(const DataLabel& a);
                DataLabel&      operator= (const DataLabel& a);
            private:
                void            LocalCopy(const DataLabel& a);
        };

        enum            {maxDataLabels = 16};  // was 6, increased to 16 in FB333

        //  input objects
        DataLabel       labelData[maxDataLabels];
        int             minDataFieldWidth;


    public:
                        PFO_DataLabels(PlotDefC& assPlt);
                        PFO_DataLabels(const PFO_DataLabels& a, PlotDefC& assPlt);
                        ~PFO_DataLabels();

        PFO_DataLabels& operator= (const PFO_DataLabels& a);

        void            SetDefault();

                        //  FuncObjC virtuals
        void            DoStatusChk();
                        //  PlotObjC  draw virtuals in derived PFO_DataLabelsGL

    protected:
        void            FullCopy(const PFO_DataLabels& a);  // called by copy in derived class

    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_DataLabels& a);  // common to copy constructor and = operator
};

#endif // !PFO_DATALABELS_H

