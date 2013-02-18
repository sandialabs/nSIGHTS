///////////////////////////////////////////////////////////////////////////////////
//
// U_Str.cpp
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
// DESCRIPTION: String processing Utilities
//
///////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <genClass/C_Common.h>

#include <genClass/SC_RealFormat.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>


namespace genStringSupport {


    static const char blank = ' ';
    static const char tab   = '\t';

    bool NotWhiteSpace(char inChar)
    {
        return ((inChar != blank) && (inChar != tab));
    }

    void TrimLeft(char inStr[])
    //  trims trailing blanks;
    {
        int len = StringLength(inStr);
        if ( !len )
            return;

        for ( int i = len - 1; i >= 0; i--)
            {
                if (NotWhiteSpace(inStr[i]))
                    return;
                inStr[i] = '\0';
            }
    }



    void TrimRight(char inStr[])
    //  trims leading blanks;
    {
        int len = StringLength(inStr);
        if ((len == 0) || (NotWhiteSpace(inStr[0])))
            return;

        int firstNB = 0;
        int i;
        for (i = 1; i < len; i++)
            if (NotWhiteSpace(inStr[i]))
                {
                    firstNB = i;
                    break;
                }

        if (!firstNB)                      // no non-blank, clear str
            {
                inStr[0] = '\0';
                return;
            }

        len = len - firstNB;
        for (i = 0; i < len; i++)
            inStr[i] = inStr[i+firstNB];
        inStr[len] = '\0';
    }

    void FullTrim(char inStr[])
    {
        TrimLeft(inStr);
        TrimRight(inStr);
    }

    int     StringLength(const char* inStr)
    {
        if (inStr == NULL)
            return 0;

        return (int) strlen(inStr);
    }


    void DeleteChar(char inStr[],
                    int  delPos,
                    int  nDel)
    //  deletes nDel chars starting at delPos
    {
        int len = StringLength(inStr);
        if ((delPos > len) || (nDel == 0))
            return;

        if ((delPos + nDel) > len )
            {
                inStr[delPos] = '\0';
                return;
            }

        for (int i = delPos; i <= (len - nDel + 1); i++)
            inStr[i] = inStr[i + nDel];
    }



    void CopyString(char destStr[],
                    const char srcStr[],
                    int  maxLen)
    {
        if (maxLen < 1)
            return;

        if (IsNull(srcStr))
            {
                SetToNull(destStr);
                return;
            }

        int len = StringLength(srcStr);
        if (len >= maxLen)
            len = maxLen - 1;

        for (int i = 0; i < len; i++)
            destStr[i] = srcStr[i];
        destStr[len] = '\0';
    }

    void AllocAndCopyString(char*&    destPtr,
                            const char* inStr)
    {
        if (destPtr != NULL)
            {
                delete [] destPtr;
                destPtr = NULL;
            }

        if (inStr == NULL)
            return;

        int strLen = StringLength(inStr) + 1;
        try
            {
                destPtr = new char[strLen];
            }
        catch (std::bad_alloc)
            {
                GenAppInternalError("U_Str::AllocAndCopy");
            }

        CopyString(destPtr, inStr, strLen);
    }

    void InsertString(char       destStr[],
                      const char insStr[],
                      int        insertPos,
                      int        maxLen)
    {
        if (IsNull(insStr))
            return;

        int insLen = StringLength(insStr);

        int currLen = StringLength(destStr);

        int nshift = currLen - insertPos;

        // shift
        int srcPos = currLen - 1;
        int destPos = srcPos + insLen;
        for (int i = 0; i < nshift; i++)
            {
                if (destPos < maxLen)
                    destStr[destPos] = destStr[srcPos];
                destPos--;
                srcPos--;
            }

        int newLen = currLen + insLen;
        if (newLen >= maxLen)
            newLen = maxLen - 1;
        destStr[newLen] = '\0';

        destPos = insertPos;
        maxLen--;
        for (int i = 0; i < insLen; i++)
            {
                if (destPos < maxLen)
                    destStr[destPos] = insStr[i];
                destPos++;
            }
    }


    void ConcatString(char       destStr[],
                      const char addStr[],
                      int        maxLen)
    //  adds addStr to end of destStr up to maxLen - 1 characters total in dest
    //  pads with \0 until maxLen
    {
        if (IsNull(addStr))
            return;

        int nextChar = StringLength(destStr);
        int copyPos = 0;
        for (int i = nextChar; i < maxLen - 1; i++ )
            {
                destStr[i] = addStr[copyPos];
                if ( destStr[i] != '\0')
                    copyPos++;
            }
        destStr[maxLen - 1] = '\0';
    }


    void PrepareParseString(char* inStr, char* parseStr)
    // if any chars other than blank are to be used
    {
        int nin = StringLength(inStr);
        for (int i = 0; i < nin; i++)
            if (strchr(parseStr, inStr[i]) != NULL)
                inStr[i] = ' ';
    }


    bool ParseGetNextString(char*   inStr, char* nextStr, int maxLen)
    {
        FullTrim(inStr);

        //  copy up to blank or NULL
        int lastCopy = maxLen - 1;
        for (int i = 0; i < maxLen; i++)
            {
                nextStr[i] = inStr[i];

                //  last portion ?
                if (nextStr[i] == '\0')
                    return (i > 0);

                // clear char from input
                inStr[i] = ' ';

                //  parse char ?
                if (nextStr[i] == ' ')
                    {
                        lastCopy = i;
                        break;
                    }
            }

        //  set end of str
        nextStr[lastCopy] = '\0';
        return (lastCopy > 0);
    }




    void MakeString(      char destStr[],
                          const char baseStr[],
                          const char addStr[],
                          int  maxLen)
    {
        CopyString(destStr, baseStr, maxLen);
        ConcatString(destStr, addStr, maxLen);
    }

    void MakeString(      char destStr[],
                          const char baseStr[],
                          const char addStr1[],
                          const char addStr2[],
                          int  maxLen)
    {
        CopyString(destStr, baseStr, maxLen);
        ConcatString(destStr, addStr1, maxLen);
        ConcatString(destStr, addStr2, maxLen);
    }

    void MakeString(      char destStr[],
                          const char baseStr[],
                          const char addStr1[],
                          const char addStr2[],
                          const char addStr3[],
                          int  maxLen)
    {
        CopyString(destStr, baseStr, maxLen);
        ConcatString(destStr, addStr1, maxLen);
        ConcatString(destStr, addStr2, maxLen);
        ConcatString(destStr, addStr3, maxLen);
    }

    void MakeString(      char destStr[],
                          const char baseStr[],
                          const char addStr1[],
                          const char addStr2[],
                          const char addStr3[],
                          const char addStr4[],
                          int  maxLen)
    {
        CopyString(destStr, baseStr, maxLen);
        ConcatString(destStr, addStr1, maxLen);
        ConcatString(destStr, addStr2, maxLen);
        ConcatString(destStr, addStr3, maxLen);
        ConcatString(destStr, addStr4, maxLen);
    }
    void MakeString(      char destStr[],
                          const char baseStr[],
                          const char addStr1[],
                          const char addStr2[],
                          const char addStr3[],
                          const char addStr4[],
                          const char addStr5[],
                          int  maxLen)
    {
        CopyString(destStr, baseStr, maxLen);
        ConcatString(destStr, addStr1, maxLen);
        ConcatString(destStr, addStr2, maxLen);
        ConcatString(destStr, addStr3, maxLen);
        ConcatString(destStr, addStr4, maxLen);
        ConcatString(destStr, addStr5, maxLen);
    }
    void MakeString(      char destStr[],
                          const char baseStr[],
                          const char addStr1[],
                          const char addStr2[],
                          const char addStr3[],
                          const char addStr4[],
                          const char addStr5[],
                          const char addStr6[],
                          int  maxLen)
    {
        CopyString(destStr, baseStr, maxLen);
        ConcatString(destStr, addStr1, maxLen);
        ConcatString(destStr, addStr2, maxLen);
        ConcatString(destStr, addStr3, maxLen);
        ConcatString(destStr, addStr4, maxLen);
        ConcatString(destStr, addStr5, maxLen);
        ConcatString(destStr, addStr6, maxLen);
    }
    void MakeString(      char destStr[],
                          const char baseStr[],
                          const char addStr1[],
                          const char addStr2[],
                          const char addStr3[],
                          const char addStr4[],
                          const char addStr5[],
                          const char addStr6[],
                          const char addStr7[],
                          int  maxLen)
    {
        CopyString(destStr, baseStr, maxLen);
        ConcatString(destStr, addStr1, maxLen);
        ConcatString(destStr, addStr2, maxLen);
        ConcatString(destStr, addStr3, maxLen);
        ConcatString(destStr, addStr4, maxLen);
        ConcatString(destStr, addStr5, maxLen);
        ConcatString(destStr, addStr6, maxLen);
        ConcatString(destStr, addStr7, maxLen);
    }

    void MakeString(      char destStr[],
                          const char baseStr[],
                          const char addStr1[],
                          const char addStr2[],
                          const char addStr3[],
                          const char addStr4[],
                          const char addStr5[],
                          const char addStr6[],
                          const char addStr7[],
                          const char addStr8[],
                          int  maxLen)
    {
        CopyString(destStr, baseStr, maxLen);
        ConcatString(destStr, addStr1, maxLen);
        ConcatString(destStr, addStr2, maxLen);
        ConcatString(destStr, addStr3, maxLen);
        ConcatString(destStr, addStr4, maxLen);
        ConcatString(destStr, addStr5, maxLen);
        ConcatString(destStr, addStr6, maxLen);
        ConcatString(destStr, addStr7, maxLen);
        ConcatString(destStr, addStr8, maxLen);
    }

    void UpperCase(char inStr[])
    {
        int len = StringLength(inStr);
        for (int i = 0; i < len; i++)
            if (islower(inStr[i]))
                inStr[i] = toupper(inStr[i]);
    }

    void LowerCase(char inStr[])
    {
        int len = StringLength(inStr);
        for (int i = 0; i < len; i++)
            if (isupper(inStr[i]))
                inStr[i] = tolower(inStr[i]);
    }


    void RightJustify(char inStr[],
                      int  len)
    //  inserts leading blanks until inStr is at len
    {
        int currLen = StringLength(inStr);
        if ( currLen >= len)
            return;
        int nAdd = len - currLen;

        int i;
        for (i = len - 1; i >= nAdd; i--)
            inStr[i] = inStr[i - nAdd];

        for ( i = 0; i < nAdd; i++)
            inStr[i] = blank;

        inStr[len] = '\0';
    }


    void LeftJustify(char inStr[],
                     int  len)
    //  adds trailing blanks until inStr is at len
    {
        int currLen = StringLength(inStr);
        if ( currLen >= len)
            return;

        for (int i = currLen; i < len; i++)
            inStr[i] = blank;
        inStr[len] = '\0';
    }

    void CenterJustify(char inStr[],
                       int  len)
    //  inserts leading  and trailing blanks until inStr is at len
    {
        int currLen = StringLength(inStr);
        int rLen = (len - currLen) / 2;
        if (rLen <= 0)
            return;
        RightJustify(inStr, rLen + currLen);
        LeftJustify(inStr, len);
    }

    void FillBlanks(char inStr[])
    {
        FillBlanks(inStr, '_');
    }

    void FillBlanks(char inStr[], char fillChar)
    {
        //  replaces blanks in inStr with underscores, or other char if fillChar specified
        int currLen = StringLength(inStr);
        for ( int i = 0; i < currLen; i++)
            if (inStr[i] == blank)
                inStr[i] = fillChar; //'_';
    }

    void ReplaceUnderscores(char inStr[])
    {
        ReplaceUnderscores(inStr, '_');
    }

    void ReplaceUnderscores(char inStr[], char fillChar)
    {
        int currLen = StringLength(inStr);
        for ( int i = 0; i < currLen; i++)
            if (inStr[i] == fillChar) //'_')
                inStr[i] = blank;
    }

    void ReplaceNonNumeric(char inStr[])
    {
        int currLen = StringLength(inStr);
        for ( int i = 0; i < currLen; i++)
            if ((inStr[i] < '0') || (inStr[i] > '9'))
                inStr[i] = blank;
    }

    void FillWithChar(char inStr[],
                      char fillChar,
                      int  len)
    {
        for (int i = 0; i < len; i++)
            inStr[i] = fillChar;
        inStr[len] = '\0';
    }


    int  CharPositn(const char inStr[],
                    char findChar,
                    int  startPos)
    // returns position of findChar in str, starting at startPos, -1 if not found
    {
        int currLen = StringLength(inStr);
        for ( int i = startPos; i < currLen; i++)
            if (inStr[i] == findChar)
                return i;

        return -1;
    }

    void  DoubleApostrophes(char       destStr[],
                            const char srcStr[],
                            int        maxLen)
    // copies string, replacing single apostrophes with double apostrophes
    {
        if (maxLen < 1)
            return;

        if (IsNull(srcStr))
            {
                SetToNull(destStr);
                return;
            }

        int len = StringLength(srcStr);
        if (len >= maxLen)
            len = maxLen - 1;

        for (int i = 0; i < len; i++)
            {
                if (srcStr[i] == '\'')
                    destStr[i] = '"';
                else
                    destStr[i] = srcStr[i];

            }
        destStr[len] = '\0';
    }

    void ExtractFileName(char       destStr[],
                         const char fullStr[],
                         int       maxLen)
    {
        if ( fullStr == 0)
            {
                destStr[0] = '\0';
                return;
            }

        //  char after find trailing slash
        int slashPos = 0;
        int i;
        int len = int(StringLength(fullStr));
        for (i = 0; i < len; i++)
            if ((fullStr[i] == '/') || (fullStr[i] == '\\'))
                slashPos = i + 1;

        //  copy to end
        for (i = 0; i < maxLen - 1; i++)
            {
                destStr[i] = fullStr[slashPos];
                if ( destStr[i] != '\0')
                    slashPos++;
            }
        destStr[maxLen - 1] = '\0';
    }

    void ExtractFileExtension(  char        destStr[],
                                const char  fullStr[],
                                int             maxLen)
    {
        if ( fullStr == 0)
            {
                destStr[0] = '\0';
                return;
            }

        //  char after find decimal point
        int slashPos = 0;
        int i;
        int len = int(StringLength(fullStr));
        for (i = 0; i < len; i++)
            if (fullStr[i] == '.')
                slashPos = i + 1;

        //  copy to end
        for (i = 0; i < maxLen - 1; i++)
            {
                destStr[i] = fullStr[slashPos];
                if ( destStr[i] != '\0')
                    slashPos++;
                else
                    break;
            }
        destStr[maxLen - 1] = '\0';
        FullTrim(destStr);
    }


    void ExtractFilePath(char       destStr[],
                         const char fullStr[],
                         int       maxLen)
    {
        destStr[0] = '\0';
        if ( fullStr == 0)
            return;

        //  char after find trailing slash
        int slashPos = -1;
        int len = int(StringLength(fullStr));
        for (int i = 0; i < len; i++)
            if ((fullStr[i] == '/') || (fullStr[i] == '\\'))
                slashPos = i;

        if (slashPos < 0)
            return;

        //  special case for root
        if (slashPos == 0)
            {
                destStr[0] = '/';
                destStr[1] = '\0';
                return;
            }

        CopyString(destStr, fullStr, slashPos + 1);
        destStr[slashPos] = '\0';
    }

    void ExtractFolderPath(char       destStr[],
                           const char fullStr[],
                           int        maxLen)
    {
        destStr[0] = '\0';
        if ( fullStr == 0)
            return;

        //  char after find trailing slash
        int slashPos = -1;
        int len = int(StringLength(fullStr));
        for (int i = 0; i < (len-1); i++) //don't check last slash if there is one
            if ((fullStr[i] == '/') || (fullStr[i] == '\\'))
                slashPos = i;

        if (slashPos < 0)
            return;

        //  special case for root
        if (slashPos == 0)
            {
                destStr[0] = '/';
                destStr[1] = '\0';
                return;
            }

        if (fullStr[slashPos] == '/')
            MakeString(destStr, fullStr, "/", slashPos + 2);  //add space for slash and null char
        else //if (fullStr[slashPos] == '\\')
            MakeString(destStr, fullStr, "\\", slashPos + 2);  //add space for slash and null char
        destStr[slashPos+1] = '\0';
    }

    void MakeFullFileName(char       fullName[],
                          const char pathStr[],  // made by ExtractFilePath (root handling)
                          const char fileStr[],
                          int       maxLen)
    {
        CopyString(fullName, pathStr, maxLen);
        if (StringLength(pathStr) > 1)
            ConcatString(fullName, "/", maxLen);
        ConcatString(fullName, fileStr, maxLen);
    }

    bool    AddFileNameSuffix(char*          fName,
                              const char* fExt,
                              const char* suffix,
                              int       maxLen)

    {
        char* extPos = strstr(fName, fExt);
        if (extPos == NULL)
            return false;

        *extPos ='\0';

        // concat suffix & extensiom
        ConcatString(fName, suffix, maxLen);
        ConcatString(fName, fExt, maxLen);

        return true;
    }

    void IntToString(int   iVal,
                     char  str[],
                     int   maxLen)
    {
        {
            using namespace std;
            ostringstream ostr;
            ostr << iVal << ends;
            CopyString(str, ostr.str().c_str(), maxLen);
        }
        FullTrim(str);
    }

    void IntToString(int iVal, char str[], int padLen, int maxLen)
    {
        IntToString(iVal, str, maxLen);
        RightJustify(str, padLen);
        // replace leading blanks with 0
        int len = int(StringLength(str));
        for (int i = 0; i < len; i++)
            {
                if (NotWhiteSpace(str[i]))
                    break;
                str[i] = '0';
            }
    }


    void RealToString(const SC_RealFormat& rForm, const double& rVal, char str[], int maxLen)
    {
        rForm.RealToString(rVal, str, maxLen);
    }


    bool IsValidReal(const char         inStr[],
                     double&   dVal)
    {
        // modified behaviour FB Case 43 - incorrect D exponent processing
        // used to fail on stream conversion, now OK just ignores D

        // look for D or d
        char  temp[160];
        if (strrchr(inStr, 'D') || strrchr(inStr, 'd'))
            {
                CopyString(temp, inStr, 160);
                char* dchar = strrchr(temp, 'D');
                if (dchar == NULL)
                    dchar = strrchr(temp, 'd');

                if (dchar == NULL)
                    GenAppInternalError("U_Str::IsValidReal - no D");

                *dchar = 'E';
                inStr = temp;
            }

        {
            // try simple case first
            using namespace std;
            istringstream iStr(inStr);
            iStr >> dVal;
            if (iStr.fail() == 0)
                return true;
        }

        // further processing
        if (temp != inStr) // not already copied for D case above
            CopyString(temp, inStr, 160);
        FullTrim(temp);

        // null kluge added for testing
        if ((temp[0] == 'n') && (strcmp(temp, "null") == 0))
            {
                dVal = nullReal;
                return true;
            }
        if ((temp[0] == 'N') && (strcmp(temp, "NaN") == 0))
            {
                dVal = nullReal;
                return true;
            }

        // european char replacement
        if (SC_RealFormat::european)
            {
                char* comma = strrchr(temp, ',');
                if (comma)
                    *comma = '.';
            }


        using namespace std;
        istringstream iStr(temp);

        iStr >> dVal;

        bool badConv = iStr.fail() != 0;
        if (badConv)
            {
                // check for FRAC3DVS E-31
                if ((strstr(temp, "E-32") != NULL) ||
                    (strstr(temp, "E-31") != NULL) ||
                    (strstr(temp, "E-30") != NULL))
                    {
                        dVal = 0.0;
                        return true;
                    }

                dVal = nullReal;
            }

        return !badConv;
    }

    bool IsValidInt(const char  inStr[],
                    int&  iVal,
                    bool  leadZero)
    {
        if (!leadZero)
            {
                // basic case first
                using namespace std;
                istringstream iStr(inStr);
                iStr >> iVal;
                if (iStr.fail() == 0)
                    return true;
            }

        char  temp[160];
        CopyString(temp, inStr, 160);
        FullTrim(temp);
        if (leadZero)
            {
                int strLen = StringLength(temp) - 1;
                for (int i = 0; i < strLen; i++)
                    {
                        leadZero = leadZero && (temp[i] == '0');
                        if (leadZero)
                            temp[i] = ' ';
                    }
                FullTrim(temp);
            }


        using namespace std;

        istringstream iStr(temp);
        iStr >> iVal;

        return !iStr.fail();
    }

    void ConcatInt(int iVal, char str[], int maxLen)
    {
        char intStr[25];
        IntToString(iVal, intStr, 25);
        ConcatString(str, intStr, maxLen);
    }

    void ConcatInt(int iVal, char str[], int padLen, int maxLen)
    {
        char intStr[25];
        IntToString(iVal, intStr, padLen, 25);
        ConcatString(str, intStr, maxLen);

    }

    void ConcatInt(int iVal, const char* preStr, char str[], int padLen, int maxLen)
    {
        char intStr[25];
        IntToString(iVal, intStr, padLen, 25);
        MakeString(str, preStr, intStr, maxLen);
    }


    void SetToNull(char* str)
    {
        if (str != NULL)
            str[0] = '\0';
    }

    bool SameString(const char* str1, const char* str2)
    {
        if (IsNull(str1) || IsNull(str2))
            {
                return str1 == str2;
            }

        return strcmp(str1, str2) == 0;
    }

    bool SameUCString(const char* str1, const char* str2)
    {
        if (IsNull(str1) || IsNull(str2))
            {
                return str1 == str2;
            }

        if (strcmp(str1, str2) == 0)
            return true;

        int len1 = StringLength(str1);
        if (int(StringLength(str2)) != len1)
            return false;

        for (int i = 0; i < len1; i++)
            {
                char char1 = str1[i];
                if (islower(char1))
                    char1 = toupper(char1);
                char char2 = str2[i];
                if (islower(char2))
                    char2 = toupper(char2);
                if (char1 != char2)
                    return false;
            }

        return true;
    }

    int     GetFirstNum(const char  inStr[])
    {
        if (inStr == 0)
            return -1;

        //copy number
        char numString[10];  //num consequences not likely to be more than 1e4
        int indx = 0;
        int nChar = StringLength(inStr);
        bool gotNum = false;
        for (int i=0; i<nChar; i++)
            {
                if (CharIsNumeric(inStr[i]))
                    {
                        numString[indx++] = inStr[i];
                        gotNum = true;
                    }
                else
                    {
                        if (!gotNum)
                            continue;
                        numString[indx] = '\0';
                        break;
                    }
            }

        if (!gotNum)
            return -1;

        //convert number string to int
        int num;
        if (!IsValidInt(numString, num, true))  //there may be lead zeros
            return -1;

        return num;
    }

    int     GetSecondNum(const char     inStr[])
    {
        if (inStr == 0)
            return -1;

        //copy number
        char numString[10];  //num consequences not likely to be more than 1e4
        int indx = 0;
        int nChar = StringLength(inStr);
        bool gotNum = false;
        bool gotFirstNum = false;
        bool finishedFirstNum = false;
        for (int i=0; i<nChar; i++)
            {
                if (CharIsNumeric(inStr[i]))
                    {
                        if (finishedFirstNum)
                            {
                                numString[indx++] = inStr[i];
                                gotNum = true;
                            }
                        gotFirstNum = true;
                    }
                else
                    {
                        if (!gotNum)
                            {
                                if (gotFirstNum)
                                    finishedFirstNum = true;
                                continue;
                            }
                        numString[indx] = '\0';
                        break;
                    }
            }

        //convert number string to int
        int num;
        if (!IsValidInt(numString, num, true))  //there may be lead zeros
            return -1;

        return num;
    }
    bool    CharIsNumeric(char inChar)
    {
        if ((inChar < '0') || (inChar > '9'))
            return false;
        else
            return true;
    }

};

