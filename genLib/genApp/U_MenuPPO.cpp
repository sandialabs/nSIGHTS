///////////////////////////////////////////////////////////////////////////////////
//
// U_MenuPPO.cpp
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

#include "GenLibAfx.h"
#include "U_Menu.h"
#include "U_MenuPPO.h"


#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace genAppMenuUtilities {


    /////////////////////////////////////////////////////////////////////////////
    // ColorOptionChooserWidget

    ColorOptionChooserWidget::ColorOptionChooserWidget()
    {
    }

    bool ColorOptionChooserWidget::SetChoice(int inChoice)
    {
        bool bChange = currChoice != inChoice;
        currChoice = inChoice;
        CColorTextComboBox::SetCurSel(inChoice);
        return bChange;
    }

    CWnd* ColorOptionChooserWidget::GetWindow()
    {
        return this;
    }

    void ColorOptionChooserWidget::Create(const CRect& rect)
    {
        int nID = GetID();
        CWnd* pParent = GetParentWnd();
        VERIFY(CColorTextComboBox::Create(WS_VISIBLE|WS_CHILD|WS_VSCROLL|
                                          CBS_DROPDOWNLIST|CBS_HASSTRINGS|CBS_OWNERDRAWFIXED|CBS_AUTOHSCROLL|WS_TABSTOP,
                                          rect,
                                          pParent,
                                          nID));
        CColorTextComboBox::SetFont(GetMenuFont(), TRUE);

        TRACE("ColorOptionChooserWidget %d\r\n", nID);
    }

    void ColorOptionChooserWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        rcOff.bottom += (nchoice+1) * rcOff.Height(); // 'nchoice' rows
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    int ColorOptionChooserWidget::GetWindowWidth()
    {
        if (windowWidth > 0) {
            return BaseWindow::GetWindowWidth() + 12;
        }
        if (!m_bWidthCalculated) {
            CClientDC dc(AfxGetMainWnd());

            CFont* pOldFont = dc.SelectObject(GetMenuFont());
            m_nCalculatedWidth = 0;
            for (int i = 0; i < nchoice; i++) {
                CSize sz = dc.GetTextExtent(choiceLabels[i], StringLength(choiceLabels[i]));
                if (m_nCalculatedWidth < sz.cx) {
                    m_nCalculatedWidth = sz.cx;
                }
            }
            dc.SelectObject(pOldFont);

            m_nCalculatedWidth = int((m_nCalculatedWidth + 2)/3.*4);
            m_nCalculatedBorder = 3 + ::GetSystemMetrics(SM_CXVSCROLL) + 3*::GetSystemMetrics(SM_CXEDGE);
            m_bWidthCalculated = true;
        }

        return m_nCalculatedWidth + m_nCalculatedBorder + GetWidthOffset();
    }

    int ColorOptionChooserWidget::GetWindowHeight()
    {
        return GetHeightOffset() + GetFontHeight() + 2 + 2*::GetSystemMetrics(SM_CYEDGE);
    }

    BEGIN_MESSAGE_MAP(ColorOptionChooserWidget, CColorTextComboBox)
    //{{AFX_MSG_MAP(ColorOptionChooserWidget)
    //}}AFX_MSG_MAP
    ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
    END_MESSAGE_MAP()

    void ColorOptionChooserWidget::OnSelchange()
    {
        int nIndex = CColorTextComboBox::GetCurSel();
        currChoice = nIndex;
        Callback();
    }

    void ColorOptionChooserWidget::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
    {
        lpMIS->itemHeight = 2 + GetFontHeight() + 2;
    }


    /////////////////////////////////////////////////////////////////////////////
    // LineTypeOptionChooserWidget

    LineTypeOptionChooserWidget::LineTypeOptionChooserWidget()
    {
    }

    bool LineTypeOptionChooserWidget::SetChoice(int inChoice)
    {
        bool bChange = currChoice != inChoice;
        currChoice = inChoice;
        CLineTypeComboBox::SetCurSel(inChoice);
        return bChange;
    }

    CWnd* LineTypeOptionChooserWidget::GetWindow()
    {
        return this;
    }

    void LineTypeOptionChooserWidget::Create(const CRect& rect)
    {
        int nID = GetID();
        CWnd* pParent = GetParentWnd();
        VERIFY(CLineTypeComboBox::Create(WS_VISIBLE|WS_CHILD|WS_VSCROLL|
                                         CBS_DROPDOWNLIST|CBS_HASSTRINGS|CBS_OWNERDRAWFIXED|CBS_AUTOHSCROLL|WS_TABSTOP,
                                         rect,
                                         pParent,
                                         nID));
        CLineTypeComboBox::SetFont(GetMenuFont(), TRUE);

        for (int i = 0; i < lineTypeData.MaxLineType(); i++)
            AddLineType(i);

        TRACE("LineTypeOptionChooserWidget %d\r\n", nID);
    }

    void LineTypeOptionChooserWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        rcOff.bottom += (nchoice+1) * rcOff.Height(); // 'nchoice' rows
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    int LineTypeOptionChooserWidget::GetWindowWidth()
    {
        if (windowWidth > 0) {
            return BaseWindow::GetWindowWidth() + 12;
        }
        if (!m_bWidthCalculated) {
            CClientDC dc(AfxGetMainWnd());

            CFont* pOldFont = dc.SelectObject(GetMenuFont());
            m_nCalculatedWidth = 0;
            for (int i = 0; i < nchoice; i++) {
                CSize sz = dc.GetTextExtent(choiceLabels[i], StringLength(choiceLabels[i]));
                if (m_nCalculatedWidth < sz.cx) {
                    m_nCalculatedWidth = sz.cx;
                }
            }
            dc.SelectObject(pOldFont);

            m_nCalculatedWidth = int((m_nCalculatedWidth + 2)/3.*4);
            m_nCalculatedBorder = 3 + ::GetSystemMetrics(SM_CXVSCROLL) + 3*::GetSystemMetrics(SM_CXEDGE);
            m_bWidthCalculated = true;
        }

        return m_nCalculatedWidth + m_nCalculatedBorder + GetWidthOffset();
    }

    int LineTypeOptionChooserWidget::GetWindowHeight()
    {
        return GetHeightOffset() + GetFontHeight() + 2 + 2*::GetSystemMetrics(SM_CYEDGE);
    }

    BEGIN_MESSAGE_MAP(LineTypeOptionChooserWidget, CLineTypeComboBox)
    //{{AFX_MSG_MAP(LineTypeOptionChooserWidget)
    //}}AFX_MSG_MAP
    ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
    END_MESSAGE_MAP()

    void LineTypeOptionChooserWidget::OnSelchange()
    {
        int nIndex = CLineTypeComboBox::GetCurSel();
        currChoice = nIndex;
        Callback();
    }

    void LineTypeOptionChooserWidget::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
    {
        lpMIS->itemHeight = 2 + GetFontHeight() + 2;
    }


    /////////////////////////////////////////////////////////////////////////////
    // SymbolTypeOptionChooserWidget

    SymbolTypeOptionChooserWidget::SymbolTypeOptionChooserWidget()
    {
    }

    bool SymbolTypeOptionChooserWidget::SetChoice(int inChoice)
    {
        bool bChange = currChoice != inChoice;
        currChoice = inChoice;
        CSymbolTypeComboBox::SetCurSel(inChoice);
        return bChange;
    }

    CWnd* SymbolTypeOptionChooserWidget::GetWindow()
    {
        return this;
    }

    void SymbolTypeOptionChooserWidget::Create(const CRect& rect)
    {
        int nID = GetID();
        CWnd* pParent = GetParentWnd();
        VERIFY(CSymbolTypeComboBox::Create(WS_VISIBLE|WS_CHILD|WS_VSCROLL|
                                           CBS_DROPDOWNLIST|CBS_HASSTRINGS|CBS_OWNERDRAWFIXED|CBS_AUTOHSCROLL|WS_TABSTOP,
                                           rect,
                                           pParent,
                                           nID));
        CSymbolTypeComboBox::SetFont(GetMenuFont(), TRUE);

        for (int i = 0; i <= int(ps_FDiamond); i++)
            AddSymbol(PlotSymbol (i));

        TRACE("SymbolTypeOptionChooserWidget %d\r\n", nID);
    }

    void SymbolTypeOptionChooserWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        rcOff.bottom += (nchoice+1) * rcOff.Height(); // 'nchoice' rows
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    int SymbolTypeOptionChooserWidget::GetWindowWidth()
    {
        if (windowWidth > 0) {
            return BaseWindow::GetWindowWidth() + 12;
        }
        if (!m_bWidthCalculated) {
            CClientDC dc(AfxGetMainWnd());

            CFont* pOldFont = dc.SelectObject(GetMenuFont());
            m_nCalculatedWidth = 0;
            for (int i = 0; i < nchoice; i++) {
                CSize sz = dc.GetTextExtent(choiceLabels[i], StringLength(choiceLabels[i]));
                if (m_nCalculatedWidth < sz.cx) {
                    m_nCalculatedWidth = sz.cx;
                }
            }
            dc.SelectObject(pOldFont);

            m_nCalculatedWidth = int((m_nCalculatedWidth + 2)/3.*4);
            m_nCalculatedBorder = 3 + ::GetSystemMetrics(SM_CXVSCROLL) + 3*::GetSystemMetrics(SM_CXEDGE);
            m_bWidthCalculated = true;
        }

        return m_nCalculatedWidth + m_nCalculatedBorder + GetWidthOffset();
    }

    int SymbolTypeOptionChooserWidget::GetWindowHeight()
    {
        return GetHeightOffset() + GetFontHeight() + 2 + 2*::GetSystemMetrics(SM_CYEDGE);
    }

    BEGIN_MESSAGE_MAP(SymbolTypeOptionChooserWidget, CSymbolTypeComboBox)
    //{{AFX_MSG_MAP(SymbolTypeOptionChooserWidget)
    //}}AFX_MSG_MAP
    ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
    END_MESSAGE_MAP()

    void SymbolTypeOptionChooserWidget::OnSelchange()
    {
        int nIndex = CSymbolTypeComboBox::GetCurSel();
        currChoice = nIndex;
        Callback();
    }

    void SymbolTypeOptionChooserWidget::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
    {
        lpMIS->itemHeight = 2 + GetFontHeight() + 2;
    }

    static const char* penLabels[]  = { "Background", "pen 1", "pen 2",
                                        "pen 3", "pen 4", "pen 5",
                                        "pen 6", "pen 7", "pen 8",
                                        "pen 9", "pen 10","pen 11",
                                        "pen 12","pen 13","pen 14",
                                        "pen 15","pen 16","pen 17",
                                        "pen 18","pen 19","pen 20",
                                        "pen 21","pen 22","pen 23"};

    PenOptionChooserWidget::PenOptionChooserWidget() :
        pensSet(false)
    {
        PreInitSetUp(penLabels, DC_PenSet::maxPenSet);
    }

    void PenOptionChooserWidget::ResetPens(DC_PenSet* penData)
    {
        ResetContent();
        for ( int i = 0; i < DC_PenSet::maxPenSet; i++)
            {
                penColors[i] = ConvertSC_ColorSpec(penData->GetColor(i));

                AddColor(penData->GetLabel(i), penColors[i]);
            }

        pensSet = true;

    }

    void PenOptionChooserWidget::SetPenSelection(int penIndx)
    {
        SetChoice(penIndx);
    }



    LineThickOptionWidget::LineThickOptionWidget()
    {
        PreInitSetUp();
    }

    const int LineThickOptionWidget::lineThickMap[] = {1, 2, 3, 5, 8, 10, 12, 16, 24, 32};

    void LineThickOptionWidget::SetLineThick(int lThick)
    {
        for ( int i = 0; i < nlineThickChoice; i++)
            if (lThick == lineThickMap[i])
                {
                    SetChoice(i);
                    return;
                }
        SetChoice(0);
    }

    int LineThickOptionWidget::GetLineThick()
    {
        return lineThickMap[GetChoice()];
    }

    void LineThickOptionWidget::PreInitSetUp()
    {
        static const char* thkLabels[]  = {"1 pix",  "2 pix",  "3 pix",
                                           "5 pix",  "8 pix",  "10 pix",
                                           "12 pix", "16 pix", "24 pix", "32 pix"};
        ChooserWidget::PreInitSetUp(thkLabels, nlineThickChoice);
    }


    LineTypeOptionWidget::LineTypeOptionWidget()
    {
        PreInitSetUp();
    }

    void LineTypeOptionWidget::PreInitSetUp()
    {
        SC_ConstStringArray ltStrs;
        lineTypeData.GetPatternNames(ltStrs);
        ChooserWidget::PreInitSetUp(ltStrs);
    }

    void LineTypeOptionWidget::SetLineColor(COLORREF inColor)
    {
        lineColor = inColor;
        Invalidate();
    }

    void LineTypeOptionWidget::SetLineThick(int lThick)
    {
        lineThick = lThick;
        Invalidate();
    }

    void LineTypeOptionWidget::SetLineColorAndThick(COLORREF inColor, int lThick)
    {
        lineColor = inColor;
        lineThick = lThick;
        Invalidate();
    }


    PlotSymLineOptionWidget::PlotSymLineOptionWidget()
    {
        PreInitSetUp();
    }

    void PlotSymLineOptionWidget::PreInitSetUp()
    {
        static const char* slLabels[]  = {"  Line  ", "  Symbol  ", "Line+Sym"};
        ChooserWidget::PreInitSetUp(slLabels, 3);
    }

    SymbolTypeOptionWidget::SymbolTypeOptionWidget()
    {
        PreInitSetUp();
    }

    void SymbolTypeOptionWidget::PreInitSetUp()
    {
        static const char* stLabels[]  = {"XXX", "XXX", "XXX", "XXX", "XXX",
                                          "XXX", "XXX", "XXX", "XXX", "XXX"};
        ChooserWidget::PreInitSetUp(stLabels, 10);
    }

    void SymbolTypeOptionWidget::SetSymbolColor(COLORREF inColor)
    {
        symColor = inColor;
        Invalidate();
    }

    void SymbolTypeOptionWidget::SetSymbolSize(int sSize)
    {
        symSize  = sSize;
        Invalidate();
    }

    void SymbolTypeOptionWidget::SetSymbolLThick(int sLThick)
    {
        symLThick = sLThick;
        Invalidate();
    }
    void SymbolTypeOptionWidget::SetSymbolAttributes(COLORREF inColor, int sSize, int sLThick)
    {
        symColor = inColor;
        symSize  = sSize;
        symLThick = sLThick;
        Invalidate();
    }

    SymbolSizeOptionWidget::SymbolSizeOptionWidget()
    {
        PreInitSetUp();
    }


    const int   SymbolSizeOptionWidget::symbolSizeMap[] = {2,  4,  6,  8,  10, 12,
                                                           14, 16, 18, 25, 30, 35,
                                                           40, 50, 60, 70, 80, 100};

    void SymbolSizeOptionWidget::SetSymbolSize(int sSize)
    {
        for ( int i = 0; i < nsymbolSizeChoice; i++)
            {
                if (sSize == symbolSizeMap[i])
                    {
                        SetChoice(i);
                        return;
                    }
            }
        SetChoice(0);
    }

    int SymbolSizeOptionWidget::GetSymbolSize()
    {
        return symbolSizeMap[GetChoice()];
    }

    void SymbolSizeOptionWidget::PreInitSetUp()
    {
        static const char* sizLabels[nsymbolSizeChoice]  = {" 2 pix "," 4 pix", " 6 pix", " 8 pix", "10 pix", "12 pix",
                                                            "14 pix", "16 pix", "18 pix", "25 pix", "30 pix", "35 pix",
                                                            "40 pix", "50 pix", "60 pix", "70 pix", "80 pix",  "100 pix"};


        ChooserWidget::PreInitSetUp(sizLabels, nsymbolSizeChoice);
    }


    TubePointsOptionWidget::TubePointsOptionWidget()
    {
        PreInitSetUp();
    }

    const int   TubePointsOptionWidget::tubePointsMap[] = {3, 4, 8, 20};

    void TubePointsOptionWidget::SetTubePoints(int sSize)
    {
        for ( int i = 0; i < ntubePointsChoice; i++)
            {
                if (sSize == tubePointsMap[i])
                    {
                        SetChoice(i);
                        return;
                    }
            }
        SetChoice(0);
    }

    int TubePointsOptionWidget::GetTubePoints()
    {
        return tubePointsMap[GetChoice()];
    }

    void TubePointsOptionWidget::PreInitSetUp()
    {
        static const char* tubePointLabels[]  = {" Triangle ", " Square ", " Octagon ", " Round "};
        ChooserWidget::PreInitSetUp(tubePointLabels, ntubePointsChoice);
    }


    HAlignOptionWidget::HAlignOptionWidget()
    {
        PreInitSetUp();
    }

    void HAlignOptionWidget::PreInitSetUp()
    {
        static const char* haLabels[]  = {"Left", "Centre", "Right"};
        ChooserWidget::PreInitSetUp(haLabels, 3);
    }

    VAlignOptionWidget::VAlignOptionWidget()
    {
        PreInitSetUp();
    }

    void VAlignOptionWidget::PreInitSetUp()
    {
        static const char* vaLabels[]  = {"Top", "Centre", "Bottom"};
        ChooserWidget::PreInitSetUp(vaLabels, 3);
    }

    AxesIncOptionWidget::AxesIncOptionWidget()
    {
        PreInitSetUp();
    }

    void AxesIncOptionWidget::PreInitSetUp()
    {
        static const char* incLabels[]  = {"None", "Tic", "Grid"};
        ChooserWidget::PreInitSetUp(incLabels, 3);
    }
    void AxesIncOptionWidget::ShortTextPreInitSetUp()
    {
        static const char* incLabels[]  = {"N", "T", "G"};
        ChooserWidget::PreInitSetUp(incLabels, 3);
    }


    // ************************************************************* series

    void SeriesDataUIC::SeriesSensitiveCB(void* currObj)
    {
        SeriesDataUIC* currSer = static_cast<SeriesDataUIC*>(currObj);
        currSer->SetSensitive(true);
    }

    void  SeriesDataUIC::InitSeriesWidgets()
    {
        ArgListC args;

        args.StdFormAttach();
        topRowForm.InitWidget(GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.BottomFormAttach();
        args.SetPlaceHolder();

        args.RightPosAttach(40);
        seriesPlotType.InitWidget(topRowForm.GetWidget(), args);
        seriesPlotType.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.ResetPlace();
        args.RightPosAttach(65);
        symbolType.InitWidget(topRowForm.GetWidget(), args);

        args.ResetPlace();
        args.RightFormAttach();
        lineType.InitWidget(topRowForm.GetWidget(), args);

        args.StdTopObjectAttach(topRowForm.GetWidget());
        botRowForm.InitWidget(GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.TopOffset(5);
        args.BottomFormAttach();
        args.SetPlaceHolder();
        args.RightPosAttach(40);
        seriesPen.InitWidget(botRowForm.GetWidget(), args);
        seriesPen.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.ResetPlace();
        args.RightPosAttach(65);
        symbolSize.InitWidget(botRowForm.GetWidget(), args);
        symbolSize.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.ResetPlace();
        args.RightPosAttach(80);
        mitreLinesUI.StdToggleInit("mitre", botRowForm.GetWidget(), args);

        args.ResetPlace();
        args.RightFormAttach();
        lineThickness.InitWidget(botRowForm.GetWidget(), args);
        lineThickness.AddOtherObjectCallback(SeriesSensitiveCB, this);
    }

    void  SeriesDataUIC::InitFramedUI(      char       frameTitle[],
                                            Widget     parent,
                                            const ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        InitSeriesWidgets();
    }

    void  SeriesDataUIC::InitFormUI(      Widget     parent,
                                          const ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);
        InitSeriesWidgets();
    }

    void SeriesDataUIC::ResetPens(DC_PenSet* penData)
    {
        seriesPen.ResetPens(penData);
    }

    void SeriesDataUIC::SetSeriesData(const PSC_SeriesSpec& seriesData)
    {
        seriesPlotType.SetPlotSymLine(seriesData.seriesPlotType);
        seriesPen.SetPenSelection(seriesData.seriesPen);

        symbolType.SetSymbolType(seriesData.symbolType);
        symbolSize.SetSymbolSize(seriesData.symbolSize);

        lineType.SetLineType(seriesData.lineType);
        lineThickness.SetLineThick(seriesData.lineThickness);
        mitreLinesUI.SetValue(seriesData.mitreLines);
    }


    void SeriesDataUIC::GetSeriesData(PSC_SeriesSpec& seriesData)
    {
        seriesData.seriesPlotType   = seriesPlotType.GetPlotSymLine();
        seriesData.seriesPen        = seriesPen.GetPenSelection();

        seriesData.symbolType       = symbolType.GetSymbolType();
        seriesData.symbolSize       = symbolSize.GetSymbolSize();

        seriesData.lineType         = lineType.GetLineType();
        seriesData.lineThickness    = lineThickness.GetLineThick();
        seriesData.mitreLines           = mitreLinesUI.GetValue();
    }


    void SeriesDataUIC::SetSensitive(bool inSens)
    {
        BaseUIC::SetSensitive(inSens);
        if (!inSens)
            {
                seriesPlotType.SetSensitive(false);
                seriesPen.SetSensitive(false);

                symbolType.SetSensitive(false);
                symbolSize.SetSensitive(false);

                lineType.SetSensitive(false);
                lineThickness.SetSensitive(false);
                mitreLinesUI.SetSensitive(false);
            }
        else
            {
                seriesPlotType.SetSensitive(true);
                seriesPen.SetSensitive(true);
                //  also symbol thick
                lineThickness.SetSensitive(true);

                bool symOK = seriesPlotType.GetPlotSymLine() != psl_LineOnly;
                bool lineOK = seriesPlotType.GetPlotSymLine() != psl_SymbolOnly;
                symbolType.SetSensitive(symOK);
                symbolSize.SetSensitive(symOK);
                symbolType.SetSymbolAttributes(seriesPen.GetSelectedColorValue(), symbolSize.GetSymbolSize(), lineThickness.GetLineThick());

                lineType.SetSensitive(lineOK);
                lineType.SetLineColorAndThick(seriesPen.GetSelectedColorValue(), lineThickness.GetLineThick());
                mitreLinesUI.SetSensitive(lineOK);
            }
    }

    // ************************************************************* symbols only

    void SymbolSpecUIC::SetSensitiveCB(void* currObj)
    {
        SymbolSpecUIC* currSer = static_cast<SymbolSpecUIC*>(currObj);
        currSer->SetSensitive(true);
    }


    void  SymbolSpecUIC::InitWidgets()
    {
        ArgListC args;

        args.StdSplitSetup(30);
        symbolTypeUI.InitWidget(GetForm(), args);

        args.StdSplitMiddle(35, 65);
        symbolSizeUI.InitWidget(GetForm(), args);
        symbolSizeUI.AddOtherObjectCallback(SetSensitiveCB, this);


        args.StdSplitFinish(30);
        symbolPenUI.InitWidget(GetForm(), args);
        symbolPenUI.AddOtherObjectCallback(SetSensitiveCB, this);
    }

    void  SymbolSpecUIC::InitFramedUI(char             frameTitle[],
                                      Widget           parent,
                                      const ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        InitWidgets();
    }

    void  SymbolSpecUIC::InitFormUI(Widget           parent,
                                    const ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);
        InitWidgets();
    }

    void SymbolSpecUIC::ResetPens(DC_PenSet* penData)
    {
        symbolPenUI.ResetPens(penData);
    }

    void SymbolSpecUIC::SetSymbolSpec(const PSC_SymbolSpec& inSpec)
    {
        symbolPenUI.SetPenSelection(inSpec.symbolPen);
        symbolTypeUI.SetSymbolType(inSpec.symbolType);
        symbolSizeUI.SetSymbolSize(inSpec.symbolSize);
    }


    void SymbolSpecUIC::GetSymbolSpec(PSC_SymbolSpec& outSpec)
    {
        outSpec.symbolPen  = symbolPenUI.GetPenSelection();
        outSpec.symbolType = symbolTypeUI.GetSymbolType();
        outSpec.symbolSize = symbolSizeUI.GetSymbolSize();
    }


    void SymbolSpecUIC::SetSensitive(bool inSens)
    {
        BaseUIC::SetSensitive(inSens);
        symbolPenUI.SetSensitive(inSens);
        symbolTypeUI.SetSensitive(inSens);
        if (inSens)
            symbolTypeUI.SetSymbolAttributes(symbolPenUI.GetSelectedColorValue(), symbolSizeUI.GetSymbolSize(), 1);
        symbolSizeUI.SetSensitive(inSens);
    }
    // ************************************************************* lines only


    void LineSpecUIC::SetSensitiveCB(void* currObj)
    {
        static_cast<LineSpecUIC*>(currObj)->SetSensitive(true);
    }

    void  LineSpecUIC::InitWidgets()
    {
        ArgListC args;

        args.StdSplitSetup(30);
        lineTypeUI.InitWidget(GetForm(), args);

        args.StdSplitMiddle(35, 65);
        lineThkUI.InitWidget(GetForm(), args);
        lineThkUI.AddOtherObjectCallback(SetSensitiveCB, this);

        args.StdSplitFinish(30);
        linePenUI.InitWidget(GetForm(), args);
        linePenUI.AddOtherObjectCallback(SetSensitiveCB, this);
    }

    void  LineSpecUIC::InitFramedUI(      char             frameTitle[],
                                          Widget           parent,
                                          const ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        InitWidgets();
    }

    void  LineSpecUIC::InitFormUI(      Widget           parent,
                                        const ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);
        InitWidgets();
    }

    void LineSpecUIC::ResetPens(DC_PenSet* penData)
    {
        linePenUI.ResetPens(penData);
    }

    void LineSpecUIC::SetLineSpec(const PSC_LineSpec& inSpec)
    {
        linePenUI.SetPenSelection(inSpec.linePen);
        lineTypeUI.SetLineType(inSpec.lineType);
        lineThkUI.SetLineThick(inSpec.lineThickness);
    }


    void LineSpecUIC::GetLineSpec(PSC_LineSpec& outSpec)
    {
        outSpec.linePen  = linePenUI.GetPenSelection();
        outSpec.lineType = lineTypeUI.GetLineType();
        outSpec.lineThickness = lineThkUI.GetLineThick();
    }


    void LineSpecUIC::SetSensitive(bool inSens)
    {
        BaseUIC::SetSensitive(inSens);
        linePenUI.SetSensitive(inSens);
        lineTypeUI.SetSensitive(inSens);
        lineThkUI.SetSensitive(inSens);

        lineTypeUI.SetLineColor(linePenUI.GetSelectedColorValue());
        lineTypeUI.SetLineThick(lineThkUI.GetLineThick());
    }

    // ************************************************************* stipples only


    void StippleSpecUIC::SetSensitiveCB(void* currObj)
    {
        static_cast<StippleSpecUIC*>(currObj)->SetSensitive(true);
    }

    void  StippleSpecUIC::InitWidgets()
    {
        ArgListC args;

        args.StdSplitSetup(25);
        SC_ConstStringArray specStrs;
        stippleSpecData.GetStippleNames(specStrs);
        stippleTypeUI.PreInitSetUp(specStrs);
        stippleTypeUI.AttachLabelToObject();
        stippleTypeUI.InitLabeledUI("Type", GetForm(), args);

        args.StdSplitMiddle(25, 50);
        stippleSizeUI.AttachLabelToObject();
        stippleSizeUI.InitLabeledUI("Size", GetForm(), args);

        args.StdSplitMiddle(50, 75);
        stippleLineThkUI.AttachLabelToObject();
        stippleLineThkUI.InitLabeledUI("Line thk", GetForm(), args);
        stippleLineThkUI.AddOtherObjectCallback(SetSensitiveCB, this);

        args.StdSplitFinish(20);
        stipplePenUI.InitWidget(GetForm(), args);
        stipplePenUI.AddOtherObjectCallback(SetSensitiveCB, this);
    }

    void  StippleSpecUIC::InitFramedUI(char             frameTitle[],
                                       Widget           parent,
                                       const ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        InitWidgets();
    }

    void  StippleSpecUIC::InitFormUI(Widget           parent,
                                     const ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);
        InitWidgets();
    }

    void StippleSpecUIC::ResetPens(DC_PenSet* penData)
    {
        stipplePenUI.ResetPens(penData);
    }

    void StippleSpecUIC::SetStippleSpec(const PSC_StippleSpec& inSpec)
    {
        stippleTypeUI.SetChoice(inSpec.stippleType);
        stipplePenUI.SetPenSelection(inSpec.stipplePen);
        stippleLineThkUI.SetLineThick(inSpec.stippleLineThickness);
        stippleSizeUI.SetSymbolSize(inSpec.stippleSize);
    }


    void StippleSpecUIC::GetStippleSpec(PSC_StippleSpec& outSpec)
    {
        outSpec.stippleType = stippleTypeUI.GetChoice();
        outSpec.stipplePen  = stipplePenUI.GetPenSelection();
        outSpec.stippleLineThickness = stippleLineThkUI.GetLineThick();
        outSpec.stippleSize = stippleSizeUI.GetSymbolSize();
    }


    void StippleSpecUIC::SetSensitive(bool inSens)
    {
        BaseUIC::SetSensitive(inSens);
        stippleTypeUI.SetSensitive(inSens);
        stipplePenUI.SetSensitive(inSens);
        stippleLineThkUI.SetSensitive(inSens);
        stippleSizeUI.SetSensitive(inSens);
    }



    void Series3DDataUIC::SeriesSensitiveCB(void* currObj)
    {
        static_cast<Series3DDataUIC*>(currObj)->SetSensitive(true);;
    }

    void  Series3DDataUIC::InitSeriesWidgets()
    {
        ArgListC args;

        args.StdFormAttach();
        topRowForm.InitWidget(GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.BottomFormAttach();
        args.SetPlaceHolder();

        args.RightPosAttach(20);
        seriesPlotType.InitWidget(topRowForm.GetWidget(), args);
        seriesPlotType.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.ResetPlace();
        args.RightPosAttach(40);
        symbolType.InitWidget(topRowForm.GetWidget(), args);

        args.ResetPlace();
        args.RightPosAttach(60);
        plotLineAsTubeUI.PreInitSetUp("Line", "Tube");
        plotLineAsTubeUI.InitWidget(topRowForm.GetWidget(), args);
        plotLineAsTubeUI.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.ResetPlace();
        args.RightPosAttach(80);
        plotLineTypeUI.InitWidget(topRowForm.GetWidget(), args);
        plotLineTypeUI.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.ResetPlace();
        args.RightFormAttach();
        nextrusionPointsUI.PreInitSetUp();
        nextrusionPointsUI.InitWidget(topRowForm.GetWidget(), args);

        args.Clear();
        args.StdTopObjectAttach(topRowForm.GetWidget());
        midRowForm.InitWidget(GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.BottomFormAttach();
        args.SetPlaceHolder();
        args.RightPosAttach(20);
        seriesPen.InitWidget(midRowForm.GetWidget(), args);
        seriesPen.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.ResetPlace();
        args.RightPosAttach(40);
        symbolSize.InitWidget(midRowForm.GetWidget(), args);
        symbolSize.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.ResetPlace();
        args.RightPosAttach(80);
        lineThickness.InitWidget(midRowForm.GetWidget(), args);
        lineThickness.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.ResetPlace();
        args.RightFormAttach();
        extrusionPixelSizeUI.InitWidget(midRowForm.GetWidget(), args);

        args.StdTopObjectAttach(midRowForm.GetWidget());
        hSep.InitWidget(GetForm(), args);

        args.StdTopObjectAttach(hSep.GetWidget());
        botRowForm.InitWidget(GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.LeftPosAttach(5);

        plotSymEdgesUI.StdToggleInit("SymEdges", &botRowForm, args);
        plotSymEdgesUI.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.Clear();
        args.TopWidgetAttach(plotSymEdgesUI.GetWidget());
        args.LeftPosAttach(5);

        plotTubeEdgesUI.StdToggleInit("TubeEdges", &botRowForm, args);
        plotTubeEdgesUI.AddOtherObjectCallback(SeriesSensitiveCB, this);

        args.Clear();
        args.TopFormAttach(10);
        args.RightPosAttach(50);
        edgePenUI.InitWidget(botRowForm.GetWidget(), args);

        args.Clear();
        args.TopFormAttach(10);
        args.RightPosAttach(75);
        edgeThkUI.InitWidget(botRowForm.GetWidget(), args);

        args.Clear();
        args.TopFormAttach(10);
        args.LeftWidgetAttach(edgeThkUI.GetWidget());
        args.LeftOffset(10);
        args.RightFormAttach();
        edgePolyOffsetUI.SetFieldLen(6);
        edgePolyOffsetUI.InitLabeledUI("PolyOff:", botRowForm.GetWidget(), args);
        edgePolyOffsetUI.format = SC_RealFormat(ncf_Decimal, 3);
    }

    void  Series3DDataUIC::InitFramedUI(char             frameTitle[],
                                        Widget           parent,
                                        const ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        InitSeriesWidgets();
    }

    void  Series3DDataUIC::InitFormUI(Widget           parent,
                                      const ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);
        InitSeriesWidgets();
    }

    void Series3DDataUIC::ResetPens(DC_PenSet* penData)
    {
        seriesPen.ResetPens(penData);
        edgePenUI.ResetPens(penData);
    }

    void Series3DDataUIC::Set3DSeriesData(const PSC_3DSeriesSpec& seriesData)
    {
        seriesPlotType.SetPlotSymLine(seriesData.seriesPlotType);
        seriesPen.SetPenSelection(seriesData.seriesPen);

        symbolType.SetSymbolType(seriesData.symbolType);
        symbolSize.SetSymbolSize(seriesData.symbolSize);

        plotLineTypeUI.SetLineType(seriesData.plotLineType);
        lineThickness.SetLineThick(seriesData.lineThickness);
        plotLineAsTubeUI.SetBoolChoice(seriesData.plotLineAsTube);

        nextrusionPointsUI.SetTubePoints(seriesData.nextrusionPoints);
        extrusionPixelSizeUI.SetSymbolSize(seriesData.extrusionPixelSize);

        plotSymEdgesUI.SetValue(seriesData.plotSymEdges);
        plotTubeEdgesUI.SetValue(seriesData.plotTubeEdges);

        edgePenUI.SetPenSelection(seriesData.edgePen);
        edgeThkUI.SetLineThick(seriesData.edgeThk);
        edgePolyOffsetUI.SetFieldValue(seriesData.edgePolyOffset);
    }


    void Series3DDataUIC::Get3DSeriesData(PSC_3DSeriesSpec& seriesData)
    {
        seriesData.seriesPlotType   = seriesPlotType.GetPlotSymLine();
        seriesData.seriesPen        = seriesPen.GetPenSelection();

        seriesData.symbolType       = symbolType.GetSymbolType();
        seriesData.symbolSize       = symbolSize.GetSymbolSize();

        seriesData.plotLineAsTube       = plotLineAsTubeUI.GetBoolChoice();
        seriesData.plotLineType     = plotLineTypeUI.GetLineType();
        seriesData.lineThickness    = lineThickness.GetLineThick();

        seriesData.nextrusionPoints = nextrusionPointsUI.GetTubePoints();
        seriesData.extrusionPixelSize = extrusionPixelSizeUI.GetSymbolSize();

        seriesData.plotSymEdges         = plotSymEdgesUI.GetValue();
        seriesData.plotTubeEdges        = plotTubeEdgesUI.GetValue();

        seriesData.edgePen  = edgePenUI.GetPenSelection();
        seriesData.edgeThk  = edgeThkUI.GetLineThick();
        seriesData.edgePolyOffset   = edgePolyOffsetUI.GetFieldValue();
    }


    void Series3DDataUIC::SetSensitive(bool inSens)
    {
        BaseUIC::SetSensitive(inSens);
        if (!inSens)
            {
                topRowForm.SetSensitive(false);
                midRowForm.SetSensitive(false);
                botRowForm.SetSensitive(false);
            }
        else
            {
                seriesPlotType.SetSensitive(true);
                seriesPen.SetSensitive(true);

                //  also symbol thick

                bool lineOK = seriesPlotType.GetPlotSymLine() != psl_SymbolOnly;
                bool symOK = seriesPlotType.GetPlotSymLine() != psl_LineOnly;
                bool isLine = lineOK && (!plotLineAsTubeUI.GetBoolChoice());
                bool isTube = lineOK && plotLineAsTubeUI.GetBoolChoice();

                symbolType.SetSensitive(symOK);
                symbolType.SetSymbolAttributes(seriesPen.GetSelectedColorValue(),
                                               symbolSize.GetSymbolSize(), lineThickness.GetLineThick());

                plotLineAsTubeUI.SetSensitive(lineOK);

                symbolSize.SetSensitive(symOK);
                lineThickness.SetSensitive(symOK || isLine);

                plotLineTypeUI.SetSensitive(isLine);
                plotLineTypeUI.SetLineColorAndThick(seriesPen.GetSelectedColorValue(),
                                                    lineThickness.GetLineThick());
                nextrusionPointsUI.SetSensitive(isTube);
                extrusionPixelSizeUI.SetSensitive(isTube);

                plotSymEdgesUI.SetSensitive(symOK);
                plotTubeEdgesUI.SetSensitive(isTube);

                bool edgeOK = ((symOK && plotSymEdgesUI.GetValue()) || (isTube && plotTubeEdgesUI.GetValue()));

                edgePenUI.SetSensitive(edgeOK);
                edgeThkUI.SetSensitive(edgeOK);
                edgePolyOffsetUI.SetSensitive(edgeOK);;
            }
    }

    // ************************************************************* fonts


    const int FontUIC::fontSizeMap[FontUIC::nfontSize] = {4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 18, 24, 30, 40};
    const double FontUIC::fontThickMap[FontUIC::nfontThick] = {0.0, 0.05, 0.1, 0.2, 0.5};

    void  FontUIC::InitFramedUI(      char       frameTitle[],
                                      Widget     parent,
                                      const ArgListC&  inArgs)

    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);

        static const char*   familyLabels[]  = {"Arial", "Times", "Courier"};
        fontFamilyChooser.PreInitSetUp(familyLabels, 3);
        fontFamilyChooser.AttachLabelToObject();
        fontFamilyChooser.SetOffsets(0, 0, 0, 0);

        static const char*   sizeLabels[nfontSize]  = {" 4 ", " 6 ", " 7 ", " 8 ", " 9 ", " 10 ", " 11 ", " 12 ", " 13 ", " 14 ", " 16 ", " 18 ", " 24 ", " 30 ", " 40 "};
        fontSizeChooser.PreInitSetUp(sizeLabels, nfontSize);
        fontSizeChooser.AttachLabelToObject();
        fontSizeChooser.SetOffsets(0, 0, 0, 0);

        fontWeightChooser.PreInitSetUp("Medium", "Bold");
        fontWeightChooser.AttachLabelToObject();
        fontWeightChooser.SetOffsets(0, 0, 0, 0);

        fontSlantChooser.PreInitSetUp("Reg.", "Italic");
        fontSlantChooser.AttachLabelToObject();
        fontSlantChooser.SetOffsets(0, 0, 0, 0);

        int xInc = 25;
        if (useRotation)
            {
                static const char*   rotationLabels[]  = {"Horiz", "Left", "Right"};
                fontRotationUI.PreInitSetUp(rotationLabels, 3);
                fontRotationUI.AttachLabelToObject();
                fontRotationUI.SetOffsets(0, 0, 0, 0);
                xInc = 20;
            }
        else if (is3DFont)
            {
                static const char*   thicknessLabels[nfontThick]  = {" Flat ", " Thin ", " Med ", "Thick", "V.Thk"};
                fontThicknessUI.PreInitSetUp(thicknessLabels, nfontThick);
                fontThicknessUI.AttachLabelToObject();
                fontThicknessUI.SetOffsets(0, 0, 0, 0);
                xInc = 20;
            }

        int xPos = xInc;

        ArgListC args;
        args.TopFormAttach();
        args.SetPlaceHolder();
        args.RightPosAttach(xPos);
        args.LeftFormAttach();
        fontFamilyChooser.InitLabeledUI("Fam:", GetForm(), args);

        args.ResetPlace();
        xPos += xInc;
        args.RightPosAttach(xPos);
        args.LeftBaseAttach(fontFamilyChooser);
        fontWeightChooser.InitLabeledUI("Wt:", GetForm(), args);

        args.ResetPlace();
        xPos += xInc;
        args.LeftBaseAttach(fontWeightChooser);
        args.RightPosAttach(xPos);
        fontSlantChooser.InitLabeledUI("Slnt:", GetForm(), args);

        args.ResetPlace();
        xPos += xInc;
        args.RightPosAttach(xPos);
        args.LeftBaseAttach(fontSlantChooser);
        fontSizeChooser.InitLabeledUI("Size:", GetForm(), args);

        if (useRotation || is3DFont)
            {
                args.ResetPlace();
                args.RightFormAttach();
                args.LeftBaseAttach(fontSizeChooser);
                if (useRotation)
                    fontRotationUI.InitLabeledUI("Rot:", GetForm(), args);
                else
                    fontThicknessUI.InitLabeledUI("Thk:", GetForm(), args);
            }

    }

    void FontUIC::SetFontSettings(const PSC_Font& inFont)
    {
        fontFamilyChooser.SetChoice(inFont.fontFamily);

        fontWeightChooser.SetBoolChoice(!inFont.weightMedium);

        fontSlantChooser.SetBoolChoice(!inFont.slantRegular);

        int fontIndx = 1;
        int i;
        for (i = 0; i < nfontSize; i++ )
            if (RealsAreSame(inFont.fontSize, double(fontSizeMap[i]), 0.01))
                fontIndx = i;
        fontSizeChooser.SetChoice(fontIndx);

        if (useRotation)
            fontRotationUI.SetChoice(int(inFont.fontRotation));
        else if (is3DFont)
            {
                fontIndx = 1;
                for (i = 0; i < nfontThick; i++ )
                    if (RealsAreSame(inFont.fontThickness, fontThickMap[i], 0.01))
                        fontIndx = i;
                fontThicknessUI.SetChoice(fontIndx);
            }

    }

    void FontUIC::GetFontSettings(PSC_Font& outFont)
    {
        outFont.fontFamily = PSC_Font::FontFamily(fontFamilyChooser.GetChoice());
        outFont.weightMedium = (!fontWeightChooser.GetBoolChoice());
        outFont.slantRegular = !fontSlantChooser.GetBoolChoice();
        outFont.fontSize = double(fontSizeMap[fontSizeChooser.GetChoice()]);

        if (useRotation)
            outFont.fontRotation = PSC_Font::FontRotation(fontRotationUI.GetChoice());
        else if (is3DFont)
            outFont.fontThickness = fontThickMap[fontThicknessUI.GetChoice()];
    }

    void FontUIC::SetSensitive(bool isSensitive)
    {
        BaseUIC::SetSensitive(isSensitive);
        fontFamilyChooser.SetSensitive(isSensitive);
        fontWeightChooser.SetSensitive(isSensitive);
        fontSlantChooser.SetSensitive(isSensitive);
        fontSizeChooser.SetSensitive(isSensitive);

        if (useRotation)
            fontRotationUI.SetSensitive(isSensitive);
        else if (is3DFont)
            fontThicknessUI.SetSensitive(isSensitive);

    }

    // ************************************************************* numeric format

    RealFormatUIC::RealFormatUIC(bool allowExt)
        :allowExtended(allowExt)
    {
    };

    void  RealFormatUIC::SetPrecisionSettings()
    {
        tformat.format = NumConvFormat(formatTypeChooser.GetChoice());
        tformat.digitsPrecision = formatPrecChooser.GetChoice();

        bool precOK = (tformat.format != ncf_General) && (tformat.format != ncf_Exponent) && (tformat.format != ncf_Exponent2);
        formatPrecChooser.SetSensitive(precOK);
        if (!precOK)
            return;

        SC_StringArray formats;
        formats.Alloc(RealFormatUIC::maxPrecSettings);

        SC_RealFormat tempFormat = tformat;
        char tempStr[40];
        double rVal = 0.0;
        if (tformat.format == ncf_DecimalSpace)
            rVal = 1000.0;
        for (int i = 0; i < RealFormatUIC::maxPrecSettings; i++)
            {
                tempFormat.digitsPrecision = i;
                tempFormat.RealToString(rVal, tempStr, 40);
                if (tformat.format == ncf_DecimalSpace)
                    {
                        rVal *= 10.0;
                        if (i == 3)
                            rVal = 1000.0;
                    }
                formats += tempStr;
            }
        formatPrecChooser.SetLabels(formats);
        formatPrecChooser.SetCurSel(formatPrecChooser.GetChoice());
    }


    void RealFormatUIC::SetPrecisionSettingsCB(void* currObj)
    {
        static_cast<RealFormatUIC*>(currObj)->SetPrecisionSettings();
    }

    void  RealFormatUIC::InitFramedUI(char             frameTitle[],
                                      Widget           parent,
                                      const ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        DoUIInit();
    }

    void  RealFormatUIC::InitFormUI(Widget           parent,
                                    const ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);
        DoUIInit();

    }

    void  RealFormatUIC::DoUIInit()
    {
        static const char*   typeLabels[]  = {"General", "Decimal", "Scientific", "Scientific 2", "Exponent", "Exponent2", "Delimited"};
        int nChoice = 3;
        if (allowExtended)
            nChoice = 7;
        formatTypeChooser.PreInitSetUp(typeLabels, nChoice);

        static const char*   precLabels[]  = {"0", "1", "2", "3", "4", "5", "6", "7", " 0.00000000E+00 ", "9"};
        formatPrecChooser.PreInitSetUp(precLabels, maxPrecSettings);

        ArgListC args;
        args.TopFormAttach(2);
        args.SetPlaceHolder();
        args.RightFormAttach();

        formatPrecChooser.InitWidget(GetForm(), args);
        formatPrecChooser.AddOtherObjectCallback(SetPrecisionSettingsCB, this);

        args.ResetPlace();
        args.LeftFormAttach();
        formatTypeChooser.InitWidget(GetForm(), args);
        formatTypeChooser.AddOtherObjectCallback(SetPrecisionSettingsCB, this);
    }

    void RealFormatUIC::SetRealFormatSettings(const SC_RealFormat&    objSC_RealFormat)
    {
        tformat = objSC_RealFormat;
        formatTypeChooser.SetChoice(tformat.format);
        formatPrecChooser.SetChoice(tformat.digitsPrecision);

        SetPrecisionSettings();
    }

    void RealFormatUIC::GetRealFormatSettings(SC_RealFormat&    objSC_RealFormat)
    {
        objSC_RealFormat = tformat;
    }

    void RealFormatUIC::SetSensitive(bool isSensitive)
    {
        BaseUIC::SetSensitive(isSensitive);
        formatTypeChooser.SetSensitive(isSensitive);
        formatPrecChooser.SetSensitive(isSensitive &&
                                       (tformat.format != ncf_General) &&
                                       (tformat.format != ncf_Exponent) &&
                                       (tformat.format != ncf_Exponent2));
    }

    // ************************************************************* date format


    void  DateFormatUIC::SetInternalSensitive()
    {
        int dateOrder = dateOrderUI.GetChoice();
        bool usesYear = (dateOrder < 5);
        bool usesDay = (dateOrder < 3) || (dateOrder > 4);
        bool usesdelim2 = (dateOrder < 3);
        dayHasLeadingZeroUI.SetSensitive(usesDay);
        yearIsFullUI.SetSensitive(usesYear);
        delim2UI.SetSensitive(usesdelim2);
    }


    void DateFormatUIC::SetInternalSensitiveCB(void* currObj)
    {
        static_cast<DateFormatUIC*>(currObj)->SetInternalSensitive();
    }

    void  DateFormatUIC::InitFramedUI(char             frameTitle[],
                                      Widget           parent,
                                      const ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);

        ArgListC args;
        args.StdSplitSetup(20);

        static const char*   doStrs[]  = {"DMY", "MDY", "YMD", "MY", "YM", "MD", "DM"};
        dateOrderUI.PreInitSetUp(doStrs, 7);
        dateOrderUI.AttachObjectToLabel();
        dateOrderUI.InitLabeledUI("Order", GetForm(), args);
        dateOrderUI.AddOtherObjectCallback(SetInternalSensitiveCB, this);

        args.StdSplitMiddle(20, 40);
        static const char*   mfStrs[]  = {"01", "1", "Jan", "JAN", "January"};
        monthFormatUI.PreInitSetUp(mfStrs, 5);
        monthFormatUI.InitLabeledUI("Month", GetForm(), args);

        args.StdSplitMiddle(40, 55);
        dayHasLeadingZeroUI.PreInitSetUp("1", "01");
        dayHasLeadingZeroUI.InitLabeledUI("Day", GetForm(), args);

        args.StdSplitMiddle(55, 70);
        yearIsFullUI.PreInitSetUp("01", "2001");
        yearIsFullUI.InitLabeledUI("Year", GetForm(), args);

        args.StdSplitMiddle(70, 85);
        delim1UI.SetFieldLen(5);
        delim1UI.InitLabeledUI("Del1", GetForm(), args);

        args.StdSplitFinish(15);
        delim2UI.SetFieldLen(5);
        delim2UI.InitLabeledUI("Del2", GetForm(), args);
    }


    void DateFormatUIC::SetDateFormat(const DC_DateFormat&  inDF)
    {
        dateOrderUI.SetChoice(inDF.dateOrder);
        monthFormatUI.SetChoice(inDF.monthFormat);
        dayHasLeadingZeroUI.SetBoolChoice(inDF.dayHasLeadingZero);
        yearIsFullUI.SetBoolChoice(inDF.yearIsFull);
        delim1UI.SetFieldString(inDF.delim1);
        delim2UI.SetFieldString(inDF.delim2);

        SetInternalSensitive();
    }

    void DateFormatUIC::GetDateFormat(DC_DateFormat&    outDF)
    {
        outDF.dateOrder = DC_DateFormat::DateOrder(dateOrderUI.GetChoice());
        outDF.monthFormat = DC_DateFormat::MonthFormat(monthFormatUI.GetChoice());
        outDF.dayHasLeadingZero = dayHasLeadingZeroUI.GetBoolChoice();
        outDF.yearIsFull = yearIsFullUI.GetBoolChoice();
        delim1UI.GetFieldString(outDF.delim1, DC_DateFormat::delimStrLen);
        delim2UI.GetFieldString(outDF.delim2, DC_DateFormat::delimStrLen);
    }

    void DateFormatUIC::SetSensitive(bool isSensitive)
    {
        BaseUIC::SetSensitive(isSensitive);
        if (isSensitive)
            SetInternalSensitive();
    }


    TimeFormatUIC::TimeFormatUIC()
    {
        static const char*   tfStrs[]  = {"13:30:45", "1:30:45 PM", "13:30", "1:30 PM", "30:45"};
        PreInitSetUp(tfStrs, 5);
    }

    void TimeFormatUIC::SetTimeFormat(const DC_DateTime::TimeFormat&    timeFormat)
    {
        SetChoice(int(timeFormat));
    }

    DC_DateTime::TimeFormat TimeFormatUIC::GetTimeFormat()
    {
        return DC_DateTime::TimeFormat(GetChoice());
    }


    // ************************************************************* miscellania

    static void ToggleAndPenCB(void* inObject)
    {
        ToggleAndPenUIC* currUIC = (ToggleAndPenUIC*) inObject;
        currUIC->dataPen.SetSensitive(currUIC->showData.GetValue());
    }

    void ToggleAndPenUIC::StdInit(char             togLabel[],
                                  Widget           parent,
                                  const ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);

        ArgListC args;

        args.TopLeftAttach(5, 5);
        showData.StdToggleInit(togLabel, GetForm(), args);
        showData.AddOtherObjectCallback(ToggleAndPenCB, this);

        args.Clear();
        args.TopBaseAttach(showData);
        args.LeftFormAttach(5);
        dataPen.InitWidget(GetForm(), args);
    }

    void ToggleAndPenUIC::SetToggleAndPen(bool show,
                                          int  penNum)
    {
        showData.SetValue(show);
        dataPen.SetPenSelection(penNum);
        dataPen.SetSensitive(show);
    }

    void ToggleAndPenUIC::GetToggleAndPen(bool& show,
                                          int&  penNum)
    {
        show = showData.GetValue();
        penNum = dataPen.GetPenSelection();
    }

    void ToggleAndPenUIC::SetSensitive(bool isSensitive)
    {
        showData.SetSensitive(isSensitive);
        dataPen.SetSensitive(isSensitive && showData.GetValue());
    }

    void PSC_EdgeSpecUIC::StdInit(Widget           parent,
                                  const ArgListC&  inArgs,
                                  bool             is3Dobject)
    {
        polyOffsetSet = is3Dobject;

        BaseUIC::InitFramedUI("Edges", parent, inArgs);

        ArgListC args;

        args.Clear();
        args.TopLeftAttach(5, 10);
        plotEdgesUI.StdToggleInit("Plot", GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.RightPosAttach(40);
        edgePenUI.InitWidget(GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.RightPosAttach(65);
        edgeThkUI.InitWidget(GetForm(), args);

        if (is3Dobject)
            {
                args.Clear();
                args.TopFormAttach();
                args.LeftWidgetAttach(edgeThkUI.GetWidget());
                args.LeftOffset(5);
                args.RightFormAttach();
                edgePolyOffsetUI.SetFieldLen(6);
                edgePolyOffsetUI.InitLabeledUI("PolyOff:", GetForm(), args);
                edgePolyOffsetUI.format = SC_RealFormat(ncf_Decimal, 3);
            }
    }

    void PSC_EdgeSpecUIC::SetEdgeSpec(const PSC_EdgeSpec& inSpec)
    {
        plotEdgesUI.SetValue(inSpec.plotEdges);
        edgePenUI.SetPenSelection(inSpec.edgePen);
        edgeThkUI.SetLineThick(inSpec.edgeThk);

        if (polyOffsetSet)
            edgePolyOffsetUI.SetFieldValue(inSpec.edgePolyOffset);
    }

    void PSC_EdgeSpecUIC::GetEdgeSpec(PSC_EdgeSpec& outSpec)
    {
        outSpec.plotEdges = plotEdgesUI.GetValue();
        if (outSpec.plotEdges)
            {
                outSpec.edgePen = edgePenUI.GetPenSelection();
                outSpec.edgeThk = edgeThkUI.GetLineThick();
                if (polyOffsetSet)
                    outSpec.edgePolyOffset = edgePolyOffsetUI.GetFieldValue();
            }
    }

    bool  PSC_EdgeSpecUIC::UIdataOK(bool& allOK)

    {
        if (polyOffsetSet && plotEdgesUI.GetValue())
            edgePolyOffsetUI.FieldValueOK(allOK, "Edge Poly offset", -1.0E+04, 1.0E+04);

        return allOK;
    }

    void PSC_EdgeSpecUIC::SetInternalSensitive()
    {
        bool edgesOn = plotEdgesUI.GetValue();
        edgePenUI.SetSensitive(edgesOn);
        edgeThkUI.SetSensitive(edgesOn);
        if (polyOffsetSet)
            edgePolyOffsetUI.SetSensitive(edgesOn);
    }


    void PSC_ExtrusionSpecUIC::StdInit(Widget           parent,
                                       const ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI("Extrusion Options", parent, inArgs);

        ArgListC args;

        args.StdToggleSetup(5);
        capEndsUI.StdToggleInit("Cap Ends", GetForm(), args);

        args.StdToggleSetup(50);
        adjustForScalingUI.StdToggleInit("Adjust For Axes Scaling", GetForm(), args);

    }

    void PSC_ExtrusionSpecUIC::SetExtrusionSpec(const PSC_ExtrusionSpec& inSpec)
    {
        capEndsUI.SetValue(inSpec.capEnds);
        adjustForScalingUI.SetValue(inSpec.adjustForScaling);
    }

    void PSC_ExtrusionSpecUIC::GetExtrusionSpec(PSC_ExtrusionSpec& outSpec)
    {
        outSpec.capEnds = capEndsUI.GetValue();
        outSpec.adjustForScaling = adjustForScalingUI.GetValue();
    }

    bool  PSC_ExtrusionSpecUIC::UIdataOK(bool& allOK)
    {
        return allOK;
    }


    void OffsetCoordUIC::StdInit(char             label[],
                                 Widget           parent,
                                 const ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(label, parent, inArgs);

        ArgListC args;

        args.Clear();
        args.TopFormAttach();
        args.LeftFormAttach();
        args.RightPosAttach(33);
        xOffsetUI.SetFieldLen(9);
        xOffsetUI.InitLabeledUI("X:", GetForm(), args);
        xOffsetUI.format = SC_RealFormat(ncf_Decimal, 3);

        args.Clear();
        args.TopFormAttach();
        args.LeftPosAttach(40);
        args.RightPosAttach(65);
        yOffsetUI.SetFieldLen(9);
        yOffsetUI.InitLabeledUI("Y:", GetForm(), args);
        yOffsetUI.format = SC_RealFormat(ncf_Decimal, 3);

        args.Clear();
        args.TopFormAttach();
        args.LeftPosAttach(70);
        args.RightFormAttach();
        zOffsetUI.SetFieldLen(9);
        zOffsetUI.InitLabeledUI("Z:", GetForm(), args);
        zOffsetUI.format = SC_RealFormat(ncf_Decimal, 3);
    }

    void OffsetCoordUIC::SetOffsetCoord(const Coord3D& inCoord)
    {
        xOffsetUI.SetFieldValue(inCoord.cX);
        yOffsetUI.SetFieldValue(inCoord.cY);
        zOffsetUI.SetFieldValue(inCoord.cZ);
    }

    void OffsetCoordUIC::GetOffsetCoord(Coord3D& outCoord)
    {
        outCoord.cX = xOffsetUI.GetFieldValue();
        outCoord.cY = yOffsetUI.GetFieldValue();
        outCoord.cZ = zOffsetUI.GetFieldValue();
    }

    bool OffsetCoordUIC::UIdataOK(bool& allOK)
    {
        xOffsetUI.FieldValueOK(allOK, "x offset");
        yOffsetUI.FieldValueOK(allOK, "y offset");
        zOffsetUI.FieldValueOK(allOK, "z offset");
        return allOK;
    }


    void PSC_LegendOverrideUIC::InitFramedUI(char              frameTitle[],
                                             Widget            parent,
                                             const  ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);

        ArgListC args;

        args.StdToggleSetup(2);
        overrideDefaultLabelUI.StdToggleInit("New label", GetForm(), args);

        args.StdSplitFinish(80);
        overrideLabelUI.SetFieldLen(PSC_LegendOverride::labelLen);
        overrideLabelUI.InitWidget(GetForm(), args);
    }

    void PSC_LegendOverrideUIC::InitFormUI(Widget            parent,
                                           const  ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);

        ArgListC args;
        args.StdToggleSetup(2);
        overrideDefaultLabelUI.StdToggleInit("New Label", GetForm(), args);

        args.StdSplitFinish(80);
        overrideLabelUI.SetFieldLen(PSC_LegendOverride::labelLen);
        overrideLabelUI.InitWidget(GetForm(), args);
    }


    void PSC_LegendOverrideUIC::SetLegendOverride(const PSC_LegendOverride& inSpec)
    {
        overrideDefaultLabelUI.SetValue(inSpec.overrideDefaultLabel);
        overrideLabelUI.SetFieldString(inSpec.overrideLabel);
    }

    void PSC_LegendOverrideUIC::GetLegendOverride(PSC_LegendOverride& outSpec)
    {
        outSpec.overrideDefaultLabel = overrideDefaultLabelUI.GetValue();
        overrideLabelUI.GetFieldString(outSpec.overrideLabel, PSC_LegendOverride::labelLen);
    }

    bool  PSC_LegendOverrideUIC::UIdataOK(bool& allOK)
    {
        if (overrideDefaultLabelUI.GetValue())
            {
                char tempStr[PSC_LegendOverride::labelLen];
                overrideLabelUI.FieldStringOK(allOK, "New label", tempStr, PSC_LegendOverride::labelLen);
            }
        return allOK;
    }

    void PSC_LegendOverrideUIC::SetSensitive(bool inSens)
    {
        BaseUIC::SetSensitive(inSens);
        overrideDefaultLabelUI.SetSensitive(inSens);
        overrideLabelUI.SetSensitive(inSens && overrideDefaultLabelUI.GetValue());
    }


    void PSC_GridLineLabelFormatUIC::InitFramedUI(char             frameTitle[],
                                                  Widget           parent,
                                                  const ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);

        ArgListC args;

        args.StdSplitSetup(15);
        args.LeftOffset(5);
        const char* labs[] = {"none", "text", "value"};
        labelTypeUI.PreInitSetUp(labs, 3);
        labelTypeUI.InitWidget(GetForm(), args);

        args.StdSplitMiddle(15, 35);
        labelPositionUI.SetFieldLen(5);
        labelPositionUI.format = SC_RealFormat(ncf_Decimal, 3);
        labelPositionUI.AttachLabelToObject();
        labelPositionUI.InitLabeledUI("Pos", GetForm(), args);

        args.StdSplitMiddle(35, 52);
        halignUI.AttachLabelToObject();
        halignUI.InitLabeledUI("H", GetForm(), args);

        args.StdSplitMiddle(52, 70);
        valignUI.AttachLabelToObject();
        valignUI.InitLabeledUI("V", GetForm(), args);

        args.StdSplitMiddle(72, 88);
        labelOffsetUI.SetFieldLen(3);
        labelOffsetUI.AttachLabelToObject();
        labelOffsetUI.InitLabeledUI("Off", GetForm(), args);

        args.StdSplitFinish(10);
        blankLabelUI.StdToggleInit("Blank", GetForm(), args);
    }

    void PSC_GridLineLabelFormatUIC::SetSensitive(bool inSens)
    {
        BaseUIC::SetSensitive(inSens);
        bool doLab = (labelTypeUI.GetChoice() > 0) && inSens;
        labelPositionUI.SetSensitive(doLab);
        labelOffsetUI.SetSensitive(doLab);
        halignUI.SetSensitive(doLab);
        valignUI.SetSensitive(doLab);
        blankLabelUI.SetSensitive(doLab);
    }


    void PSC_GridLineLabelFormatUIC::SetGridLineLabelFormat(const PSC_GridLineLabelFormat& inFormat)
    {
        labelTypeUI.SetChoice(inFormat.labelType);
        labelPositionUI.SetFieldValue(inFormat.labelPosition);
        halignUI.SetHAlign(inFormat.halign);
        valignUI.SetVAlign(inFormat.valign);
        labelOffsetUI.SetFieldValue(inFormat.labelOffset);
        blankLabelUI.SetValue(inFormat.blankLabel);
    }

    void PSC_GridLineLabelFormatUIC::GetGridLineLabelFormat(PSC_GridLineLabelFormat& outFormat)
    {
        outFormat.labelType = PSC_GridLineLabelFormat::LabelType(labelTypeUI.GetChoice());

        if (outFormat.labelType != PSC_GridLineLabelFormat::ltNone)
            {
                outFormat.labelPosition = labelPositionUI.GetFieldValue();
                outFormat.labelOffset = labelOffsetUI.GetFieldValue();
                outFormat.halign = halignUI.GetHAlign();
                outFormat.valign = valignUI.GetVAlign();
                outFormat.blankLabel = blankLabelUI.GetValue();
            }

    }

    bool PSC_GridLineLabelFormatUIC::UIdataOK(bool& allOK)
    {
        if (labelTypeUI.GetChoice() > 0)
            {
                labelPositionUI.FieldValueOK(allOK, "Position", 0.0, 100.0);
                labelOffsetUI.FieldValueOK(allOK, "Offset", 0, 100);
            }
        return allOK;
    }

};

