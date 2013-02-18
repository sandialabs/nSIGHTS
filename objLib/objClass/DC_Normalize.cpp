///////////////////////////////////////////////////////////////////////////////////
//
// DC_Normalize.cpp
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

#include <genClass/C_Common.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objClass/DC_Normalize.h>

DC_Normalize::DC_Normalize()
{
    normalizeOp   = noBoth;
    autoLimit     = true;
    inputMinimum  = 0.0;
    inputMaximum  = 1.0;
    outputMinimum = 0.0;
    outputMaximum = 1.0;
    normPower     = -0.50;
}

DC_Normalize::DC_Normalize(const DC_Normalize& a)
{
    LocalCopy(a);
}


DC_Normalize::~DC_Normalize()
{
}

DC_Normalize& DC_Normalize::operator= (const DC_Normalize& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}


void  DC_Normalize::LocalCopy(const DC_Normalize& a)
{
    normalizeOp   = a.normalizeOp;
    autoLimit     = a.autoLimit;
    inputMinimum  = a.inputMinimum;
    inputMaximum  = a.inputMaximum;
    outputMinimum = a.outputMinimum;
    outputMaximum = a.outputMaximum;
    normPower     = a.normPower;
}


void DC_Normalize::DoNormalize(SC_DoubleArray& inData)
{
    if (normalizeOp != noPower)
    {
        if (autoLimit)
        {
            inData.CalcMinMax(inputMinimum, inputMaximum);
            if (RealIsNull(inputMinimum))
                return;
        }

        if (fabs(inputMaximum - inputMinimum) < stdEps)
            spanMult = 0.0;
        else
            spanMult = (outputMaximum - outputMinimum) / (inputMaximum - inputMinimum);
    }
    intPower = double(int(normPower));

    for (int i = 0; i < inData.Size(); i++)
        inData[i] = Normalize(inData[i]);

}


double DC_Normalize::Normalize(double inNorm)
{
    if (RealIsNull(inNorm))
        return nullReal;

    double outVal = inNorm;
    if (normalizeOp != noPower)
    {
        if (outVal <= inputMinimum)
            outVal = outputMinimum;
        else if (outVal >= inputMaximum)
            outVal = outputMaximum;
        else
            outVal = (inNorm - inputMinimum) * spanMult + outputMinimum;
    }

    if ((normalizeOp != noLimit) && (fabs(outVal) > stdEps))
    {
        if (outVal < stdEps)
            outVal = pow(outVal, intPower);
        else
            outVal = pow(outVal, normPower);
    }

    return outVal;
}





