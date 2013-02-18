///////////////////////////////////////////////////////////////////////////////////
//
// PFO_LabelAxes.h
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
//      Basic plotting objects used by all applications, primarily concerned with plot annotation:
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_LABELAXES_H
#define PFO_LABELAXES_H

#include <genClass/C_Graphic.h>

#include <genClass/DO_LabelArray.h>
#include <genClass/SC_BoolArray.h>

#include <genPlotClass/C_PlotObj.h>


class PFO_LabelAxes : public PlotObjC  {
    public:
        enum                        {maxLabelLen = 80};

    protected:
        //                          properties
        enum LabelSource            {lsLabelArray, lsTableCol, lsTableRow};

        LabelSource                 labelSource;
        FuncObjRef                  labelArrayObjRef;
        FuncObjRef                  tableObjRef;
        bool                        isXAxis;  //X or Y axis
        bool                        skipCol0; // for lsTableCol
        bool                        limTxtLen;
        int                         maxTxtLen;
        char                        axesLabel[maxLabelLen];

    protected:
        //                          data
        SC_StringArray              labelText;
        SC_DoubleArray              labelVal;
        SC_BoolArray                labelInLimits;

    private:
        bool*                       checkLabelArray;
        bool*                       checkTable;

    public:
                        PFO_LabelAxes(PlotDefC& assPlt);
                        PFO_LabelAxes(const PFO_LabelAxes& a, PlotDefC& assPlt);
                        ~PFO_LabelAxes();

        PFO_LabelAxes&   operator= (const PFO_LabelAxes& a);

                        //  FuncObjC virtuals
        virtual void    DoStatusChk();
        virtual void    CalcOutput(FOcalcType  calcType);

                        //  PlotObjC  draw virtuals in derived PFO_LabelAxesGL
        virtual bool    ContainsLinesOnly();

                        //  PlotObjC virtuals
        virtual Limit3D GetPlotObjLimits();

    protected:
        bool            ClipToLimits();
    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_LabelAxes& a);  // common to copy constructor and = operator

        void            SortLabels(const SC_StringArray& unsortedText);
        void            LimitTextLength();
};

#endif // !PFO_LABELAXES_H

