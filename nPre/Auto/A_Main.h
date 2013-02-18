///////////////////////////////////////////////////////////////////////////////////
//
// A_Main.h
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
//      Implements auto setup feature.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef A_MAIN_H
#define A_MAIN_H

#include <genClass/C_Graphic.h>

#include <genApp/C_MenuObj.h>
#include <genApp/C_MenuPage.h>

//  namespace containing main auto control variables and methods used by all auto procs

class MenuRootC;
class PFO_2DXYData;

namespace nsAutoSetup  {

    extern  MenuRootC*  autoRoot;
    extern  MenuPageC*  autoPage;

    extern SC_StringArray   currentWells;
    extern bool             processTestZonePressureData;
    extern bool             processTestZoneFlowData;
    extern bool             processObsWellPressureData;

    extern  MenuObjC*   AddNewObject(const char* objAlloc, const char* objID, bool execute = false);
    extern  MenuObjC*   CopyNewObject(MenuObjC* objToCopy, const char* objID, bool execute = false);
    extern  void        RecalcPage(MenuPageC* currPage);
    extern  void        RecalcPage();
    extern  void        RecalcAll();    

    extern  void            AddNewDataPage(const char*      pageID, 
                                                 MenuPageC* dataParent = NULL);
    extern  void            AddNewXYPage(const char* rootID, const char* pageID, bool linX, bool linY);
    extern  PFO_2DXYData*   AddXYDataPFO(const char* objID);

    extern  void            SetXYDisplay(       PFO_2DXYData&   objPtr,
                                                int             index,
                                         const  FuncObjRef&     inRef,
                                                bool            isSymbol,
                                                PlotSymbol      symType,
                                         const  char*           objDesc = NULL);

    extern  void            SetXYDisplay(       PFO_2DXYData&   objPtr,
                                                int             index,
                                         const  FuncObjRef&     inRef,
                                                PlotSymbol      symType,
                                         const  char*           objDesc = NULL);


    extern  void            SetXYDisplay(       PFO_2DXYData&   objPtr,
                                                int             index,
                                         const  FuncObjRef&     inRef,
                                         const  char*           objDesc = NULL);

    enum {idLen = 80};
    extern char seqID[idLen];
    extern char fieldID[idLen];
    extern char simID[idLen];
    extern const char* wellID;

    extern void MakeObjID(char* outID, const char* suffix1, const char* suffix2, const char* suffix3);
    extern void MakeObjID(char* outID, const char* suffix1, const char* suffix2);
    extern void MakeObjID(char* outID, const char* suffix);
    extern void MakeObjID(const char* suffix);

    extern MenuObjC* lastObject;
    extern FuncObjC* AddFieldObject(const char* objID);
    extern FuncObjC* AddSimObject();

    extern  void        IncrementID(const char* baseID,
                                          char* nextID,
                                          int&  idNum);

    extern  FuncObjRefArray     autoObjects;
    extern  void                SetObjRef(const type_info&  objType);
    extern  FuncObjRef          GetMatchingObj(const char* idToMatch);

    extern  bool                MultiWellIDS();

};

//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace nsAutoSetup;


#endif //A_MAIN_H

