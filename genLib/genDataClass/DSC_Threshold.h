///////////////////////////////////////////////////////////////////////////////////
//
// DSC_Threshold.h
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
// DESCRIPTION: thresholding values
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DSC_THRESHOLD_H
#define DSC_THRESHOLD_H

class DSC_Threshold {
public:

    enum ThresholdOp   {toNone,    // no thresholding
                        toNull,    // set to null above/below
                        toClamp};  // clamp at value

    ThresholdOp         minThreshold;
    ThresholdOp         maxThreshold;
    double              minVal;
    double              maxVal;

public:
    DSC_Threshold();
    DSC_Threshold(const DSC_Threshold& a);
    ~DSC_Threshold() {};

    //  copy operator
    DSC_Threshold&   operator= (const DSC_Threshold& a);


    double              ApplyThresholds(const double& inVal) const;

private:
    void                LocalCopy(const DSC_Threshold& a);

};

#endif // DSC_THRESHOLD_H

