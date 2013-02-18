///////////////////////////////////////////////////////////////////////////////////
//
// G_Vary.h
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
//      more support for multiple value runs.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef G_VARY_H
#define G_VARY_H

#include <osClass/C_VaryVar.h>

#include <genListClass/C_ListObj.h>
#include <genListClass/C_ErrorListObj.h>
#include <genListClass/C_ListFactory.h>

#include <genApp/C_CfgFileObj.h>


namespace osVary  {

    enum {maxVary = 3};

    class VaryListing : public ListObjC, public VaryControlStatics {
        public:
                    VaryListing();
                    ~VaryListing() {};

                                                    // C_Listing virtuals
            virtual void  CreateListing();
        private:

    };

    class VaryErrorListing : public ErrorListObjC, public VaryControlStatics {
        public:
                    VaryErrorListing();
                    ~VaryErrorListing() {};

            virtual void  CreateListing();

    };

    extern VaryErrorListing varyErrorListing;

    class VaryStaticSupport :   public ListFactory,
                                public CfgFileObj,
                                public VaryControlStatics  // IO & listing support
    {
        private:
                // object versioning support
            static const char*  groupHeader;
            static const int    majorVersion;
            static const int    minorVersion;

        public:
                        VaryStaticSupport();     // constructor initializes all parameters and vars
                        ~VaryStaticSupport() {};

            static void     SetToInitial();     // for file New
            static void     SetForFlags();      // modifies settings
            static bool     VaryOK();           // pre-run check

            static int      GetnVaryCombinations();
            static void     SetVaryIndexes(int comboNum);   //  assumes pre-run set and varyOK

            static void     GetCaseID(char* idStr, int maxLen);
            static void     GetCaseValues(SC_DoubleArray& caseVals);

            virtual void    WriteToFile();
            virtual bool    ReadFromFile();

            virtual ListObjC*       CreateListObject() {return new VaryListing();}
            virtual ErrorListObjC*  GetErrorListObject() {return &varyErrorListing;}

        private:
            static void     SetupOrdered();

    };

    extern  VaryVarArray        orderedVary;
    extern  VaryStaticSupport   vary;           // the one and only

};


//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace osVary;


#endif // !G_VARY_H

