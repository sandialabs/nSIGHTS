///////////////////////////////////////////////////////////////////////////////////
//
// PC_Report.h
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
//      adds class to support cursor reporting on 2D plot objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PC_REPORT_H
#define PC_REPORT_H

class   PD_2D;

class   PC_Report {
    protected:
        enum                {maxReportRows = 2};    //  report can contain 2 rows of labels
        PD_2D*              assocPlot;            //  ref to plot using report

    public:
        bool                showReport;         // if true then report is ON
        bool                reportInitialized;  // set true if init performed

    public:
                            PC_Report(PD_2D&  assPlt);
        virtual             ~PC_Report();

        virtual void        ReportInit(const char* maxTitle) = 0;
        //  initialization in related object instantiation
        //  maxTitle -- title with greatest width
        void                ReportInit();  // default  is ReportInit("Data:");}                     // default is 2 rows, 4 chars


        virtual void        ReportTitleSetup(const char* title) = 0;
        //  - sets ID - also checks showReport to manage/unmanage

        //  each report row consists of a label on right and data on left
        virtual void        ReportLabelSetup(int rowNum, const char* rowLab) = 0;
        virtual void        ReportUpdate(int rowNum, const char* rowVal) = 0;
};


#endif // !PC_REPORT_H

