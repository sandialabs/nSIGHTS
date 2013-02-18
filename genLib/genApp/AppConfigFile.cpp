///////////////////////////////////////////////////////////////////////////////////
//
// AppConfigFile.cpp
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              Provides basic support for object serialization to/from text files or clipboard.
//              Provides object header support and general data type I/O.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include <genClass/C_TxtFile.h>
#include <genClass/C_FuncObj.h>
#include <genClass/U_Str.h>

#include <genClass/SC_StringArray.h>
#include <genClass/SC_IntArray.h>
#include <genClass/SC_PointArray.h>
#include <genClass/C_MPISupport.h>

#include <genDataClass/DSC_RealMSSpec.h>
#include <genDataClass/DSC_IndexMSSpec.h>
#include <genDataClass/DSC_Threshold.h>

#ifndef ADCONSOLEAPP
#include <genPlotClass/PSC_Font.h>
#include <genClassPS/PS_TxtClipboard.h>
#include <genApp/ProjectUtil.h>
#include <genApp/DefaultObjects.h>
#endif

#include <genApp/AppConfigFile.h>

namespace appConfigFile {

    //  statics
    TextC*        cfgObject = 0;
    char          currConfigFileName[stdFileStrLen] = {""};

    int           currentCfgFileReadMajorVersion = 0;
    int           currentCfgFileReadMinorVersion = 0;

    int           currentObjReadMajorVersion = 0;
    int           currentObjReadMinorVersion = 0;
    FuncObjArray  prevVerSystemObjects;  // for nPre backward compatibility only

    bool          usePageObjectVersioning = false;


    //      for file implementation
    static TxtFileC txtCfgFile;

    void OpenAsFile(const char* fName, bool openForRead)
    {
        CopyString(currConfigFileName, fName, stdFileStrLen);
        cfgObject = &txtCfgFile;
        txtCfgFile.OpenFile(fName, openForRead);
    }

#ifndef ADCONSOLEAPP

    //      for clipboard implementation
    static PS_TxtClipboard  clipCfgFile;

    void OpenAsClipboard(bool openForRead)
    {
        cfgObject = &clipCfgFile;
        if (!clipCfgFile.OpenTxtClipboard(!openForRead))
            clipCfgFile.ThrowTextError("cannot open clipboard");
    }
#endif // ADCONSOLEAPP

    void Close()
    {
        cfgObject->Close();
        cfgObject = 0;
    }


    // sets currObjReadMajorVersion & currObjReadMajorVersion
    void  ReadGroupHeader(const char*  headerToMatch, int  maxMajorVer)
    {
        char tempHeader[stdFileStrLen];
        static char errMsg[80];

        ReadFileName(tempHeader);
        if (strcmp(tempHeader, headerToMatch) != 0)
            {
                CopyString(errMsg, "Header read:  ", 80);
                ConcatString(errMsg, headerToMatch, 80);
                cfgObject->ThrowTextError(errMsg);
            }

        currentObjReadMajorVersion = ReadInt();
        currentObjReadMinorVersion = ReadInt();

        if (currentObjReadMajorVersion > maxMajorVer)
            {
                CopyString(errMsg, "Reading future version:  ", 80);
                ConcatString(errMsg, headerToMatch, 80);
                cfgObject->ThrowTextError(errMsg);
            }

        cfgObject->CheckTextError("Reading file header");
    }
    // ******************************************  app group

    static const char* appGlobalHeader = "AppCommonGlobals";
    //enum    {majorVersionNum = 0};
    //enum    {majorVersionNum = 1};   // added tree view controls
    //enum    {majorVersionNum = 2};   // removed tree view controls, revised system objects
    //enum    {majorVersionNum = 3};   // added new default objects
    enum    {majorVersionNum = 4};   // added page object versioning

    void WriteAppGlobals()
    {
        WriteGroupHeader(appGlobalHeader, majorVersionNum, 0);

        //  number of FO objects
        WriteInt(FuncObjC::funcObjList.Size());
        WriteLine();

        //  writes all objects (system objects will be written here but we cant be sure where, or in what order)
        for (int i = 0; i < FuncObjC::funcObjList.Size(); i++)
            WriteFuncObj(FuncObjC::funcObjList[i]);

        // current app system objects
#ifndef ADCONSOLEAPP
        WriteGlobalFunc(appDefaultObjects::sysInfoObj);
        WriteGlobalFunc(appDefaultObjects::defaultPenObj);
        WriteGlobalFunc(appDefaultObjects::coldHotMapObj);
        WriteGlobalFunc(appDefaultObjects::whiteBlackMapObj);
#else
        // dummy write
        for (int i = 0; i < 4; i++)
            WriteFuncObj(FuncObjC::funcObjList[0]);
#endif // ADCONSOLEAPP
    }

    void ReadAppGlobals()
    {
        ReadGroupHeader(appGlobalHeader, majorVersionNum);

        usePageObjectVersioning = (currentObjReadMajorVersion > 3);

        if (currentObjReadMajorVersion > 1)
            {
                ReadFuncObjList();
#ifndef ADCONSOLEAPP
                ReadGlobalFunc(appDefaultObjects::sysInfoObj);
                if (currentObjReadMajorVersion > 2)
                    {
                        ReadGlobalFunc(appDefaultObjects::defaultPenObj);
                        ReadGlobalFunc(appDefaultObjects::coldHotMapObj);
                        ReadGlobalFunc(appDefaultObjects::whiteBlackMapObj);
                    }

#else
                // all will be > ver 1
                for (int i = 0; i < 4; i++)
                    FuncObjC* dummy = ReadFuncObj();

#endif //ADCONSOLEAPP

                return;
            }

#ifndef ADCONSOLEAPP
        // all cmd line will be new
        // older version
        ReadBool();   // small fonts
        if (currentObjReadMajorVersion > 0)
            {
                // tre settings
                int nSingleLine = ReadInt();
                int nSingleLineType = ReadInt();
            }

        int nSysToRead = ReadInt();
        ReadFuncObjList();

        // sys info was always first
        FuncObjC::updateData[0].newRef = &appDefaultObjects::sysInfoObj;

        // app specific objects start at 1
        for (int i = 1; i < nSysToRead; i++)
            FuncObjC::updateData[i].newRef = prevVerSystemObjects[i - 1];

#endif // ADCONSOLEAPP

    }


    // ******************************************  File I/O

    void WriteFuncObj(const FuncObjC* fFuncObj)
    {
        WritePointer(fFuncObj);
        WriteLine();
    }

    FuncObjC*  ReadFuncObj()
    {
        return static_cast<FuncObjC*>(ReadPointer());
    }

    void  WriteFuncObjRef(const FuncObjRef& fFuncObjRef)
    {
        WritePointer(fFuncObjRef.objRef);
        WriteInt(fFuncObjRef.objIndx);
        WriteLine();
    }

    void ReadFuncObjRef(FuncObjRef& rFuncObjRef)
    {
        rFuncObjRef.objRef  = ReadFuncObj();
        rFuncObjRef.objIndx = ReadInt();
    }

    //  ----------------------------------------- ptr matching

    void  DeallocFuncObjList()
    {
        FuncObjC::updateData.DeAlloc();
    }

    void ReadFuncObjList()
    {
        int nobjSaveList = ReadInt();
        CheckForInputFailure("Reading # of objects");
        FuncObjC::updateData.AllocAndSetSize(nobjSaveList);
        for (int i = 0; i < nobjSaveList; i++)
            FuncObjC::updateData[i].oldRef = ReadFuncObj();

        CheckForInputFailure("Reading object address list");
    }

    void ReadAndMatchFuncObj(const FuncObjC* currFO)
    {
        const FuncObjC* oldThis = ReadFuncObj();
        CheckForInputFailure("Reading old object address");

        for (int i = 0; i < FuncObjC::updateData.Size(); i++)
            {
                const FuncObjC* oldRef = FuncObjC::updateData[i].oldRef;
                if (oldRef == oldThis)
                    {
                        FuncObjC::updateData[i].newRef = currFO;
                        return;
                    }
            }
        InputError("No object address match");
    }


    void WriteGlobalFunc(const FuncObjC& globFunc)
    {
        WriteFuncObj(&globFunc);
    }
    void ReadGlobalFunc(const FuncObjC& globFunc)
    {
        ReadAndMatchFuncObj(&globFunc);
    }


    void WriteRealFormat(const SC_RealFormat& inForm)
    {
        WriteInt(inForm.format         );
        WriteInt(inForm.digitsPrecision);
    }
    void ReadRealFormat(SC_RealFormat& inForm)
    {
        inForm.format             = NumConvFormat(ReadInt());
        inForm.digitsPrecision    = ReadInt();
    }


    void WriteStringArray(const SC_StringArray& strings)
    {

        WriteInt(strings.Size());
        WriteInt(strings.GetStringLen());
        WriteLine();
        for (int i = 0; i < strings.Size(); i++ )
            {
                WriteText(strings[i]);
            }
    }

    void ReadStringArray(SC_StringArray& strings)
    {
        strings.DeAlloc();
        int nLines = ReadInt();
        int strLen = ReadInt();
        strings.SetStringLen(strLen);
        strings.AllocAndSetSize(nLines);
        for (int i = 0; i < nLines; i++ ) {
            ReadText(strings[i], strLen);
        }
    }

#ifndef ADCONSOLEAPP

    void WriteFont(const PSC_Font& font)
    {
        static const int fontVer = 3;         // replaced stroke font with font thickness
        //    static const int fontVer = 2;       // added font rotation
        //    static const int fontVer = 1;       // added stroke font weight
        //    static const int fontVer = 0;
        WriteInt(fontVer);

        WriteInt(font.fontFamily);
        WriteBool(font.slantRegular);
        WriteBool(font.weightMedium);
        WriteInt(int(font.fontSize));
        WriteInt(font.fontRotation);
        WriteDouble(font.fontThickness);

        WriteLine();
    }

    void ReadFont(PSC_Font& font)
    {
        int readVer = ReadInt();

        font.fontFamily      = PSC_Font::FontFamily(ReadInt());
        font.slantRegular    = ReadBool();
        font.weightMedium    = ReadBool();
        font.fontSize        = double(ReadInt());

        if (readVer > 0)
            {
                if (readVer < 3)
                    {
                        int strokeFontWeight  = ReadInt();
                        font.weightMedium = (strokeFontWeight < 2);
                    }

                if (readVer > 1)
                    font.fontRotation  = PSC_Font::FontRotation(ReadInt());

                if (readVer > 2)
                    font.fontThickness = ReadDouble();

            }

    }
#endif

    void  WriteCoord(const Coord3D& fCoord)
    {
        WriteDouble(fCoord.cX);
        WriteDouble(fCoord.cY);
        WriteDouble(fCoord.cZ);
        WriteLine();
    }

    void  ReadCoord(Coord3D& fCoord)
    {
        fCoord.cX = ReadDouble();
        fCoord.cY = ReadDouble();
        fCoord.cZ = ReadDouble();
    }

    void WriteRealMSSpec(const DSC_RealMSSpec& outSpec)
    {
        SetRealConvFormat(SC_RealFormat(ncf_Scientific, 8));
        WriteBool(outSpec.valueIsMaster);
        WriteFuncObjRef(outSpec.valueMasterObjRef);
        WriteDouble(outSpec.enteredRealValue);
        WriteLine();
    }

    void ReadRealMSSpec(DSC_RealMSSpec& inSpec)
    {
        inSpec.valueIsMaster        = ReadBool();
        ReadFuncObjRef(inSpec.valueMasterObjRef);
        inSpec.enteredRealValue    = ReadDouble();
    }

    void WriteIndexMSSpec(const DSC_IndexMSSpec& outSpec)
    {
        WriteBool(outSpec.indexIsMaster);
        WriteFuncObjRef(outSpec.indexMasterObjRef);
        WriteInt(outSpec.enteredIndexValue);
        WriteLine();

        //  new index spec stuff
        if (outSpec.MultipleOK())
            {
                if (outSpec.SingleOK())
                    WriteBool(outSpec.enteredIndexIsSingle);

                WriteIntArray(outSpec.enteredIndexArray);
            }
    }

    void ReadIndexMSSpec(DSC_IndexMSSpec& inSpec)
    {
        inSpec.indexIsMaster        = ReadBool();
        ReadFuncObjRef(inSpec.indexMasterObjRef);
        inSpec.enteredIndexValue    = ReadInt();
        if (inSpec.MultipleOK())
            {
                if (inSpec.SingleOK())
                    inSpec.enteredIndexIsSingle = ReadBool();
                ReadIntArray(inSpec.enteredIndexArray);
            }
    }

    void ReadThreshold(DSC_Threshold& threshold)
    {
        int readVer = ReadInt();
        threshold.minThreshold = DSC_Threshold::ThresholdOp(ReadInt());
        threshold.maxThreshold = DSC_Threshold::ThresholdOp(ReadInt());
        threshold.minVal       = ReadDouble();
        threshold.maxVal       = ReadDouble();
    }


    void WriteThreshold(const DSC_Threshold& threshold)
    {
        static const int thVer = 0;
        WriteInt(thVer);

        WriteInt(int(threshold.minThreshold));
        WriteInt(int(threshold.maxThreshold));
        WriteDouble(threshold.minVal, SC_RealFormat(ncf_Scientific, 10));
        WriteDouble(threshold.maxVal, SC_RealFormat(ncf_Scientific, 10));
        WriteLine();
    }




    void WritePointArray(const SC_PointArray& pointArray)
    {
        WriteInt(pointArray.Size());
        WriteLine();

        SetRealConvFormat(SC_RealFormat(ncf_Scientific, 6));
        for (int i = 0; i < pointArray.Size(); i++)
            {
                WriteDouble(pointArray[i].pX);
                WriteDouble(pointArray[i].pY);
                WriteLine();
            }
    }

    void ReadPointArray(SC_PointArray& pointArray)
    {
        int nPt = ReadInt();
        pointArray.Alloc(nPt);
        Point2D tmp;
        for (int i = 0; i < nPt; i++)
            {
                tmp.pX = ReadDouble();
                tmp.pY = ReadDouble();
                pointArray += tmp;

                //      pointArray += Point2D(ReadDouble(), ReadDouble());  // MS bug -- reads Y first
            }
    }


    void WriteIntArray(const SC_IntArray& intArray)
    {
        WriteInt(intArray.Size());
        WriteLine();
        for (int i = 0; i < intArray.Size(); i++)
            {
                WriteInt(intArray[i]);
                WriteLine();
            }
    }

    void ReadIntArray(SC_IntArray& intArray)
    {
        int nPt = ReadInt();
        intArray.Alloc(nPt);
        for (int i = 0; i < nPt; i++)
            intArray += ReadInt();
    }

    void WriteNodeFileArray(const NodeFileArray& nfArray)
    {
        const int writeVer = 0;
        WriteInt(writeVer);
        WriteLine();

        WriteInt(nfArray.Size());
        WriteLine();
        for (int i = 0; i < nfArray.Size(); i++)
            {
                WriteBool(nfArray[i].nodeBasedFile);
            }
        WriteLine();
    }

    void ReadNodeFileArray(NodeFileArray& nfArray)
    {
        int readVer = ReadInt();

        int nNF = ReadInt();
        int maxNF = nfArray.Size();
        for (int i = 0; i < nNF; i++)
            {
                bool nextNF = ReadBool();
                if (i < maxNF)
                    nfArray[i].nodeBasedFile = nextNF;
            }

        for (int i = nNF; i < maxNF; i++)
            nfArray[i].nodeBasedFile = false;
    }


};

