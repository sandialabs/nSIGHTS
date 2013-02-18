///////////////////////////////////////////////////////////////////////////////////
//
// IO_LabelArray.cpp
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
// DESCRIPTION: Platform independent and application independent base classes
//              and support classes: Reads/Wrietes data class from/to files.
//
///////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genClass/IO_LabelArray.h>

IO_LabelArray::I_LabelArrayFile::I_LabelArrayFile(const char* inputFile)
    : ConvFileC(inputFile, 8192)
{
}

void IO_LabelArray::I_LabelArrayFile::Init()
{
    nextInc = 5;
    nextCount = 50;
    CFsetComment("#!");
    SetParseChars(",");
}

void IO_LabelArray::I_LabelArrayFile::ReadPoints(DO_LabelArray&    labelArray)
{

    SC_CoordArray&   coordsData = labelArray.dataCoords;
    SC_StringArray&  labelData  = labelArray.dataLabels;


    Coord3D readCoord;
    bool lastWasNull = false;
    char  currLab[DO_LabelArray::maxLabelLen];
    while (true)
        {
            bool nonBlank =  CFreadText();
            if (CFeof())
                break;

            if (!nonBlank)
                {
                    //  skip multiple blanks or leading blanks
                    if (lastWasNull || coordsData.IsEmpty())
                        continue;

                    // default constructor sets to null
                    readCoord = Coord3D();
                    currLab[0] = '\0';
                    lastWasNull = true;
                }
            else
                {
                    readCoord.cX = CFgetNextDouble();
                    readCoord.cY = CFgetNextDouble();
                    readCoord.cZ = CFgetNextDouble();
                    // remainder is label
                    FullTrim(parsedLine);
                    CopyString(currLab, parsedLine, DO_LabelArray::maxLabelLen);
                    lastWasNull = false;
                }
            coordsData += readCoord;
            labelData += currLab;
        }

    //  clear off end nulls
    int lastOK = 0;
    for (int i = 0; i < coordsData.Size(); i++)
        if (!coordsData[i].CoordIsNull())
            lastOK = i;

    coordsData.SetSize(lastOK + 1);
    labelData.SetSize(lastOK + 1);
}

void IO_LabelArray::I_LabelArrayFile::ReadDXF(DO_LabelArray&    labelArray)
{
    SC_CoordArray&   coordsData = labelArray.dataCoords;
    SC_StringArray&  labelData  = labelArray.dataLabels;

    Coord3D readCoord;
    bool lastWasNull = false;
    char  currLab[DO_LabelArray::maxLabelLen];
    while (true)
        {
            bool nonBlank =  CFreadText();
            if (CFeof())
                break;
            if (!nonBlank)
                continue;

            // looks for POLYLINE, grabs layer & first vertex
            CFtrim();
            // look for LINE or POLYLINE
            if (!CFmatches("POLYLINE"))
                continue;

            if (!CFreadUntil(" 8", false))
                CFtextError("Expecting 8 in POLYLINE layer id");

            // next line is layer ID - using as label
            CFreadText();
            FullTrim(parsedLine);
            CopyString(currLab, parsedLine, DO_LabelArray::maxLabelLen);

            // now read coord
            if (!CFreadUntil("VERTEX", false))
                CFtextError("Expecting VERTEX in POLYLINE");

            // find
            if (!CFreadUntil(" 10", 1, false))
                CFtextError("Expecting 10 in POLYLINE VERTEX");

            CFreadDXFCoord(readCoord);


            coordsData += readCoord;
            labelData += currLab;
        }
}



IO_LabelArray::O_LabelArrayFile::O_LabelArrayFile(const char* outputFile)
    :TxtFileC(outputFile, false)
{
}

void IO_LabelArray::O_LabelArrayFile::Init()
{
}

void IO_LabelArray::O_LabelArrayFile::WritePoints(const DO_LabelArray& labelArray)
{
    for ( int i = 0; i < labelArray.Size(); i++)
        {
            WriteDouble(labelArray.dataCoords[i].cX);
            WriteSolidText("\t");
            WriteDouble(labelArray.dataCoords[i].cY);
            WriteSolidText("\t");
            WriteDouble(labelArray.dataCoords[i].cZ);
            WriteSolidText("\t");
            WriteText(labelArray.dataLabels[i]);

        }
};



bool IO_LabelArray::ReadLabelFile(LabelFileFormat    labelFileFormat,
                                  const char*        inputFileName,
                                  DO_LabelArray&     labelArray)
{
    try
        {
            I_LabelArrayFile cmFile(inputFileName);
            cmFile.CFsetMsgStuff("ReadLabelArray", "LabelArrayFile", 100);
            cmFile.Init();
            switch (labelFileFormat)
                {
                case lffPoints : {
                    cmFile.ReadPoints(labelArray);
                    break;
                }
                case lffDXF : {
                    cmFile.ReadDXF(labelArray);
                    break;
                }
                case lffOther : {
                    GenAppErrorMsg("ReadLabelArray", "other not supported");
                    return false;
                    break;
                }
                }
        }
    catch (ConvFileC::CFReadError re) {
        labelArray.Reset("Labels");
        GenAppErrorMsg("ReadLabelArray", re.errMsg);
        return false;
    }

    {
        using namespace std;
        ostringstream ostr;
        ostr <<  "Label Data read OK: " << labelArray.Size() << " labels "<< ends;
        GenAppInfoMsg("ReadLabelArray", ostr.str().c_str());
    }
    return true;
}

void IO_LabelArray::WriteLabelFile(const char*          outputFileName,
                                   const DO_LabelArray& labelArray)
{
    try
        {
            O_LabelArrayFile cmFile(outputFileName);
            cmFile.Init();
            cmFile.WritePoints(labelArray);
        }
    catch (TextC::TextError re) {
        GenAppErrorMsg("WriteLabelArray", re.errMsg);
        return;
    }

    GenAppInfoMsg("WriteLabelArray", "file written successfully");
    return;
}

