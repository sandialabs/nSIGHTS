///////////////////////////////////////////////////////////////////////////////////
//
// LFO_OptBase.h
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
//      Listing functional objects for optimizer results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef LFO_OPTBASE_H
#define LFO_OPTBASE_H

#include <genListClass/C_ListObj.h>

#include <osClass/DC_Covar.h>
#include <osClass/DC_CovarArray.h>
#include <osClass/DC_Jacobian.h>
#include <osClass/DC_OptSimResults.h>



class LFO_OptBase : public ListObjC {

    public:
        enum            CovarianceBasis {   cb_Estimated,
                                            cb_Actual,
                                            cb_Fit};

    public:
                        LFO_OptBase(const char* listID, ListDefC& assLst);
                        LFO_OptBase(const LFO_OptBase& a, ListDefC& assLst);
                        ~LFO_OptBase();

        LFO_OptBase&    operator= (const LFO_OptBase& a);

    protected:
        void            ListMatrix(const char*            matID,
                                   const SC_StringArray&  colTitles,
                                   const SC_StringArray&  colUnits,
                                   const SC_StringArray&  rowTitles,
                                   const SC_DoubleMatrix& matData,
                                         int              maxCol = 5,
                                         int              col1Width = 20,
                                         int              colnWidth = 15);

        void            ListMatrix(const char*            matID,
                                   const SC_StringArray&  colTitles,
                                   const SC_StringArray&  rowTitles,
                                   const SC_DoubleMatrix& matData,
                                         int              maxCol = 5,
                                         int              col1Width = 20,
                                         int              colnWidth = 15);

        void            ListUtriMatrix(const char*            matID,
                                       const SC_StringArray&  titles,
                                       const SC_DoubleMatrix& matData,
                                         int                maxCol = 5,
                                         int                col1Width = 20,
                                         int                colnWidth = 15);


        void            ListJacobian(const DC_Jacobian& jacData);
        void            ListCovar(const DC_FullCovar&   covarData);

        void            ListCovarCorr(const DC_FullCovar&   covarData);

        void            SetList95();

        void            List95(const char*          covarTitle1,
                               const char*          covarTitle2,
                               const DC_FullCovar&  covarData);

    private:
        void            LocalCopy(const LFO_OptBase& a);  // common to copy constructor and = operator
};

#endif // !LFO_OPTBASE_H

