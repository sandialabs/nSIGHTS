///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PulseNormalize.h
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

#ifndef DPO_PULSENORMALIZE_H
#define DPO_PULSENORMALIZE_H

#include <nsDFO/DFO_PulseNormalize.h>
#include <genApp/ExposedReal.h>
#include <genApp/C_MenuObj.h>

    class DPO_PulseNormalizePS;

    class DPO_PulseNormalize : public MenuObjC, public DFO_PulseNormalize {
        public:
            friend class DPO_PulseNormalizePS;
        private:
            static const char*  objLongDesc;
            ExposedRealWrapper  exposedP0Value;
            ExposedRealWrapper  exposedPIValue;

        public:
                            DPO_PulseNormalize();
                            DPO_PulseNormalize(const DPO_PulseNormalize& a);
                            ~DPO_PulseNormalize() {};

            DPO_PulseNormalize& operator= (const DPO_PulseNormalize& a);

            void            WriteToFile();
            bool            ReadFromFile();

#ifndef ADCONSOLEAPP
            void            PropOpenMenu();
            void            PropUpdate();
            void            PropApply();

            //  added for UI update
            void            CalcOutput(FOcalcType  calcType);

            bool            ContainsExposedObject();

        private:
            DPO_PulseNormalizePS&   MenuRef();
#endif //ADCONSOLEAPP


    };

#endif //DPO_PULSENORMALIZE_H

