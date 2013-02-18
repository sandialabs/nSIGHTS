///////////////////////////////////////////////////////////////////////////////////
//
// U_MenuPPO.h
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
//              basic UI support classes used primarily by PPO_XXXPS functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef U_MENUPPO_H
#define U_MENUPPO_H

#include <genClass/C_Graphic.h>
#include <genClass/DC_PenSet.h>
#include <genClass/SC_RealFormat.h>
#include <genClass/DC_DateTime.h>

#include <genPlotClass/PSC_Font.h>
#include <genPlotClass/PC_Axes.h>

#include <genClassPS/PS_Color.h>
#include <genClassPS/PS_UIClass.h>

#include <genUIExt/ColorTextCB.h>
#include <genUIExt/LineTypeCB.h>
#include <genUIExt/SymbolTypeCB.h>

#include <genPlotClass/PSC_SeriesSpec.h>
#include <genPlotClass/PSC_SymbolSpec.h>
#include <genPlotClass/PSC_StippleSpec.h>
#include <genPlotClass/PSC_LineSpec.h>
#include <genPlotClass/PSC_EdgeSpec.h>
#include <genPlotClass/PSC_3DSeriesSpec.h>
#include <genPlotClass/PSC_ExtrusionSpec.h>
#include <genPlotClass/PSC_LegendOverride.h>
#include <genPlotClass/PSC_GridLineLabelFormat.h>

using namespace genClassPS;

namespace genAppMenuUtilities {

    //      were in PS_UIBase - moved here to minimize recompiles

    class ColorOptionChooserWidget : public CColorTextComboBox, public ChooserWidget {
    public:
        ColorOptionChooserWidget();

        virtual bool    SetChoice(int inChoice);
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CColorTextComboBox)
    public:
        virtual void    MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
        //}}AFX_VIRTUAL

    protected:
        //{{AFX_MSG(ColorOptionChooserWidget)
        //}}AFX_MSG
        afx_msg void OnSelchange();
        DECLARE_MESSAGE_MAP()
    };

    class LineTypeOptionChooserWidget : public CLineTypeComboBox, public ChooserWidget {
    public:
        LineTypeOptionChooserWidget();

        virtual bool    SetChoice(int inChoice);
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CLineTypeComboBox)
    public:
        virtual void    MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
        //}}AFX_VIRTUAL

    protected:
        //{{AFX_MSG(LineTypeOptionChooserWidget)
        //}}AFX_MSG
        afx_msg void OnSelchange();
        DECLARE_MESSAGE_MAP()
    };


    class SymbolTypeOptionChooserWidget : public CSymbolTypeComboBox, public ChooserWidget {
    public:
        SymbolTypeOptionChooserWidget();

        virtual bool    SetChoice(int inChoice);
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CSymbolTypeComboBox)
    public:
        virtual void    MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
        //}}AFX_VIRTUAL

    protected:
        //{{AFX_MSG(SymbolTypeOptionChooserWidget)
        //}}AFX_MSG
        afx_msg void OnSelchange();
        DECLARE_MESSAGE_MAP()
    };



    class PenOptionChooserWidget : public ColorOptionChooserWidget  {
    private:
        Pixel           penColors[DC_PenSet::maxPenSet];
        bool            pensSet;

    public:
        PenOptionChooserWidget();

        void            ResetPens(DC_PenSet* penData);

        void            SetPenSelection(int penIndx);
        int             GetPenSelection() {return currChoice;}
    };


    class LineThickOptionWidget : public OptionChooserWidget  {
    private:
        enum                {nlineThickChoice = 10};
        static const int    lineThickMap[nlineThickChoice];

    public:
        LineThickOptionWidget();

        void            PreInitSetUp();
        void            SetLineThick(int  lThick);
        int             GetLineThick();

    };

    class LineTypeOptionWidget : public LineTypeOptionChooserWidget  {
    public:
        LineTypeOptionWidget();
        void            PreInitSetUp();
        void            SetLineType(PlotLineType lineType) {SetChoice(int(lineType));}
        PlotLineType    GetLineType() {return PlotLineType(currChoice);}

        void            SetLineColor(COLORREF inColor);
        void            SetLineThick(int lThick);
        void            SetLineColorAndThick(COLORREF inColor, int lThick);

    };

    class PlotSymLineOptionWidget : public OptionChooserWidget  {

    public:
        PlotSymLineOptionWidget();

        void            PreInitSetUp();
        void            SetPlotSymLine(PlotSymLine plotSymLine) {SetChoice(int(plotSymLine));}
        PlotSymLine     GetPlotSymLine() {return PlotSymLine(currChoice);}

    };

    class SymbolTypeOptionWidget : public SymbolTypeOptionChooserWidget  {

    public:
        SymbolTypeOptionWidget();

        void           PreInitSetUp();
        void           SetSymbolType(PlotSymbol symbolType) {SetChoice(int(symbolType));}
        PlotSymbol     GetSymbolType() {return PlotSymbol(currChoice);}

        void           SetSymbolColor(COLORREF inColor);
        void           SetSymbolSize(int sSize);
        void           SetSymbolLThick(int sLThick);
        void           SetSymbolAttributes(COLORREF inColor, int sSize, int sLThick);

    };

    class SymbolSizeOptionWidget : public OptionChooserWidget  {
    private:
        enum                {nsymbolSizeChoice = 18};
        static const int    symbolSizeMap[nsymbolSizeChoice];

    public:
        SymbolSizeOptionWidget();

        void            PreInitSetUp();
        void            SetSymbolSize(int  sSize);
        int             GetSymbolSize();

    };

    class TubePointsOptionWidget : public OptionChooserWidget  {
    private:
        enum                {ntubePointsChoice = 4};
        static const int    tubePointsMap[ntubePointsChoice];

    public:
        TubePointsOptionWidget();

        void            PreInitSetUp();
        void            SetTubePoints(int  tSize);
        int             GetTubePoints();

    };

    class HAlignOptionWidget : public OptionChooserWidget  {

    public:
        HAlignOptionWidget();

        void            PreInitSetUp();
        void            SetHAlign(HorizontalTextAlignment hAlign) {SetChoice(int(hAlign));}
        HorizontalTextAlignment GetHAlign() {return HorizontalTextAlignment(currChoice);}

    };

    class VAlignOptionWidget : public OptionChooserWidget  {

    public:
        VAlignOptionWidget();
        void            PreInitSetUp();
        void            SetVAlign(VerticalTextAlignment vAlign) {SetChoice(int(vAlign));}
        VerticalTextAlignment GetVAlign() {return VerticalTextAlignment(currChoice);}

    };

    class AxesIncOptionWidget : public OptionChooserWidget {
    public:
        AxesIncOptionWidget();
        void            PreInitSetUp();
        void            ShortTextPreInitSetUp();
        void                    SetIncType(PC_Axes::AxesIncType inInc) {SetChoice(int(inInc));}
        PC_Axes::AxesIncType    GetIncType() {return PC_Axes::AxesIncType(currChoice);}
    };

#pragma warning( push )
#pragma warning( disable : 4355 ) // 'this' : used in base member initializer list

    class PenOptionUIC : public OptionBaseUIC, public PenOptionChooserWidget  {

    public:
        PenOptionUIC(): OptionBaseUIC(this) {}
        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };


    class LineThickOptionUIC : public OptionBaseUIC, public LineThickOptionWidget  {

    public:
        LineThickOptionUIC(): OptionBaseUIC(this) {}
        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };

    class LineTypeOptionUIC : public OptionBaseUIC, public LineTypeOptionWidget  {

    public:
        LineTypeOptionUIC(): OptionBaseUIC(this) {}
        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };


    class SymbolSizeOptionUIC : public OptionBaseUIC, public SymbolSizeOptionWidget  {

    public:
        SymbolSizeOptionUIC(): OptionBaseUIC(this) {}
        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };

    class HAlignOptionUIC : public OptionBaseUIC, public HAlignOptionWidget  {

    public:
        HAlignOptionUIC(): OptionBaseUIC(this) {}
        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };

    class VAlignOptionUIC : public OptionBaseUIC, public VAlignOptionWidget  {

    public:
        VAlignOptionUIC(): OptionBaseUIC(this) {}
        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };

    class AxesIncOptionUIC : public OptionBaseUIC, public AxesIncOptionWidget {
    public:
        AxesIncOptionUIC(): OptionBaseUIC(this) {}
        void        SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };


#pragma warning( pop )


    // ************************************************************* series
    class SeriesDataUIC : public BaseUIC {
    private:
        FormWidget  topRowForm;
        FormWidget  botRowForm;

    public:
        ToggleBoxWidget             doSeries;

        PenOptionChooserWidget      seriesPen;
        PlotSymLineOptionWidget     seriesPlotType;

        SymbolTypeOptionWidget      symbolType;
        SymbolSizeOptionWidget      symbolSize;

        LineThickOptionWidget       lineThickness;
        LineTypeOptionWidget        lineType;
        ToggleBoxWidget             mitreLinesUI;

    public:

        void    InitFramedUI(char             frameTitle[],
                             Widget           parent,
                             const ArgListC&  inArgs);    // attachment resources + others

        void    InitFormUI(Widget           parent,
                           const ArgListC&  inArgs);      // attachment resources + others

        void    ResetPens(DC_PenSet* penData);

        void    InitSeriesWidgets();  // parent is form

        void    SetSeriesData(const PSC_SeriesSpec& seriesData);
        void    GetSeriesData(PSC_SeriesSpec& seriesData);
        void    SetSensitive(bool inSens);

    private:
        static void SeriesSensitiveCB(void* currObj);

    };

    class SymbolSpecUIC : public BaseUIC {
    public:
        PenOptionChooserWidget      symbolPenUI;
        SymbolTypeOptionWidget      symbolTypeUI;
        SymbolSizeOptionWidget      symbolSizeUI;

    public:

        void    InitFramedUI(char             frameTitle[],
                             Widget           parent,
                             const ArgListC&  inArgs);      // attachment resources + others

        void    InitFormUI(Widget           parent,
                           const ArgListC&  inArgs);        // attachment resources + others

        void        ResetPens(DC_PenSet* penData);


        void        SetSymbolSpec(const PSC_SymbolSpec& inSpec);
        void        GetSymbolSpec(PSC_SymbolSpec& outSpec);
        void        SetSensitive(bool inSens);

    private:
        void  InitWidgets();  // parent is form
        static void SetSensitiveCB(void* currObj);
    };

    class LineSpecUIC : public BaseUIC {
    public:
        PenOptionChooserWidget      linePenUI;
        LineThickOptionWidget       lineThkUI;
        LineTypeOptionWidget        lineTypeUI;
        ToggleBoxWidget             mitreLinesUI;

    public:

        void    InitFramedUI(char             frameTitle[],
                             Widget           parent,
                             const ArgListC&  inArgs);            // attachment resources + others

        void    InitFormUI(        Widget           parent,
                                   const ArgListC&  inArgs);      // attachment resources + others

        void        ResetPens(DC_PenSet* penData);


        void        SetLineSpec(const PSC_LineSpec& inSpec);
        void        GetLineSpec(PSC_LineSpec& outSpec);
        void        SetSensitive(bool inSens);

    private:
        void        InitWidgets();  // parent is form
        static void SetSensitiveCB(void* currObj);
    };

    class StippleSpecUIC : public BaseUIC {
    public:
        OptionUIC                  stippleTypeUI;
        PenOptionUIC               stipplePenUI;
        LineThickOptionUIC         stippleLineThkUI;
        SymbolSizeOptionUIC        stippleSizeUI;

    public:

        void    InitFramedUI(char       frameTitle[],
                             Widget     parent,
                             const ArgListC&  inArgs);    // attachment resources + others

        void    InitFormUI(Widget     parent,
                           const ArgListC&  inArgs);      // attachment resources + others

        void        ResetPens(DC_PenSet* penData);


        void        SetStippleSpec(const PSC_StippleSpec& inSpec);
        void        GetStippleSpec(PSC_StippleSpec& outSpec);
        void        SetSensitive(bool inSens);

    private:
        void        InitWidgets();  // parent is form
        static void SetSensitiveCB(void* currObj);
    };



    class Series3DDataUIC : public BaseUIC {
    private:
        FormWidget                  topRowForm;
        FormWidget                  midRowForm;
        HorizontalSeparator         hSep;
        FormWidget                  botRowForm;

    public:
        PlotSymLineOptionWidget     seriesPlotType;
        PenOptionChooserWidget      seriesPen;

        SymbolTypeOptionWidget      symbolType;
        SymbolSizeOptionWidget      symbolSize;

        BoolOptionChooserWidget     plotLineAsTubeUI;

        LineThickOptionWidget       lineThickness;
        LineTypeOptionWidget        plotLineTypeUI;

        TubePointsOptionWidget      nextrusionPointsUI;
        SymbolSizeOptionWidget      extrusionPixelSizeUI;

        ToggleBoxWidget             plotSymEdgesUI;
        ToggleBoxWidget             plotTubeEdgesUI;

        PenOptionChooserWidget      edgePenUI;
        LineThickOptionWidget       edgeThkUI;
        RealEntryUIC                edgePolyOffsetUI;

        ToggleBoxWidget             capEndsUI;

    public:

        void    InitFramedUI(char             frameTitle[],
                             Widget           parent,
                             const ArgListC&  inArgs);    // attachment resources + others

        void    InitFormUI(Widget           parent,
                           const ArgListC&  inArgs);      // attachment resources + others

        void    ResetPens(DC_PenSet* penData);

        void    Set3DSeriesData(const PSC_3DSeriesSpec& seriesData);
        void    Get3DSeriesData(PSC_3DSeriesSpec& seriesData);
        void    SetSensitive(bool inSens);

    private:
        void        InitSeriesWidgets();  // parent is form
        static void SeriesSensitiveCB(void* currObj);
    };


    // ************************************************************* fonts

    class FontUIC : public BaseUIC {
    private:
        enum                {nfontSize = 15};
        static const int    fontSizeMap[nfontSize];
        enum                {nfontThick = 5};
        static const double fontThickMap[nfontThick];

        OptionUIC           fontFamilyChooser;
        BoolOptionUIC       fontWeightChooser;
        BoolOptionUIC       fontSlantChooser;
        OptionUIC           fontSizeChooser;
        OptionUIC           fontThicknessUI;  // only if init to 3D Font
        OptionUIC           fontRotationUI;   // only if useRotation

        bool                is3DFont;         // cannot be both
        const bool          useRotation;

    public:
        FontUIC() : useRotation(false) {is3DFont = false;}
        FontUIC(bool inRot) : useRotation(inRot) {is3DFont = false;}

        void    PreInitSetup(bool is3D) {is3DFont = is3D;}

        void    InitFramedUI(char             frameTitle[],
                             Widget           parent,
                             const ArgListC&  inArgs);      // attachment resources + others

        void    SetFontSettings(const PSC_Font&    inFont);
        void    GetFontSettings(PSC_Font&         outFont);
        void    SetSensitive(bool isSensitive);
    };

    // ************************************************************* numeric format

    class RealFormatUIC  : public BaseUIC  {
    public:
        OptionChooserWidget formatTypeChooser;
        OptionChooserWidget formatPrecChooser;

    private:
        enum                {maxPrecSettings = 9};
        SC_RealFormat       tformat;
        const bool          allowExtended;

    public:
        RealFormatUIC(bool allowExt = false);

        void    InitFramedUI(char             frameTitle[],
                             Widget           parent,
                             const ArgListC&  inArgs);      // attachment resources + others

        void    InitFormUI(Widget    parent,
                           const ArgListC& inArgs);        // attachment resources + others

        void    SetSensitive(bool isSensitive);

        void    SetRealFormatSettings(const SC_RealFormat&  objSC_RealFormat);
        void    GetRealFormatSettings(SC_RealFormat&        objSC_RealFormat);


    private:
        void    DoUIInit(); //  common parts of InitFramedUI and InitFormUI

        void    SetPrecisionSettings();
        static void SetPrecisionSettingsCB(void* currObj);


    };

    // ************************************************************* numeric format

    class DateFormatUIC : public BaseUIC {
    private:
        OptionUIC           dateOrderUI;
        OptionUIC           monthFormatUI;
        BoolOptionUIC       dayHasLeadingZeroUI;
        BoolOptionUIC       yearIsFullUI;
        TextEntryUIC        delim1UI;
        TextEntryUIC        delim2UI;


    public:
        DateFormatUIC() {}

        void    InitFramedUI(char             frameTitle[],
                             Widget           parent,
                             const ArgListC&  inArgs);      // attachment resources + others

        void    SetDateFormat(const DC_DateFormat&    inDateFormat);
        void    GetDateFormat(DC_DateFormat&    outDateFormat);
        void    SetSensitive(bool isSensitive);

    private:
        void        SetInternalSensitive();
        static void SetInternalSensitiveCB(void* currObj);
    };

    class TimeFormatUIC : public OptionUIC {

    public:
        TimeFormatUIC();

        void    SetTimeFormat(const DC_DateTime::TimeFormat&    timeFormat);
        DC_DateTime::TimeFormat GetTimeFormat();
    };


    // ************************************************************* miscellaneous

    class ToggleAndPenUIC : public BaseUIC {
    public:
        ToggleBoxWidget          showData;
        PenOptionChooserWidget   dataPen;

    public:
        void            StdInit(char             label[],
                                Widget           parent,
                                const ArgListC&  inArgs);

        void            SetToggleAndPen(bool show,
                                        int  penNum);

        void            GetToggleAndPen(bool& show,
                                        int&  penNum);

        void            SetSensitive(bool isSensitive);

    };

    class PSC_EdgeSpecUIC : public BaseUIC {
    public:
        ToggleBoxWidget         plotEdgesUI;
        PenOptionChooserWidget  edgePenUI;
        LineThickOptionWidget   edgeThkUI;
        RealEntryUIC            edgePolyOffsetUI;

    private:
        bool                    polyOffsetSet;

    public:
        void            StdInit(Widget           parent,
                                const ArgListC&  inArgs,
                                bool             is3Dobject);


        void            SetEdgeSpec(const PSC_EdgeSpec& inSpec);
        void            GetEdgeSpec(PSC_EdgeSpec& outSpec);
        bool            UIdataOK(bool& allOK);

        void            SetInternalSensitive(); //used in CB - must be hooked to plotEdgesUI

    };

    class PSC_ExtrusionSpecUIC : public BaseUIC {
    public:
        ToggleBoxWidget             capEndsUI;
        ToggleBoxWidget             adjustForScalingUI;

    public:
        void            StdInit(Widget           parent,
                                const ArgListC&  inArgs);

        void            SetExtrusionSpec(const PSC_ExtrusionSpec& inSpec);
        void            GetExtrusionSpec(PSC_ExtrusionSpec& outSpec);
        bool            UIdataOK(bool& allOK);

    };

    class OffsetCoordUIC : public BaseUIC {
    public:
        RealEntryUIC    xOffsetUI;
        RealEntryUIC    yOffsetUI;
        RealEntryUIC    zOffsetUI;

    public:
        void            StdInit(char             label[],
                                Widget           parent,
                                const ArgListC&  inArgs);

        void            SetOffsetCoord(const Coord3D& inCoord);
        void            GetOffsetCoord(Coord3D& outCoord);
        bool            UIdataOK(bool& allOK);

    };


    class PSC_LegendOverrideUIC : public BaseUIC {
    public:
        ToggleBoxWidget             overrideDefaultLabelUI;
        TextEntryWidget                             overrideLabelUI;

    public:
        void            InitFramedUI(char             frameTitle[],
                                     Widget           parent,
                                     const ArgListC&  inArgs);

        void            InitFormUI(Widget          parent,
                                   const ArgListC& inArgs);

        void            SetLegendOverride(const PSC_LegendOverride& inLegendOverride);
        void            GetLegendOverride(PSC_LegendOverride& outLegendOverride);
        bool            UIdataOK(bool& allOK);
        void            SetSensitive(bool inSens);
    };

    class PSC_GridLineLabelFormatUIC : public BaseUIC {
    public:
        OptionChooserWidget         labelTypeUI;
        RealEntryUIC                labelPositionUI;
        HAlignOptionUIC             halignUI;
        VAlignOptionUIC             valignUI;
        IntEntryUIC                 labelOffsetUI;
        ToggleBoxWidget             blankLabelUI;

    public:
        void            InitFramedUI(char             frameTitle[],
                                     Widget           parent,
                                     const ArgListC&  inArgs);

        void            SetGridLineLabelFormat(const PSC_GridLineLabelFormat& inFormat);
        void            GetGridLineLabelFormat(PSC_GridLineLabelFormat& outFormat);
        bool            UIdataOK(bool& allOK);
        void            SetSensitive(bool inSens);
    };


};  // end namespace

#endif // U_MENUPPO_H

