///////////////////////////////////////////////////////////////////////////////////
//
// LSC_Listing.h
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
//      Platform independent base classes to allow creation of listings in HTML browser windows.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef LSC_LISTING_H
#define LSC_LISTING_H

#include <genClass/SC_StringArray.h>
#include <genClass/SC_IntArray.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/SC_RealFormat.h>


class FuncObjRef;
//  This creates the listing text,
// included in the listing objects by multiple inheritance,
// also used by some data objects to create results listings to be viewed with
// LFO_ObjectResults

class LSC_Listing {

    public:
        enum                    {listingLineLen = 256};

    protected:
        SC_StringArray          listingData; // the complete listing
        SC_IntArray             tableColPos;
        SC_IntArray             tableColWidth;
        SC_BoolArray            tableLeftJustify;

    private:
        char                    currStr[listingLineLen];
        bool                    lastLineBlank;


        SC_StringArray          enumStrs;
        SC_IntArray             enumMap;

        bool                    doingPreFormat;
        bool                    suppressHTML;

        SC_RealFormat           defaultRealFormat;


    public:

        // these are defaulted to usual values - can be modified if required
        int                     stdLeftMargin;  // 0
        int                     stdRightPos;    // 55
        int                     stdDataLen;     // 15
        int                     wideRightPos;   // 75
        int                     unitLeftPos;    // 60 (stdRightPos + 5)

    public:

                        LSC_Listing();
                        LSC_Listing(const LSC_Listing& a);  // non-modal

        virtual         ~LSC_Listing();

        LSC_Listing&    operator= (const LSC_Listing& a);


        //suppress HTML for dialogs (i.e. not using HTML window) - created for Singen Run dialog
        inline void                 SuppressHTML() {suppressHTML = true;}
        inline SC_StringArray&      GetListingStrArray() {return listingData;}

        //  actual routines for building listing
        inline void     SetDefaultFormat(const SC_RealFormat& inForm) {defaultRealFormat = inForm;}


        void            AddNextLine();          // adds currStr to arrays, clears, sets blankflag

        void            AddHTML(const char* inStr);  // stops pre formatting, adds string

        void            AddBlankLine();
        void            AddConditionalBlankLine();

        void            AddText(const char inStr[],
                                      int  atPos);

        void            AddText(const char inStr[],
                                      int  atPos,
                                      int  fieldLen,
                                      bool leftJustify);

        // HTML titles
        void            BigTitle(const char title[]);
        void            StdTitle(const char title[]);
        void            SubTitle(const char title[]);
        void            AddLine(const char inStr[],
                                      int  atPos = 0);

        // text titles
        void            AddTextTitle(const char title[],
                                           char underChar,
                                           char overChar,
                                           bool blankAfter = true);
        void            AddTextTitle(const char title[],
                                           char underChar,
                                           bool blankAfter = true);
        void            TextBigTitle(const char title[]); // ** above & below
        void            TextStdTitle(const char title[]); // === below
        void            TextSubTitle(const char title[]); // --- below, no blank after

        void            AddConditionalBlank();

        // basic
        void            SetStdLeftMargin(int newMargin);
        void            ResetStdLeftMargin();
        int             GetStdLeftMargin() const {return stdLeftMargin;}

        void            AddStdText(const char* dataDesc,
                                   const char* dataStr);
        void            AddWideText(const char* dataDesc,
                                   const char* dataStr);
        void            AddBoolText(const char* dataDesc,
                                          bool  flag,
                                    const char* trueStr,
                                    const char* falseStr);
        void            AddUnitText(const char* dataDesc,
                                    const char* dataStr,
                                    const char* unitStr);
        void            AddUnitConst(const char*    dataDesc,
                                     const double&  dataVal,
                                     const char*    unitStr);

        void            AddYesNo(const char* dataDesc,
                                          bool  flag);
        void            AddStatus(const char* dataDesc,
                                          bool  okFlag);
        void            AddWideBoolText(const char* dataDesc,
                                          bool  flag,
                                    const char* trueStr,
                                    const char* falseStr);

        void            AddStdInt(const char* dataDesc,
                                        int   dataVal);
        void            AddStdReal(const char*   dataDesc,
                                   const double& dataVal,
                                   const SC_RealFormat& dataFormat);
        void            AddStdReal(const char*   dataDesc,
                                   const double& dataVal);
        void            AddRealText(const double& dataVal,
                                    const SC_RealFormat& dataFormat,
                                    int  atPos);
        void            AddRealText(const double& dataVal,
                                    int  atPos);

        void            AddFileName(const char* dataDesc,
                                    const char* fileStr);

        void            ClearEnumStrs();
        void            FirstEnumStr(const char* enumStr, int enumVal);  // calls clear folowed by add
        void            AddEnumStr(const char* enumStr, int enumVal);
        void            AddEnumStr(const char* enumStr);
        void            AddStdEnum(const char* dataDesc, int enumVal);

        void            AddFuncObjRef(const char* dataDesc,
                                      const FuncObjRef& inRef);


        void            TableSetup(const SC_IntArray&   colWidth,
                                   const SC_BoolArray&  leftJust,
                                         int            tabInc,
                                         int            startCol = 0);
        void            TableSetup(const SC_IntArray&   colWidth,
                                         int            tabInc,         // first col left, all others right
                                         int            startCol = 0);

        void            TableSetup(const int*   cWidth,
                                   const bool*  lJust,
                                         int    colInc = 0,
                                         int    startCol = 0);

        void            TableSetup(int cWidth, int tabInc, int startCol, int nCol);



        void            SetTableCol(      int    colNum,
                                    const char*  colText);
        void            SetTableCol(const char*  colText[]);

        inline int      GetColWidth(int colNum) {return tableColWidth[colNum];}

        // basic overloads for titles
        void            SetTableCol(const char*  col0Text);
        void            SetTableCol(const char*  col0Text,
                                    const char*  col1Text);
        void            SetTableCol(const char*  col0Text,
                                    const char*  col1Text,
                                    const char*  col2Text);
        void            SetTableCol(const char*  col0Text,
                                    const char*  col1Text,
                                    const char*  col2Text,
                                    const char*  col3Text);
        void            SetTableCol(const char*  col0Text,
                                    const char*  col1Text,
                                    const char*  col2Text,
                                    const char*  col3Text,
                                    const char*  col4Text);
        void            SetTableCol(const char*  col0Text,
                                    const char*  col1Text,
                                    const char*  col2Text,
                                    const char*  col3Text,
                                    const char*  col4Text,
                                    const char*  col5Text);
        void            SetTableCol(const char*  col0Text,
                                    const char*  col1Text,
                                    const char*  col2Text,
                                    const char*  col3Text,
                                    const char*  col4Text,
                                    const char*  col5Text,
                                    const char*  col6Text);


        void            SetTableReal(     int            colNum,
                                     const double&       dataVal,
                                     const SC_RealFormat& dataFormat);

        void            SetTableReal(     int            colNum,
                                     const double&       dataVal);

        void            SetTableInt(int colNum,
                                    int dataVal);

        void            SetTableInt(int colNum,
                                    int dataVal,
                                    int padLen);

        void            SetTableBool(      int      colNum,
                                           bool     dataVal,
                                     const char*    trueText,
                                     const char*    falseText);

    protected:
        virtual void    InitListingData();   // resets normalListing
        virtual void    CloseListingData();

        //  routines to build/format listing
        void            StartPreFormatting();   // for sections bracketed with <pre> tags
        void            EndPreFormatting();     // for sections bracketed with <pre> tags
                                                // in general everything except titles and
                                                // AddHTML()

        void            AddHTML(const char* h1, const char* h2, const char* h3);

    private:
        void            AddTitleHTML(const char* h1, const char* h2, const char* h3);
        void            InitCommon();
        void            ResetCurrStr();     // clears strings to all blanks
        void            LocalCopy(const LSC_Listing& a);  // common to copy constructor and = operator
};

#endif // !LSC_LISTING_H

