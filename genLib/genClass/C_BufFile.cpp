///////////////////////////////////////////////////////////////////////////////////
//
// C_BufFile.cpp
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
//              and support classes.
//              Reads/writes binary formatted files
//
///////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <genClass/C_Common.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_File.h>

#include <genClass/C_BufFile.h>

const float BufFileC::nullFloat = -1.0E+35F;

using namespace std;


BufFileC::BufferFileError::BufferFileError(const char* eMsg)
{
    CopyString(errMsg, eMsg, bufferFileErrLen);
}

BufFileC::BufFileC(int bufSize)
{
    bufferSize      = bufSize;
    buffer          = 0;
    if (bufSize > 0)
        buffer      = new char [bufferSize];

    bufferFile      = 0;
    currRecNum      = 0;
    currByteNum     = 0;
    openWasOK       = false;
    realsAreFloats  = false;
}

BufFileC::~BufFileC()
{
    delete [] buffer;
    if (bufferFile != 0)
        {
#ifdef MSCVS2005
            fclose(bufferFile);
#else
            bufferFile->close();
            delete bufferFile;
#endif
        }
}

void BufFileC::OpenFile(const char fName[],
                        bool forRead,
                        bool readOnly)
{
    entireFileRead = false;

    // for write or size > 0 must be allocated
    if (((!forRead) || (bufferSize >= 0)) && (buffer == 0))
        BufferError("buffer not allocated  ??");

    if (bufferFile != 0)
        GenAppInternalError("BufFileC::OpenFile - file already open");

#ifndef MSCVS2005
    bufferFile = new fstream();
    if (bufferFile == 0)
        BufferError("error creating file");
#endif

    bool tmpSuccess = false;
    if (forRead)
        {

            /*  moved to generic U_File routine - JDA
                ifstream tmp;
                tmp.open(fName, ios::in);
                tmp.seekg(0, ios::end);
                fileSize = tmp.tellg();
                if (tmp.fail())
                BufferError("failure determining file length");
                else
                tmpSuccess = true;
            */

            // kluge to get length: MS specific
            if (bufferSize < 0)
                {
                    entireFileRead = true;
                    // paranoi
                    if (buffer != 0)
                        BufferError("buffer already allocated ??");

                    /*
                      Win32 code

                      int fh;
                      fh = _open(fName, _O_RDONLY);
                      if(fh == -1 )
                      BufferError("error getting file length");
                      bufferSize = _filelength(fh);
                      _close(fh);
                    */

                    /*              ned -- I moved this above so I could get access to fileSize when forRead && !(bufferSize < 0)
                                    gcc compatible
                                    ifstream tmp;
                                    tmp.open(fName, ios::in);
                                    tmp.seekg(0, ios::end);
                                    bufferSize = tmp.tellg();
                                    if (tmp.fail())
                                    BufferError("failure determining file length");
                                    else
                                    tmpSuccess = true;
                    */

                    bufferSize = GetFileSize(fName);  // in U_File - JDA - 20 Feb 06
                    if (bufferSize < 0)
                        BufferError("error getting file size");

                    if (bufferSize == 0)
                        BufferError("file has zero length");

                    buffer  = new char [bufferSize];
                    tmpSuccess = true;
                }

#ifdef MSCVS2005
            const char* attrib = "r+b";
            if (readOnly)
                attrib = "rb";
            bufferFile = _fsopen(fName, attrib, _SH_DENYNO);
            if (bufferFile == NULL)
                if (readOnly)
                    BufferError("error on x64 fopen rb");
                else
                    BufferError("error on x64 fopen r+b");
#else
            if (readOnly)
                bufferFile->open(fName, ios::in | ios::out | ios::binary );
            else
                bufferFile->open(fName, ios::in | ios::binary );
#endif
        }
    else
        {
#ifdef MSCVS2005
            bufferFile = _fsopen(fName, "w+b", _SH_DENYNO);
            if (bufferFile == NULL)
                {
                    BufferError("error on x64 fopen w+b");
                }
#else
            bufferFile->open(fName, ios::in | ios::out | ios::trunc | ios::binary);
#endif
        }

    char fileName[stdFileStrLen];
    ExtractFileName(fileName, fName, stdFileStrLen);
    char errStr[80];
    if (tmpSuccess)
        CopyString(errStr, " (check if read-only?)", 80);
    else
        CopyString(errStr, " (bad path or file name?)", 80);
    char tempStr[80];
    MakeString(tempStr, "error opening binary ", fileName, errStr, 80);
    CheckBufferError(tempStr);

    BufferReset(0);
    openWasOK = true;
}


void BufFileC::CloseFile()
{
    if (openWasOK)
        if (bufferFile == 0)
            GenAppInternalWarning("BufFileC::CloseFile - file NULL");
        else
            {
#ifdef MSCVS2005
                fclose(bufferFile);
#else
                bufferFile->close();
#endif
            }

#ifndef MSCVS2005
    delete bufferFile;
#endif
    openWasOK  = false;
    bufferFile = 0;
}

void  BufFileC::CheckBufferError(const char* errMsg)
{
#ifdef MSCVS2005
    if (ferror(bufferFile) != 0)
        BufferError(errMsg);
#else
    if (bufferFile->fail())
        BufferError(errMsg);
#endif
}

char BufFileC::Peek()
{
#ifdef MSCVS2005
    char tmp;
    fread(&tmp, 1, 1, bufferFile);
    _fseeki64(bufferFile, -1, SEEK_CUR);
    return tmp;
#else
    return bufferFile->peek();
#endif
}

bool BufFileC::IsEOF()
{
    if (entireFileRead)
        return currByteNum >= (bufferSize - 1);
    // modified so EOF at last byte, not when past end

#ifdef MSCVS2005
    return feof(bufferFile)!= 0;
#else
    return bufferFile->eof() != 0;
#endif
}

void BufFileC::WriteAbsolute(int recNum)
{
    BUFSIZETYPE  recStartPos = BUFSIZETYPE(recNum) * bufferSize;
#ifdef MSCVS2005
    _fseeki64(bufferFile, recStartPos, SEEK_SET);
    CheckBufferError("x64 write seek error");
    fwrite(buffer, bufferSize, 1, bufferFile);
    CheckBufferError("x64 write error");
#else
    bufferFile->seekg(recStartPos);
    CheckBufferError("write seek error");
    bufferFile->write(buffer, bufferSize);
    CheckBufferError("write error");
#endif

    BufferReset(recNum);
}

void BufFileC::WriteCurrent()
{
    WriteAbsolute(currRecNum);
}

void BufFileC::ReadAbsolute(int recNum)
{
    BUFSIZETYPE recStartPos = BUFSIZETYPE(recNum) * bufferSize;
#ifdef MSCVS2005
    _fseeki64(bufferFile, recStartPos, SEEK_SET);
    CheckBufferError("x64 read seek error");

    fread(buffer, bufferSize, 1, bufferFile);
    CheckBufferError("x64 read error");
#else
    bufferFile->seekg(recStartPos);
    CheckBufferError("read seek error");

    bufferFile->read(buffer, bufferSize);
    CheckBufferError("read error");
#endif
    BufferReset(recNum);
}

void BufFileC::ReadNextRecord()
{
    int nextRecNum = currRecNum;
    if (currByteNum > 0)
        nextRecNum++;
    ReadAbsolute(nextRecNum);
}

void BufFileC::WriteSequentialBytes(const void* dataAdr,
                                    int   nbytes)
{
    char* temp = static_cast<char*> (const_cast<void*> (dataAdr));
    for ( int i = 0; i < nbytes; i++)
        {
            if (currByteNum >= bufferSize)
                {
                    WriteCurrent();
                    currRecNum++;
                    currByteNum = 0;
                }
            buffer[currByteNum++] = temp[i];
        }
}

void BufFileC::ReadSequentialBytes(void* dataAdr,
                                   int   nbytes)
{
    char* temp = static_cast<char*> (dataAdr);
    for ( int i = 0; i < nbytes; i++)
        {
            if (currByteNum >= bufferSize)
                {
                    currRecNum++;
                    ReadAbsolute(currRecNum);
                    currByteNum = 0;
                }
            temp[i] = buffer[currByteNum++];
        }
}

void BufFileC::FlushBuffer(int& nextRecNum)
{
    if (currByteNum == 0)
        nextRecNum = currRecNum;
    else
        {
            for (int i = currByteNum; i < bufferSize; i++)
                buffer[i] = '\0';
            WriteCurrent();
            nextRecNum = currRecNum + 1;
        }
}
void BufFileC::FlushBuffer()
{
    int dummy;
    FlushBuffer(dummy);
}


void BufFileC::BufferReset(int recNum)
{
    currRecNum  = recNum;
    currByteNum = 0;
}

void BufFileC::BufferError(const char* errMsg)
{
    throw BufferFileError(errMsg);
}


void BufFileC::WriteString(const char* fText, int strSize)
{
    WriteSequentialBytes(fText, strSize);
}
void BufFileC::WriteBool(bool fBool)
{
    bool tbool = fBool;
    WriteSequentialBytes(&tbool, sizeof(bool));
}
void BufFileC::WriteInt(int fInt)
{
    int tint = fInt;
    WriteSequentialBytes(&tint, sizeof(int));
}
void BufFileC::WriteLong(long fLong)
{
    WriteSequentialBytes(&fLong, sizeof(long));
}
void BufFileC::WriteDouble(const double& fDouble)
{
    WriteSequentialBytes(&fDouble, sizeof(double));
}
// written as double even if realsAreFloats is true
void BufFileC::WriteReal(const double& fReal)
// written as double if realsAreFloats is false
// written as float if realsAreFloats is true
{
    if (realsAreFloats)
        {
            if (RealIsNull(fReal))
                {
                    WriteSequentialBytes(&nullFloat, sizeof(float));
                    return;
                }

            float tFloat;
            try
                {
                    tFloat = float(fReal);
                }
            catch (...)
                {
                    BufferError("float conversion");
                }
            WriteSequentialBytes(&tFloat, sizeof(float));
        }
    else
        WriteDouble(fReal);
}


//  reading routines
void BufFileC::ReadString(char* fText, int strSize)
{
    ReadSequentialBytes(fText, strSize);
}
void BufFileC::ReadBool(bool& fBool)
{
    ReadSequentialBytes(&fBool, sizeof(bool));
}

bool BufFileC::ReadBool()
{
    bool retVal;
    ReadSequentialBytes(&retVal, sizeof(bool));
    return retVal;
}

void BufFileC::ReadInt(int& fInt)
{
    ReadSequentialBytes(&fInt, sizeof(int));
}

int BufFileC::ReadInt()
{
    int retVal;
    ReadSequentialBytes(&retVal, sizeof(int));
    return retVal;
}

void BufFileC::ReadLong(long& fLong)
{
    ReadSequentialBytes(&fLong, sizeof(long));
}

void BufFileC::ReadDouble(double& fDouble)
{
    ReadSequentialBytes(&fDouble, sizeof(double));
}

double BufFileC::ReadDouble()
{
    double retVal;
    ReadSequentialBytes(&retVal, sizeof(double));
    return retVal;
}


void BufFileC::ReadReal(double& fReal)
// reads double if realsAreFloats is false
// reads floats and converts if realsAreFloats is true
{
    if (realsAreFloats)
        {
            float tFloat;
            ReadSequentialBytes(&tFloat, sizeof(float));
            if (tFloat == nullFloat)
                fReal = nullReal;
            else
                fReal = double(tFloat);
        }
    else
        ReadDouble(fReal);
}


void BufFileC::WriteStdFileHeader(const char* headerID,
                                  int         headerSize,
                                  int         majorVerNum,
                                  int         minorVerNum)
{
    BufferReset(0);

    WriteString(headerID, headerSize);
    WriteInt(majorVerNum);
    WriteInt(minorVerNum);
}

void BufFileC::ReadStdFileHeader(const char* headerID,
                                 int         headerSize,
                                 int&        majorVerNum,
                                 int&        minorVerNum)
{
    ReadAbsolute(0);
    char* tmp = new char[headerSize];
    ReadString(tmp, headerSize);
    if (strcmp(headerID, tmp) != 0)
        {
            delete [] tmp;
            BufferError("headers don't match");
        }
    ReadInt(majorVerNum);
    ReadInt(minorVerNum);
    delete [] tmp;
}


void BufFileC::WriteRealArray(const SC_DoubleArray& fRealArray,
                              int&                  nextRec)
{
    for ( int i = 0; i < fRealArray.Size(); i++)
        WriteReal(fRealArray[i]);
    FlushBuffer(nextRec);
}
void BufFileC::ReadRealArray(SC_DoubleArray& fRealArray, int nRead)
// as for ReadReal
{
    fRealArray.Alloc(nRead);
    for (int i = 0; i < nRead; i++)
        ReadReal(fRealArray[i]);
    fRealArray.SetSize(nRead);
}

void  BufFileC::WriteIntArray(const SC_IntArray&  intVals,
                              int&                nextRec)
{
    WriteIntArray(intVals);
    FlushBuffer(nextRec);
}

void  BufFileC::WriteIntArray(const SC_IntArray&  intVals)
{
    WriteInt(intVals.Size());
    for (int i = 0; i < intVals.Size(); i++)
        WriteInt(intVals[i]);
}

void BufFileC::WriteMappedIntArray(const SC_IntArray& intData,
                                   const SC_IntArray& validMap)
{
    for (int i = 0; i < validMap.Size(); i++)
        WriteInt(intData[validMap[i]]);
}


void BufFileC::ReadIntArray(SC_IntArray&  intVals)
{
    intVals.AllocAndSetSize(ReadInt());
    for (int i = 0; i < intVals.Size(); i++)
        ReadInt(intVals[i]);
}

void BufFileC::ReadMappedIntArray(SC_IntArray&       intData,
                                  const SC_IntArray& validMap)
{
    for (int i = 0; i < validMap.Size(); i++)
        intData[validMap[i]] = ReadInt();
}


void  BufFileC::WriteDoubleArray(const SC_DoubleArray&  dblVals,
                                 int&                   nextRec)
{
    WriteDoubleArray(dblVals);
    FlushBuffer(nextRec);
}

void  BufFileC::WriteDoubleArray(const SC_DoubleArray&  dblVals)
{
    WriteInt(dblVals.Size());
    for (int i = 0; i < dblVals.Size(); i++)
        WriteDouble(dblVals[i]);
}

void BufFileC::WriteMappedDoubleArray(const SC_DoubleArray& dblData,
                                      const SC_IntArray&    validMap)
{
    for (int i = 0; i < validMap.Size(); i++)
        WriteDouble(dblData[validMap[i]]);
}

void BufFileC::ReadDoubleArray(SC_DoubleArray&  dblVals)
{
    dblVals.AllocAndSetSize(ReadInt());
    for (int i = 0; i < dblVals.Size(); i++)
        ReadDouble(dblVals[i]);
}

void BufFileC::ReadMappedDoubleArray(SC_DoubleArray&       dblData,
                                     const SC_IntArray&    validMap)
{
    for (int i = 0; i < validMap.Size(); i++)
        dblData[validMap[i]] = ReadDouble();
}

void  BufFileC::WriteDoubleMatrix(const SC_DoubleMatrix&  dblVals,
                                  int&                    nextRec)
{
    WriteDoubleMatrix(dblVals);
    FlushBuffer(nextRec);
}

void  BufFileC::WriteDoubleMatrix(const SC_DoubleMatrix&  dblVals)
{
    WriteInt(dblVals.Size());
    for (int i = 0; i < dblVals.Size(); i++)
        WriteDoubleArray(dblVals[i]);
}

void BufFileC::ReadDoubleMatrix(SC_DoubleMatrix&  dblVals)
{
    dblVals.AllocAndSetSize(ReadInt());
    for (int i = 0; i < dblVals.Size(); i++)
        ReadDoubleArray(dblVals[i]);
}

void  BufFileC::WriteStringArray(const SC_StringArray&  strVals,
                                 int&                   nextRec)
{
    WriteStringArray(strVals);
    FlushBuffer(nextRec);
}

void  BufFileC::WriteStringArray(const SC_StringArray&  strVals)
{
    WriteInt(strVals.Size());
    int nchar = strVals.GetStringLen();
    WriteInt(nchar);
    for (int i = 0; i < strVals.Size(); i++)
        WriteString(strVals[i], nchar);
}

void BufFileC::ReadStringArray(SC_StringArray&  strVals)
{
    int nstr = ReadInt();
    int nchar = ReadInt();
    if (nchar > strVals.GetStringLen())
        {
            strVals.DeAlloc();
            strVals.SetStringLen(nchar);
        }
    strVals.AllocAndSetSize(nstr);
    for (int i = 0; i < strVals.Size(); i++)
        ReadString(strVals[i], nchar);
}

void  BufFileC::WriteBoolArray(const SC_BoolArray&  boolVals,
                               int&                 nextRec)
{
    WriteBoolArray(boolVals);
    FlushBuffer(nextRec);
}

void  BufFileC::WriteBoolArray(const SC_BoolArray&  boolVals)
{
    WriteInt(boolVals.Size());
    for (int i = 0; i < boolVals.Size(); i++)
        WriteBool(boolVals[i]);
}

void  BufFileC::ReadBoolArray(SC_BoolArray&  boolVals)
{
    boolVals.AllocAndSetSize(ReadInt());
    for (int i = 0; i < boolVals.Size(); i++)
        ReadBool(boolVals[i]);
}

void BufFileC::WindowsToUnix(const char* fileName)
{
    BufFileC unixFile(-1);

    unixFile.OpenFile(fileName, true);
    unixFile.ReadAbsolute(0);
    for (int i = 0; i < unixFile.bufferSize; i++)
        {
            if (unixFile.buffer[i] == '\r')
                unixFile.buffer[i] = ' ';
        }
    unixFile.WriteAbsolute(0);

    unixFile.CloseFile();
}


void BufFileC::WindowsToUnix(bool  convertToUnix, // calls base if flag true
                             const char* fileName)
{
    if (convertToUnix)
        WindowsToUnix(fileName);
}

