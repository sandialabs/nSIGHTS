///////////////////////////////////////////////////////////////////////////////////
//
// SC_RealFormat.cpp
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

#include <math.h>
#include <sstream>

#include <genClass/U_Str.h>

#include <genClass/C_Common.h>

#include <genClass/SC_RealFormat.h>


bool SC_RealFormat::european = false;  // if true dec delimiter is ,

SC_RealFormat& SC_RealFormat::operator= (const SC_RealFormat& a)
{
    if (&a != this)
        {
            format = a.format;
            digitsPrecision = a.digitsPrecision;
        }
    return *this;
}


bool SC_RealFormat::StarString (char inStr[],
                                char outStr[],
                                int  maxLen)

/* procedure used to indicate overflow on conversion to string routines.
   if in_str is shorter than or equal to the maximimum allowed by outstring
   then in_str is copied to out_str. Otherwise out_str is completely filled
   with asterisks.
*/
{
    FullTrim(inStr);
    if (int(StringLength(inStr)) < maxLen)
        CopyString(outStr, inStr, maxLen);
    else
        {
            for (int i = 0; i < maxLen - 1; i ++)
                outStr[i] = '*';
            outStr[maxLen - 1] = '\0';
            return false;
        }

    return true;
}


void SC_RealFormat::SciConversion (char             inStr[],
                                   const double&    rVal,
                                   int              nDigits,
                                   int              maxLen)
{
    if (nDigits > 15)
        nDigits = 15;

    // bug/difference in new <iostream> -- precision 0 not supported
    if (nDigits == 0)
        {
            // massive kluge
#ifdef MSCVS2005
            _snprintf_s(inStr, maxLen, maxLen, "%.0E", rVal);
#else
            sprintf(inStr, "%.0E", rVal);
#endif


        }
    else
        {
            using namespace std;
            ostringstream ostr;
            ostr.flags(ios::uppercase);
            ostr.setf(ios::scientific, ios::floatfield);
            ostr.precision(nDigits);
            ostr << rVal << ends;
            CopyString(inStr, ostr.str().c_str(), maxLen);
        }

    FullTrim(inStr);

    // delete extra 0 in exponent
    int len = StringLength(inStr);
    if ((len > 3) && (inStr[len-3] == '0'))
        {
            inStr[len-3] = inStr[len-2];
            inStr[len-2] = inStr[len-1];
            inStr[len-1] = '\0';
        }
}

bool SC_RealFormat::GetExponent(char    inStr[],
                                double&         mVal,
                                int&    eVal)
{
    // look for 'E'
    char* ePos = strchr(inStr, int('E'));
    if (ePos == NULL)
        return false;
    // go past E
    ePos++;

    // get rest of string
    char temp[40];
    CopyString(temp, ePos, 40);

    // get/kill sign if any
    int multVal = 1;
    if (temp[0] == '+')
        temp[0] = '0';

    if (temp[0] == '-')
        {
            temp[0] = '0';
            multVal = -1;
        }

    bool retVal = IsValidInt(temp, eVal, true);
    if (retVal)
        {
            eVal *= multVal;
            char temp[40];
            CopyString(temp, inStr, 40);
            ePos = strchr(temp, int('E'));
            *ePos = '\0'; // stop at E
            retVal = IsValidReal(temp, mVal);
        }
    return retVal;
}



void SC_RealFormat::Sci2Conversion (char  inStr[],
                                    int   maxLen)
{
    int eVal;
    double mVal;
    if (!GetExponent(inStr, mVal, eVal))
        return;

    // stop at 'E'
    char* ePos = strchr(inStr, int('E'));
    *ePos = '\0';

    double tVal;
    // special case for 0.0
    if (IsValidReal(inStr, tVal) && (tVal == 0.0))
        {
            int ndig = StringLength(inStr);
            if (ndig > 2) // decimal place processing
                ndig -= 2;
            else
                ndig = 0;
            FixedConversion(inStr, 0.0, ndig, maxLen);
            return;
        }

    // add 10 stuff
    ConcatString(inStr, "x10<^", maxLen);
    ConcatInt(eVal, inStr, maxLen);
    ConcatString(inStr, "^>", maxLen);

}


void SC_RealFormat::ExpConversion (char inStr[],
                                   int  maxLen)
{
    int eVal;
    double mVal;
    if (!GetExponent(inStr, mVal, eVal))
        return;

    // mantissa must be close to 1.0
    double fabsM = fabs(mVal);
    if (((fabsM > 1.01) && (fabsM < 9.99)) || (fabsM < 0.99))
        {
            // do Sci2
            Sci2Conversion (inStr, maxLen);
            return;
        }

    if (fabsM > 9.9)
        eVal++;

    if (mVal < 0.0)
        CopyString(inStr, "-", maxLen);
    else
        SetToNull(inStr);


    if (eVal == 0)
        {
            ConcatString(inStr, "1", maxLen);
        }
    else
        {
            ConcatString(inStr, "10<^", maxLen);
            ConcatInt(eVal, inStr, maxLen);
            ConcatString(inStr, "^>", maxLen);
        }
}

void SC_RealFormat::Exp2Conversion (char inStr[],
                                    int  maxLen)
{
    int eVal;
    double mVal;
    if (!GetExponent(inStr, mVal, eVal))
        return;

    // mantissa must be close to 1.0
    double fabsM = fabs(mVal);
    if (((fabsM > 1.01) && (fabsM < 9.99)) || (fabsM < 0.99))
        {
            // do Sci2
            Sci2Conversion (inStr, maxLen);
            return;
        }

    if (fabsM > 9.9)
        eVal++;

    if ((eVal < -1) || (eVal > 2))
        {
            ExpConversion (inStr, maxLen);
        }
    else
        {
            char* tempStr;
            switch (eVal)   {
            case -1 : {
                tempStr = "0.1";
                break;
            }
            case 0 : {
                tempStr = "1";
                break;
            }
            case 1 : {
                tempStr = "10";
                break;
            }
            case 2 : {
                tempStr = "100";
                break;
            }
            }

            if (mVal < 0.0)
                CopyString(inStr, "-", maxLen);
            else
                SetToNull(inStr);
            ConcatString(inStr, tempStr, maxLen);
        }
}

bool SC_RealFormat::FixedConversion (char             inStr[],
                                     const double&    rVal,
                                     int              n_digits,
                                     int              maxLen)
{
    double temp = fabs(rVal);
    if (temp > 1.0E+06)
        {
            double log_data = log10(temp);
            int  n_place = int(ceil(log_data));
            if ((n_digits + n_place) > 18)
                return false;
        }
    n_digits = n_digits > 10 ? 10 : n_digits;

    {
        using namespace std;
        ostringstream ostr;
        ostr.flags(ios::showpoint);
        ostr.setf(ios::fixed, ios::floatfield);
        ostr.precision(n_digits);
        ostr << rVal << ends;
        CopyString(inStr, ostr.str().c_str(), maxLen);
    }

    // special case for n_digits == 0
    if (n_digits == 0)
        {
            // delete trailing decimal if there is one
            for (int i=0; i < maxLen; i++)
                {
                    if (inStr[i] == '.')
                        {
                            inStr[i]='\0';
                            break;
                        }
                    if (inStr[i] == '\0')
                        break;
                }
        }

    FullTrim(inStr);
    return true;
}

void SC_RealFormat::DefaultConversion (char             inStr[],
                                       const double&    rVal,
                                       int             maxLen)
{
    // check for small param
    double  temp = fabs(rVal);

    // close to 0 or greater than 0.00001 but less than  9 999 999 999 ?
    if (((temp < 1.0E-75) || (temp > 1.0E-5)) && (temp < 1.0E+11))
        {
            // try fixed point with seven dec
            if (FixedConversion(inStr, rVal, 7, maxLen))
                {
                    // OK, delete trailing zeroes
                    int len = StringLength(inStr) - 1;
                    while (inStr[len] == '0')
                        {
                            inStr[len] = '\0';
                            len--;
                        }

                    // is last decimal place ? if not then OK
                    if (inStr[len] != '.')
                        return;

                    // if less than 8 fixed is OK
                    if (len <  9)
                        {
                            // 100. should be 100
                            if (len >  3)
                                inStr[len] = '\0';
                            else
                                {
                                    // 99. should be 99.0
                                    len ++;
                                    inStr[len] = '0';
                                    inStr[len+1] = '\0';
                                }
                            return;
                        }
                }
        }

    // do sci conversion
    SciConversion(inStr, rVal, 8, maxLen);

    // delete excess 0s

    // find exponent sign
    int e_pos = 0;
    while (inStr[e_pos] != 'E')
        e_pos++;

    // work backward to find pos of last 0
    int last_zero = e_pos;
    int i;
    for (i = e_pos - 1; i > 0; i--)
        {
            if (inStr[i] != '0')
                break;
            last_zero = i;
        }

    //  make sure there is at least one 0 after the point
    if (inStr[last_zero - 1]== '.')
        last_zero++;

    // get rid of excess
    if (last_zero < e_pos)
        {
            int n_zero = e_pos - last_zero;
            //  delete n_zero chars starting at last_zero
            int len = StringLength(inStr) - n_zero;

            for ( i = last_zero; i < len; i++)
                inStr[i] = inStr[i + n_zero];
            inStr[len] = '\0';
        }

}

void SC_RealFormat::AddDecimalSpaces(char inStr[],
                                     int  formatSpec,
                                     int  maxLen)
{
    int actLen = StringLength(inStr);
    int currLen = actLen;
    if (inStr[0] == '-')
        currLen--;

    int prec = formatSpec %4;

    if ((prec == 1) && (currLen < 5))
        return;
    if ((prec == 2) && (currLen < 6))
        return;
    if ((prec > 2) && (currLen < 7))
        return;

    int nsp = currLen / 3;
    if ((currLen % 3) == 0)
        nsp--;

    if (nsp < 1)
        return;

    const char* insStr = " ";
    if (formatSpec > 3)
        insStr = ",";

    int spPos = actLen - 3;
    for (int i = 0; i < nsp; i++)
        {
            InsertString(inStr, insStr, spPos, maxLen);
            spPos -= 3;
        }
}


bool SC_RealFormat::RealToString(const double& rVal,
                                 char          str[],
                                 int           maxLen) const
{
    if ( RealIsNull(rVal))
        {
            CopyString(str, "null", maxLen);
            return false;
        }

    char tempStr[128];
    switch ( format)
        {
        case ncf_General :  {
            DefaultConversion(tempStr, rVal, 128);
            break;
        }
        case ncf_Decimal :  {
            FixedConversion(tempStr, rVal, digitsPrecision, 128);
            break;
        }
        case ncf_Scientific :  {
            SciConversion(tempStr, rVal, digitsPrecision, 128);
            break;
        }
        case ncf_Scientific2 :  {
            SciConversion(tempStr, rVal, digitsPrecision, 128);
            Sci2Conversion(tempStr, 128);
            break;
        }
        case ncf_Exponent :  {
            SciConversion(tempStr, rVal, 2, 128);
            ExpConversion(tempStr, 128);
            break;
        }
        case ncf_Exponent2 :  {
            SciConversion(tempStr, rVal, 2, 128);
            Exp2Conversion(tempStr, 128);
            break;
        }
        case ncf_DecimalSpace :  {
            FixedConversion(tempStr, rVal, 0, 128);
            AddDecimalSpaces(tempStr, digitsPrecision, 128);
            break;
        }
        }

    if (european)
        {
            int nstr = StringLength(tempStr);
            for (int i = 0; i < nstr; i++)
                if (tempStr[i] == '.')
                    tempStr[i] = ',';
        }

    StarString(tempStr, str, maxLen);
    return(str[0] != '*');
}


#ifdef _WIN32
bool SC_RealFormat::RealToString(const double& rVal,
                                 CString&  str) const
{
    char tempStr[160];
    bool result = RealToString(rVal, tempStr, 160);
    str = tempStr;
    return result;
}

#endif

char* SC_RealFormat::GetCformat() const
{

    static char* sci_formats[] = {
        "%5.0E",
        "%6.1E",
        "%7.2E",
        "%8.3E",
        "%9.4E",
        "%10.5E",
        "%11.6E",
        "%12.7E",
        "%13.8E",
        "%14.9E",
        "%15.10E"};

    static char* fix_formats[] = {
        "%1.0f",
        "%2.1f",
        "%3.2f",
        "%4.3f",
        "%5.4f",
        "%6.5f",
        "%7.6f",
        "%8.7f",
        "%9.8f",
        "%10.9f",
        "%11.10f"};

    static char* g_format = "%g";


    switch ( format)
        {
        case ncf_General :  return (g_format);
        case ncf_Decimal :  return (fix_formats[digitsPrecision]);
        default :  ;
        }
    return (sci_formats[digitsPrecision]);
}

