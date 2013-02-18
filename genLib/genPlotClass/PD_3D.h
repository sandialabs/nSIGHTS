///////////////////////////////////////////////////////////////////////////////////
//
// PD_3D.h
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
//      defines basic 3D plot
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PD_3D_H
#define PD_3D_H

#include <genClass/DO_Real.h>

#include <genDataClass/DSC_RealMSSpecBase.h>

#include <genPlotClass/PC_Report.h>
#include <genPlotClass/PC_3DAxesFormat.h>
#include <genPlotClass/PC_3DAxesLabel.h>
#include <genPlotClass/PC_VRMLOutData.h>
#include <genPlotClass/PC_Lighting.h>
#include <genPlotClass/PC_TransparencySpec.h>

#include <genPlotClass/C_PlotDef.h>

//  This is the base class for all 3D plot definitions
class PD_3D : public PlotDefC  {
    public:
        //  view stuff
        enum    ThreeDProjection    {tdp_Perspective, tdp_Orthographic};

        ThreeDProjection    plotProjection;
        double              fieldOfView;  // in degrees - for perspective only

                            // stereo only available for perspective
        bool                useStereo3DIfAvailable;
        double              eyeSeparationDivisor;

        PC_3DAxesFormat     axesFormat;
        PC_3DAxesLabel      axesLabel;
        PC_VRMLOutData      vrmlOutData;

        PC_Lighting         plotLighting;

        PC_TransparencySpec transSpec;

        DSC_RealMSSpecBase  fieldOfViewMS;
        DSC_RealMSSpecBase  eyeSeparationDivisorMS;

        bool                optimizeRedraw; // set on plot create

    private:
        DO_Real             elevationDO;
        DO_Real             azimuthDO;
        DO_Real             scaleDO;


        bool                transOK;
        bool*               fieldOfViewCheckRef;
        bool*               eyeSeparationDivisorCheckRef;


    public:
                            PD_3D(const char* dType);
                            PD_3D(const PD_3D& a);
                            ~PD_3D();

        PD_3D&              operator= (const PD_3D& a);

        void                SetDefault();
        void                DoStatusChk();

        void                SetInitialView(const PC_View& initView);  // for file routines
        void                ChangeView(const PC_View&  newView);  // sets elev/azimuth/scale data. calls base class

        //  PlotDefC virtual
        void                DrawPlotObjects();

        virtual void        SetTransparencyFactor(const double& tranFactor) = 0;
        virtual void        StartTransparentSection() = 0;
        virtual void        EndTransparentSection() = 0;

        bool                ProjectionIsOrtho() const {return plotProjection == tdp_Orthographic;}

    protected:
        void                FullCopy(const PD_3D& a);  // called by copy in derived class

        virtual void        DrawAnnoObjects(); // anno transparency

        bool                PlotDefIsStereo() const;

    private:
        void                InitCommon();               // common to normal and copy constructor
        void                LocalCopy(const PD_3D& a);  // common to copy constructor and = operator
        void                UpdateViewDO(const PC_View& newView);


};


#endif // !PD_3D_H

