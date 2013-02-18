///////////////////////////////////////////////////////////////////////////////////
//
// U_Str.h
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
// DESCRIPTION: String processing utilities
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef U_STR_H
#define U_STR_H

#include <string>

class SC_RealFormat;

namespace genStringSupport {

    extern void IntToString(int iVal, char str[], int maxLen);

    extern void IntToString(int iVal, char str[], int padLen, int maxLen);  // pads to padLen chars with leading 0s

    extern void RealToString(const SC_RealFormat& rForm, const double& rVal, char str[], int maxLen);

    extern bool NotWhiteSpace(char inChar);
    //  returns true if char is not tab or blank

    extern void TrimLeft(char inStr[]);
    //  removes trailing WS

    extern void TrimRight(char inStr[]);
    //  removes leading WS

    extern void FullTrim(char inStr[]);
    //  removes trailing & leading WS

    extern int  StringLength(const char* inStr);
    //  replacement for StringLength to avoid various Win64 warnings

    extern void DeleteChar(char inStr[],
                           int  delPos,
                           int  nDel);
    //  deletes nDel chars starting at delPos


    extern void CopyString(char       destStr[],
                           const char srcStr[],
                           int        maxLen);
    //  copies up to maxLen - 1 characters from source to dest

    extern void AllocAndCopyString(char*&     destPtr,
                                   const char* inStr);

    extern void ConcatString(char       destStr[],
                             const char addStr[],
                             int        maxLen);
    //  adds addStr to end of destStr up to maxLen - 1 characters total in dest

    extern void InsertString(char       destStr[],
                             const char insStr[],
                             int        insertPos,
                             int        maxLen);
    //  inserts insStr into dest str starting at insert pos
    //  pads with \0 until maxLen


    // basic parsing/string extraction
    extern void PrepareParseString(char* inStr, char* parseStr);
    // if any chars other than blank are to be used

    extern bool ParseGetNextString(char*    inStr, char* nextStr, int maxLen);
    // returns true if nextStr is found, removes nextStr from inStr, trims



    // multiple signatures for common ops
    //  copies base str to dest str, concats add str
    extern void MakeString(char destStr[],
                           const char baseStr[],
                           const char addStr[],
                           int  maxLen);
    extern void MakeString(char destStr[],
                           const char baseStr[],
                           const char addStr1[],
                           const char addStr2[],
                           int  maxLen);
    extern void MakeString(char destStr[],
                           const char baseStr[],
                           const char addStr1[],
                           const char addStr2[],
                           const char addStr3[],
                           int  maxLen);
    extern void MakeString(char destStr[],
                           const char baseStr[],
                           const char addStr1[],
                           const char addStr2[],
                           const char addStr3[],
                           const char addStr4[],
                           int  maxLen);
    extern void MakeString(char destStr[],
                           const char baseStr[],
                           const char addStr1[],
                           const char addStr2[],
                           const char addStr3[],
                           const char addStr4[],
                           const char addStr5[],
                           int  maxLen);
    extern void MakeString(char destStr[],
                           const char baseStr[],
                           const char addStr1[],
                           const char addStr2[],
                           const char addStr3[],
                           const char addStr4[],
                           const char addStr5[],
                           const char addStr6[],
                           int  maxLen);
    extern void MakeString(char destStr[],
                           const char baseStr[],
                           const char addStr1[],
                           const char addStr2[],
                           const char addStr3[],
                           const char addStr4[],
                           const char addStr5[],
                           const char addStr6[],
                           const char addStr7[],
                           int  maxLen);
    extern void MakeString(char destStr[],
                           const char baseStr[],
                           const char addStr1[],
                           const char addStr2[],
                           const char addStr3[],
                           const char addStr4[],
                           const char addStr5[],
                           const char addStr6[],
                           const char addStr7[],
                           const char addStr8[],
                           int  maxLen);

    extern void UpperCase(char inStr[]);
    //  converts to upper case

    extern void LowerCase(char inStr[]);
    //  converts to lower case

    extern void RightJustify(char inStr[],
                             int  len);
    //  inserts leading blanks until inStr is at len

    extern void LeftJustify(char inStr[],
                            int  len);
    //  adds trailing blanks

    extern void CenterJustify(char inStr[],
                              int  len);
    //  inserts leading  and trailing blanks until inStr is at len

    extern void FillBlanks(char inStr[], char fillChar);
    //  replaces blanks in inStr with underscores

    extern void FillBlanks(char inStr[]);
    // overloaded instead of default fillChar for Linux build

    extern void ReplaceUnderscores(char inStr[], char fillChar);
    //  replaces underscores in inStr with blanks

    extern void ReplaceUnderscores(char inStr[]);
    // overloaded instead of default fillChar for Linux build

    extern void ReplaceNonNumeric(char inStr[]);
    //  replaces non 0 - 9 in inStr with blanks

    extern void FillWithChar(char inStr[],
                             char fillChar,
                             int  len);

    extern int  CharPositn(const char inStr[],
                           char findChar,
                           int  startPos = 0);
    // returns position of findChar in str, starting at startPos, -1 if not found

    void  DoubleApostrophes(char       destStr[],
                            const char srcStr[],
                            int        maxLen);
    // copies string, replacing single apostrophes with double apostrophes (if there are any)

    extern void ExtractFileName(char       destStr[],
                                const char fullStr[],
                                int       maxLen);
    //  finds last / and adds remaining characters to destStr up to maxLen

    extern void ExtractFileExtension(   char        destStr[],
                                        const char  fullStr[],
                                        int             maxLen);
    // finds characters after decimal point up to maxLen


    extern void ExtractFilePath(char       destStr[],
                                const char fullStr[],
                                int       maxLen);
    //  copies fullStr to DestStr up to last slash
    //  last slash is not included unless it is first char (i.e. root)

    extern void ExtractFolderPath(char       destStr[],
                                  const char fullStr[],
                                  int        maxLen);
    //  copies fullStr to DestStr up to last slash
    //(not including slash at end of fullStr indicating folder rather than file)
    //  slash is added to end of string

    extern void MakeFullFileName(char       fullName[],
                                 const char pathStr[],  // made by ExtractFilePath (root handling)
                                 const char fileStr[],
                                 int       maxLen);

    extern bool AddFileNameSuffix(char*          fName,
                                      const char* fExt,
                                      const char* suffix,
                                      int       maxLen);

    // looks for fExt in fName, returns false if not found
    // if found replaces fExt suffix & fExt and returns true
    // fExt should contain std dot delimiter

    extern bool IsValidInt(const char   inStr[],
                           int&  dVal,
                           bool  leadZero = false);

    extern bool IsValidReal(const char      inStr[],
                            double&   dVal);

    extern void ConcatInt(int iVal, char str[], int maxLen);

    extern void ConcatInt(int iVal, char str[], int padLen, int maxLen);

    extern void ConcatInt(int iVal, const char* preStr, char str[], int padLen, int maxLen);

    extern void SetToNull(char* str);

    inline bool IsNull(const char* str) {return (str == NULL) || (str[0] == '\0');}
    inline bool IsNotNull(const char* str) {return (str != NULL) && (str[0] != '\0');}

    // string comparisons
    extern bool  SameString(const char* str1, const char* str2);  // just does strcmp

    extern bool  SameUCString(const char* str1, const char* str2);  // upper case then match

    // extended Atof
    //[whitespace] [sign] [digits] [{.|,}digits] [ {d | D | e | E }[sign]digits]
    // double Atof(const char* szNumber, bool& bError);
    // removed -- use IsValidReal

    //Added for Singen
    extern int GetFirstNum(const char   inStr[]);
    //returns first number encountered, returns a negative if no number found
    extern int GetSecondNum(const char  inStr[]);
    //returns second number encountered, returns a negative if no number found
    extern bool CharIsNumeric(char inChar);
};

using namespace genStringSupport;

#endif // U_STR_H

