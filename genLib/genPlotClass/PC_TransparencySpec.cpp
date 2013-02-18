///////////////////////////////////////////////////////////////////////////////////
//
// PC_TransparencySpec.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotObj.h>
#include <genPlotClass/C_AnnoObj.h>
#include <genPlotClass/PD_3D.h>
#include <genPlotClass/PC_TransparencySpec.h>

PC_TransparencySpec::TransparencyGroup::TransparencyGroup()
{
    groupIsTransparent = false;
    transparencyIsConstant  = true;
    constantTransFactor = 0.5;
    startTransFactor    = 0.0;
    endTransFactor      = 1.0;
}

PC_TransparencySpec::TransparencyGroup::TransparencyGroup(const TransparencyGroup& a)
{
    LocalCopy(a);
}

PC_TransparencySpec::TransparencyGroup&  PC_TransparencySpec::TransparencyGroup::operator= (const TransparencyGroup& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void PC_TransparencySpec::TransparencyGroup::LocalCopy(const TransparencyGroup& a)
{
    groupIsTransparent = a.groupIsTransparent;
    transparencyIsConstant = a.transparencyIsConstant;
    constantTransFactor = a.constantTransFactor;
    startTransFactor = a.startTransFactor;
    endTransFactor = a.endTransFactor;
}


double  PC_TransparencySpec::TransparencyGroup::GetTransFactor(int index, int maxIndex) const
{
    if (!groupIsTransparent)
        return 1.0;

    if (transparencyIsConstant)
        return constantTransFactor;

    return startTransFactor + (double(index) / double(maxIndex)) * (endTransFactor - startTransFactor);
}


PC_TransparencySpec :: PC_TransparencySpec(PD_3D& inDef)
: plotDef(inDef)
{
    SetDefault();
}

PC_TransparencySpec :: PC_TransparencySpec(const PC_TransparencySpec& a)
: plotDef(a.plotDef)
{
    // private -not used
}

PC_TransparencySpec& PC_TransparencySpec::operator= (const PC_TransparencySpec& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void PC_TransparencySpec::LocalCopy(const PC_TransparencySpec& a)
{
    for (int i = 0; i < maxTransparencyGroup; i++)
        transparencyGroups[i] = a.transparencyGroups[i];

    useTransparency = a.useTransparency;
    ntransSteps     = a.ntransSteps;
}


void PC_TransparencySpec::SetDefault()
{
    transFactor.Alloc(maxTransparencyGroup);
    for (int i = 0; i < maxTransparencyGroup; i++)
        transparencyGroups[i] = TransparencyGroup();

    useTransparency = false;
    ntransSteps     = 100;
}

/*
ObjPtrList& currList = objectSortArray[k];
        // sort ascending distance
        for (int i = 0; i < currList.UpperBound(); i++)
            for (int j = i + 1; j < currList.Size(); j++)
                if (currList[i]->objDist > currList[j]->objDist)
                {
                    ObjectTransDesc* tmp = currList[i];
                    currList[i] = currList[j];
                    currList[j] = tmp;
                }
*/

void PC_TransparencySpec::QBSort(ObjPtrList& a, int lo, int hi)
{
    for (int j= hi; j > lo; j--)
    {
        for (int i=lo; i < j; i++)
        {
            if (a[i]->objDist > a[i+1]->objDist)
            {
                ObjectTransDesc* t = a[i];
                a[i] = a[i+1];
                a[i+1] = t;
            }
        }
    }
}

void PC_TransparencySpec::QSort(ObjPtrList& a, int lo0, int hi0)
{
    int lo = lo0;
    int hi = hi0;

    // Bubble sort if the number of elements is less than 6
    if ((hi-lo) <= 6)
    {
        QBSort(a, lo, hi);
        return;
    }

    // Pick a pivot and move it out of the way /

    ObjectTransDesc* pivot = a[(lo + hi) / 2];
    a[(lo + hi) / 2] = a[hi];
    a[hi] = pivot;

    const double& pivotDist = pivot->objDist;
    while( lo < hi )
    {
        // Search forward from a[lo] until an element is found that * is greater than the pivot or lo >= hi
        while ((lo < hi) && (a[lo]->objDist <= pivotDist))
        {
            lo++;
        }

        // Search backward from a[hi] until element is found that * is less than the pivot, or hi <= lo
        while ((lo < hi) && (pivotDist <= a[hi]->objDist))
        {
            hi--;
        }
        // Swap elements a[lo] and a[hi]
        if( lo < hi )
        {
            ObjectTransDesc* T = a[lo];
            a[lo] = a[hi];
            a[hi] = T;
        }
    }

    // Put the median in the "center" of the list
    a[hi0] = a[hi];
    a[hi] = pivot;
    // Recursive calls, elements a[lo0] to a[lo-1] are less than or * equal to pivot, elements a[hi+1] to a[hi0] are greater than * pivot.
    QSort(a, lo0, lo-1);
    QSort(a, hi+1, hi0);
}




bool PC_TransparencySpec::TransparencySetup()
{
    int nobj = plotDef.plotObjList.Size();
    objectIsTransparent.AllocAndFill(nobj, false);

    int nanno = plotDef.annoObjList.Size();
    annoIsTransparent.AllocAndFill(nanno, false);
    if (!useTransparency)
        return false;

    // get initial counts & objects
    int tranCount = 0;
    for (int i = 0; i < nobj; i++)
    {
        const PlotObjC& nextObj = plotDef.plotObjList.GetRef(i);

        if (nextObj.doPlot && nextObj.StatusOK() && nextObj.SupportsTransparency())
        {
            int objCount = nextObj.GetnTransObjects();
            if ((objCount > 0) &&
            (transparencyGroups[nextObj.GetTransGroup()].groupIsTransparent))
            {
                objectIsTransparent[i] = true;
                tranCount += objCount;
            }
        }
    }

    int annoTranCount = 0;
    for (int i = 0; i < nanno; i++)
    {
        const AnnoObjC& nextObj = plotDef.annoObjList.GetRef(i);

        if (nextObj.doPlot && nextObj.StatusOK() && nextObj.SupportsTransparency())
        {
            if (transparencyGroups[nextObj.GetTransGroup()].groupIsTransparent)
            {
                annoIsTransparent[i] = true;
                annoTranCount++;
            }
        }
    }

    annoOnly = (tranCount == 0);
    if (annoOnly)
        return (annoTranCount > 0);

    PC_View currView = plotDef.GetCurrentView();

    Coord3D zeroOffset(0.0, 0.0, 0.0);
    Coord3D minCoord = plotDef.GetNormalizedCoord(currView.viewLimits.minLim, zeroOffset);
    Coord3D maxCoord = plotDef.GetNormalizedCoord(currView.viewLimits.maxLim, zeroOffset);;

    Line3D tempLine(minCoord, maxCoord);
    double maxLength = tempLine.Length() * 2.0;
    Coord3D lookAtPoint = tempLine.PointOnLine(0.5);

    double elevAngle = Radians(currView.elevation);
    double rotAngle = Radians(currView.azimuth);

    double dz = maxLength * sin(elevAngle);
    double xylen = maxLength * cos(elevAngle);

    double dx = - xylen * sin(rotAngle);
    double dy = - xylen * cos(rotAngle);

    eyeCoord = lookAtPoint;
    eyeCoord.cX += dx;
    eyeCoord.cY += dy;
    eyeCoord.cZ += dz;

    tranObjectList.Alloc(tranCount);
    tranCount = 0;
    Coord3D objCoord;
    double minDist, maxDist;
    for (int i = 0; i < nobj; i++)
        if (objectIsTransparent[i])
        {
            PlotObjC& nextObj = plotDef.plotObjList.GetRef(i);
            nextObj.SetupForGetCoord();
            int objCount = nextObj.GetnTransObjects();
            for (int j = 0; j < objCount; j++)
            {
                ObjectTransDesc& nextDesc = tranObjectList[tranCount];
                objCoord = nextObj.GetTransObjectCoord(j);
                if (objCoord.CoordIsNull())
                    continue;

                objCoord = plotDef.GetNormalizedCoord(objCoord, nextObj.offsetCoord);
                if (objCoord.CoordIsNull())
                    continue;

                double nextDist = objCoord.Distance(eyeCoord);
                if (tranCount == 0)
                {
                    minDist = nextDist;
                    maxDist = nextDist;
                }
                else
                {
                    if (nextDist < minDist)
                        minDist = nextDist;
                    else if (nextDist > maxDist)
                        maxDist = nextDist;
                }

                nextDesc.objDist = nextDist;
                nextDesc.objRef = &nextObj;
                nextDesc.objIndex = j;
                nextDesc.objGroup = nextObj.GetTransGroup();
                tranCount++;
            }
        }

    if (tranCount == 0)
    {
        annoOnly = (annoTranCount > 0);
        objectIsTransparent.FillToAlloc(false);
        return annoOnly;
    }

    int ndistanceGroups = tranCount / 500;

    // now place in buckets for sorting
    if (ndistanceGroups < 2)
    {
        ndistanceGroups = 1;
        // easy just use 1 bucket
        objectSortArray.AllocAndSetSize(1);
        objectSortArray[0].Alloc(tranCount);
        for (int i = 0; i < tranCount; i++)
            objectSortArray[0] += &(tranObjectList[i]);
    }
    else
    {
        if (ndistanceGroups > 5000)
            ndistanceGroups = 5000;
        if (ndistanceGroups < 100)
            ndistanceGroups = 100;

        objectSortArray.AllocAndSetSize(ndistanceGroups);
        for (int i = 0; i < ndistanceGroups; i++)
            objectSortArray[i].SetResizable(tranCount / ndistanceGroups * 5);

        double deltaDist = (maxDist - minDist) / double(ndistanceGroups);
        for (int i = 0; i < tranCount; i++)
        {
            ObjectTransDesc& nextDesc = tranObjectList[i];
            int currGroup = int((nextDesc.objDist - minDist) / deltaDist);
            if (currGroup >= ndistanceGroups)
                currGroup = ndistanceGroups - 1;
            if (currGroup < 0)
                currGroup = 0;
            objectSortArray[currGroup] += &nextDesc;
        }
    }

    // now sort each
    for (int k = 0; k < objectSortArray.Size(); k++)
    {
        ObjPtrList& currList = objectSortArray[k];
        QSort(currList, 0, currList.UpperBound());
    }

    return true;
}

void PC_TransparencySpec::DrawTransparentObjects()
{
    for (int i = 0; i < maxTransparencyGroup; i++)
        transFactor[i] = transparencyGroups[i].GetTransFactor(currTransStep,ntransSteps);

    if (annoOnly)
        return;

    // clear flag
    for (int i = 0; i < plotDef.plotObjList.Size(); i++)
        plotDef.plotObjList.GetRef(i).transPlotted = false;

    // draw from the back forward
    for (int i = objectSortArray.UpperBound(); i >= 0; i--)
    {
        ObjPtrList& currList = objectSortArray[i];
        for (int j = currList.UpperBound(); j >= 0; j--)
        {
            ObjectTransDesc& nextDesc = currList.GetRef(j);
            double nextTransFactor = transFactor[nextDesc.objGroup];
            if (nextTransFactor > 0.01)
            {
                plotDef.SetTransparencyFactor(nextTransFactor);
                nextDesc.objRef->DrawTransObject(nextDesc.objIndex);
            }
        }
    }
}

void PC_TransparencySpec::DrawTransparentAnno()
{
    for (int i = 0; i < plotDef.annoObjList.Size(); i++)
    {
        if (annoIsTransparent[i])
        {
            AnnoObjC& nextObj = plotDef.annoObjList.GetRef(i);
            double nextTransFactor = transFactor[nextObj.transGroup];
            if (nextTransFactor > 0.01)
            {
                plotDef.SetTransparencyFactor(nextTransFactor);
                nextObj.DrawPlotObject();
            }
        }
    }
}


