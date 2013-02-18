///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CombineTables.h
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DPO_COMBINETABLES_H
#define DPO_COMBINETABLES_H

#include <genApp/C_MenuObj.h>
#include <objDFO/DFO_CombineTables.h>


 class DPO_CombineTablesPS;

 class DPO_CombineTables :  public MenuObjC, protected DFO_CombineTables {
     private:
         static const char*  objLongDesc;

     public:
         friend class DPO_CombineTablesPS;

                         DPO_CombineTables();
                         DPO_CombineTables(const DPO_CombineTables& a);
                         ~DPO_CombineTables(){};

         DPO_CombineTables& operator= (const DPO_CombineTables& a);

         void            WriteToFile();
         bool            ReadFromFile();

#ifndef ADCONSOLEAPP
         void            PropOpenMenu();
         void            PropUpdate();
         void            PropApply();

     private:
         DPO_CombineTablesPS&  MenuRef();
#endif

 };

#endif // !DPO_COMBINETABLES_H

