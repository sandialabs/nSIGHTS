///////////////////////////////////////////////////////////////////////////////////
//
// AppConfigFile.h
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

#ifndef APPCONFIGFILE_H
#define APPCONFIGFILE_H

#include <genClass/C_Text.h>
#include <genClass/C_FuncObj.h>


// forward refs to avoid massive rebuilds
class SC_StringArray;
class SC_IntArray;
class genCommonTypes::SC_PointArray;
class DSC_RealMSSpec;
class DSC_IndexMSSpec;
class DSC_Threshold;
class PSC_Font;
class NodeFileArray;

namespace appConfigFile {

    // globals
    extern TextC*       cfgObject;

    extern char         currConfigFileName[stdFileStrLen];

    extern int          currentCfgFileReadMajorVersion; //  these are set by main config file routines
    extern int          currentCfgFileReadMinorVersion; //  for app wide version control

    //  these are available for obj specific version control if needed
    extern int          currentObjReadMajorVersion;
    extern int          currentObjReadMinorVersion;

    //      kluge to signal transition to page object versioning -- used by MenuPageC only
    extern bool         usePageObjectVersioning;

    //      ********************************  group contains appCommon level constants
    extern void         WriteAppGlobals();
    extern void         ReadAppGlobals();

    extern  FuncObjArray prevVerSystemObjects;  // for nPre backward compatibility only


    //      file read write
    //  ************  member access
    inline void         SetRealConvFormat(const SC_RealFormat& inForm) {cfgObject->SetRealConvFormat(inForm);}

    //  ************  standard text file methods restated for use of static cfgFile
    void                OpenAsFile(const char* fName, bool openForRead);
    void                OpenAsClipboard(bool openForRead);

    void                Close();


    // app generic read/write
    inline void         WriteFileHeader(const char* headerText, int majVer, int minVer)
    {cfgObject->WriteFileHeader(headerText, majVer, minVer);}

    inline void         ReadFileHeader(const char*  headerToMatch,
                                       int&   majVer,
                                       int&   minVer,
                                       const char*  fileDesc)
    {cfgObject->ReadFileHeader(headerToMatch, majVer, minVer, fileDesc);}


    //      error checking for sub-portions of cfg files
    inline void         WriteGroupHeader(const char* headerText, int majVer, int minVer)
    {cfgObject->WriteFileHeader(headerText, majVer, minVer);}

    // sets currObjReadMajorVersion & currObjReadMajorVersion
    extern void         ReadGroupHeader(const char*  headerToMatch, int  maxMajorVer);


    //      basic type support
    inline void         WriteLine()
    {cfgObject->WriteLine();}
    inline void         WriteText(const char* fText)
    {cfgObject->WriteText(fText);}
    inline void         WriteSolidText(const char* fText)
    {cfgObject->WriteSolidText(fText);}
    inline void         WriteTextWithTrailingBlanks(const char* fText)
    {cfgObject->WriteTextWithTrailingBlanks(fText);}
    inline void         WriteBool(bool fBool)
    {cfgObject->WriteBool(fBool);}
    inline void         WriteInt(int fInt)
    {cfgObject->WriteInt(fInt);}
    inline void         WritePointer(const void* fPtr)
    {cfgObject->WritePointer(fPtr);}
    inline void         WriteDouble(const double& fDouble)
    {cfgObject->WriteDouble(fDouble);}
    inline void         WriteNullDouble(const double& fDouble)
    {cfgObject->WriteNullDouble(fDouble);}
    inline void         WriteDouble(const double& fDouble, const SC_RealFormat& inForm)
    {
        cfgObject->SetRealConvFormat(inForm);
        cfgObject->WriteDouble(fDouble);
    }
    inline void         WriteDoubleArray(const SC_DoubleArray& dblData,
                                         bool                  writeSize)
    {cfgObject->WriteDoubleArray(dblData, writeSize);}


    inline void         ReadLine() {}  // for congruence

    inline void         ReadText(char* fText, int maxLen)
    {cfgObject->ReadText(fText, maxLen);}
    inline void         ReadSolidText(char* fText)
    {cfgObject->ReadSolidText(fText);}
    inline void         ReadTextWithTrailingBlanks(char* fText, int maxLen)
    {cfgObject->ReadTextWithTrailingBlanks(fText, maxLen);}

    inline void         ReadFileName(char* fName)
    {cfgObject->ReadFileName(fName);}
    inline bool         ReadBool()
    {return cfgObject->ReadBool();}
    inline int          ReadInt()
    {return cfgObject->ReadInt();}
    inline void*        ReadPointer()
    {return cfgObject->ReadPointer();}
    inline double       ReadDouble()
    {return cfgObject->ReadDouble();}
    inline double       ReadNullDouble()
    {return cfgObject->ReadNullDouble();}
    inline void         ReadDoubleArray(SC_DoubleArray& dblData,
                                        bool            readSize)
    {cfgObject->ReadDoubleArray(dblData, readSize);}


    inline void         CheckForInputFailure(const char* errMsg)
    {cfgObject->CheckTextError(errMsg);}
    inline void         InputError(const char* errMsg)
    {cfgObject->ThrowTextError(errMsg);}

    //  ************  new object specific methods

    // for reading/writing FO addresses
    extern void         WriteFuncObj(const FuncObjC* fFuncObj);
    extern FuncObjC*    ReadFuncObj();

    extern void         WriteFuncObjRef(const FuncObjRef& fFuncObjRef);
    extern void         ReadFuncObjRef(FuncObjRef& rFuncObjRef);

    //  FO pointer matching stuff to reconcile current and file addresses
    extern void         ReadFuncObjList();                            // reads n then next n FO pointer values
    extern void         ReadAndMatchFuncObj(const FuncObjC* currFO);  // reads  next line as FuncObj and sets up match
    extern void         DeallocFuncObjList();                         // clean up list when complete

    //      more specific global func support
    extern void         WriteGlobalFunc(const FuncObjC& globFunc);
    extern void         ReadGlobalFunc(const FuncObjC& globFunc);


    //  other genClass
    extern void         WriteCoord(const Coord3D& fCoord);
    extern void         ReadCoord(Coord3D& fCoord);

    extern  void        WriteRealFormat(const SC_RealFormat& inForm);
    extern  void        ReadRealFormat(SC_RealFormat& inForm);

    extern void         WriteStringArray(const SC_StringArray& strings);
    extern void         ReadStringArray(SC_StringArray& strings);

    extern  void        WritePointArray(const SC_PointArray& pointArray);
    extern  void        ReadPointArray(SC_PointArray& pointArray);

    extern  void        WriteIntArray(const SC_IntArray& intArray);
    extern  void        ReadIntArray(SC_IntArray& intArray);

    extern  void        WriteNodeFileArray(const NodeFileArray& nfArray);
    extern  void        ReadNodeFileArray(NodeFileArray& nfArray);

    // genDataClass
    extern  void        WriteRealMSSpec(const DSC_RealMSSpec& outSpec);
    extern  void        ReadRealMSSpec(DSC_RealMSSpec& inSpec);

    extern  void        WriteIndexMSSpec(const DSC_IndexMSSpec& outSpec);
    extern  void        ReadIndexMSSpec(DSC_IndexMSSpec& inSpec);

    extern void         ReadThreshold(DSC_Threshold& threshold);
    extern void         WriteThreshold(const DSC_Threshold& threshold);

#ifndef ADCONSOLEAPP
    // genPlotClass
    extern void         WriteFont(const PSC_Font& font);
    extern void         ReadFont(PSC_Font& font);
#endif
};

using namespace appConfigFile;

#endif // APPCONFIGFILE_H

