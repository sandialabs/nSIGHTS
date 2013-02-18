///////////////////////////////////////////////////////////////////////////////////
//
// C_WorkDataCapture.h
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_WORKDATACAPTURE_H
#define C_WORKDATACAPTURE_H

#include <Var/C_DataCapture.h>
#include <Sim/C_WorkCommon.h>


class RadiusPressureExtraction {
    protected:
        int     radIndex;
        double  weight;
        bool    doScale;        // for anisotropy
        double  staticP;
        double  scaleFactor;
    public:
                RadiusPressureExtraction() : radIndex(0), weight(0.0) {}
        void    SetExtraction(const SC_DoubleArray& nodeRadii,
                              const double&         capRadius);

        void    SetExtraction(const SC_DoubleArray& nodeRadii,  // for aniso
                              const double&         capRadius,
                              const double&         staticP,
                              const double&         scaleFactor);

        double  DoExtraction(const SC_DoubleArray& nodePressure) const;
};

// superposition record
class WorkingSuperComponent {
    public:
        SuperComponent::SuperOp     superOp;
        RadiusPressureExtraction    radData;

    public:
                            WorkingSuperComponent() {};
                            ~WorkingSuperComponent() {};
        void                SetSuper(const SC_DoubleArray& nodeRadii,
                                     const SuperComponent& inData);

};

typedef T_SC_Array<WorkingSuperComponent> WorkingSuperComponentArray;


// for confined obs well & unconfined in constant zone
class FixedVerticalPressureExtraction : public  RadiusPressureExtraction {
    protected:
        const SC_DoubleArray*   fixedNodeZ;

        mutable int     vertBotNodeIndx;
        mutable double  vertTopWeight;

    public:
                        FixedVerticalPressureExtraction() {}

        void            SetExtraction(const SC_DoubleArray& nodeRadii,
                                      const SC_DoubleArray& fixedZ,
                                      const double&         capRadius,
                                            bool            vertNormalized,
                                     const  double&         vertActualOffset,
                                     const  double&         vertNormalizedOffset);

        double          DoExtraction(const SC_DoubleMatrix& nodePressure) const;

    protected:
        void    CalcConstantVert(const double& capZ) const;

};


// for unconfined obs well
class VariableVerticalPressureExtraction : public  FixedVerticalPressureExtraction {
    private:
        double          thicknessToPressure;
        bool            normalized;
        double          offsetVal;
        bool            doFixedZ;

    public:
                        VariableVerticalPressureExtraction() {}

        void            SetExtraction(const SC_DoubleArray& nodeRadii,
                                      const SC_DoubleArray& fixedZ,
                                      const double&         thickToPress,
                                      const double&         capRadius,
                                            bool            vertNormalized,
                                     const  double&         vertActualOffset,
                                     const  double&         vertNormalizedOffset);

        double          DoExtraction(const SC_DoubleArray& uncThickness,
                                     const SC_DoubleMatrix& nodePressure) const;


};

class WorkingDataCaptureSpecBase {
    public:
        DataCaptureSpec::CaptureType            captureType;
        DataCaptureSpec::PressureCapType        pressureCapType;
        DataCaptureSpec::FlowCapType            flowCapType;
        DataCaptureSpec::ProductionCapType      productionCapType;
        DataCaptureSpec::TestZoneCapType        testZoneCapType;

        UnitIndex                               outputUnits;

    protected:
        double                                  dcapStaticPressure;
    public:
                                WorkingDataCaptureSpecBase();
                                ~WorkingDataCaptureSpecBase();

        void                    SetupSpec(const DataCaptureSpec& inSpec,
                                          const double&          inStatic);

        double                  ApplySpec(const double&         prevVal,         // prev val of spec for production calc
                                          const double&         deltaT,          // time for production calc
                                          const SeqBoundVals&   tsData);
};


class WorkingDataCaptureSpec1D : public WorkingDataCaptureSpecBase {
    public:
        RadiusPressureExtraction                oneDRadData;
        WorkingSuperComponentArray              superData;

    public:
                                WorkingDataCaptureSpec1D();
                                ~WorkingDataCaptureSpec1D();

        void                    SetupSpec(const DataCaptureSpec& inSpec,
                                          const double&          inStatic,
                                          const SC_DoubleArray& nodeRadii,
                                         const double&          inxyRatio,
                                        const double&           inxyAngle);

        double                  ApplySpec(const SC_DoubleArray& node1DPressure,
                                          const double&         prevVal,         // prev val of spec for production calc
                                          const double&         deltaT,          // time for production calc
                                          const SeqBoundVals&   tsData);
};

class WorkingDataCaptureSpec2DUnconfined : public WorkingDataCaptureSpecBase {
    public:
        RadiusPressureExtraction                wtRadData;
        VariableVerticalPressureExtraction      pressureRadData;

    public:
                                WorkingDataCaptureSpec2DUnconfined();
                                ~WorkingDataCaptureSpec2DUnconfined();

        void                    SetupSpec(const DataCaptureSpec& inSpec,
                                          const double&          inStatic,
                                          const SC_DoubleArray& nodeRadii,
                                           const SC_DoubleArray& fixedZ,
                                           const double&          thickToPress);


        double                  ApplySpec(const SC_DoubleArray& uncThickness,
                                          const SC_DoubleMatrix& node2DPressure,
                                          const double&         prevVal,         // prev val of spec for production calc
                                          const double&         deltaT,          // time for production calc
                                          const SeqBoundVals&   tsData);
};

class WorkingDataCaptureSpec2DConfined : public WorkingDataCaptureSpecBase {
    public:
        FixedVerticalPressureExtraction     pressureRadData;

    public:
                                WorkingDataCaptureSpec2DConfined();
                                ~WorkingDataCaptureSpec2DConfined();

        void                    SetupSpec(const DataCaptureSpec& inSpec,
                                          const double&          inStatic,
                                          const SC_DoubleArray& nodeRadii,
                                          const SC_DoubleArray& fixedZ);

        double                  ApplySpec(const SC_DoubleMatrix& node2DPressure,
                                          const double&         prevVal,         // prev val of spec for production calc
                                          const double&         deltaT,          // time for production calc
                                          const SeqBoundVals&   tsData);
};



class WorkingCaptureDataBase {

    friend class WorkingDataCaptureSpecBase;
    friend class WorkingDataCaptureSpec1D;

    protected:
        SC_DoubleMatrix*            currCapturedData;  // this is a ptr for now.. in thread version it will be whole
        SC_DoubleArray              capturedVals;   // set in derived
        SC_IntArray                 productionCapture;
        double                      dcapStatic;
    private:
        double                      lastMetricTime;
        int                         nextRestart;

    public:
                                    WorkingCaptureDataBase();

        bool                        WorkingCaptureSetup();

        double                      GetDeltaT(const double& testTime);

        void                        ApplySpec(const double&         testTime);

        void                        UpdateCapture();  // updates G_Capture FO vars

};


typedef T_SC_Array<WorkingDataCaptureSpec1D>  WorkingDataCaptureSpec1DArray;

class WorkingCaptureData1D : public WorkingCaptureDataBase {

    friend class WorkingDataCaptureSpec1D;

    private:
        WorkingDataCaptureSpec1DArray capturedSpec;
    public:
                                    WorkingCaptureData1D() {};

        bool                        WorkingCaptureSetup(const SC_DoubleArray&   nodeRadii,
                                                        const double&           currStatic,
                                                        const double&           inxyRatio,
                                                        const double&           inxyAngle);

        void                        ApplySpec(const double&         testTime,  // in user units
                                              const SC_DoubleArray& node1DPressure,
                                              const SeqBoundVals&   tsData);

};



typedef T_SC_Array<WorkingDataCaptureSpec2DUnconfined>  WorkingDataCaptureSpec2DUnconfinedArray;

class WorkingCaptureData2DUnconfined : public WorkingCaptureDataBase {

    friend class WorkingDataCaptureSpec2D;
    friend class VerticalPressureExtraction;
    friend class WorkingCaptureData;


    private:
        WorkingDataCaptureSpec2DUnconfinedArray capturedSpec;

    public:
                                    WorkingCaptureData2DUnconfined() {};

        bool                        WorkingCaptureSetup(const SC_DoubleArray&   nodeRadii,
                                                        const double&           currStatic,
                                                        const double&           currThicknessToPressure, // conversion to calc pressure associated with uncThick
                                                        const SC_DoubleArray&   currConstantNodeZ);

        void                        ApplySpec(const double&         testTime,  // in user units
                                              const SC_DoubleArray& uncThickness,
                                              const SC_DoubleMatrix& node2DPressure,
                                              const SeqBoundVals&   tsData);

};


typedef T_SC_Array<WorkingDataCaptureSpec2DConfined>  WorkingDataCaptureSpec2DConfinedArray;

class WorkingCaptureData2DConfined : public WorkingCaptureDataBase {

    friend class WorkingDataCaptureSpec2D;
    friend class VerticalPressureExtraction;
    friend class WorkingCaptureData;


    private:
        WorkingDataCaptureSpec2DConfinedArray capturedSpec;

    public:
                                    WorkingCaptureData2DConfined() {};

        bool                        WorkingCaptureSetup(const SC_DoubleArray&   nodeRadii,
                                                        const double&           currStatic,
                                                        const SC_DoubleArray&   currConstantNodeZ);

        void                        ApplySpec(const double&         testTime,  // in user units
                                              const SC_DoubleMatrix& node2DPressure,
                                              const SeqBoundVals&   tsData);

};

#endif // C_WORKDATACAPTURE_H

