///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ColorCubeIsovolume.h
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPO_ColorCubeIsovolume_H
#define PPO_ColorCubeIsovolume_H

#include <genPPO/PPO_Base.h>
#include <genApp/ExposedReal.h>
#include <objPFOGL/PFO_ColorCubeIsovolumeGL.h>

class PlotDefC;
class PPO_ColorCubeIsovolumePS;

class PPO_ColorCubeIsovolume : public PPO_Base, protected PFO_ColorCubeIsovolumeGL {
    public:
        friend class PPO_ColorCubeIsovolumePS;
    private:
        ExposedRealWrapper  exposedisoValue;

    public:
                        PPO_ColorCubeIsovolume(PlotDefC* assPlt);
                        PPO_ColorCubeIsovolume(PlotDefC* assPlt, const PPO_ColorCubeIsovolume& a);
                        ~PPO_ColorCubeIsovolume(){};

        PPO_ColorCubeIsovolume& operator= (const PPO_ColorCubeIsovolume& a);

        void            PropOpenMenu();
        void            PropUpdate();
        void            PropApply();

        void            WriteToFile();
        bool            ReadFromFile();

        //  added for UI update
        void            CalcOutput(FOcalcType  calcType);

        bool            ContainsExposedObject();

    private:
        PPO_ColorCubeIsovolumePS&   MenuRef();
};
#endif // !PPO_ColorCubeIsovolume_H

