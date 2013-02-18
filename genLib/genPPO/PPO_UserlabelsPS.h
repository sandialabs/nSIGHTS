///////////////////////////////////////////////////////////////////////////////////
//
// PPO_UserlabelsPS.h
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPO_USERLABELSPS_H
#define PPO_USERLABELSPS_H

#include <genPPO/PPO_AnnoBasePS.h>

class PPO_UserLabelsPS : public PPO_AnnoBasePS {

    class UserLabelWidget {
        private:
            ToggleBoxWidget      doLabelToggle;
            HAlignOptionWidget   labAlign;
            TextEntryWidget      userLabel;
        public:
            FormWidget           outsideForm;


            void  InitUserLabel();
            void  SetUserLabel(const PFO_UserLabels::UserLabel& labelData);
            void  GetUserLabel(PFO_UserLabels::UserLabel& labelData);
            void  SetSensitive();

        private:
            static  void    SetSensitiveCB(void* inObj);
    };

    class UserLabelScrolledForm : public ScrolledForm {
        protected:

            virtual CSize SetCalculateFormSize();
    };

    private:
        FormWidget              topRowForm;
        BaseUIC                 coordFrame;
        BaseUIC                 formatFrame;

        RealEntryUIC            xField;
        RealEntryUIC            yField;

        PenOptionUIC            legendPenOption;
        ToggleBoxWidget         opaqueToggle;
        ToggleBoxWidget         frameToggle;
        LineThickOptionUIC      frameThk;

        FontUIC                 legendFontUI;

        UserLabelScrolledForm   scrollArea;
        UserLabelWidget         labData[PFO_UserLabels::maxUserLabels];

    public:
                        PPO_UserLabelsPS();
                        ~PPO_UserLabelsPS(){};

                void    InitMenu(PPO_UserLabels&    inData);

                void    MenuOpen(PPO_UserLabels&    inData);

                void    SetMenuData(PPO_UserLabels& inData);
                bool    UIdataOK();
                void    GetMenuData(PPO_UserLabels& outData);

                void    MenuClear();

                void    UpdateULUI(PPO_UserLabels&  inData);

    private:
                void    SetAllSensitive();

        static  void    SetAllSensitiveCB(void* inObj);

};


#endif // !PPO_USERLABELSPS_H

