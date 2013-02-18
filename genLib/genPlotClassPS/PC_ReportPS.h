/////////////////////////////////////////////////////////////////////////////////
//
// PC_ReportPS.h
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
//      implements reporting window management
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PC_REPORTPS_H
#define PC_REPORTPS_H

#include <genClassPS/PS_UIBase.h>
#include <genClassPS/PS_UIClass.h>
#include <genPlotClass/PC_Report.h>
#include <genPlotClassPS/PC_Platform2DPS.h>

using namespace genClassPS;

class PC_ReportPS : public PC_Report {
private:
    enum        {rightPosition = 20};

    PC_Platform2DPS*    m_Platform2D;
    BaseUIC             reportFrame;    // frame to hold report -- parent is report RC in
                                        // associated PC_PlatformPS
    // changed due to MS bug --
    //  BUG: The "this" Pointer Is Incorrect in Destructor of Base Class
    //  ID: Q168385
    //StatusUIC       reportData[maxReportRows];
    StatusUIC       reportDataX;
    StatusUIC       reportDataY;

public:
                    PC_ReportPS(PD_2D& assPlt);
    virtual         ~PC_ReportPS();

    virtual void    ReportInit(const char* maxTitle);
    void            ReportTitleSetup(const char* title);
    void            ReportLabelSetup(int rowNum, const char* rowLab);
    void            ReportUpdate(int rowNum, const char* rowVal);
};

#endif // !PC_REPORTPS_H

