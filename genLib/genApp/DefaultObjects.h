///////////////////////////////////////////////////////////////////////////////////
//
// DefaultObjects.h
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
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DEFAULTOBJECTS_H
#define DEFAULTOBJECTS_H

#include <genClass/C_FuncObj.h>
#include <genClass/DO_Label.h>
#include <genClass/DO_PenSet.h>
#include <genClass/DO_ColorMap.h>

namespace appDefaultObjects {


    class DefaultPens : public FuncObjC {
    public:
        DC_PenSet   penSet;

    private:
        DO_PenSet   penSetDO;

    public:
        //  sets pens to default values
        DefaultPens();
        ~DefaultPens() {};

        void           CalcOutput(FOcalcType  calcType);

    private:
        //  dont need copy -- hide to prevent
        DefaultPens(const DefaultPens& a);
        DefaultPens&   operator= (const DefaultPens& a);
    };



    class DefaultColorMap : public FuncObjC {

    private:
        DC_ColorMap    colorMapDC;
        DO_ColorMap    colorMapDO;

    public:
        DefaultColorMap(const char* inID);
        ~DefaultColorMap() {};


        void           CalcOutput(FOcalcType  calcType);


    protected:
        void           SetColorMap(bool useHSV,
                                   SC_ColorSpec& mapStart,
                                   SC_ColorSpec& mapEnd);

    private:
        DefaultColorMap(const DefaultColorMap& a);
        DefaultColorMap& operator= (const DefaultColorMap& a);

    };

    class ColdHotMap : public DefaultColorMap {
    public:

        ColdHotMap();

    private:
        ColdHotMap(const ColdHotMap& a);
        ColdHotMap& operator= (const ColdHotMap& a);

    };
    class WhiteBlackMap : public DefaultColorMap {
    public:

        WhiteBlackMap();

    private:
        WhiteBlackMap(const WhiteBlackMap& a);
        WhiteBlackMap& operator= (const WhiteBlackMap& a);

    };

    class SysInfoObj : public FuncObjC {
    private:
        DO_Label            versionIDDO;
        DO_Label            versionDateDO;
        DO_Label            todaysDateDO;
        DO_Label            currentTimeDO;
        DO_Label            currentConfigFileOnlyDO;
        DO_Label            currentConfigFileAndPathDO;

    public:
        SysInfoObj();
        ~SysInfoObj() {};

        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);

    private:
        //  dont need copy -- hide to prevent
        SysInfoObj(const SysInfoObj& a) : FuncObjC(a) {};
        SysInfoObj&         operator= (const SysInfoObj& a) {return *this;}
    };


    extern SysInfoObj      sysInfoObj;
    extern DefaultPens     defaultPenObj;
    extern ColdHotMap      coldHotMapObj;
    extern WhiteBlackMap   whiteBlackMapObj;

}

#endif // DEFAULTOBJECTS_H

