///////////////////////////////////////////////////////////////////////////////////
//
// PS_UIBase.h
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
// DESCRIPTION: Platform Specific but application independent code.
//              Individual UI elements.  MFC implementation of original Unix X
//              wrappers. Defines BaseWidget and derived classes for all common
//              UI controls. All layout automatically managed by X Form like attachments
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PS_UIBASE_H
#define PS_UIBASE_H

#include <genClass/C_Common.h>

#include <genClass/SC_StringArray.h>
#include <genClass/SC_IntArray.h>
#include <genClass/SC_ColorSpec.h>
#include <genClass/SC_RealFormat.h>
#include <genClassPS/PS_ArgList.h>

#include <genUIExt/TabbedComboBox.h>
#include <genUIExt/TabbedListBox.h>

// namespace for all genClass stuff
namespace genClassPS {

    //  forward reference for window creation
    class LayoutManager;

    // helper class for assignment of unique child window IDs inside CWnd class
    class OrdinalDispatcher {
    public:
        OrdinalDispatcher();

        int         GetNextID();
        void    SetInterval(int nSize);
    protected:
        int         m_nID;
    };
    inline OrdinalDispatcher::OrdinalDispatcher() :
        m_nID(0)
    {
    }
    inline int OrdinalDispatcher::GetNextID()
    {
        return m_nID++;
    }
    inline void     OrdinalDispatcher::SetInterval(int nSize)
    {
        m_nID += nSize;
    }

    class WindowAttachment {
    public:
        // enum describing basic attachments

        enum    AttachType      {atNone,        // no attachment -- edge floats as required by size
                                 atParent,      // directly to same edge of parent
                                 atPosition,    // at percentage (0 - 100) position of parent (only used for horizontal)
                                 atChild};      // attached to opposite edge of other child
    public:
        AttachType      attachType;
        int             attachPos;         // position for atPosition
        BaseWindow*     attachChild;       // attached window      for atChild

        int             offset;            // distance in pixels from attachment position to window start edge

        WindowAttachment() {attachType = atNone; attachPos =  0; attachChild = 0; offset = 0;}
    };

    //  all adjustable UI controls are descended from this
    class BaseWindow {
        friend class LayoutManager;

    public:

        //  attachment to parent
        WindowAttachment    topAttach;
        WindowAttachment    bottomAttach;
        WindowAttachment    leftAttach;
        WindowAttachment    rightAttach;

        bool                bInitialized;
        CRect               rect;
        int                 calcHeight;
        int                 calcWidth;
        int                 ordinal;
        LayoutManager*      layoutManager;
        int                 windowWidth;
        int                 windowHeight;
        CFont               menuFont;
        CFont               menuFont2;

        static int          fontWidth;
        static int          fontWidthNumber;
        static int          fontHeight;
        static LOGFONT      menuLogFont;
        // smalll font support
        static int          fontWidth2;
        static int          fontWidthNumber2;
        static int          fontHeight2;
        static LOGFONT      menuLogFont2;

        static bool         bGlobalInit;

        COLORREF            m_clrBackground;

    protected:
        bool                m_bSmall;
        bool                m_bWidthCalculated;
        int                 m_nCalculatedWidth;
        int                 m_nCalculatedBorder;

    public:
        BaseWindow();
        virtual         ~BaseWindow();


        void InitGlobal();
        //  called to create an instance of the control - two signatures
        //  most common is following which assumes that all controls are children of layout managers
        virtual void    Create(const CRect& rect) = 0;
        virtual CWnd*   GetParentWnd();

        virtual     OrdinalDispatcher* GetOrdinalDispatcher();
        int             GetID();

        //  basic access and state info
        virtual CWnd*   GetWindow() = 0;    // checks for validity, if OK  returns reference, if not OK aborts
        bool            IsCreated();        // returns true if windows is created
        bool            IsInitialized();    // true if window is initialzed (InitWidget)

        void            InitWidget(BaseWindow& form, const ArgListC& inArgs);
        void            InitWidget(BaseWindow* form, const ArgListC& inArgs);
        BaseWindow*     GetWidget() {return this;}
        void            Remove();           // Removes widget from layout manager

        //  base class methods -- map to std Motif routines
        virtual void    SetSensitive(bool isSensitive);     // sets sensitive to input
        virtual void    Manage();                           // makes window visible and active
        virtual void    Unmanage();                         // makes window invisible/non responsive
        bool            IsManaged();                        // gets visibility status
        void            SetManaged(bool manageFlag);        // Manage() if flag is true, Unmanage() if false

        //  color management -- I've left it in for now -- it probably is not needed as very few
        //  controls will need to modify colors

        // background color
        void            SetWidgetColor(COLORREF             windowColor);
        void            SetWidgetColor(SC_ColorSpec&        windowColor);
        void            SetWidgetColor(StdColors            windowColor);

        // foreground color
        void            SetForegroundColor(COLORREF         foregroundColor);
        void            SetForegroundColor(SC_ColorSpec&    foregroundColor);
        void            SetForegroundColor(StdColors        foregroundColor);

        //  the following are to provide support to the layout managers
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();

        virtual void    SetWindowPosition(const CRect& rect, int level = 0);    // sets control window position relative to parent
        virtual int     SetWindowPosX(int x1, int x2);
        virtual int     SetWindowPosY(int y1, int y2);

        //  cleanup
        virtual void    Delete(); // delete object from memory and destroy window if exist

        int             GetWidthOffset();
        int             GetHeightOffset();
        CRect           AdjustOffsets(const CRect& rect);

        virtual void    DoSetSensitive(bool isSensitive);
        virtual void    DoManage();
        virtual void    DoUnmanage();

        virtual void    SetSmall(bool bSmall = true);
        bool            IsSmall();

    protected:
        static void     CalculateFontSizes(CDC& dc, const LOGFONT& logFont, int& fontWidth, int& fontWidthNumber);
        CFont*          GetMenuFont();
        int             GetFontHeight();
        int             GetFontWidth();
        int             GetFontWidthNumber();

    private:
        // no copy
        BaseWindow(const BaseWindow& a) {};
        BaseWindow&         operator=(const BaseWindow& a) {return *this;}

    };
    typedef BaseWindow* Widget;

    //  basic array of *BaseWindow used by LayoutManager classes
    class SC_BaseWindowArray: public T_SC_Array<BaseWindow*> {
        //  may add some specialization functions later
    };


    class LayoutManager : public  BaseWindow {
        friend class FrameManager;
    protected:
        SC_BaseWindowArray      children;

    public:
        LayoutManager();
        virtual         ~LayoutManager();


        //  called to create an instance of the control - two signatures
        //  most common is following which assumes that all controls are children of layout managers
        virtual  void   Create(const CRect& rect);

        virtual CWnd*   GetWindow();

        LayoutManager*  GetWidget() { return this; }

        //  processes children
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     SetWindowPosX(int x1, int x2);
        virtual int     SetWindowPosY(int y1, int y2);
        virtual void    Delete();    // delete object from memory
        virtual void    SetSensitive(bool isSensitive);     // sets sensitive to input
        virtual void    Manage();
        virtual void    Unmanage();

        //  adds a child to be managed
        void            AddChild(BaseWindow& child);
        void            RemoveChild(BaseWindow* child);
        void            AdjustChildren(BaseWindow* child, int rightPosOffset);

        virtual void    Resize(int x1, int x2, int y1, int y2);

        virtual void    SetSmall(bool bSmall = true);

    protected:
        virtual void    DoSetSensitive(bool isSensitive);
        virtual void    DoManage();
        virtual void    DoUnmanage();
    };
    typedef LayoutManager FormWidget;

    // labeled group control with managed interior.  Interior management from Form
    class FrameManager : public LayoutManager {
    private:
        enum            {maxFrameLabelLen = 80};
        char            frameLabel[maxFrameLabelLen];
        CButton         groupBox;

    public:
        FrameManager();
        FrameManager(const char* frameLabel);  // when label known at construction
        virtual         ~FrameManager();

        void            SetFrameText(const char* frameLabel);  // to modify label after creation

        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     SetWindowPosX(int x1, int x2);
        virtual int     SetWindowPosY(int y1, int y2);
        virtual void    Delete();
    };
    typedef FrameManager FrameWidget;

    class CFormWnd : public CWnd
    {
    protected:
        virtual BOOL PreTranslateMessage( MSG* pMsg );

    protected:
        //{{AFX_MSG(CFormWnd)
        afx_msg void OnSetFocus(CWnd* pOldWnd);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // scrolling window
    class ScrolledArea : public CFormWnd {
    public:
        bool            m_bUseScrollSkills;

    protected:
        CSize           m_szForm;
        CSize           m_szFull;
        CSize           m_szPage;
        CSize           m_szLine;
        bool            m_bInsideUpdate;

    public:
        ScrolledArea(bool bUseScrollSkills = true);
    protected:

        bool            OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll = true);
        bool            OnScrollBy(CSize sizeScroll, bool bDoScroll = true);
        BOOL            DoMouseWheel(UINT fFlags, short zDelta, CPoint point);
        void            UpdateBars();
        CSize           GetScrollBarSizes();
        void            ScrollToPosition(CPoint& pt);

        // necessery to implement in descendants

        virtual void    SetupScrolledArea() = 0;
        virtual CRect&  GetAreaRect() = 0;

        virtual BOOL PreTranslateMessage( MSG* pMsg );
    protected:
        //{{AFX_MSG(ScrolledArea)
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };


    LPCTSTR RegisterFormClass();

    class ScrolledForm : public ScrolledArea, public LayoutManager {
    protected:
        CFormWnd    m_wndForm;

    public:
        ScrolledForm(bool bUseScrollSkills = true);

        void            Destroy();

        virtual void    Manage();
        virtual void    Unmanage();

    protected:
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        virtual CWnd*   GetParentWnd();

    protected:
        // rewrite for setting up ScrolledForm width/height
        virtual void    SetupScrolledArea();

        virtual CRect&  GetAreaRect();
        void            CalculateSizes();

        virtual CSize   SetCalculateFormSize();

        //{{AFX_MSG(ScrolledForm)
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnSetFocus(CWnd* pOldWnd);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    class FormManager : public ScrolledForm, public OrdinalDispatcher {
    public:
        FormManager();
        virtual         ~FormManager();

        void            Create(CWnd* pWnd, DWORD dwStyleAdd = 0);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);

        virtual void    Resize(int x1, int x2, int y1, int y2);
    protected:
        virtual CWnd*   GetParentWnd();
        virtual CWnd*   GetWindow();
        virtual OrdinalDispatcher* GetOrdinalDispatcher();

    protected:
        // rewrite for setting up ScrolledForm width/height
        virtual CSize   SetCalculateFormSize();

        //{{AFX_MSG(FormManager)
        afx_msg LRESULT OnHelpHitTest(WPARAM, LPARAM);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };


    // ************************************************************* interactive widgets

    class InteractiveWidget;

    //  callback types
    typedef void (*OtherObjectCB)(void *);                              // most common
    typedef void (*CombinedObjectCB)(void *, InteractiveWidget *);      // used in slider/val buddy

    class InteractiveWidget : public BaseWindow {
    private:
        enum             {maxCallbacks = 5};
        OtherObjectCB    otherObjectCB[maxCallbacks];
        void*            otherObjectData[maxCallbacks];
        CombinedObjectCB combinedObjectCB[maxCallbacks];
        void*            combinedObjectData[maxCallbacks];

    protected:

        bool        useDefaultHeight;  // initialized true


    public:
        // basic constructor/destructor
        InteractiveWidget();
        virtual         ~InteractiveWidget();

        virtual void    Delete();

        //  callback setup
        void            AddOtherObjectCallback(OtherObjectCB    otherObjectCB,
                                               void*            otherObj);
        //  adds  function as callback
        //  will be called with otherObj as parameter

        void            AddCombinedObjectCallback(CombinedObjectCB  combinedObjectCB,
                                                  void*           otherObj);
        //  adds  function as callback
        //  will be called with otherObj and this as parameter


        void            Callback();

        void            DontUseDefaultHeight() {useDefaultHeight = false;}

        static void     InputFieldError(const char   fieldDesc[],
                                        const char   errorMsg[]);
        static void     InputError(const char   errorMsg[]);

    };

    //******************************************************************** text entry

    class TextEntryWidget : public CEdit, public InteractiveWidget {
    public:
        int             textFieldLen;

    public:
        TextEntryWidget();
        virtual         ~TextEntryWidget();

        //              default window size is based on textFieldLen
        void            SetFieldLen(int fieldLen) {textFieldLen = fieldLen;}
        void            SetWindowLen(int nChars);   // when windows must be specific size - based on M

        // must be called before Init ... routines

        void            SetFieldString(const char inStr[]);

        void            GetFieldString(char outStr[],
                                       int  maxLen);

        bool            FieldStringOK(bool&      prevWasOK,
                                      const char fieldDesc[],
                                      char       outStr[],
                                      int        maxLen);

        bool            FieldStringOK(bool&      prevWasOK,
                                      const char fieldDesc[]);

    protected:
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);

        virtual BOOL PreTranslateMessage( MSG* pMsg );

    protected:
        //{{AFX_MSG(TextEntryWidget)
        afx_msg void OnKillfocus();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    //  enters real values
    class RealEntryWidget : public TextEntryWidget {
    public:
        double          tval;
        SC_RealFormat   format;

    public:
        RealEntryWidget();
        virtual         ~RealEntryWidget();

        void            SetFieldFormat(const SC_RealFormat& inForm) {format = inForm;}

        void            SetFieldValue(const double& inVal);

        bool            FieldValueOK( bool&      prevWasOK,
                                      const char fieldDesc[],
                                      const double& minVal,
                                      const double& maxVal,
                                      bool    showMsgs = true);

        bool            FieldValueOK( bool&      prevWasOK,
                                      const char fieldDesc[],
                                      bool    showMsgs = true);

        double          GetFieldValue() {return tval;}

    protected:
        virtual void    Create(const CRect& rect);
    };

    class IntEntryWidget : public TextEntryWidget {
    public:
        int         tval;

    public:
        IntEntryWidget();
        virtual     ~IntEntryWidget();

        void        SetFieldValue(int inVal);

        bool        FieldValueOK( bool&      prevWasOK,
                                  const char fieldDesc[],
                                  int        minVal,
                                  int        maxVal,
                                  bool          showMsgs = true);


        int         GetFieldValue() {return tval;}

    protected:
        virtual void    Create(const CRect& rect);
    };


    // ************************************************************* buttons

    class ButtonWidget: public InteractiveWidget {
    public:
        static void     FixedSizeButtonArgs(ArgListC&  inArgs);  // adds XmNrecomputeSize, false arg to list
    };

    class DrawnButton: public CButton, public InteractiveWidget {
    public:
        DrawnButton();

        // sets shadow resources for in/out appearances
        void            ButtonIn();
        void            SelectButton(bool bSelect = true);
        virtual CWnd*   GetWindow();

        virtual void    Create(const CRect& rect);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
    protected:
        bool            m_bSelected;

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(DrawnButton)
    public:
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
        //}}AFX_VIRTUAL
    protected:
        //{{AFX_MSG(PushButton)
        afx_msg void OnClick();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    };

    class PushButton: public CButton, public InteractiveWidget {
    protected:
        char*           label;
    public:
        PushButton();

        void            StdButtonInit(char   buttonLabel[],
                                      LayoutManager* parent,
                                      const ArgListC&  inArgs);
        void            SetButtonText(char newLab[]);
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);

    protected:
        //{{AFX_MSG(PushButton)
        afx_msg void OnClick();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // ************************************************************* file name


    class FileEntryWidget;

    class FileEntryPushButton : public CButton {
    public:
        FileEntryPushButton();

        FileEntryWidget*    assocFileEntry;
    protected:
        //{{AFX_MSG(FileEntryPushButton)
        afx_msg void OnClick();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    class FileEntryWidget : public TextEntryWidget {
    private:
        int                 buttonRightOffset;   // kluge for attaching button during init
        int                 buttonBottomOffset;  // must be set before init widget call

        char                browsePrompt[40];
        char                browseFileMask[80];   // modified for extended browse mask
        bool                folderEntry;

    public:
        char                tfileStr[stdFileStrLen];
        FileEntryPushButton browseButton;

    public:
        FileEntryWidget();

        virtual void    Create(const CRect& rect);
        virtual void    Delete();
        virtual void    SetWindowPosition(const CRect& rect, int level);
        virtual void    SetSensitive(bool isSensitive);

        void        SetButtonOffsets(int rOff, int bOff) {buttonRightOffset = rOff;
            buttonBottomOffset = bOff;}

        void        SetPromptAndMask(char newPrompt[],
                                     char newMask[]);

        void        SetFileType(bool fileIsFolder) {folderEntry = fileIsFolder;}

        bool        FieldFileOK(bool&      prevWasOK,
                                const char fieldDesc[],
                                bool       mustExist,
                                bool       chkOverwrite);

        bool        FieldPathOK(bool&      prevWasOK,
                                const char fieldDesc[],
                                bool       mustExist = true);

        void        GetFieldFile(char outFileStr[]);

        void        BrowseForFiles();

        void        CloseDialog();  // closes browse dialog box if open, empty in Win

    protected:
        void GetRects(const CRect& rect, CRect& rc1, CRect& rc2);

        virtual int GetWindowHeight();
    };

    // ************************************************************* basic selection

    class ChooserWidget : public InteractiveWidget {
    protected:
        SC_ConstStringArray choiceLabels;
        int                 nchoice;        // number of selections
    public:
        int                 currChoice;

    public:
        ChooserWidget();
        virtual             ~ChooserWidget();

        //  sets up private vars
        // must be called before Init ... routines
        void            PreInitSetUp(const char**   inLabels,
                                     int            nlabels);

        void            PreInitSetUp(const char**   inLabels);              // last must be NULL

        void            PreInitSetUp(const SC_ConstStringArray& inLabels);  // last must be NULL

        // extreme caution: inLabs must remain in scope & unchanged for life of control !!
        void            PreInitSetUp(const SC_StringArray& inLabs);

        virtual bool    SetChoice(int inChoice) = 0;
        int             GetChoice() {return currChoice;}
    };


    class OrientedChooserWidget : public ChooserWidget {
    protected:
        bool            horizontal;     // default true
        DWORD           m_dwStyleItem;

        int             m_nIDStart;
    protected:
        bool            IsCreated();
        int             GetItemID(int i);
        void            SetupButtons(bool bCreate, const CRect& rect, CWnd* pParent);
        void            CreateButton(int i, const CRect& rect, CWnd* pParent, int nID);
        void            MoveButton(int i, const CRect& rect, CWnd* pParent, int nID);

        virtual void    DoSetSensitive(bool isSensitive);
        virtual void    DoManage();
        virtual void    DoUnmanage();

    public:
        OrientedChooserWidget(DWORD dwStyle);
        ~OrientedChooserWidget();

        //  sets up private vars
        // must be called before Init ... routines
        void            PreInitSetUp(const char**   inLabels,
                                     int      nlabels,
                                     bool     isHoriz = true);

        void            PreInitSetUp(const SC_StringArray& inLabels,
                                     bool     isHoriz = true);


        bool            SetChoice(int inChoice);
        int             GetChoice() {return currChoice;}

        virtual void    Create(const CRect& rect);
        virtual void    Delete();
        virtual void    SetWindowPosition(const CRect& rect, int level);
        virtual CWnd*   GetWindow();

        // hmmm -- had to add this JDA
        virtual void    SetSensitive(bool isSensitive) {DoSetSensitive(isSensitive);}

        void            PreviousValue(int nButtonID);
        void            NextValue(int nButtonID);
    private:
        // no default
        OrientedChooserWidget() {};
    };

    class RadioBoxChooserWidget : public OrientedChooserWidget {
    public:
        RadioBoxChooserWidget();

        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
    };


    class BoolRadioBoxChooserWidget : public RadioBoxChooserWidget {
    public:
        //  sets up private vars
        // must be called before Init ... routines
        void            PreInitSetUp(const char   falseDesc[],
                                     const char   trueDesc[],
                                     bool   isHoriz = true);

        void            SetBoolChoice(bool inBool) {SetChoice(int(inBool));}
        bool            GetBoolChoice() {return (currChoice == 1);}

    };

    class ButtonBoxChooserWidget : public OrientedChooserWidget {
    public:
        ButtonBoxChooserWidget();
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
    };


    // ************************************************************* option menus

    class OptionChooserWidget : public CComboBox, public ChooserWidget {
    public:
        OptionChooserWidget();

        void            SetLabels(SC_StringArray& inLabs);

        virtual bool    SetChoice(int inChoice);
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();


    protected:
        //{{AFX_MSG(OptionChooserWidget)
        //}}AFX_MSG
        afx_msg void OnSelchange();
        DECLARE_MESSAGE_MAP()
    };

    class BoolOptionChooserWidget : public OptionChooserWidget {
    public:
        BoolOptionChooserWidget() {}
        //  sets up private vars
        // must be called before Init ... routines
        void            PreInitSetUp(const char   falseDesc[],
                                     const char   trueDesc[]);

        void            SetBoolChoice(bool inBool) {SetChoice(int(inBool));}

        bool            GetBoolChoice() {return (currChoice == 1);}

    };

    // ************************************************************* toggle boxes

    class ToggleBoxWidget: public CButton, public  InteractiveWidget {
    private:
        char        toggleLabel[80];    // must be set before Init

    public:
        bool        currValue;          // public for CBs

    public:
        ToggleBoxWidget();

        void            SetToggleText(const char togLabel[]);
        void            PreInitSetUp(const char  togLabel[]) { SetToggleText(togLabel);}

        void            StdToggleInit(const char   togLabel[],           // combines Pre and Init
                                      LayoutManager* parent,
                                      const  ArgListC&  inArgs);

        void            SetValue(bool inFlag);
        bool            GetValue() {return currValue;}


        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();

    protected:
        //{{AFX_MSG(ToggleBoxWidget)
        afx_msg void OnClick();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // ************************************************************* sliders

    class SliderWidget: public CSliderCtrl, public InteractiveWidget {
    public:
        int             minVal;     // default 0
        int             maxVal;     // default 1000
        int             pageSize;   // default 0
        // if 0 calculated on create as max - min /10

        double          realMin;    // default 0
        double          realMax;    // default 1.0

        bool            showVal;    // default true
        bool            horizontal; // default true

        bool            m_bThumbtrack; // state variable for callback

    public:
        int             currValue;     // public for CBs
        bool            IsTracking()    { return m_bThumbtrack; } // just for CBs

    public:
        SliderWidget();

        void            SetValue(int newVal);
        int             GetValue() {return currValue;}

        void            SetRealValue(double newVal);
        double          GetRealValue();

        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level);
        virtual CWnd*   GetWindow();
        virtual int     GetWindowHeight();

    protected:
        //{{AFX_MSG(SliderWidget)
        afx_msg void HScroll(UINT nSBCode, UINT nPos);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // ************************************************************* lists

    //  base class for List widgets and combo box widgets
    class ListBaseWidget : public InteractiveWidget {
    public:
        int                 listWidth;
        SC_StringArray  listData;

    public:
        ListBaseWidget() {listWidth = 0;}

        virtual void  SetListText(const SC_StringArray& listData) = 0;
        void            SetListNA();

    };

    //  combo box widget -- single select only
    class ComboBoxSelectionWidget : public CTabbedComboBox, public ListBaseWidget {
    public:
        int             currSelection;  // ***** starts at 0 - not at 1

    public:
        ComboBoxSelectionWidget();

        void            ClearList();
        virtual void    SetListText(const SC_StringArray& listData);

        void            SetListWidth(int newWidth);
        void            SetListWidth(int           minWidth,
                                     SC_StringArray& listData);

        void            SetSelection(int inSel);  // **** inSel is 0 based
        int             GetSelection() { return currSelection; }

        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(ComboBoxSelectionWidget)
    public:
        virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
        //}}AFX_VIRTUAL
    protected:
        //{{AFX_MSG(ComboBoxSelectionWidget)
        afx_msg void OnSelchange();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    //  scrolled lists
    class ScrolledListWidget : public CTabbedListBox, public ListBaseWidget {
    public:
        int             currSingleSelection;  // ***** starts at 0 - not at 1
        SC_IntArray     currMultipleSelection;
        bool            isSingleSel;

    public:
        ScrolledListWidget();

        void            ClearList();
        virtual void    SetListText(const SC_StringArray& listData);

        void            SetSelectionPolicy(bool singleSel);

        void            SetSingleSelection(int inSel);                   // **** inSel is 0 based
        void            SetMultipleSelection(const SC_IntArray& inSel);  // **** inSel is 0 based
        int             GetSingleSelection(){return currSingleSelection;}
        void            GetMultipleSelection(SC_IntArray& inSel);        // **** inSel is 0 based
        void            ClearSelections();
        void            SelectAll(); // mult only
        void            ClearAll();  // mult only

        void            SetSelectionVisible();  // for single sel; makes sure is in displayed area

        void            SetSelections();  // for use in CB

        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
    private:
        void            UpdateSelections();

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CColorTextComboBox)
    public:
        virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
        //}}AFX_VIRTUAL
    protected:

        //{{AFX_MSG(ScrolledListWidget)
        //}}AFX_MSG
        afx_msg void OnSelchange();
        DECLARE_MESSAGE_MAP()
    };

    // ************************************************************* drawing area

    class DrawingAreaWidget : public CWnd, public BaseWindow {
    public:
        DrawingAreaWidget();

    protected:
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();

        virtual void OnDraw(CDC* pDC) = 0;

    protected:
        //{{AFX_MSG(DrawingAreaWidget)
        afx_msg void OnPaint();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };


    // ************************************************************* scrolled text

    class ScrolledTextWidget : public CRichEditCtrl, public InteractiveWidget
    {
    protected:
        int             m_nRows;
        int             m_nColumns;
    public:
        ScrolledTextWidget();
        virtual         ~ScrolledTextWidget();

        void        SetRowCol(int nRows, int nCols);
        void        SetText(const SC_StringArray& inStrs);
        void        GetText(SC_StringArray& outStrs);

    protected:
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);

        virtual BOOL PreTranslateMessage( MSG* pMsg );

    protected:

        //{{AFX_MSG(ScrolledTextWidget)
        afx_msg UINT OnGetDlgCode();
        afx_msg void OnKillfocus();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    };


    // ************************************************************* miscellaneous non-interactive

    // displays non-editable text & values

    class StatusWidget : public CEdit, public InteractiveWidget {
    public:
        int             fieldLen;

    public:
        StatusWidget();
        StatusWidget(int fLen) {fieldLen = fLen;}
        virtual         ~StatusWidget();

        void            SetFieldLen(int fLen) {fieldLen = fLen;}
        // must be called before Init ... routines

        void            SetFieldString(const char inStr[]);

        // case for bool ops
        void            SetBoolValue(bool statusVal,
                                     const char trueText[],
                                     const char falseText[]);
        void            SetBoolValue(bool statusVal); // yes/no

        void            ClearFieldString();
        void            SetStatusNA() {SetFieldString("n/a");}

    protected:
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
    protected:
        //{{AFX_MSG(CEdit)
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };


    class RealStatusWidget : public StatusWidget {
    public:
        double          tval;
        SC_RealFormat   format;

    public:
        RealStatusWidget() {};
        RealStatusWidget(int fLen) {fieldLen = fLen;}

        void        SetFieldFormat(const SC_RealFormat& inForm) {format = inForm;}

        void        SetFieldValue(double inVal);

    protected:
        virtual int GetWindowWidth();
    };

    class IntStatusWidget : public StatusWidget {
    public:
        int                 tval;

    public:
        IntStatusWidget() {};
        IntStatusWidget(int fLen) {fieldLen = fLen;}

        void        SetFieldValue(int inVal);

    protected:
        virtual int GetWindowWidth();
    };

    // basic text string
    class LabelWidget : public CStatic, public BaseWindow {
    private:
        const char* labelText;

    public:
        LabelWidget();

        void            SetLabelText(const char newLab[]);
        void            StdLabelInit(const char   labelText[],
                                     LayoutManager*     parent,
                                     const  ArgListC&  inArgs);

        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
    };

    class HorizontalSeparator : public CStatic, public InteractiveWidget {
    public:
        HorizontalSeparator();
        ~HorizontalSeparator();

    protected:
        virtual CWnd*   GetWindow();
        virtual void    Create(const CRect& rect);
        virtual int     GetWindowWidth();
        virtual int     GetWindowHeight();
        virtual void    SetWindowPosition(const CRect& rect, int level = 0);

        // Generated message map functions
    protected:
        //{{AFX_MSG(HorizontalSeparator)
        afx_msg void OnPaint();
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()
    };


} // end namespace

using namespace genClassPS;


#endif // PS_UIBASE_H

