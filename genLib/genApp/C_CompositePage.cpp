///////////////////////////////////////////////////////////////////////////////////
//
// C_CompositePage.cpp
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              Page type for composite plots.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClass/U_Msg.h>

#include <genPlotClass/PD_Composite.h>
#include <genPlotClassPS/PC_PlatformPS.h>

#include <genPPD/PPD_Composite.h>
#include <genPPD/PPD_CompositeLayout.h>

#include <genApp/C_PlotFolderPage.h>

#include <genApp/C_CompositePage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CompositePageC::CompositePageC(const char* pID) : PlotPageC(pID)
{
}



bool CompositePageC::AcceptAsChild(MenuPageC& newChild) const
{
    // no nested composites
    if (IsSameType(&newChild))
        return false;

    return newChild.IsPlotPage() || newChild.IsSameType(PlotFolderPageAlloc::PageTypeName());
}

bool CompositePageC::AcceptAsParent(MenuPageC* newParent) const
{
    // only root allowed
    return (newParent == NULL);
}


void CompositePageC::ResetChildren()
{
    // reset anno objects on main and in folders
    PlotPageC::ResetChildren();

    // no look for changes in child plots
    PD_Composite* currPlot = static_cast<PD_Composite*>(assocPlot);

    // sanity
    int nplotPages = 0;
    for (int i = 0; i < childPages.Size(); i++)
        if (childPages[i]->IsPlotPage())
            nplotPages++;
    if (nplotPages != currPlot->subPlots.Size())
        GenAppInternalError("CompositePageC::ResetChildren()-1");

    // do check
    bool doReset = false;
    nplotPages = 0;
    for (int i = 0; i < childPages.Size(); i++)
        {
            if (childPages[i]->IsPlotPage())
                {
                    PlotDefC* newPlot = static_cast<PlotPageC*>(childPages[i])->GetPlot();
                    doReset = doReset || (newPlot != currPlot->subPlots[nplotPages]);
                    currPlot->subPlots[nplotPages] = newPlot;
                    nplotPages++;
                }
        }

    if (doReset)
        currPlot->DrawPlot();

}


// allocator code

const char* CompositePlotPageAlloc::pageTypeDesc = "PG_Composite_Plot";

MenuPageC*  CompositePlotPageAlloc::CreatePage() const
{
    CompositePageC* newPage = new CompositePageC("Composite");

    PPD_Composite* plot = new PPD_Composite();
    newPage->SetIcon(IDI_COMPOSITEPLOT);
    newPage->assocPlot = plot;
    newPage->assocWindow = static_cast<PC_PlatformPS*>(plot->platformData);

    newPage->AddObject(plot);
    newPage->AddObject(new PPD_CompositeLayout(*plot));

    return newPage;
}

const char* CompositePlotPageAlloc::GetPageTypeName() const
{
    return pageTypeDesc;
}

bool CompositePlotPageAlloc::AcceptAsChild(const char*  pageToCheck) const
{
    // no composite children
    if (PageNameMatch(pageToCheck))
        return false;

    // other plot pages are OK
    return AllocPageC::IsPlotPage(pageToCheck);
}

