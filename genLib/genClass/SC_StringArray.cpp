///////////////////////////////////////////////////////////////////////////////////
//
// SC_StringArray.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/SC_StringArray.h>

SC_StringArray::SC_StringArray()
{
    strLenAlloc     = 40;
    labelListData   = 0;
    stringBuffer    = 0;
};
SC_StringArray::SC_StringArray(int lenAlloc)
{
    strLenAlloc     = lenAlloc;
    labelListData   = 0;
    stringBuffer    = 0;
};

SC_StringArray::SC_StringArray(const SC_StringArray& a)
{
    labelListData   = 0;
    stringBuffer    = 0;
    strLenAlloc     = a.GetStringLen();
    Alloc(a.nlistAlloc);
    nlistData = a.Size();
    resizable = a.resizable;
    sorted    = a.sorted;
    for ( int i = 0; i < nlistData; i++)
        CopyString(labelListData[i], a[i], strLenAlloc);
};

SC_StringArray :: ~SC_StringArray()
{
    DeAlloc();
}

char*& SC_StringArray::operator[](int indx)
{
    if (BadIndex(indx))
        SC_ArrayError("op []");

    return labelListData[indx];
}

char*  SC_StringArray::operator[](int indx) const
{
    if (BadIndex(indx))
        SC_ArrayError("op [] const");

    return labelListData[indx];
}

bool  SC_StringArray::operator==(const SC_StringArray& a)
{
    if (Size() != a.Size())
        return false;

    for (int i = 0; i < Size(); i++)
        if (!SameString((*this)[i], a[i]))
            return false;

    return true;
}

char* SC_StringArray::LastIndex() const
{
    if ((nlistAlloc == 0) || (nlistData == 0))
        SC_ArrayError("LastIndex");
    return labelListData[nlistData - 1];
}

void SC_StringArray::SetStringLen(int newLen)
{
    if (labelListData == 0)
        strLenAlloc = newLen;
}

void  SC_StringArray::ForceMinStringLen(int newLen)
{
    if (newLen > strLenAlloc)
        {
            DeAlloc();
            strLenAlloc = newLen;
        }
}

void SC_StringArray::DeAlloc()
{
    delete [] stringBuffer;
    delete [] labelListData;
    labelListData = 0;
    stringBuffer = 0;
    SC_Array::DeAlloc();
}

SC_StringArray& SC_StringArray::operator= (const SC_StringArray& a)
{
    if ( this != &a)
        {
            ForceMinStringLen(a.strLenAlloc);
            Alloc(a.nlistAlloc);
            nlistData = a.Size();
            resizable = a.resizable;
            sorted    = a.sorted;
            for (int i = 0; i < nlistData; i++)
                CopyString((*this)[i], a[i], strLenAlloc);
        }
    return *this;
}

SC_StringArray& SC_StringArray::operator+= (const char* a)
{
    if (nlistData == nlistAlloc)
        if (resizable)
            ReAlloc(nlistData * 2 + 1);
        else
            SC_ArrayError("::+=");

    CopyString(labelListData[nlistData++], a, strLenAlloc);
    return *this;
}


SC_StringArray&  SC_StringArray::operator+=(const SC_StringArray& a)
{
    for ( int i = 0; i < a.Size(); i++)
        (*this) += a[i];
    return *this;
}


bool SC_StringArray::ChkAlloc(int nAll)
{
    nlistData = 0;
    if (nAll <= nlistAlloc)
        return true;
    DeAlloc();

    try
        {
            labelListData = new char*[nAll];
        }
    catch (std::bad_alloc)
        {
            labelListData = 0;
            return false;
        }

    // force 4 byte alignment
    int allocLen = strLenAlloc / 4;
    if ((strLenAlloc % 4) != 0)
        allocLen++;
    allocLen *= 4;

    try
        {
            stringBuffer  = new char[allocLen * nAll];
        }
    catch (std::bad_alloc)
        {
            delete [] labelListData;
            labelListData = 0;
            stringBuffer = 0;
            return false;
        }

    if (stringBuffer == 0)
        GenAppInternalError("SC_StringArray::ChkAlloc");

    nlistAlloc = nAll;

    int currIndex = 0;
    for (int j = 0; j < nlistAlloc; j++ )
        {
            labelListData[j] = &stringBuffer[currIndex];
            currIndex += allocLen;
            SetToNull(labelListData[j]);
        }

    return true;
}


void SC_StringArray::ReAlloc(int nnewAll)
{
    SC_StringArray tmpData(strLenAlloc);
    tmpData = *this;

    Alloc(nnewAll);

    if (nnewAll < tmpData.Size())
        nlistData = nnewAll;
    else
        nlistData = tmpData.Size();

    for (int i = 0; i < nlistData; i++)
        CopyString(labelListData[i], tmpData[i], strLenAlloc);
}


void SC_StringArray:: Clear()
{
    if (nlistAlloc == 0)
        Alloc(1);
    nlistData = 0;
    for (int j = 0; j < nlistAlloc; j++ )
        SetToNull(labelListData[j]);
}

void SC_StringArray::SetString(const char inLab[],
                               int        listPos)
{
    if ((listPos < 0) || (listPos >= nlistAlloc))
        SC_ArrayError("SetString");

    CopyString(labelListData[listPos], inLab, strLenAlloc);
    if (listPos >= nlistData)
        nlistData = listPos + 1;
}

void SC_StringArray::AddString(const char inLab[],
                               int        listPos)
{
    if ((listPos < 0) || (listPos >= nlistAlloc))
        SC_ArrayError("AddString");

    ConcatString(labelListData[listPos], inLab, strLenAlloc);
    if (listPos >= nlistData)
        nlistData = listPos + 1;
}


void SC_StringArray::InsertItem(const char inLab[],
                                int        insertPos)
{
    if (IsFull())
        if (resizable)
            ReAlloc(nlistData * 2 + 1);
        else
            SC_ArrayError("InsertItem");

    if (BadIndex(insertPos))
        SC_ArrayError("InsertItem");

    if (insertPos < Size())
        {
            char* lastAlloc = labelListData[nlistData];
            for (int i = nlistData; i > insertPos; i--)
                labelListData[i] = labelListData[i - 1];
            labelListData[insertPos] = lastAlloc;

            SetSize(Size() + 1);
        }
    else
        SetSize(insertPos + 1);

    CopyString(labelListData[insertPos], inLab, strLenAlloc);
}


void SC_StringArray::DeleteItem(int delPos)
{
    if (BadIndex(delPos))
        SC_ArrayError("DeleteItem");

    if (delPos < Size())
        {
            char* delAlloc = labelListData[delPos];
            for (int i = delPos; i < UpperBound(); i++)
                labelListData[i] = labelListData[i + 1];
            labelListData[UpperBound()] = delAlloc;
            SetToNull(labelListData[UpperBound()]);
            SetSize(Size() - 1);
        }
    else
        CopyString(labelListData[delPos], "", strLenAlloc);

}

bool SC_StringArray::SearchForKey(const char key[],
                                  int& findIndx) const

{
    if ( ! sorted)
        {
            for ( int i = 0; i < nlistData; i++)
                if (SameString(labelListData[i], key))
                    {
                        findIndx = i;
                        return true;
                    }
            findIndx = -1;
            return false;
        }

    // binary search
    int l = 0;
    int r = nlistData - 1;
    int cmpres;
    do
        {
            findIndx = (l + r) / 2;
            cmpres = strcmp(labelListData[findIndx], key);
            if ( cmpres == 0)
                return true;

            if ( cmpres < 0)
                l = findIndx + 1;
            else
                r = findIndx - 1;

        } while ( l < r);

    //  key not found so find insertpos
    int newIndx = (l + r) / 2;
    if ( newIndx != findIndx)
        {
            findIndx = newIndx;
            cmpres = strcmp(labelListData[findIndx], key);
        }
    if ( cmpres < 0)                    // data in vec at pos < keyID
        findIndx++;

    return cmpres == 0;
}

int  SC_StringArray::SearchForKey(const char key[]) const
{
    int retVal;
    if (SearchForKey(key, retVal))
        return retVal;
    return -1;
}

bool SC_StringArray::AddIfNotFound(const char newStr[])
{
    if ( IsEmpty())
        {
            (*this) += newStr;
            return true;
        }

    int insertPos;
    if (SearchForKey(newStr, insertPos))
        return true;

    if (IsFull() && (!IsResizable()))
        return false;

    if (sorted)
        InsertItem(newStr, insertPos);
    else
        (*this) += newStr;

    return true;
}


void SC_StringArray::ApplyReverseMap(const SC_IntArray& reverseMap)
{
    int lastMap = -1;
    for (int i = 0; i < Size(); i++)
        {
            int nextMap = reverseMap[i];
            if (nextMap >= 0)
                {
                    if (nextMap < i)
                        CopyString(labelListData[nextMap], labelListData[i], strLenAlloc);
                    lastMap = nextMap;
                }
        }
    SetSize(lastMap + 1);
}


void  SC_StringArray::Sort()
{
    SC_IntArray dummy;
    Sort(dummy);
}

void  SC_StringArray::Sort(SC_IntArray&  indexes)

{
    bool swapInt = indexes.Size() == Size();
    for ( int i = 0; i < (Size() - 1); i++)
        for ( int j = i + 1; j < Size(); j++)
            if (strcmp(labelListData[i], labelListData[j]) > 0)
                {
                    char* tmp = labelListData[i];
                    labelListData[i] = labelListData[j];
                    labelListData[j] = tmp;
                    if ( swapInt)
                        {
                            int tInt = indexes[i];
                            indexes[i] = indexes[j];
                            indexes[j] = tInt;
                        }
                }

    sorted = true;
}


int SC_StringArray::GetMaxStringLen() const
{
    int maxLen = 0;
    for (int i = 0; i < nlistData; i++)
        {
            int labLen = StringLength(labelListData[i]);
            if ( labLen > maxLen)
                maxLen = labLen;
        }
    return maxLen;
}

bool SC_StringArray::CreateFromString(const char* inputString,
                                      const char* tokenString)
{
    int inLen = StringLength(inputString);
    int tokLen = StringLength(tokenString);
    // first pass -- count number of tokens
    const char* nextToken = strstr(inputString, tokenString);

    //      no tokens, simple case
    if (nextToken == NULL)
        {
            DeAlloc();
            if (inLen == 0)
                return false;
            SetStringLen(inLen + 1);
            Alloc(1);
            SetString(inputString, 0);
            return true;
        }

    //      count lines and find max length
    int currLen = (int) (nextToken - inputString);
    int maxLen = currLen;
    const char* prevToken;

    int nLines = 1;
    while (nextToken != NULL)
        {
            //      skip token itself
            nextToken += tokLen;

            //      keep next start
            prevToken = nextToken;

            // get next start
            nextToken = strstr(nextToken, tokenString);
            if (nextToken == NULL)
                currLen = (int) (&inputString[inLen] - prevToken);
            else
                currLen = (int) (nextToken - prevToken);
            if (currLen > maxLen)
                maxLen = currLen;
            nLines++;
        }

    DeAlloc();
    SetStringLen(maxLen + 1);  // extra for term char
    Alloc(nLines);

    // parse and add
    prevToken = inputString;
    nextToken = strstr(inputString, tokenString);
    nLines = 0;
    bool done = false;
    while (true)
        {
            char* currStr = (*this)[nLines];
            const char* nextChar;
            for (nextChar = prevToken; nextChar < nextToken; nextChar++)
                (*currStr++) = (*nextChar);
            *currStr = '\0';
            nLines++;

            // break if at end
            if (done)
                break;

            nextToken += tokLen;
            prevToken = nextToken;
            nextToken = strstr(nextToken, tokenString);

            // if null set to end
            if ( nextToken == NULL)
                {
                    nextToken = &inputString[inLen];
                    done = true;
                }
        }

    SetSize(nLines);
    return true;

}

bool SC_StringArray::ExtractQuotedStrings(const char* inputString)
{
    const char quoteChr = '\"';

    // first pass -- count number of quotes
    const char* nextToken = strchr(inputString, quoteChr);

    int nStr = 0;
    int maxLen = 0;
    while(nextToken)
        {
            const char* matchToken = strchr(++nextToken, quoteChr);
            if (!matchToken)
                break;
            nStr++;
            int quoteLen = (int) (matchToken - nextToken);
            if (quoteLen > maxLen)
                maxLen = quoteLen;

            nextToken = strchr(++matchToken, quoteChr);
        }

    if (nStr == 0)
        {
            DeAlloc();
            return false;
        }

    ForceMinStringLen(maxLen + 2);
    AllocAndSetSize(nStr);

    // repeat and add
    nextToken = strchr(inputString, quoteChr);
    nStr = 0;
    while(nextToken)
        {
            const char* matchToken = strchr(++nextToken, quoteChr);
            if (!matchToken)
                break;

            char* startStr = (*this)[nStr];
            while (nextToken != matchToken)
                {
                    *startStr=*nextToken;
                    startStr++;
                    nextToken++;
                }
            *startStr = '\0';
            nStr++;

            nextToken = strchr(++matchToken, quoteChr);
        }

    return true;
}



void SC_StringArray::Clean()
{
    int lastNonBlank = -1;
    for (int i = 0; i < Size(); i++)
        {
            char* currStr = (*this)[i];
            int currLen = StringLength(currStr);
            for (int j = 0; j < currLen; j++)
                {
                    if (currStr[j] < ' ')
                        currStr[j] = ' ';
                }

            TrimLeft(currStr);
            if (StringLength(currStr) > 0)
                lastNonBlank = i;
        }
    SetSize(lastNonBlank + 1);
}


void SC_StringArray::SetIndexed(const char* baseDesc, int offset)
{
    int ndesc = AllocSize();
    int maxVal = ndesc + offset;
    int npad = 2;
    if (maxVal > 100000)
        npad = 6;
    else if (maxVal > 10000)
        npad = 5;
    else if (maxVal > 1000)
        npad = 4;
    else if (maxVal > 100)
        npad = 3;

    char tempStr[256];
    for (int i = 0; i < ndesc; i++)
        {
            CopyString(tempStr, baseDesc, 256);
            ConcatInt(i + offset, tempStr, npad, 256);
            SetString(tempStr, i);
        }
    SetSizeToAlloc();
}

