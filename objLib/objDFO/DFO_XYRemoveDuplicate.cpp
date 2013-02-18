///////////////////////////////////////////////////////////////////////////////////
//
// DFO_XYRemoveDuplicate.cpp
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
//      Removes duplicate X and Y values from a data set
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/SC_DoubleArray.h>
#include <genClass/U_Real.h>
#include <genClass/U_Str.h>

#include <objDFO/DFO_XYRemoveDuplicate.h>

DFO_XYRemoveDuplicate :: DFO_XYRemoveDuplicate() :
    DFO_XYBase("Remove Duplicates")
{
    removeDuplicateXValues  = false;
    yValueToKeepOp          = rdMin;
    duplicateXLimit         = 1.0E-6;
    removeOutOfSeq          = false;
    removeDuplicateYValues  = false;
    duplicateYLimit         = 1.0E-6;
    InitCommon();
}

DFO_XYRemoveDuplicate::DFO_XYRemoveDuplicate(const DFO_XYRemoveDuplicate& a) :
    DFO_XYBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_XYRemoveDuplicate :: ~DFO_XYRemoveDuplicate ()
{
}

void DFO_XYRemoveDuplicate::InitCommon()
{
    DoStatusChk();
}

DFO_XYRemoveDuplicate& DFO_XYRemoveDuplicate::operator= (const DFO_XYRemoveDuplicate& a)
{
    if (&a != this)
    {
        DFO_XYBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_XYRemoveDuplicate::LocalCopy(const DFO_XYRemoveDuplicate& a)
{
    removeDuplicateXValues  = a.removeDuplicateXValues;
    yValueToKeepOp          = a.yValueToKeepOp;
    duplicateXLimit         = a.duplicateXLimit;
    removeOutOfSeq          = a.removeOutOfSeq;
    removeDuplicateYValues  = a.removeDuplicateYValues;
    duplicateYLimit         = a.duplicateYLimit;
}

void  DFO_XYRemoveDuplicate:: DoStatusChk()
{
    outputXY.SetID("dup free");

    DFO_XYBase::DoStatusChk();
    if (StatusOK() && (inputXY->Size() < 2))
    {
        SetObjErrMsg("insufficient data in input");
    }
}


void DFO_XYRemoveDuplicate:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    DC_XYData temp;
    temp.CreateFrom(inputXY->xData, inputXY->yData);
    if (temp.Size() < 2)
    {
        SetObjErrMsg("insufficient data after cleaning");
        return;
    }

    // alloc output
    if (!outputXY.Alloc(temp.Size()))
    {
        SetObjErrMsg("allocation problem ???");
        return;
    }


    if (removeDuplicateXValues)
    {
        nDup = 0;
        currX = temp.xData[0];
        currY = temp.yData[0];

        AddCurrXY();
        AddXRedu();
        bool lastWasOK = false;
        for (int j = 1; j < temp.Size();j++)
        {
            currX = temp.xData[j];
            currY = temp.yData[j];

            if (removeOutOfSeq && (currX < lastX))
                continue;

            if (fabs(currX - lastX) < duplicateXLimit)
            {
                lastWasOK = false;
                AddXRedu();
            }
            else
            {
                lastWasOK = true;
                AddLastX();
            }
        }
        if (lastWasOK && (nDup > 1))
            AddLastX();
    }
    else
    {
        outputXY = temp;
    }

    if (removeDuplicateYValues)
    {
        // if three consective ys the same, remove middle one

        int currNum = 0;
        while ((currNum + 2) < outputXY.Size())
        {
            currY  = outputXY.yData[currNum];
            double midY = outputXY.yData[currNum + 1];
            double endY  = outputXY.yData[currNum + 2];

            if ((fabs(currY - midY) > duplicateYLimit) || (fabs(currY - endY) > duplicateYLimit))
            {
                currNum++;
                continue;
            }

            outputXY.xData.DeleteItem(currNum);
            outputXY.yData.DeleteItem(currNum);
        }
    }

    CalcFinish();

    char msg[80];
    IntToString(inputXY->Size() - outputXY.Size(), msg, 80);
    ConcatString(msg, " duplicates removed", 80);
    ApplyInfoMsg(calcType, msg);
}

void DFO_XYRemoveDuplicate::AddCurrXY()
{
    outputXY.xData += currX;
    outputXY.yData += currY;

    nDup = 0;
    lastX = currX;
    lastY = currY;
}

void DFO_XYRemoveDuplicate::AddXRedu()
{
    if (nDup == 0)
    {
        yKeepData[rdMin]   = currY;
        yKeepData[rdMax]   = currY;
        yKeepData[rdAvg]   = currY;
        yKeepData[rdFirst] = currY;
    }
    else
    {
        if (yKeepData[rdMin] > currY)
            yKeepData[rdMin] = currY;

        if (yKeepData[rdMax] < currY)
            yKeepData[rdMax] = currY;

        yKeepData[rdAvg] = yKeepData[rdAvg] + currY;
    }
    yKeepData[rdLast] = currY;
    nDup++;
}


void DFO_XYRemoveDuplicate::AddLastX()
{
    if (yValueToKeepOp == rdAvg)
    {
        lastY = yKeepData[rdAvg] / double(nDup);
    }
    else
    {
        lastY = yKeepData[yValueToKeepOp];
    }
    outputXY.yData.LastIndex() = lastY;
    AddCurrXY();
    AddXRedu();
}


////////////////////////////////////////////////////////////

/*

PROCEDURE remove_duplicates(CONST  in_data : xy_data_record;
                                   n_xy    : WORD);
TYPE
     y_keep_vector = ARRAY [y_keep_type] OF REAL8;
VAR
     curr_x, curr_y, last_x, last_y, mid_y, end_y : REAL8;
     nDup, currNum : WORD;
     yKeepData : y_keep_vector;

     PROCEDURE AddCurrXY;
     BEGIN
          n_red_data := n_red_data + 1;
          WITH reduced_data DO BEGIN
               x^[n_red_data] := curr_x;
               y^[n_red_data] := curr_y;
          END;
          nDup := 0;
          last_x := curr_x;
          last_y := curr_y;
     END;

     PROCEDURE AddXRedu;
     BEGIN
          nDup := nDup + 1;
          IF nDup = 1 THEN
               BEGIN
                    yKeepData[yk_min] := curr_y;
                    yKeepData[yk_max] := curr_y;
                    yKeepData[yk_avg] := curr_y;
                    yKeepData[yk_1st] := curr_y;
               END
          ELSE
               BEGIN
                    yKeepData[yk_min] := MIN(yKeepData[yk_min], curr_y);
                    yKeepData[yk_max] := MAX(yKeepData[yk_max], curr_y);
                    yKeepData[yk_avg] := yKeepData[yk_avg] + curr_y;
               END;
          yKeepData[yk_last] := curr_y;
     END;

     PROCEDURE set_curr_xy(indx : WORD);
     BEGIN
          WITH in_data DO BEGIN
               curr_x := x^[indx];
               curr_y := y^[indx];
          END;
     END;

     PROCEDURE AddLastX;
     BEGIN
          IF y_keep = yk_avg THEN
               last_y := yKeepData[yk_avg] / REAL8(nDup)
          ELSE
               last_y := yKeepData[y_keep];
          reduced_data.y^[n_red_data] := last_y;
          AddCurrXY;
          AddXRedu;
     END;

BEGIN
--   remove x dups first
     IF dup_x_remove THEN
          BEGIN
               nDup := 0;
               set_curr_xy(1);
               AddCurrXY;
               AddXRedu;
               FOR j : WORD := 2 TO n_xy DO BEGIN

                    set_curr_xy(j);

                    IF x_neg_remove AND (curr_x < last_x) THEN
                         CYCLE;

                    IF ABS(curr_x - last_x) < dup_x_eps THEN
                         AddXRedu
                    ELSE
                         AddLastX;
               END;
               IF nDup > 1 THEN
                    AddLastX;
          END
     ELSE
          FOR j : WORD := 1 TO n_xy DO BEGIN
               set_curr_xy(j);
               AddCurrXY;
          END;

     IF NOT dup_y_remove THEN
          RETURN;

--   if three consective ys the same, remove middle one

     currNum := 1;
     WITH reduced_data DO
          WHILE (currNum + 2) <= n_red_data DO BEGIN

               curr_y := y^[currNum];
               mid_y  := y^[currNum + 1];
               end_y  := y^[currNum + 2];

               IF (ABS(curr_y - mid_y) > dup_y_eps) OR
                  (ABS(curr_y - end_y) > dup_y_eps) THEN
                    BEGIN
                         currNum := currNum + 1;
                         CYCLE;
                    END;

               FOR j : WORD := currNum + 1 TO n_red_data - 1 DO BEGIN
                    y^[j] := y^[j + 1];
                    x^[j] := x^[j + 1];
               END;
               n_red_data := n_red_data - 1;
          END;
END;

*/

