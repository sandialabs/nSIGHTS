///////////////////////////////////////////////////////////////////////////////////
//
// PS_UIBase.cpp
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

#include "GenLibAfx.h"

#include <AfxPriv.h>

#include <genClass/SC_ColorSpec.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>
#include <genClass/U_File.h>

#include <genUIext/FileDialog.h>

#include <genClassPS/PS_Color.h>
#include <genClassPS/PS_UIBase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#include <typeinfo.h>
#endif

namespace genClassPS {

    bool BaseWindow::bGlobalInit = false;

    // normal font metrics
    int BaseWindow::fontWidth = 9;
    int BaseWindow::fontWidthNumber = 7;
    int BaseWindow::fontHeight = 8+2+2;
    LOGFONT BaseWindow::menuLogFont;

    // tiny font metrics
    int BaseWindow::fontWidth2 = 9;
    int BaseWindow::fontWidthNumber2 = 7;
    int BaseWindow::fontHeight2 = 6+2+2;
    LOGFONT BaseWindow::menuLogFont2;

    /////////////////////////////////
    // BaseWindow
    BaseWindow::BaseWindow() :
        windowWidth(-1),
        windowHeight(-1),
        bInitialized(false),
        layoutManager(NULL),
        ordinal(0),
        m_bWidthCalculated(false),
        m_nCalculatedWidth(0),
        m_nCalculatedBorder(0),
        m_bSmall(false)
    {
        if (!bGlobalInit) {
            InitGlobal();
        }
        VERIFY(menuFont.CreateFontIndirect(&menuLogFont));
        VERIFY(menuFont2.CreateFontIndirect(&menuLogFont2));
    }

    void BaseWindow::InitGlobal()
    {
        // get basic font
        CFont font;
        font.CreateStockObject(DEFAULT_GUI_FONT);
        if (font.GetSafeHandle() == NULL) {
            font.CreateStockObject(SYSTEM_FONT);
        }

        CClientDC dc(CWnd::GetDesktopWindow());

        // Normal font
        {
            LOGFONT& lf = BaseWindow::menuLogFont;
            // Retrieve logical font information
            if (font.GetSafeHandle() != NULL) {
                font.GetObject(sizeof(lf), &lf);
            } else {
                // set "Arial" as default font
                memset(&lf, 0, sizeof(LOGFONT));
                lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
                lf.lfQuality = DEFAULT_QUALITY;
                lf.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS;
                CopyString(lf.lfFaceName, "Arial", 32);
            }

            // set attributes
            lf.lfItalic         = 0;
            lf.lfUnderline  = 0;
            int nLogPixelY = ::GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY);
            lf.lfHeight = -MulDiv(8, nLogPixelY, 72);
            lf.lfWeight = FW_NORMAL;

            fontHeight = 8 + 2 + 2;
            CalculateFontSizes(dc, menuLogFont, fontWidth, fontWidthNumber);
        }

        // Small Font
        {
            LOGFONT& lf = BaseWindow::menuLogFont2;
            memset(&lf, 0, sizeof(LOGFONT));
            lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
            lf.lfQuality = DEFAULT_QUALITY;
            lf.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS;
            CopyString(lf.lfFaceName, "Small Fonts", 32);

            lf.lfItalic         = 0;
            lf.lfUnderline  = 0;
            int nLogPixelY = ::GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY);
            lf.lfHeight = -MulDiv(6, nLogPixelY, 72);
            lf.lfWeight = FW_NORMAL;

            fontHeight2 = 6 + 1 + 1;

            CalculateFontSizes(dc, menuLogFont2, fontWidth2, fontWidthNumber2);
        }

        // RichEdit for scrolled window -- used in data pg
        VERIFY( AfxInitRichEdit() );

        bGlobalInit = true;
    }

    void BaseWindow::CalculateFontSizes(CDC& dc, const LOGFONT& logFont, int& fontWidth, int& fontWidthNumber)
    {
        CFont font;
        font.CreateFontIndirect(&logFont);
        CFont* pOldFont = dc.SelectObject(&font);
        {
            static char szText[] = "MMMMMMMMMMMMMM";
            CSize sz = dc.GetTextExtent(szText, StringLength(szText));
            fontWidth = sz.cx / StringLength(szText);
        }
        {
            static char szText[] = "99999999999999";
            CSize sz = dc.GetTextExtent(szText, StringLength(szText));
            fontWidthNumber = sz.cx / StringLength(szText);
        }
        dc.SelectObject(pOldFont);
    }

    BaseWindow::~BaseWindow()
    {
    }

    bool BaseWindow::IsCreated()
    {
        CWnd* pWnd = GetWindow();
        if (pWnd == NULL) {
            return false;
        }
        return ::IsWindow(pWnd->m_hWnd) != 0;
    }

    bool BaseWindow::IsInitialized()
    {
        return bInitialized;
    }

    void BaseWindow::SetSensitive(bool isSensitive)
    {
        CWnd* pWnd = GetWindow();
        if (pWnd != NULL) {
            pWnd->EnableWindow(isSensitive);
        }
    }

    void BaseWindow::DoSetSensitive(bool isSensitive)
    {
        BaseWindow::SetSensitive(isSensitive);
    }

    void BaseWindow::Manage()
    {
        CWnd* pWnd = GetWindow();
        if (pWnd != NULL) {
            pWnd->ShowWindow(SW_SHOW);
        }
    }

    void BaseWindow::DoManage()
    {
        BaseWindow::Manage();
    }

    void BaseWindow::Unmanage()
    {
        CWnd* pWnd = GetWindow();
        if (pWnd != NULL) {
            pWnd->ShowWindow(SW_HIDE);
        }
    }

    void BaseWindow::DoUnmanage()
    {
        BaseWindow::Unmanage();
    }

    bool BaseWindow::IsManaged()
    {
        CWnd* pWnd = GetWindow();
        if (pWnd != NULL) {
            return pWnd->IsWindowVisible() != FALSE;
        }
        return false;
    }

    void BaseWindow::SetManaged(bool manageFlag)
    {
        if (manageFlag) {
            Manage();
        } else {
            Unmanage();
        }
    }

    void BaseWindow::Delete()
    {
        CWnd* pWnd = GetWindow();
        if (pWnd != NULL) {
            pWnd->DestroyWindow();
        }
    }

    CWnd* BaseWindow::GetParentWnd()
    {
        return layoutManager->GetParentWnd();
    }

    OrdinalDispatcher* BaseWindow::GetOrdinalDispatcher()
    {
        return layoutManager->GetOrdinalDispatcher();
    }

    int     BaseWindow::GetID()
    {
        return ordinal;
    }

    // Initialisation
    void BaseWindow::InitWidget(BaseWindow* form, const ArgListC& inArgs)
    {
        ASSERT(dynamic_cast<LayoutManager*>(form) != NULL);
        layoutManager = static_cast<LayoutManager*>(form);
        inArgs.GetAttachment(*this);
        layoutManager->AddChild(*this);

#ifdef _DEBUG
        //const type_info& t1 = typeid(*form);
        //const type_info& t2 = typeid(*this);
        //TRACE("Parent %s has a new child %s\r\n", t1.name(), t2.name());
#endif

        bInitialized = true;
    }

    void BaseWindow::InitWidget(BaseWindow& form, const ArgListC& inArgs)
    {
        InitWidget(&form, inArgs);
    }

    void BaseWindow::SetSmall(bool bSmall)
    {
        m_bWidthCalculated = false;
        m_bSmall = bSmall;
        CWnd* pWnd = GetWindow();
        if (pWnd != NULL && ::IsWindow(pWnd->m_hWnd)) {
            pWnd->SetFont(GetMenuFont());
        }
    }

    void BaseWindow::Remove()
    {
        ASSERT(layoutManager != NULL);
        layoutManager->RemoveChild(this);
    }


    bool BaseWindow::IsSmall()
    {
        return m_bSmall;
    }

    CFont* BaseWindow::GetMenuFont()
    {
        if (m_bSmall)
            return &menuFont2;
        else
            return &menuFont;
    }

    int BaseWindow::GetFontHeight()
    {
        return m_bSmall ? fontHeight2 : fontHeight;
    }

    int BaseWindow::GetFontWidth()
    {
        return m_bSmall ? fontWidth2 : fontWidth;
    }

    int BaseWindow::GetFontWidthNumber()
    {
        return m_bSmall ? fontWidthNumber2 : fontWidthNumber;
    }

    int BaseWindow::GetWindowWidth()
    {
        if (windowWidth > 0) {
            return windowWidth + GetWidthOffset();
        } else {
            return 50 + GetWidthOffset();
        }
    }

    int BaseWindow::GetWindowHeight()
    {
        if (windowHeight > 0) {
            return GetHeightOffset() + windowHeight;
        } else {
            return GetHeightOffset() + GetFontHeight() + 10;
        }
    }

    int BaseWindow::SetWindowPosX(int x1, int x2)
    {
        return GetWindowWidth();
    }

    int BaseWindow::SetWindowPosY(int y1, int y2)
    {
        return GetWindowHeight();
    }

    int BaseWindow::GetWidthOffset()
    {
        return leftAttach.offset + rightAttach.offset;
    }

    int BaseWindow::GetHeightOffset()
    {
        return topAttach.offset + bottomAttach.offset;
    }

    CRect BaseWindow::AdjustOffsets(const CRect& rect)
    {
        CRect rcOff = rect;
        rcOff.top += topAttach.offset;
        rcOff.bottom -= bottomAttach.offset;
        rcOff.left += leftAttach.offset;
        rcOff.right -= rightAttach.offset;
        return rcOff;
    }

    void BaseWindow::SetWidgetColor(COLORREF windowColor)
    {
        m_clrBackground = windowColor;
    }

    void BaseWindow::SetWidgetColor(SC_ColorSpec& windowColor)
    {
        m_clrBackground = ConvertSC_ColorSpec(windowColor);
    }

    void BaseWindow::SetWidgetColor(StdColors windowColor)
    {
        m_clrBackground = GetStdColor(windowColor);
    }


    ////////////////////////////////////////////////////
    // LayoutManager

    LayoutManager::LayoutManager()
    {
        children.SetResizable();
    }

    LayoutManager::~LayoutManager()
    {
    }

    void LayoutManager::AddChild(BaseWindow& child)
    {
        child.ordinal = GetOrdinalDispatcher()->GetNextID();
        child.m_bSmall = m_bSmall;
        children += &child;
    }

    void LayoutManager::SetSmall(bool bSmall)
    {
        BaseWindow::SetSmall(bSmall);
        for (int i = 0; i < children.Size(); i++) {
            children[i]->SetSmall(bSmall);
        }
    }

    // this is a special case method called only when ~PC_ReportPS
    // is called to adjust rightAttach.attachPos

    void LayoutManager::AdjustChildren(BaseWindow* child, int rightPosOffset)
    {
        int offset = 0;

        // only apply an adjustment to the children to the right of the
        // child being removed (hint..hint... LayoutManager::RemoveChild()
        // should be called next. Again, this code is a special case that
        // probably will not work well if called by a class other than PC_ReportPS
        for (int i = 0; i < children.Size(); i++) {
            if (child == children[i])
                offset = rightPosOffset;
            if (children[i]->rightAttach.attachType == WindowAttachment::atPosition
                && children[i]->rightAttach.attachPos > 0)
                {
                    children[i]->rightAttach.attachPos -= offset;
                }
        }
    }

    // this is a special case method called only when ~PC_ReportPS
    // is called when a Object is deleted and it contains a cursorReport

    void LayoutManager::RemoveChild(BaseWindow* child)
    {
        // find all widgets which reference 'child' and reassign
        // WindowAttachments. Again, this code is a special case that
        // probably will not work well if called by a class other than PC_ReportPS
        for (int i = 0; i < children.Size(); i++) {
            if (children[i]->topAttach.attachType == WindowAttachment::atChild
                && children[i]->topAttach.attachChild == child)
                {
                    children[i]->topAttach.attachChild = child->topAttach.attachChild;
                }
            if (children[i]->bottomAttach.attachType == WindowAttachment::atChild
                && children[i]->bottomAttach.attachChild == child)
                {
                    children[i]->bottomAttach.attachChild = child->bottomAttach.attachChild;
                }
            if (children[i]->leftAttach.attachType == WindowAttachment::atChild
                && children[i]->leftAttach.attachChild == child)
                {
                    children[i]->leftAttach.attachChild = child->leftAttach.attachChild;
                }
            if (children[i]->rightAttach.attachType == WindowAttachment::atChild
                && children[i]->rightAttach.attachChild == child)
                {
                    children[i]->rightAttach.attachChild = child->rightAttach.attachChild;
                }
        }
        // now remove the child
        for (int i = 0; i < children.Size(); i++) {
            if (child == children[i]) {
                children.DeleteItem(i);
                return;
            }
        }
        ASSERT(0);
    }

    CWnd* LayoutManager::GetWindow()
    {
        return NULL;
    }

    void LayoutManager::Create(const CRect& rect)
    {
    }

    void LayoutManager::SetSensitive(bool isSensitive)
    {
        BaseWindow::DoSetSensitive(isSensitive);
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            child.DoSetSensitive(isSensitive);
        }
    }

    void LayoutManager::DoSetSensitive(bool isSensitive)
    {
        BaseWindow::DoSetSensitive(isSensitive);
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            child.DoSetSensitive(isSensitive);
        }
    }

    // We recurse Show/Hide code, because we don't
    // have single parental window to do it for us -
    // we must overwrite it for windowing ancestors
    void LayoutManager::Manage()
    {
        LayoutManager::DoManage();
    }

    void LayoutManager::DoManage()
    {
        BaseWindow::DoManage();
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            child.DoManage();
        }
    }

    void LayoutManager::Unmanage()
    {
        LayoutManager::DoUnmanage();
    }

    void LayoutManager::DoUnmanage()
    {
        BaseWindow::DoUnmanage();
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            child.DoUnmanage();
        }
    }

    // delete object from memory
    void LayoutManager::Delete()
    {
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            child.Delete();
        }
        // remove all objects from list
        children.DeAlloc();
    }

    void LayoutManager::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcChild;
        CRect rcOff = AdjustOffsets(rect);
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];

#ifdef _DEBUG
            /*
            // Indend text
            for (int j = 0; j < level; j++) TRACE(" ");
            const type_info& t = typeid(child);
            TRACE("%s ", t.name());
            TRACE("%d - top:%d bottom:%d left:%d right:%d topoff:%d bottomoff:%d leftoff:%d rightoff:%d width:%d\r\n",
            i,
            child.rect.top,
            child.rect.bottom,
            child.rect.left,
            child.rect.right,
            child.topAttach.offset,
            child.bottomAttach.offset,
            child.leftAttach.offset,
            child.rightAttach.offset,
            child.rect.right - child.rect.left);
            */
#endif

            rcChild = child.rect;
            rcChild.OffsetRect(rcOff.left, rcOff.top);
            child.SetWindowPosition(rcChild, level+1);
        }
    }

    void BaseWindow::SetWindowPosition(const CRect& rect, int level)
    {
        TRACE( "    Element %d\r\n", GetID() );
    }

    int LayoutManager::SetWindowPosY(int y1, int y2)
    {
        TRACE("LayoutManager::SetWindowPosY(%d, %d)\n", y1, y2);
        for (int i = 0; i < children.Size(); i++) {

            BaseWindow& child = *children[i];
            bool bWaitHeightForBottom = false;
            bool bWaitHeightForTop = false;
            // Vertical direction
            child.rect.top = 0;
            child.rect.bottom = 0;
            child.calcHeight = 0;
            if (child.topAttach.attachType == WindowAttachment::atParent) {
                child.rect.top = 0;
                if (child.bottomAttach.attachType == WindowAttachment::atParent) {
                    if (y2 == y1) {
                        bWaitHeightForBottom = true;
                    } else {
                        child.rect.bottom = abs(0 + y2 - y1);
                    }
                } else if (child.bottomAttach.attachType == WindowAttachment::atChild) {
                    //ASSERT(y2 > y1);
                    child.rect.bottom = child.bottomAttach.attachChild->rect.top;
                } else if (child.bottomAttach.attachType == WindowAttachment::atPosition) {
                    child.rect.bottom = int((y2 - y1)/100.0*child.bottomAttach.attachPos);
                    //ASSERT(y2 > y1);
                } else { // atNone
                    bWaitHeightForBottom = true;
                }
            } else if (child.topAttach.attachType == WindowAttachment::atChild) {
                child.rect.top = child.topAttach.attachChild->rect.bottom;
                if (child.bottomAttach.attachType == WindowAttachment::atParent) {
                    if (y2 > y1) {
                        child.rect.bottom = abs(0 + y2 - y1);
                    } else {
                        child.rect.bottom = 0;
                    }
                } else if (child.bottomAttach.attachType == WindowAttachment::atChild) {
                    ASSERT(y2 > y1);
                    child.rect.bottom = child.bottomAttach.attachChild->rect.top;
                } else { // atNone
                    bWaitHeightForBottom = true;
                    child.rect.bottom = child.rect.top;
                }
            } else { // atNone
                if (child.bottomAttach.attachType == WindowAttachment::atParent) {
                    if (y2 > 0) {
                        child.rect.bottom = y2-y1;
                    } else {
                        child.rect.bottom = 0;
                    }
                    child.rect.top = child.rect.bottom;
                    bWaitHeightForTop = true;
                } else if (child.bottomAttach.attachType == WindowAttachment::atChild) {
                    child.rect.bottom = child.bottomAttach.attachChild->rect.top;
                    child.rect.top = child.rect.bottom;
                    bWaitHeightForTop = true;
                } else { // atNone
                    ASSERT(0);
                }
            }
            int nSize = child.SetWindowPosY(child.rect.top, child.rect.bottom);
            if (bWaitHeightForBottom) {
                child.rect.bottom = nSize + child.rect.top;
            }
            if (bWaitHeightForTop) {
                child.rect.top = child.rect.bottom - nSize;
            }
            // calculate Height
            child.calcHeight = abs(child.rect.top - child.rect.bottom);
            if (child.bottomAttach.attachType == WindowAttachment::atChild) {
                child.calcHeight += child.bottomAttach.attachChild->calcHeight;
            }
            if (child.topAttach.attachType == WindowAttachment::atChild) {
                child.calcHeight += child.topAttach.attachChild->calcHeight;
            }
        }
        // find the biggest Height
        int nHeight = 0;
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            if (child.calcHeight > nHeight) {
                nHeight = child.calcHeight;
            }
        }
        // Normalize children's rects
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            if (child.rect.top < 0) {
                child.rect.bottom += nHeight;
                child.rect.top += nHeight;
            }
        }

        // we go through children's attachments again (bottom attachments)
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            if (child.bottomAttach.attachType == WindowAttachment::atChild) {
                child.rect.bottom = child.bottomAttach.attachChild->rect.top;
            } else if (child.bottomAttach.attachType == WindowAttachment::atParent) {
                LayoutManager* lay = dynamic_cast<LayoutManager*>(&child);
                if (lay != NULL && dynamic_cast<ScrolledForm*>(&child) == NULL) {
                    if (nHeight - child.rect.top > child.rect.top) {
                        child.rect.bottom = nHeight - child.rect.top;
                    }
                    // DIRTY: simplified propagation of height change
                    if (child.topAttach.attachType == WindowAttachment::atParent
                        && lay->children.Size() > 0)
                        {
                            int nHeight = child.rect.bottom - child.rect.top;
                            if (lay->children[0]->topAttach.attachType == WindowAttachment::atParent
                                && lay->children[0]->bottomAttach.attachType == WindowAttachment::atParent)
                                {
                                    lay->children[0]->rect.bottom = nHeight - lay->children[0]->rect.top;
                                }
                        }
                }
            }
        }
        return nHeight + GetHeightOffset();
    }

    int LayoutManager::SetWindowPosX(int x1, int x2)
    {
        TRACE("LayoutManager::SetWindowPosX(%d, %d)\n", x1, x2);
        int dx = x2 - x1;
        if (windowWidth != -1) {
            x2 = x1 + windowWidth;
        }
        for (int i = 0; i < children.Size(); i++) {

            BaseWindow& child = *children[i];
            bool bWaitWidthForRight = false;
            bool bWaitWidthForLeft = false;
            // Vertical direction
            child.rect.left = 0;
            child.rect.right = 0;
            child.calcWidth = 0;
            if (child.leftAttach.attachType == WindowAttachment::atParent) {
                child.rect.left = 0;
                if (child.rightAttach.attachType == WindowAttachment::atParent) {
                    if (x2 == x1) {
                        bWaitWidthForRight = true;
                    } else {
                        if (windowWidth != -1) {
                            child.rect.right = x1 + windowWidth;
                        } else {
                            child.rect.right = abs(0 + x2 - x1);
                        }
                    }
                } else if (child.rightAttach.attachType == WindowAttachment::atChild) {
                    //ASSERT(x2 > x1);
                    child.rect.right = child.rightAttach.attachChild->rect.left;
                } else if (child.rightAttach.attachType == WindowAttachment::atPosition) {
                    child.rect.right = int((dx)/100.0*child.rightAttach.attachPos);
                } else { // atNone
                    bWaitWidthForRight = true;
                }
            } else if (child.leftAttach.attachType == WindowAttachment::atChild) {
                child.rect.left = child.leftAttach.attachChild->rect.right;
                if (child.rightAttach.attachType == WindowAttachment::atParent) {
                    ASSERT(x2 > x1);
                    child.rect.right = abs(0 + x2 - x1);
                } else if (child.rightAttach.attachType == WindowAttachment::atChild) {
                    ASSERT(x2 > x1);
                    child.rect.right = child.rightAttach.attachChild->rect.left;
                } else if (child.rightAttach.attachType == WindowAttachment::atPosition) {
                    child.rect.right = int((dx)/100.0*child.rightAttach.attachPos);
                } else { // atNone
                    bWaitWidthForRight = true;
                    child.rect.right = child.rect.left;
                }
            } else if (child.leftAttach.attachType == WindowAttachment::atPosition) {
                child.rect.left = int((dx)/100.0*child.leftAttach.attachPos);
                if (child.rightAttach.attachType == WindowAttachment::atParent) {
                    ASSERT(x2);
                    child.rect.right = abs(0 + x2 - x1);
                } else if (child.rightAttach.attachType == WindowAttachment::atChild) {
                    ASSERT(x2);
                    child.rect.right = child.rightAttach.attachChild->rect.left;
                } else if (child.rightAttach.attachType == WindowAttachment::atPosition) {
                    ASSERT(x2);
                    child.rect.right = int((dx)/100.0*child.rightAttach.attachPos);
                } else { // atNone
                    bWaitWidthForRight = true;
                    child.rect.right = child.rect.left;
                }
            } else { // atNone
                if (child.rightAttach.attachType == WindowAttachment::atParent) {
                    if (x2 > 0) {
                        child.rect.right = x2-x1;
                    } else {
                        child.rect.right = 0;
                    }
                    child.rect.left = child.rect.right;
                    bWaitWidthForLeft = true;
                } else if (child.rightAttach.attachType == WindowAttachment::atChild) {
                    child.rect.right = child.rightAttach.attachChild->rect.left;
                    child.rect.left = child.rect.right;
                    bWaitWidthForLeft = true;
                } else if (child.rightAttach.attachType == WindowAttachment::atPosition) {
                    child.rect.right = int((dx)/100.0*child.rightAttach.attachPos);
                    child.rect.left = child.rect.right;
                    bWaitWidthForLeft = true;
                } else { // atNone
                    ASSERT(0);
                }
            }
            int nSize = child.SetWindowPosX(child.rect.left, child.rect.right);
            if (bWaitWidthForRight) {
                child.rect.right = nSize + child.rect.left;
            }
            if (bWaitWidthForLeft) {
                if (child.rect.right == 0) {
                    child.rect.right = nSize;
                } else {
                    child.rect.left = child.rect.right - nSize;
                }
            }
            // calculate Width
            child.calcWidth = abs(child.rect.left - child.rect.right);
            if (child.rightAttach.attachType == WindowAttachment::atChild) {
                child.calcWidth += child.rightAttach.attachChild->calcWidth;
            }
            if (child.leftAttach.attachType == WindowAttachment::atChild) {
                child.calcWidth += child.leftAttach.attachChild->calcWidth;
            }
        }
        // find the biggest Width
        int nWidth = 0;
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            if (child.calcWidth > nWidth) {
                nWidth = child.calcWidth;
            }
        }
        // Normalize childern's rects
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            if (child.rect.left < 0) {
                //TODO: check it out
                if (child.rightAttach.attachType != WindowAttachment::atParent
                    && child.leftAttach.attachType != WindowAttachment::atParent)
                    {
                        child.rect.right += nWidth;
                        child.rect.left += nWidth;
                    }
            }
        }

        // we go through children's attachments again (right attachments)
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            if (child.rightAttach.attachType == WindowAttachment::atChild) {
                child.rect.right = child.rightAttach.attachChild->rect.left;
            } else if (child.rightAttach.attachType == WindowAttachment::atParent) {
                //child.rect.right = nWidth;
            }
        }
        return nWidth + GetWidthOffset();
    }

    void LayoutManager::Resize(int x1, int x2, int y1, int y2)
    {
        CWnd* pWnd = GetWindow();
        if (pWnd != NULL) {
            pWnd->ShowWindow(SW_HIDE);
        }
        int x_end = x2;
        int y_end = y2;
        SetWindowPosX(x1, x_end);
        SetWindowPosY(y1, y_end);
        CRect rectSet(x1, y1, x_end, y_end); // client rect
        SetWindowPosition(rectSet);
        if (pWnd != NULL) {
            pWnd->ShowWindow(SW_SHOW);
        }
    }

    ////////////////////////////////////////
    // FrameManager
    FrameManager::FrameManager()
    {
        frameLabel[0] = '\0';
    }

    FrameManager::FrameManager(const char* frameLabel)
    {
        CopyString(FrameManager::frameLabel, frameLabel, maxFrameLabelLen);
    }

    FrameManager::~FrameManager()
    {
    }

    void FrameManager::SetFrameText(const char* frameLabel)
    {
        CopyString(FrameManager::frameLabel, frameLabel, maxFrameLabelLen);
        if (::IsWindow(groupBox.m_hWnd)) {
            groupBox.SetWindowText(frameLabel);
        }
    }

    CWnd* FrameManager::GetWindow()
    {
        return &groupBox;
    }

    void FrameManager::Create(const CRect& rect)
    {
        int nID = GetOrdinalDispatcher()->GetNextID();
        groupBox.Create(frameLabel, WS_CHILD|WS_VISIBLE|BS_GROUPBOX, rect, GetParentWnd(), nID);
        groupBox.SetFont(GetMenuFont(), TRUE);
    }

    void FrameManager::Delete()
    {
        if (::IsWindow(groupBox.m_hWnd)) {
            groupBox.DestroyWindow();
        }
        LayoutManager::Delete();
    }

    void FrameManager::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!::IsWindow(groupBox.m_hWnd)) {
            Create(rect);
        } else {
            groupBox.MoveWindow(rect);
        }

        LayoutManager::SetWindowPosition(rect, level);
    }

    int FrameManager::SetWindowPosX(int x1, int x2)
    {
        return LayoutManager::SetWindowPosX(x1+5, x2-5);
        //return LayoutManager::SetWindowPosX(x1, x2);
    }

    int FrameManager::SetWindowPosY(int y1, int y2)
    {
        int nMove = GetFontHeight() + int(GetFontHeight()/3.);
        int nRc = LayoutManager::SetWindowPosY(y1, y2);

        // move all children by text label height down
        for (int i = 0; i < children.Size(); i++) {
            BaseWindow& child = *children[i];
            child.rect.top += nMove;
            child.rect.bottom += nMove;
        }
        return nRc + nMove + 4;
    }

    ////////////////////////
    // CFormWnd

    BOOL CFormWnd::PreTranslateMessage( MSG* pMsg )
    {
        if (pMsg->message == WM_KEYDOWN) {
            if (pMsg->wParam == VK_TAB) {
                bool bShift = GetKeyState(VK_SHIFT) < 0;
                bool bFound = false;
                CWnd* pFocusWnd = GetFocus();
                CWnd* pChildWnd;
                if (pFocusWnd != NULL) {
                    for (pChildWnd = GetWindow(GW_CHILD);
                         pChildWnd != NULL;
                         pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT))
                        {
                            if (pFocusWnd == pChildWnd) {
                                break;
                            }
                        }
                    if (pChildWnd != NULL) {
                        CWnd* pWnd = pChildWnd;
                        do {
                            pWnd = pWnd->GetWindow(bShift ? GW_HWNDPREV : GW_HWNDNEXT);
                            if (pWnd == NULL) {
                                pWnd = pChildWnd->GetWindow(bShift ? GW_HWNDLAST : GW_HWNDFIRST);
                            }
                            if (pWnd->IsWindowEnabled() && (pWnd->GetStyle() & WS_TABSTOP)) {
                                pWnd->SetFocus();
                                break;
                            }
                        } while (pWnd != pChildWnd);
                    }
                } else { //pFocusWnd == NULL)
                    // Set focus on the first child
                    for (CWnd* pChildWnd = GetWindow(GW_CHILD);
                         pChildWnd != NULL;
                         pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT))
                        {
                            if (pChildWnd->IsWindowEnabled() && (pChildWnd->GetStyle() & WS_TABSTOP)) {
                                pChildWnd->SetFocus();
                                break;
                            }
                        }
                }
                return TRUE;    // DO NOT process further
            }
        }
        return CWnd::PreTranslateMessage(pMsg);
    }

    BEGIN_MESSAGE_MAP(CFormWnd, CWnd)
    //{{AFX_MSG_MAP(CFormWnd)
    ON_WM_SETFOCUS()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void CFormWnd::OnSetFocus(CWnd* pOldWnd)
    {
        for (CWnd* pChildWnd = GetWindow(GW_CHILD);
             pChildWnd != NULL;
             pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT))
            {
                if (pChildWnd->IsWindowEnabled() && (pChildWnd->GetStyle() & WS_TABSTOP)) {
                    pChildWnd->SetFocus();
                    break;
                }
            }
    }

    ////////////////////////////////////////
    // Scrolled Area
    ScrolledArea::ScrolledArea(bool bUseScrollSkills) :
        m_bInsideUpdate(false),
        m_bUseScrollSkills(bUseScrollSkills)
    {
    }

    BEGIN_MESSAGE_MAP(ScrolledArea, CFormWnd)
    //{{AFX_MSG_MAP(ScrolledArea)
    ON_WM_SIZE()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void ScrolledArea::OnSize(UINT nType, int cx, int cy)
    {
        CWnd::OnSize(nType, cx, cy);
        if (m_bUseScrollSkills) {
            UpdateBars();
        }
    }

    void ScrolledArea::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
    {
        // ignore scroll bar msgs from other controls
        if (pScrollBar != GetScrollBarCtrl(SB_HORZ)) {
            if (pScrollBar != NULL) {
                pScrollBar->SendMessage(WM_HSCROLL, MAKELONG(nSBCode, nPos), LPARAM(pScrollBar->m_hWnd));
            }
            return;
        }

        OnScroll(MAKEWORD(nSBCode, -1), nPos);
    }

    void ScrolledArea::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
    {
        // ignore scroll bar msgs from other controls
        if (pScrollBar != GetScrollBarCtrl(SB_VERT)) {
            if (pScrollBar != NULL) {
                pScrollBar->SendMessage(WM_VSCROLL, MAKELONG(nSBCode, nPos), LPARAM(pScrollBar->m_hWnd));
            }
            return;
        }

        OnScroll(MAKEWORD(-1, nSBCode), nPos);
    }

    bool ScrolledArea::OnScroll(UINT nScrollCode, UINT nPos, bool bDoScroll)
    {
        // calc new x position
        int x = GetScrollPos(SB_HORZ);
        int xOrig = x;

        switch (LOBYTE(nScrollCode)) {
        case SB_TOP:
            x = 0;
            break;
        case SB_BOTTOM:
            x = INT_MAX;
            break;
        case SB_LINEUP:
            x -= m_szLine.cx;
            break;
        case SB_LINEDOWN:
            x += m_szLine.cx;
            break;
        case SB_PAGEUP:
            x -= m_szPage.cx;
            break;
        case SB_PAGEDOWN:
            x += m_szPage.cx;
            break;
        case SB_THUMBTRACK:
            x = nPos;
            break;
        }

        // calc new y position
        int y = GetScrollPos(SB_VERT);
        int yOrig = y;

        switch (HIBYTE(nScrollCode)) {
        case SB_TOP:
            y = 0;
            break;
        case SB_BOTTOM:
            y = INT_MAX;
            break;
        case SB_LINEUP:
            y -= m_szLine.cy;
            break;
        case SB_LINEDOWN:
            y += m_szLine.cy;
            break;
        case SB_PAGEUP:
            y -= m_szPage.cy;
            break;
        case SB_PAGEDOWN:
            y += m_szPage.cy;
            break;
        case SB_THUMBTRACK:
            y = nPos;
            break;
        }

        bool bResult = OnScrollBy(CSize(x - xOrig, y - yOrig), bDoScroll);
        if (bResult && bDoScroll)
            {
                Invalidate();
                UpdateWindow();
            }

        return bResult;
    }

    bool ScrolledArea::OnScrollBy(CSize sizeScroll, bool bDoScroll)
    {
        int xOrig, x;
        int yOrig, y;

        // don't scroll if there is no valid scroll range (ie. no scroll bar)
        CScrollBar* pBar;
        DWORD dwStyle = GetStyle();
        pBar = GetScrollBarCtrl(SB_VERT);
        if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
            (pBar == NULL && !(dwStyle & WS_VSCROLL)))
            {
                // vertical scroll bar not enabled
                sizeScroll.cy = 0;
            }
        pBar = GetScrollBarCtrl(SB_HORZ);
        if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
            (pBar == NULL && !(dwStyle & WS_HSCROLL)))
            {
                // horizontal scroll bar not enabled
                sizeScroll.cx = 0;
            }

        // adjust current x position
        xOrig = x = GetScrollPos(SB_HORZ);
        int xMax = GetScrollLimit(SB_HORZ);
        x += sizeScroll.cx;
        if (x < 0)
            x = 0;
        else if (x > xMax)
            x = xMax;

        // adjust current y position
        yOrig = y = GetScrollPos(SB_VERT);
        int yMax = GetScrollLimit(SB_VERT);
        y += sizeScroll.cy;
        if (y < 0)
            y = 0;
        else if (y > yMax)
            y = yMax;

        // did anything change?
        if (x == xOrig && y == yOrig)
            return false;

        if (bDoScroll)
            {
                // do scroll and update scroll positions
                ScrollWindow(-(x-xOrig), -(y-yOrig));
                if (x != xOrig)
                    SetScrollPos(SB_HORZ, x);
                if (y != yOrig)
                    SetScrollPos(SB_VERT, y);
            }
        return true;
    }

    BOOL ScrolledArea::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
    {
        // we don't handle anything but scrolling just now
        if (fFlags & (MK_SHIFT | MK_CONTROL))
            return FALSE;

        // we can't get out of it--perform the scroll ourselves
        return DoMouseWheel(fFlags, zDelta, point);
    }


    BOOL ScrolledArea::DoMouseWheel(UINT fFlags, short zDelta, CPoint point)
    {
        UNUSED_ALWAYS(point);
        UNUSED_ALWAYS(fFlags);

        // if we have a vertical scroll bar, the wheel scrolls that
        // if we have _only_ a horizontal scroll bar, the wheel scrolls that
        // otherwise, don't do any work at all

        DWORD dwStyle = GetStyle();
        CScrollBar* pBar = GetScrollBarCtrl(SB_VERT);
        BOOL bHasVertBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
            (dwStyle & WS_VSCROLL);

        pBar = GetScrollBarCtrl(SB_HORZ);
        BOOL bHasHorzBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
            (dwStyle & WS_HSCROLL);

        if (!bHasVertBar && !bHasHorzBar)
            return FALSE;

        BOOL bResult = FALSE;
        UINT uWheelScrollLines = 3;
        int nToScroll;
        int nDisplacement;

        if (bHasVertBar)
            {
                nToScroll = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);
                if (nToScroll == -1 || uWheelScrollLines == WHEEL_PAGESCROLL)
                    {
                        nDisplacement = m_szPage.cy;
                        if (zDelta > 0)
                            nDisplacement = -nDisplacement;
                    }
                else
                    {
                        nDisplacement = nToScroll * m_szLine.cy;
                        nDisplacement = min(nDisplacement, m_szPage.cy);
                    }
                bResult = OnScrollBy(CSize(0, nDisplacement), TRUE);
            }
        else if (bHasHorzBar)
            {
                nToScroll = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);
                if (nToScroll == -1 || uWheelScrollLines == WHEEL_PAGESCROLL)
                    {
                        nDisplacement = m_szPage.cx;
                        if (zDelta > 0)
                            nDisplacement = -nDisplacement;
                    }
                else
                    {
                        nDisplacement = nToScroll * m_szLine.cx;
                        nDisplacement = min(nDisplacement, m_szPage.cx);
                    }
                bResult = OnScrollBy(CSize(nDisplacement, 0), TRUE);
            }

        if (bResult)
            {
                Invalidate();
                UpdateWindow();
            }

        return bResult;
    }

    void ScrolledArea::UpdateBars()
    {
        // Lock out recursion
        if (m_bInsideUpdate) {
            return;
        }
        m_bInsideUpdate = true;

        CRect rcClient;
        GetClientRect(rcClient);
        ASSERT(rcClient.top == 0 && rcClient.left == 0);
        m_szPage.cx = rcClient.right;
        m_szPage.cy = rcClient.bottom;
        CSize szSb = GetScrollBarSizes();
        DWORD dwStyle = GetStyle();
        if (szSb.cx != 0 && (dwStyle & WS_VSCROLL)) {
            m_szPage.cx += szSb.cx;
        }
        if (szSb.cy != 0 && (dwStyle & WS_HSCROLL)) {
            m_szPage.cy += szSb.cy;
        }

        CPoint ptMove(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
        CSize sizeRange = m_szFull - m_szPage;
        bool bScrollBarX = sizeRange.cx > 0;
        if (!bScrollBarX) {
            ptMove.x = 0;
        } else {
            sizeRange.cy += szSb.cy;
        }
        bool bScrollBarY = sizeRange.cy > 0;
        if (!bScrollBarY) {
            ptMove.y = 0;
        } else {
            sizeRange.cx += szSb.cx;
        }
        if (bScrollBarY && !bScrollBarX && sizeRange.cx > 0) {
            // need a horizontal scrollbar after all
            bScrollBarX = true;
            sizeRange.cy += szSb.cy;
        }

        // if current scroll position will be past the limit, scroll to limit
        if (sizeRange.cx > 0 && ptMove.x >= sizeRange.cx) {
            ptMove.x = sizeRange.cx;
        }
        if (sizeRange.cy > 0 && ptMove.y >= sizeRange.cy) {
            ptMove.y = sizeRange.cy;
        }

        if (bScrollBarX) {
            m_szPage.cy -= szSb.cy;
        }
        if (bScrollBarY) {
            m_szPage.cx -= szSb.cx;
        }

        // first scroll the window as needed
        ScrollToPosition(ptMove);

        // Now update the bars as appropriate
        SCROLLINFO info;
        info.fMask = SIF_PAGE|SIF_RANGE;
        info.nMin = 0;

        EnableScrollBarCtrl(SB_HORZ, bScrollBarX);
        if (bScrollBarX) {
            info.nPage = m_szPage.cx;
            info.nMax = m_szFull.cx-1;
            if (!SetScrollInfo(SB_HORZ, &info, TRUE))
                SetScrollRange(SB_HORZ, 0, sizeRange.cx, TRUE);
        }
        EnableScrollBarCtrl(SB_VERT, bScrollBarY);
        if (bScrollBarY) {
            info.nPage = m_szPage.cy;
            info.nMax = m_szFull.cy-1;
            if (!SetScrollInfo(SB_VERT, &info, TRUE))
                SetScrollRange(SB_VERT, 0, sizeRange.cy, TRUE);
        }

        m_bInsideUpdate = false;
    }

    CSize ScrolledArea::GetScrollBarSizes()
    {
        CSize sizeSb;
        DWORD dwStyle = GetStyle();

        sizeSb.cx = ::GetSystemMetrics(SM_CXHSCROLL);
        if (dwStyle & WS_BORDER) {
            sizeSb.cx -= ::GetSystemMetrics(SM_CXBORDER);
        }
        sizeSb.cy = ::GetSystemMetrics(SM_CYHSCROLL);
        if (dwStyle & WS_BORDER) {
            sizeSb.cy -= ::GetSystemMetrics(SM_CYBORDER);
        }
        return sizeSb;
    }

    void ScrolledArea::ScrollToPosition(CPoint& pt)
    {
        ASSERT(pt.x >= 0);
        ASSERT(pt.y >= 0);

        // Note: ScrollToPosition can and is used to scroll out-of-range
        //  areas as far as ScrolledArea is concerned -- specifically in
        //  the print-preview code.  Since OnScrollBy makes sure the range is
        //  valid, ScrollToPosition does not vector through OnScrollBy.

        int xOrig = GetScrollPos(SB_HORZ);
        SetScrollPos(SB_HORZ, pt.x);
        int yOrig = GetScrollPos(SB_VERT);
        SetScrollPos(SB_VERT, pt.y);
        ScrollWindow(xOrig - pt.x, yOrig - pt.y);
    }

    ////////////////////////////////////////////////////////////////////

    LPCTSTR RegisterFormClass()
    {
        static bool bRegistered = false;
        static LPCTSTR lpszClass = NULL;
        if (bRegistered) {
            return lpszClass;
        }

        lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
                                        ::LoadCursor(NULL, IDC_ARROW),
                                        HBRUSH(COLOR_BTNFACE+1),
                                        NULL);

        bRegistered = true;
        return lpszClass;
    }

    BOOL ScrolledArea::PreTranslateMessage( MSG* pMsg )
    {
        if (!m_bUseScrollSkills) {
            return CFormWnd::PreTranslateMessage(pMsg);
        }
        return FALSE;
    }


    ////////////////////////////////////////////////////////////////////
    // ScrolledForm

    ScrolledForm::ScrolledForm(bool bUseScrollSkills) :
        ScrolledArea(bUseScrollSkills)
    {
    }

    void ScrolledForm::Destroy()
    {
        if (m_bUseScrollSkills) {
            m_wndForm.DestroyWindow();
        }
        DestroyWindow();
    }

    void ScrolledForm::Create(const CRect& rect)
    {
        int nID = GetID();
        CWnd* pParent = layoutManager->GetParentWnd();
        LPCTSTR lpszClass = RegisterFormClass();

        // frame window
        VERIFY(CWnd::CreateEx(WS_EX_CLIENTEDGE, lpszClass, "ScrolledArea", WS_CHILD|WS_VISIBLE, rect, pParent, nID));
        TRACE("ScrollArea %d\r\n", nID);

        if (m_bUseScrollSkills) {
            // window to be scrolled
            VERIFY(m_wndForm.Create(lpszClass, "Form", WS_CHILD|WS_VISIBLE, rect, this, nID));
            m_wndForm.SetFont(GetMenuFont(), TRUE);

            SetupScrolledArea();
        }
    }

    CWnd* ScrolledForm::GetWindow()
    {
        return this;
    }

    CWnd* ScrolledForm::GetParentWnd()
    {
        if (m_bUseScrollSkills)
            return &m_wndForm;
        else
            return this;
    }

    void ScrolledForm::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    int ScrolledForm::GetWindowWidth()
    {
        // it won't be used
        return 300;
    }

    int ScrolledForm::GetWindowHeight()
    {
        // it won't be used
        return 1000;
    }

    CSize ScrolledForm::SetCalculateFormSize()
    {
        CSize szForm;
        szForm.cx = LayoutManager::SetWindowPosX(0, 300);
        szForm.cy = LayoutManager::SetWindowPosY(0, 1000);
        return szForm;
    }

    void ScrolledForm::CalculateSizes()
    {
        m_szFull = m_szForm = SetCalculateFormSize();
        //m_szFull += CSize(2, 2);
        m_szPage = m_szFull;
        m_szLine = CSize(2, 2);
    }

    void ScrolledForm::SetupScrolledArea()
    {
        CalculateSizes();

        CRect rcForm(0, 0, m_szForm.cx, m_szForm.cy);

        m_wndForm.MoveWindow(rcForm, FALSE);
        LayoutManager::SetWindowPosition(rcForm);

        UpdateBars();
    }

    CRect& ScrolledForm::GetAreaRect()
    {
        return rect;
    }

    // we don't need to use recursive LayouManager algorithm
    // when we have single parent window, which shows/hides
    // all its chilren
    void ScrolledForm::Manage()
    {
        BaseWindow::Manage();
    }

    void ScrolledForm::Unmanage()
    {
        BaseWindow::Unmanage();
    }

    BEGIN_MESSAGE_MAP(ScrolledForm, ScrolledArea)
    //{{AFX_MSG_MAP(ScrolledForm)
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void ScrolledForm::OnSize(UINT nType, int cx, int cy)
    {
        ScrolledArea::OnSize(nType, cx, cy);
    }

    void ScrolledForm::OnSetFocus(CWnd* pOldWnd)
    {
        if (m_bUseScrollSkills) {
            m_wndForm.SetFocus();
        } else {
            ScrolledArea::OnSetFocus(pOldWnd);
        }
    }

    //////////////////////////////////////////
    // FormManager

    FormManager::FormManager() :
        ScrolledForm(false)
    {
    }

    FormManager::~FormManager()
    {
    }

    CWnd* FormManager::GetWindow()
    {
        return this;
    }

    CWnd* FormManager::GetParentWnd()
    {
        return ScrolledForm::GetParentWnd();
    }

    OrdinalDispatcher* FormManager::GetOrdinalDispatcher()
    {
        return this;
    }

    void FormManager::Create(CWnd* pWnd, DWORD dwStyleAdd)
    {
        CRect rect;
        pWnd->GetClientRect(rect);
        CRect rcOff = AdjustOffsets(rect);

        LPCTSTR lpszClass = RegisterFormClass();
        int nID = GetID();
        if (!m_bUseScrollSkills) {
            VERIFY(CWnd::Create(lpszClass, "Form", WS_CHILD|WS_VISIBLE|dwStyleAdd,
                                rcOff,
                                pWnd,
                                nID));
            CWnd::SetFont(GetMenuFont(), TRUE);
        } else {
            m_bInsideUpdate = true; // Don't update bars as scroll area hasn't been set yet
            VERIFY(CWnd::Create(lpszClass, "ScrolledArea", WS_CHILD, rect, pWnd, nID));
            TRACE("ScrollArea %d\r\n", nID);

            // window to be scrolled
            VERIFY(m_wndForm.Create(lpszClass, "Form", WS_CHILD, rect, this, nID));
            m_wndForm.SetFont(GetMenuFont(), TRUE);

            m_bInsideUpdate = false; // we can update bars
            SetupScrolledArea();
            CRect rcForm(0, 0, 2048, 2048);
            m_wndForm.MoveWindow(rcForm, FALSE);
        }
        bInitialized = true;
    }

    CSize FormManager::SetCalculateFormSize()
    {
        CSize szForm;
        szForm.cx = LayoutManager::SetWindowPosX(0, 500);
        szForm.cy = LayoutManager::SetWindowPosY(0, 1200);
        return szForm;
    }

    void FormManager::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcInside = rect;
        rcInside.OffsetRect(-rect.left, -rect.top);
        if (IsCreated()) {
            TRACE("Form - top:%d bottom:%d left:%d right:%d\r\n",
                  rect.top,
                  rect.bottom,
                  rect.left,
                  rect.right);
            CWnd::SetWindowPos(&CWnd::wndTop,
                               rect.top, rect.left,
                               rect.Width(),
                               rect.Height(),
                               SWP_NOMOVE);
            LayoutManager::SetWindowPosition(rcInside);
        } else {
            ScrolledForm::Create(rcInside);
        }
    }

    void FormManager::Resize(int x1, int x2, int y1, int y2)
    {
        ShowWindow(SW_HIDE);
        int x_end = x2;
        int y_end = y2;
        if (m_bUseScrollSkills) {
            m_wndForm.ShowWindow(SW_HIDE);
            CRect rect;
            GetClientRect(rect);
            CRect rectClient(x1, y1, x2, y2);
            m_bInsideUpdate = true;
            MoveWindow(rectClient, FALSE);
            m_bInsideUpdate = false;
            CSize szSb = GetScrollBarSizes();

            CSize szPage(x2-x1, y2-y1);
            CSize sizeRange = m_szForm - szPage;
            bool bScrollBarX = sizeRange.cx > 0;
            if (bScrollBarX) {
                sizeRange.cy += szSb.cy;
            }
            bool bScrollBarY = sizeRange.cy > 0;
            if (bScrollBarY) {
                sizeRange.cx += szSb.cx;
            }
            if (bScrollBarY && !bScrollBarX && sizeRange.cx > 0) {
                // need a horizontal scrollbar after all
                bScrollBarX = true;
                sizeRange.cy += szSb.cy;
            }
            if (bScrollBarX) {
                szPage.cy -= szSb.cy;
            }
            if (bScrollBarY) {
                szPage.cx -= szSb.cx;
            }
            x_end = x1 + max(m_szForm.cx, szPage.cx);
            y_end = y1 + max(m_szForm.cy, szPage.cy);

            x_end = SetWindowPosX(x1, x_end);
            y_end = SetWindowPosY(y1, y_end);

            m_szFull = CSize(x_end-x1, y_end-y1);
            UpdateBars();
            CRect rectSet(x1, y1, x_end, y_end);
            LayoutManager::SetWindowPosition(rectSet);
            m_wndForm.ShowWindow(SW_SHOW);
        } else {
            SetWindowPosX(x1, x_end);
            SetWindowPosY(y1, y_end);
            CRect rectSet(x1, y1, x_end, y_end); // client rect
            if (!m_bUseScrollSkills) {
                MoveWindow(rectSet);
            }
            SetWindowPosition(rectSet);
        }
        ShowWindow(SW_SHOW);
    }

    BEGIN_MESSAGE_MAP(FormManager, ScrolledForm)
    //{{AFX_MSG_MAP(FormManager)
    ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    LRESULT FormManager::OnHelpHitTest(WPARAM, LPARAM)
    {
        int nHelpId = GetWindowContextHelpId();
        return nHelpId;
    }

    ////////////////////////////////////////
    // InteractiveWidget

    InteractiveWidget::InteractiveWidget()
    {
        for (int i = 0; i < maxCallbacks; i++)
            {
                otherObjectCB[i] = 0;
                otherObjectData[i] = 0;
                combinedObjectCB[i] = 0;
                combinedObjectData[i] = 0;
            }
    }

    InteractiveWidget::~InteractiveWidget()
    {
    }


    void InteractiveWidget::Callback()
    {
        for (int i = 0; i < maxCallbacks; i++)
            {
                if (otherObjectCB[i] != 0)
                    otherObjectCB[i](otherObjectData[i]);
                if (combinedObjectCB[i] != 0)
                    combinedObjectCB[i](combinedObjectData[i], this);
            }
    }


    void InteractiveWidget::AddOtherObjectCallback(OtherObjectCB cbProc, void* otherObj)
    {
        for (int i = 0; i < maxCallbacks; i++)
            if (otherObjectCB[i] == 0)
                {
                    otherObjectCB[i] = cbProc;
                    otherObjectData[i] = otherObj;
                    return;
                }
        GenAppInternalError("InteractiveWidget::AddOtherObjectCallback");
    }

    void InteractiveWidget::AddCombinedObjectCallback(CombinedObjectCB cbProc, void* otherObj)
    {
        for (int i = 0; i < maxCallbacks; i++)
            if (combinedObjectCB[i] == 0)
                {
                    combinedObjectCB[i] = cbProc;
                    combinedObjectData[i] = otherObj;
                    return;
                }
        GenAppInternalError("InteractiveWidget::AddCombinedObjectCallback");
    }

    void InteractiveWidget::Delete()
    {
        BaseWindow::Delete();
        for (int i = 0; i < maxCallbacks; i++)
            {
                otherObjectCB[i] = 0;
                combinedObjectCB[i] = 0;
            }
    }


    void  InteractiveWidget::InputFieldError(const char   fieldDesc[],
                                             const char   errorMsg[])
    {
        char tempStr[255];
        CopyString(tempStr, fieldDesc, 255);
        ConcatString(tempStr, " " , 255);
        ConcatString(tempStr, errorMsg, 255);
        InputError(tempStr);
    }

    void  InteractiveWidget::InputError(const char   errorMsg[])
    {
        GenAppErrorMsg("Field", errorMsg);
    }



    /////////////////////////
    // TextEntryWidget

    BEGIN_MESSAGE_MAP(TextEntryWidget, CEdit)
    //{{AFX_MSG_MAP(TextEntryWidget)
    ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
    ON_WM_CHAR()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void TextEntryWidget::OnKillfocus()
    {
        Callback();
    }

    BOOL TextEntryWidget::PreTranslateMessage(MSG* pMsg)
    {
        if (pMsg->message == WM_KEYDOWN)
            {
                if (pMsg->wParam == VK_RETURN)
                    {
                        Callback();
                        return TRUE;    // DO NOT process further
                    }

                //  do focus out
                if (pMsg->wParam == VK_F12)
                    Callback();

                // continue processing
            }
        return CWnd::PreTranslateMessage(pMsg);
    }

    TextEntryWidget::TextEntryWidget() :
        InteractiveWidget()
    {
        textFieldLen = stdInputStrLen;
    }

    TextEntryWidget::~TextEntryWidget()
    {
    }

    CWnd* TextEntryWidget::GetWindow()
    {
        return this;
    }

    void TextEntryWidget::SetFieldString(const char inStr[])
    {
        if (IsCreated())
            CEdit::SetWindowText(inStr);
    }

    void TextEntryWidget::SetWindowLen(int nwindowChars)
    {
        windowWidth = nwindowChars * GetFontWidth();
    }

    void TextEntryWidget::Create(const CRect& rect)
    {
        CWnd* pWnd = GetParentWnd();
        UINT nID = GetID();
        TRACE( "TextEntryWidget %d %x\r\n", nID, pWnd );
        //CEdit::Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL, rect, pWnd, nID);
        VERIFY(CEdit::CreateEx(WS_EX_CLIENTEDGE,"EDIT", NULL,
                               WS_CHILD|WS_BORDER|WS_VISIBLE|ES_AUTOHSCROLL|WS_TABSTOP,
                               rect,
                               pWnd,
                               nID));
        CEdit::SetFont(GetMenuFont(), TRUE);
    }

    int TextEntryWidget::GetWindowWidth()
    {
        if (windowWidth > 0) {
            return windowWidth + GetWidthOffset();
        } else {
            if (!m_bWidthCalculated) {
                m_nCalculatedBorder = 2 * ::GetSystemMetrics(SM_CXEDGE); // edge
                m_nCalculatedBorder += 2 * ::GetSystemMetrics(SM_CXEDGE); // pace at the start and at the end

                m_bWidthCalculated = true;
            }
            return textFieldLen * GetFontWidth() + m_nCalculatedBorder + GetWidthOffset();
        }
    }

    void TextEntryWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CEdit::MoveWindow(rcOff);
        }
    }

    int TextEntryWidget::GetWindowHeight()
    {
        return BaseWindow::GetWindowHeight();
    }

    void TextEntryWidget::GetFieldString(char outStr[],
                                         int  maxLen)
    {
        // added to prevent problems reading empty into uninitialized string
        outStr[0] = '\0';

        // get data from field
        CString fieldStr;
        GetWindowText(fieldStr);
        if (!fieldStr.IsEmpty())
            CopyString(outStr, fieldStr, maxLen);
    }

    bool TextEntryWidget::FieldStringOK( bool&      prevWasOK,
                                         const char fieldDesc[],
                                         char       outStr[],
                                         int        maxLen)
    {
        if (!prevWasOK)
            return false;

        // get data from field
        CString fieldStr;
        GetWindowText(fieldStr);
        if ( fieldStr.IsEmpty())
            {
                InputFieldError(fieldDesc, "field is empty - data required");
                prevWasOK = false;
                return false;
            }

        CopyString(outStr, fieldStr, maxLen);
        return true;
    }

    bool TextEntryWidget::FieldStringOK( bool&      prevWasOK,
                                         const char fieldDesc[])
    {
        char dummy[40];
        return FieldStringOK(prevWasOK, fieldDesc, dummy, 40);
    }



    ///////////////////////////////////////////////////////
    // Real Entry Widget

    RealEntryWidget::RealEntryWidget() :
        TextEntryWidget()
    {
        tval = nullReal;
    }

    RealEntryWidget::~RealEntryWidget()
    {
    }

    void RealEntryWidget::Create(const CRect& rect)
    {
        TextEntryWidget::Create(rect);
        if (!RealIsNull(tval))
            {
                SetFieldValue(tval);
            }
    }

    void RealEntryWidget::SetFieldValue(const double& inVal)
    {
        char tempStr[40];
        format.RealToString(inVal, tempStr, 40);
        SetFieldString(tempStr);
        tval = inVal;
    }

    bool  RealEntryWidget::FieldValueOK( bool&      prevWasOK,
                                         const char fieldDesc[],
                                         bool    showMsgs)
    {
        if (!prevWasOK)
            return false;
        char tempStr[50];
        if (!FieldStringOK(prevWasOK, fieldDesc, tempStr, 50))
            return false;
        if (!IsValidReal(tempStr, tval))
            {
                if (showMsgs)
                    InputFieldError(fieldDesc, "is not a real number");
                prevWasOK = false;
                return false;
            }
        return true;
    }

    bool  RealEntryWidget::FieldValueOK( bool&      prevWasOK,
                                         const char fieldDesc[],
                                         const double& minVal,
                                         const double& maxVal,
                                         bool     showMsgs)
    {
        if (!FieldValueOK(prevWasOK, fieldDesc, showMsgs))
            return false;

        if ((tval > maxVal) || (tval < minVal))
            {
                if (showMsgs)
                    {
                        char valStr[stdInputStrLen];
                        char msgStr[60];
                        if ( tval > maxVal)
                            {
                                format.RealToString(maxVal, valStr, stdInputStrLen);
                                CopyString(msgStr, "must be < ", 60);
                            }
                        else
                            {
                                format.RealToString(minVal, valStr, stdInputStrLen);
                                CopyString(msgStr, "must be > ", 60);
                            }

                        ConcatString(msgStr, valStr, 60);

                        InputFieldError(fieldDesc, msgStr);
                    }

                prevWasOK = false;
                return false;
            }

        return true;
    }

    //////////////////////////////////////////////////////////
    // IntEntryWidget

    IntEntryWidget::IntEntryWidget() :
        TextEntryWidget()
    {
        tval = INT_MIN;
    }

    IntEntryWidget::~IntEntryWidget()
    {
    }

    void IntEntryWidget::Create(const CRect& rect)
    {
        TextEntryWidget::Create(rect);
        if (tval != INT_MIN) {
            SetFieldValue(tval);
        }
    }

    void IntEntryWidget::SetFieldValue(int inVal)
    {
        char tempStr[40];
        IntToString(inVal, tempStr, 40);
        SetFieldString(tempStr);
        tval = inVal;
    }

    bool  IntEntryWidget::FieldValueOK( bool&      prevWasOK,
                                        const char fieldDesc[],
                                        int        minVal,
                                        int        maxVal,
                                        bool     showMsgs)
    {
        if (!prevWasOK)
            return false;

        char tempStr[50];
        if (!FieldStringOK(prevWasOK, fieldDesc, tempStr, 50))
            return false;

        double tempV;
        if (!IsValidReal(tempStr, tempV))
            {
                if (showMsgs)
                    InputFieldError(fieldDesc, "is not a valid number");
                prevWasOK = false;
                return false;
            }


        if ((tempV > double(maxVal)) || (tempV < double(minVal)))
            {
                if (showMsgs)
                    {
                        char valStr[stdInputStrLen];
                        char msgStr[60];
                        if ( tempV > double(maxVal))
                            {
                                IntToString(maxVal, valStr, stdInputStrLen);
                                CopyString(msgStr, "must be < ", 60);
                            }
                        else
                            {
                                IntToString(minVal, valStr, stdInputStrLen);
                                CopyString(msgStr, "must be > ", 60);
                            }

                        ConcatString(msgStr, valStr, 60);
                        InputFieldError(fieldDesc, msgStr);
                    }
                prevWasOK = false;
                return false;
            }

        tval = int(tempV);
        return true;
    }

    ///////////////////////////////////////////////////////
    // FileEntryWidget

    FileEntryPushButton::FileEntryPushButton() :
        assocFileEntry(NULL)
    {
    }

    BEGIN_MESSAGE_MAP(FileEntryPushButton, CButton)
    //{{AFX_MSG_MAP(FileEntryPushButton)
    //}}AFX_MSG_MAP
    ON_CONTROL_REFLECT(BN_CLICKED, OnClick)
    END_MESSAGE_MAP()

    void FileEntryPushButton::OnClick()
    {
        assocFileEntry->BrowseForFiles();
    }

    FileEntryWidget::FileEntryWidget()
    {
        tfileStr[0] = '\0';
        folderEntry = false;
        buttonRightOffset  = 5;
        buttonBottomOffset = 5;
        browseButton.assocFileEntry = this;
        SetPromptAndMask("Select File", "All Files (*.*)|*.*||");
    }

    void FileEntryWidget::GetRects(const CRect& rect, CRect& rc1, CRect& rc2)
    {
        rc1 = rect;
        rc1.right -= 80;
        rc2 = rect;
        rc2.left = rc2.right - 75;
    }

    void FileEntryWidget::Create(const CRect& rect)
    {
        CWnd* pParent = GetParentWnd();
        int nID = GetID();
        CRect rc1, rc2;
        GetRects(rect, rc1, rc2);
        //CEdit::Create(ES_AUTOHSCROLL|WS_VISIBLE|WS_CHILD|WS_BORDER|WS_TABSTOP, rc1, pParent, nID);
        VERIFY(CEdit::CreateEx(WS_EX_CLIENTEDGE,"EDIT", NULL,
                               WS_CHILD|WS_BORDER|WS_VISIBLE|ES_AUTOHSCROLL|WS_TABSTOP,
                               rc1,
                               pParent,
                               nID));
        CEdit::SetFont(GetMenuFont(), TRUE);

        OrdinalDispatcher* pDispatcher = GetOrdinalDispatcher();
        int nButtonID = pDispatcher->GetNextID();
        browseButton.Create("Browse", WS_VISIBLE|WS_CHILD|WS_TABSTOP, rc2, pParent, nButtonID);
        browseButton.SetFont(GetMenuFont(), TRUE);
    }

    void FileEntryWidget::Delete()
    {
        if (IsCreated()) {
            browseButton.DestroyWindow();
        }
        InteractiveWidget::Delete();
    }

    int FileEntryWidget::GetWindowHeight()
    {
        return BaseWindow::GetWindowHeight();
    }

    void FileEntryWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CRect rc1, rc2;
            GetRects(rcOff, rc1, rc2);
            MoveWindow(rc1);
            browseButton.MoveWindow(rc2);
        }
    }

    void FileEntryWidget::SetPromptAndMask(char newPrompt[],
                                           char newMask[])
    {
        CopyString(browsePrompt, newPrompt, 40);
        CopyString(browseFileMask, newMask, 80);
    }

    void FileEntryWidget::BrowseForFiles()
    {
        //browseDialog.SetDialogStrings(browsePrompt, tfileStr, browseFileMask);
        CString sFileName = tfileStr;

        if (folderEntry)
            {
                NTCompatibleFolderDialog dlg;
                dlg.m_pOFN->lpstrTitle = browsePrompt;
                if (dlg.DoModal() == IDOK)
                    {
                        CopyString(tfileStr, dlg.m_pPath, stdFileStrLen);
                        SetWindowText(tfileStr);
                    }
            }
        else
            {
                NTCompatibleFileDialog dlg(TRUE, "", sFileName,
                                           OFN_HIDEREADONLY, browseFileMask);

                //              "All Files (*.*)|*.*||");
                //"MCV Files (*.mcv)|*.mcv|All Files (*.*)|*.*|||" );
                //ensure there are no spaces in the second string of each pair i.e. |*.mDAT| will work, |*.mDAT | will not
                dlg.m_ofn.lpstrTitle = browsePrompt;

                if (dlg.DoModal() == IDOK)
                    {
                        sFileName = dlg.GetPathName();
                        CopyString(tfileStr, sFileName, stdFileStrLen);
                        SetWindowText(tfileStr);
                    }
            }
    }

    bool FileEntryWidget::FieldFileOK(bool&      prevWasOK,
                                      const char fieldDesc[],
                                      bool       mustExist,
                                      bool       chkOverwrite)
    {
        if (! prevWasOK)
            return false;

        if (! FieldStringOK(prevWasOK, fieldDesc, tfileStr, stdFileStrLen))
            return false;

        //  checking code
        //  in Unix chkFile is 0 if the file doesn't exist
        //  not in VC++
        //    ifstream chkFile(tfileStr);


        bool fExists = FileExists(tfileStr);
        if (mustExist)
            {
                if (!fExists)
                    {
                        InputFieldError(fieldDesc, " - does not exist");
                        prevWasOK = false;
                        return false;
                    }
            }
        else
            if (chkOverwrite && fExists)
                {
                    char msgStr[200];
                    MakeString(msgStr, "File ", tfileStr, " already exists. \n Overwrite it ?", 200);
                    if (AfxMessageBox(msgStr, MB_YESNO) == IDNO )
                        {
                            prevWasOK = false;
                            return false;
                        }
                }


        return true;
    }

    bool FileEntryWidget::FieldPathOK(bool&      prevWasOK,
                                      const char fieldDesc[],
                                      bool       mustExist)
    {
        if (!prevWasOK)
            return false;

        if (! FieldStringOK(prevWasOK, fieldDesc, tfileStr, stdFileStrLen))
            return false;

        if (mustExist && (!PathExists(tfileStr)))
            {
                InputFieldError(fieldDesc, " - path does not exist");
                prevWasOK = false;
                return false;
            }

        return true;
    }


    void  FileEntryWidget::GetFieldFile(char outFileStr[])
    {
        CopyString(outFileStr, tfileStr, stdFileStrLen);
    }


    void FileEntryWidget::SetSensitive(bool isSensitive)
    {
        TextEntryWidget::SetSensitive(isSensitive);
        ASSERT(::IsWindow(browseButton.m_hWnd));
        browseButton.EnableWindow(isSensitive);
    }

    void FileEntryWidget::CloseDialog()
    {
    }

    //////////////////////////////////////////////////////////
    // LabelWidget

    LabelWidget::LabelWidget()
    {
    }

    CWnd* LabelWidget::GetWindow()
    {
        return this;
    }

    void LabelWidget::StdLabelInit(const char   labelText[],
                                   LayoutManager*     parent,
                                   const  ArgListC&  inArgs)
    {
        BaseWindow::InitWidget(parent, inArgs);
        LabelWidget::SetLabelText(labelText);
    }

    void LabelWidget::SetLabelText(const char newLab[])
    {
        labelText = newLab;
        if (IsCreated()) {
            SetWindowText(newLab);
        }
    }

    void LabelWidget::Create(const CRect& rect)
    {
        DWORD dwStyle = WS_CHILD|WS_VISIBLE;
        int nID = GetID();
        TRACE("LabelWidget %d %x\r\n", nID, GetParentWnd() );
        CStatic::Create(labelText, dwStyle, rect, GetParentWnd(), nID);
        CStatic::SetFont(GetMenuFont(), TRUE);
    }

    void LabelWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            MoveWindow(rcOff);
        }
    }

    int LabelWidget::GetWindowWidth()
    {
        if (windowWidth > 0) {
            return windowWidth + GetWidthOffset();
        } else {
            if (!m_bWidthCalculated) {
                CClientDC dc(AfxGetMainWnd());

                CFont* pOldFont = dc.SelectObject(GetMenuFont());
                m_nCalculatedWidth = 0;
                CSize sz = dc.GetTextExtent(labelText, StringLength(labelText));
                m_nCalculatedWidth = sz.cx;
                dc.SelectObject(pOldFont);

                m_nCalculatedBorder = 4;

                m_bWidthCalculated = true;
            }
            return m_nCalculatedWidth + m_nCalculatedBorder + GetWidthOffset();
        }
    }

    int LabelWidget::GetWindowHeight()
    {
        return GetFontHeight() + 2 + GetHeightOffset();
    }

    /////////////////////////
    // Toggle Widget
    ToggleBoxWidget::ToggleBoxWidget() :
        currValue(false)
    {
        toggleLabel[0] = 0;
    }

    CWnd* ToggleBoxWidget::GetWindow()
    {
        return this;
    }

    void ToggleBoxWidget::Create(const CRect& rect)
    {
        DWORD dwStyle = WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_TABSTOP;
        int nID = GetID();
        TRACE("ToggleBoxWidget %d\r\n", nID );
        CButton::Create(toggleLabel, dwStyle, rect, GetParentWnd(), nID);
        CButton::SetFont(GetMenuFont(), TRUE);
        SetValue(currValue);
    }

    void ToggleBoxWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            MoveWindow(rcOff);
        }
    }

    int ToggleBoxWidget::GetWindowWidth()
    {
        if (!m_bWidthCalculated) {

            CClientDC dc(AfxGetMainWnd());
            CFont* pOldFont = dc.SelectObject(GetMenuFont());
            m_nCalculatedWidth = 0;
            CSize sz = dc.GetTextExtent(toggleLabel, StringLength(toggleLabel));
            m_nCalculatedWidth = sz.cx;
            dc.SelectObject(pOldFont);

            m_bWidthCalculated = true;
        }
        return GetFontWidth()*2 + m_nCalculatedWidth + GetWidthOffset();
    }

    int ToggleBoxWidget::GetWindowHeight()
    {
        return BaseWindow::GetWindowHeight();
    }

    void ToggleBoxWidget::StdToggleInit(const char       togLabel[], // combines Pre and Init
                                        LayoutManager* parent,
                                        const  ArgListC&  inArgs)
    {
        PreInitSetUp(togLabel);
        InitWidget(parent, inArgs);
    }

    void ToggleBoxWidget::SetToggleText(const char togLabel[])
    {
        CopyString(toggleLabel, togLabel, 80);
        if (IsCreated()) {
            SetWindowText(toggleLabel);
        }
    }

    void ToggleBoxWidget::SetValue(bool inFlag)
    {
        currValue = inFlag;
        if (IsCreated()) {
            SetCheck(currValue ? 1 : 0);
        }
    }

    BEGIN_MESSAGE_MAP(ToggleBoxWidget, CButton)
    //{{AFX_MSG_MAP(ToggleBoxWidget)
    //}}AFX_MSG_MAP
    ON_CONTROL_REFLECT(BN_CLICKED, OnClick)
    END_MESSAGE_MAP()

    void ToggleBoxWidget::OnClick()
    {
        currValue = CButton::GetCheck() != FALSE;
        Callback();
    }

    //////////////////////////
    // DrawnButton

    DrawnButton::DrawnButton() :
        m_bSelected(false)
    {
    }

    CWnd* DrawnButton::GetWindow()
    {
        return this;
    }

    void DrawnButton::SelectButton(bool bSelect)
    {
        if (m_bSelected != bSelect) {
            m_bSelected = bSelect;
            if (IsCreated()){
                Invalidate();
                UpdateWindow();
            }
        }
    }

    void DrawnButton::Create(const CRect& rect)
    {
        DWORD dwStyle = WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW;
        int nID = GetID();
        TRACE("DrawnButton %d\r\n", nID );
        CWnd* pWnd = GetParentWnd();
        CButton::Create("", dwStyle, rect, pWnd, nID);
        CButton::SetFont(GetMenuFont(), TRUE);
    }

    int DrawnButton::GetWindowWidth()
    {
        return BaseWindow::GetWindowWidth();
    }

    int DrawnButton::GetWindowHeight()
    {
        return BaseWindow::GetWindowHeight() + 4;
    }

    void DrawnButton::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    void DrawnButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
        UINT uStyle = DFCS_BUTTONPUSH;

        ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

        // If drawing selected, add the pushed style to DrawFrameControl.
        if (lpDrawItemStruct->itemState & ODS_SELECTED || m_bSelected) {
            uStyle |= DFCS_PUSHED;
        }

        CDC dc;
        VERIFY(dc.Attach(lpDrawItemStruct->hDC));
        // Draw the button frame
        dc.DrawFrameControl(&lpDrawItemStruct->rcItem, DFC_BUTTON, uStyle);

        // Draw inside color
        CRect rect = lpDrawItemStruct->rcItem;
        rect.DeflateRect(2, 2);
        dc.FillSolidRect(rect, m_clrBackground);
        dc.Detach();
    }

    BEGIN_MESSAGE_MAP(DrawnButton, CButton)
    //{{AFX_MSG_MAP(DrawnButton)
    //}}AFX_MSG_MAP
    ON_CONTROL_REFLECT(BN_CLICKED, OnClick)
    END_MESSAGE_MAP()

    void DrawnButton::OnClick()
    {
        Callback();
    }

    //////////////////////////////////////////////
    // PushButton

    PushButton::PushButton() :
        label(NULL)
    {
    }

    CWnd* PushButton::GetWindow()
    {
        return this;
    }

    void PushButton::Create(const CRect& rect)
    {
        DWORD dwStyle = WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP;
        int nID = GetID();
        TRACE("PushButton %d\r\n", nID );
        if (label == NULL) {
            label = "Push";
        }
        CWnd* pWnd = GetParentWnd();
        CButton::Create(label, dwStyle, rect, pWnd, nID);
        CButton::SetFont(GetMenuFont(), TRUE);
    }

    int PushButton::GetWindowWidth()
    {
        return BaseWindow::GetWindowWidth();
    }

    int PushButton::GetWindowHeight()
    {
        return BaseWindow::GetWindowHeight() + 4;
    }

    void PushButton::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    void PushButton::StdButtonInit(char buttonLabel[],
                                   LayoutManager* parent,
                                   const ArgListC&  inArgs)
    {
        ArgListC args = inArgs;
        label = buttonLabel;
        InitWidget(parent, args);
    }

    void PushButton::SetButtonText(char newLab[])
    {
        SetWindowText(newLab);
    }

    BEGIN_MESSAGE_MAP(PushButton, CButton)
    //{{AFX_MSG_MAP(PushButton)
    //}}AFX_MSG_MAP
    ON_CONTROL_REFLECT(BN_CLICKED, OnClick)
    END_MESSAGE_MAP()

    void PushButton::OnClick()
    {
        Callback();
    }

    ////////////////////////////////////////////////////
    // SliderWidget

    SliderWidget::SliderWidget()
    {
        minVal      = 0;
        currValue   = 500;
        maxVal      = 1000;
        pageSize    = 0;

        realMin         = 0;
        realMax         = 1.0;
        showVal         = true;
        horizontal  = true;
    }

    void SliderWidget::Create(const CRect& rect)
    {
        DWORD dwStyle = TBS_NOTICKS|TBS_BOTH|WS_CHILD|WS_VISIBLE|WS_TABSTOP;
        int nID = GetID();
        CWnd* pWnd = GetParentWnd();
        CSliderCtrl::Create(dwStyle, rect, pWnd, nID);
        CSliderCtrl::SetFont(GetMenuFont(), TRUE);
        CSliderCtrl::SetRange(minVal, maxVal, TRUE);
        CSliderCtrl::SetPos(currValue);

        if (pageSize == 0)
            pageSize = (maxVal - minVal) / 10;
        CSliderCtrl::SetPageSize(pageSize);

        TRACE("SliderWidget %d\r\n", nID );
    }

    void SliderWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            MoveWindow(rcOff);
        }
    }

    CWnd* SliderWidget::GetWindow()
    {
        return this;
    }

    int SliderWidget::GetWindowHeight()
    {
        if (windowHeight > 0) {
            return BaseWindow::GetWindowHeight();
        } else {
            return GetHeightOffset() + GetFontHeight() + 4;
        }
    }

    BEGIN_MESSAGE_MAP(SliderWidget, CSliderCtrl)
    //{{AFX_MSG_MAP(SliderWidget)
    ON_WM_HSCROLL_REFLECT()
    ON_WM_HSCROLL()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void SliderWidget::HScroll(UINT nSBCode, UINT nPos)
    {
        currValue = GetPos();
        m_bThumbtrack = nSBCode == SB_THUMBTRACK;
        Callback();
    }

    void SliderWidget::SetValue(int newVal)
    {
        currValue = newVal;
        if (IsCreated())
            SetPos(currValue);
    }

    void SliderWidget::SetRealValue(double newVal)
    {
        double intVal = (newVal - realMin) / (realMax - realMin) * double(maxVal - minVal);
        SetValue(minVal + int(intVal));
    }

    double SliderWidget::GetRealValue()
    {
        double rval = double(currValue - minVal) / double(maxVal - minVal) * (realMax - realMin) + realMin;
        return rval;
    }

    ///////////////////////////////////////////////////
    // ChooserWidget

    ChooserWidget::ChooserWidget() :
        nchoice(0),
        currChoice(0)
    {
        choiceLabels.SetResizable(10);
    }

    ChooserWidget::~ChooserWidget()
    {
    }

    void ChooserWidget::PreInitSetUp(const char**   inLabels,
                                     int          nlabels)
    {
        choiceLabels.SetEmpty();
        for (int i = 0; i < nlabels; i++)
            choiceLabels += inLabels[i];
        nchoice = nlabels;
    }

    void ChooserWidget::PreInitSetUp(const char**  inLabels)
    {
        choiceLabels.SetEmpty();
        nchoice = 0;
        while (inLabels[nchoice] != NULL)
            {
                choiceLabels += inLabels[nchoice];
                nchoice++;
            }
    }

    void ChooserWidget::PreInitSetUp(const SC_ConstStringArray& inLabels)
    {
        choiceLabels = inLabels;
        nchoice = choiceLabels.Size();
    }



    void ChooserWidget::PreInitSetUp(const SC_StringArray& inLabs)
    {
        choiceLabels.SetEmpty();
        for (int i = 0; i < inLabs.Size(); i++)
            choiceLabels += inLabs[i];
        nchoice = inLabs.Size();
    }



    //////////////////////////////////////////////////
    // helper class RadioButton

    class RadioButton : public CButton
    {
        friend class OrientedChooserWidget;
    public:
        RadioButton(int value, OrientedChooserWidget* assignTo);

    protected:
        int value;
        OrientedChooserWidget* assignTo;

    protected:
        virtual BOOL PreTranslateMessage( MSG* pMsg );

        //{{AFX_MSG(RadioButton)
        afx_msg void OnClick();
        afx_msg void OnDestroy();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    RadioButton::RadioButton(int value, OrientedChooserWidget* assignTo)
    {
        RadioButton::value = value;
        RadioButton::assignTo = assignTo;
    }

    BEGIN_MESSAGE_MAP(RadioButton, CButton)
    //{{AFX_MSG_MAP(RadioButton)
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
    ON_CONTROL_REFLECT(BN_CLICKED, OnClick)
    END_MESSAGE_MAP()

    void RadioButton::OnClick()
    {
        assignTo->currChoice = value;
        assignTo->Callback();
    }

    void RadioButton::OnDestroy()
    {
        delete this;
    }

    BOOL RadioButton::PreTranslateMessage( MSG* pMsg )
    {
        if (pMsg->message == WM_KEYDOWN) {
            if (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_UP ) {
                int nID = GetDlgCtrlID();
                assignTo->PreviousValue(nID);
                return TRUE;
            } else if (pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DOWN) {
                int nID = GetDlgCtrlID();
                assignTo->NextValue(nID);
                return TRUE;
            }
        }
        return CButton::PreTranslateMessage(pMsg);
    }

    //////////////////////////////////////////
    //  OrientedChooserWidget

    OrientedChooserWidget::OrientedChooserWidget(DWORD dwStyle) :
        horizontal(true),
        m_dwStyleItem(dwStyle),
        m_nIDStart(-1)
    {
    }

    OrientedChooserWidget::~OrientedChooserWidget()
    {
    }

    void OrientedChooserWidget::PreInitSetUp(const char** inLabels,
                                             int          nlabels,
                                             bool         isHoriz)
    {
        ChooserWidget::PreInitSetUp(inLabels, nlabels);
        horizontal = isHoriz;
    }

    void OrientedChooserWidget::PreInitSetUp(const SC_StringArray& inLabels,
                                             bool           isHoriz)
    {
        ChooserWidget::PreInitSetUp(inLabels);
        horizontal = isHoriz;
    }


    bool OrientedChooserWidget::IsCreated()
    {
        return m_nIDStart != -1;
    }

    void OrientedChooserWidget::Create(const CRect& rect)
    {
        ASSERT(nchoice > 0); // PreInitSetUp not called

        // setup window IDs
        OrdinalDispatcher* pDispatcher = GetOrdinalDispatcher();
        m_nIDStart = pDispatcher->GetNextID();
        pDispatcher->SetInterval(nchoice);

        CWnd* pParent = GetParentWnd();
        SetupButtons(true, rect, pParent);

        TRACE("OrientedChooserWidget %d\r\n", m_nIDStart);
        SetChoice(0);
    }

    CWnd* OrientedChooserWidget::GetWindow()
    {
        return NULL; // complex visual object
    }

    void OrientedChooserWidget::DoSetSensitive(bool isSensitive)
    {
        CWnd* pParent = GetParentWnd();
        for (int i = 0; i < nchoice; i++) {
            int nID = GetItemID(i);
            CWnd* pWnd = pParent->GetDlgItem(nID);
            pWnd->EnableWindow(isSensitive);
        }
    }

    void OrientedChooserWidget::DoManage()
    {
        CWnd* pParent = GetParentWnd();
        for (int i = 0; i < nchoice; i++) {
            int nID = GetItemID(i);
            CWnd* pWnd = pParent->GetDlgItem(nID);
            pWnd->ShowWindow(SW_SHOW);
        }
    }

    void OrientedChooserWidget::DoUnmanage()
    {
        CWnd* pParent = GetParentWnd();
        for (int i = 0; i < nchoice; i++) {
            int nID = GetItemID(i);
            CWnd* pWnd = pParent->GetDlgItem(nID);
            pWnd->ShowWindow(SW_HIDE);
        }
    }

    int OrientedChooserWidget::GetItemID(int i)
    {
        return m_nIDStart + i;
    }

    void OrientedChooserWidget::Delete()
    {
        CWnd* pParent = GetParentWnd();
        for (int i = 0; i < nchoice; i++) {
            int nID = GetItemID(i);
            CWnd* pWnd = pParent->GetDlgItem(nID);
            if (pWnd != NULL) {
                pWnd->DestroyWindow();
            }
        }
        InteractiveWidget::Delete();
    }

    void OrientedChooserWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        CWnd* pParent = GetParentWnd();
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            SetupButtons(false, rcOff, pParent);
        }
    }

    void OrientedChooserWidget::CreateButton(int i, const CRect& rect, CWnd* pParent, int nID)
    {
        DWORD dwStyle = m_dwStyleItem;
        if (i == 0) {
            dwStyle |= WS_GROUP|WS_TABSTOP;
        }
        RadioButton* pRadioButton = new RadioButton(i, this); // self-destroy
        pRadioButton->Create(choiceLabels[i], dwStyle, rect, pParent, nID);
        pRadioButton->SetFont(GetMenuFont(), TRUE);
    }

    void OrientedChooserWidget::MoveButton(int i, const CRect& rect, CWnd* pParent, int nID)
    {
        CWnd* pRadioButton = pParent->GetDlgItem(nID);
        pRadioButton->MoveWindow(rect);
    }

    void OrientedChooserWidget::SetupButtons(bool bCreate, const CRect& rect, CWnd* pParent)
    {
        CRect rcButton = rect;
        if (horizontal) {
            int nSize = rect.Width()/nchoice;
            rcButton.right = rcButton.left + nSize;
            for (int i = 0; i < nchoice; i++) {
                int nID = GetItemID(i);
                if (bCreate) {
                    CreateButton(i, rcButton, pParent, nID);
                } else {
                    MoveButton(i, rcButton, pParent, nID);
                }
                rcButton.left = rcButton.right;
                rcButton.right = rcButton.left + nSize;
            }
        } else { // vertical
            int nSize = GetFontHeight() + 4;
            rcButton.bottom = rcButton.top + nSize;
            for (int i = 0; i < nchoice; i++) {
                int nID = GetItemID(i);
                if (bCreate) {
                    CreateButton(i, rcButton, pParent, nID);
                } else {
                    MoveButton(i, rcButton, pParent, nID);
                }
                rcButton.top = rcButton.bottom;
                rcButton.bottom = rcButton.top + nSize;
            }
        }
    }

    bool OrientedChooserWidget::SetChoice(int inChoice)
    {
        bool bChanged = false;
        int nID = GetID();
        CWnd* pParent = GetParentWnd();
        if (currChoice != inChoice) {
            int nIDItem = GetItemID(currChoice);
            CWnd* pWnd = pParent->GetDlgItem(nIDItem);
            ASSERT(pWnd != NULL);
            static_cast<RadioButton*>(pWnd)->SetCheck(0);
            bChanged = true;
        }

        currChoice = inChoice;
        int nIDItem = GetItemID(currChoice);
        CWnd* pWnd = pParent->GetDlgItem(nIDItem);
        ASSERT(pWnd != NULL);
        static_cast<RadioButton*>(pWnd)->SetCheck(1);
        return bChanged;
    }

    void OrientedChooserWidget::PreviousValue(int nButtonID)
    {
        if (m_nIDStart == nButtonID) {
            return;
        }
        int nChoice = nButtonID - m_nIDStart - 1;
        if (SetChoice(nChoice)) {
            int nIDItem = GetItemID(currChoice);
            CWnd* pParent = GetParentWnd();
            CWnd* pWnd = pParent->GetDlgItem(nIDItem);
            pWnd->SetFocus();
            Callback();
        }
    }

    void OrientedChooserWidget::NextValue(int nButtonID)
    {
        if (m_nIDStart+nchoice-1 == nButtonID) {
            return;
        }
        int nChoice = nButtonID - m_nIDStart + 1;
        if (SetChoice(nChoice)) {
            int nIDItem = GetItemID(currChoice);
            CWnd* pParent = GetParentWnd();
            CWnd* pWnd = pParent->GetDlgItem(nIDItem);
            pWnd->SetFocus();
            Callback();
        }
    }

    //////////////////////////////////////////
    //  RadioBoxChooserWidget

    RadioBoxChooserWidget::RadioBoxChooserWidget() :
        OrientedChooserWidget(BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE)
    {
    }

    int RadioBoxChooserWidget::GetWindowWidth()
    {
        if (horizontal) {
            if (windowWidth > 0) {
                return windowWidth + GetWidthOffset() + 12;
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
                m_nCalculatedBorder = ::GetSystemMetrics(SM_CXMENUSIZE) + 4;

                m_bWidthCalculated = true;
            }
            return nchoice*(m_nCalculatedWidth + m_nCalculatedBorder)+ GetWidthOffset();
        } else {
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
                m_nCalculatedBorder = ::GetSystemMetrics(SM_CXMENUSIZE) + 4;

                m_bWidthCalculated = true;
            }

            return m_nCalculatedWidth + m_nCalculatedBorder + GetWidthOffset();
        }
    }

    int RadioBoxChooserWidget::GetWindowHeight()
    {
        if (horizontal) {
            return GetFontHeight()+4 + GetHeightOffset();
        } else {
            return nchoice*(GetFontHeight()+4) + GetHeightOffset();
        }
    }

    //////////////////////////////////////////
    //  ButtonBoxChooserWidget

    ButtonBoxChooserWidget::ButtonBoxChooserWidget() :
        OrientedChooserWidget(BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE)
    {
    }

    int ButtonBoxChooserWidget::GetWindowWidth()
    {
        if (horizontal) {
            if (!m_bWidthCalculated) {
                m_nCalculatedBorder = 2 * ::GetSystemMetrics(SM_CXEDGE); // edge
                m_nCalculatedBorder += 6 * ::GetSystemMetrics(SM_CXEDGE); // pace at the start and at the end

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

                m_bWidthCalculated = true;
            }
            return m_nCalculatedBorder + nchoice * m_nCalculatedWidth + GetWidthOffset();
        } else {
            return 16*GetFontWidth() + GetWidthOffset();
        }
    }

    int ButtonBoxChooserWidget::GetWindowHeight()
    {
        if (horizontal) {
            return GetFontHeight() + 20 + GetHeightOffset();
        } else {
            return nchoice*(GetFontHeight()+2) + GetHeightOffset();
        }
    }

    //////////////////////////////////////////
    //  BoolRadioBoxChooserWidget

    void  BoolRadioBoxChooserWidget::PreInitSetUp(const char   falseDesc[],
                                                  const char   trueDesc[],
                                                  bool   isHoriz)
    {
        const char* choices[2];
        choices[0] = falseDesc;
        choices[1] = trueDesc;
        RadioBoxChooserWidget::PreInitSetUp(choices, 2, isHoriz);
    }

    // Drawing Area
    DrawingAreaWidget::DrawingAreaWidget()
    {
    }

    CWnd* DrawingAreaWidget::GetWindow()
    {
        return this;
    }

    void DrawingAreaWidget::Create(const CRect& rect)
    {
        int nID = GetID();
        CWnd* pParent = GetParentWnd();
        VERIFY(CWnd::Create(NULL, "DrawingArea", WS_CHILD|WS_VISIBLE|WS_BORDER, rect, pParent, nID));
        TRACE("DrawingAreaWidget %d\r\n", nID);
    }

    void DrawingAreaWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    // will be resizable ie. sizes doesn't matter
    int DrawingAreaWidget::GetWindowWidth()
    {
        return 0;
    }

    int DrawingAreaWidget::GetWindowHeight()
    {
        return 30;
    }

    BEGIN_MESSAGE_MAP(DrawingAreaWidget, CWnd)
    //{{AFX_MSG_MAP(DrawingAreaWidget)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void DrawingAreaWidget::OnPaint()
    {
        CPaintDC dc(this);
        OnDraw(&dc);
    }

    ///////////////////////////////////////////////////////
    // StatusWidget

    StatusWidget::StatusWidget() :
        InteractiveWidget()
    {
        fieldLen = stdInputStrLen;
    }

    StatusWidget::~StatusWidget()
    {
    }

    CWnd* StatusWidget::GetWindow()
    {
        return this;
    }

    void StatusWidget::SetFieldString(const char inStr[])
    {
        CEdit::SetWindowText(inStr);
    }

    void StatusWidget::SetBoolValue(bool statusVal,
                                    const char trueText[],
                                    const char falseText[])
    {
        if (statusVal)
            SetFieldString(trueText);
        else
            SetFieldString(falseText);
    }

    void StatusWidget::SetBoolValue(bool statusVal)
    {
        SetBoolValue(statusVal, "yes", "no");
    }

    void StatusWidget::Create(const CRect& rect)
    {
        CWnd* pWnd = GetParentWnd();
        UINT nID = GetID();
        TRACE( "StatusWidget %d %x\r\n", nID, pWnd );
        //CEdit::Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL, rect, pWnd, nID);
        VERIFY(CEdit::CreateEx(WS_EX_CLIENTEDGE,"EDIT", NULL,
                               WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_READONLY,
                               rect,
                               pWnd,
                               nID));
        CEdit::SetFont(GetMenuFont(), TRUE);
    }

    int StatusWidget::GetWindowWidth()
    {
        if (windowWidth > 0) {
            return windowWidth + GetWidthOffset();
        } else {
            return fieldLen * GetFontWidth() + GetWidthOffset();
        }
    }

    BEGIN_MESSAGE_MAP(StatusWidget, CEdit)
    //{{AFX_MSG_MAP(StatusWidget)
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void StatusWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CEdit::MoveWindow(rcOff);
        }
    }

    int StatusWidget::GetWindowHeight()
    {
        return BaseWindow::GetWindowHeight();
    }

    void RealStatusWidget::SetFieldValue(double inVal)
    {
        char tempStr[40];
        format.RealToString(inVal, tempStr, 40);
        SetFieldString(tempStr);
        tval = inVal;
    }

    int RealStatusWidget::GetWindowWidth()
    {
        if (windowWidth > 0) {
            return windowWidth + GetWidthOffset();
        } else {
            return fieldLen * GetFontWidthNumber() + GetWidthOffset();
        }
    }

    void IntStatusWidget::SetFieldValue(int inVal)
    {
        char tempStr[40];
        IntToString(inVal, tempStr, 40);
        SetFieldString(tempStr);
        tval = inVal;
    }

    int IntStatusWidget::GetWindowWidth()
    {
        if (windowWidth > 0) {
            return windowWidth + GetWidthOffset();
        } else {
            return fieldLen * GetFontWidthNumber() + GetWidthOffset();
        }
    }

    /////////////////////////////
    // List base widget

    void ListBaseWidget::SetListNA()
    {
        SC_StringArray tmpList;
        tmpList.Alloc(1);
        tmpList += "n/a";
        if (IsCreated()) {
            SetListText(tmpList);
        } else {
            ListBaseWidget::listData = tmpList;
        }
    }

    /////////////////////////////
    // OptionChooser

    OptionChooserWidget::OptionChooserWidget()
    {
    }

    void OptionChooserWidget::SetLabels(SC_StringArray& inLabs)
    {
        nchoice = inLabs.Size();
        CComboBox::ResetContent();
        for ( int i = 0; i < nchoice; i++)
            {
                CComboBox::AddString(inLabs[i]);
            }
        if (IsCreated()) {
            CComboBox::SetCurSel(currChoice);
        }
    }



    bool OptionChooserWidget::SetChoice(int inChoice)
    {
        bool bChange = currChoice != inChoice;
        currChoice = inChoice;
        if (IsCreated()) {
            CComboBox::SetCurSel(inChoice);
        }
        return bChange;
    }

    CWnd* OptionChooserWidget::GetWindow()
    {
        return this;
    }

    void OptionChooserWidget::Create(const CRect& rect)
    {
        int nID = GetID();
        CWnd* pParent = GetParentWnd();
        VERIFY(CComboBox::Create(WS_VISIBLE|WS_CHILD|WS_VSCROLL|
                                 CBS_DROPDOWNLIST|CBS_AUTOHSCROLL|CBS_HASSTRINGS|WS_TABSTOP,
                                 rect,
                                 pParent,
                                 nID));
        CComboBox::SetFont(GetMenuFont(), TRUE);

        if (nchoice > 0) {
            for (int i = 0; i < nchoice; i++) {
                CComboBox::AddString(choiceLabels[i]);
            }
            CComboBox::SetCurSel(currChoice);
        }
        TRACE("OptionChooserWidget %d\r\n", nID);
    }

    void OptionChooserWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        rcOff.bottom += (nchoice+1) * rcOff.Height(); // 'nchoice' rows
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    int OptionChooserWidget::GetWindowWidth()
    {
        if (windowWidth > 0) {
            return windowWidth + GetWidthOffset() + 12;
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
            m_nCalculatedBorder = 3 + ::GetSystemMetrics(SM_CXVSCROLL) + 3*::GetSystemMetrics(SM_CXEDGE);

            m_bWidthCalculated = true;
        }

        return m_nCalculatedWidth + m_nCalculatedBorder + GetWidthOffset();
    }

    int OptionChooserWidget::GetWindowHeight()
    {
        return BaseWindow::GetWindowHeight() + 2;
    }

    BEGIN_MESSAGE_MAP(OptionChooserWidget, CComboBox)
    //{{AFX_MSG_MAP(OptionChooserWidget)
    //}}AFX_MSG_MAP
    ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
    END_MESSAGE_MAP()

    void OptionChooserWidget::OnSelchange()
    {
        int nIndex = CComboBox::GetCurSel();
        currChoice = nIndex;
        Callback();
    }



    /////////////////////////////////////////////////////////////////////////////
    void BoolOptionChooserWidget::PreInitSetUp(const char   falseDesc[],
                                               const char   trueDesc[])
    {
        const char* choices[2];
        choices[0] = falseDesc;
        choices[1] = trueDesc;
        OptionChooserWidget::PreInitSetUp(choices, 2);
    }

    ///////////////////////////
    // ComboBoxSelectionWidget

    ComboBoxSelectionWidget::ComboBoxSelectionWidget() :
        currSelection(0)
    {
    }

    void ComboBoxSelectionWidget::ClearList()
    {
        ResetContent();
    }

    void ComboBoxSelectionWidget::SetListText(const SC_StringArray& listData)
    {
        ComboBoxSelectionWidget::listData = listData;
        if (IsCreated()) {
            ClearList();
            for (int i = 0; i < listData.Size(); i++) {
                AddString(listData[i]);
            }
            if (currSelection >= listData.Size())
                if (listData.IsEmpty())
                    currSelection = 0;
                else
                    currSelection = listData.Size() - 1;

            SetCurSel(currSelection);
        }
    }

    void ComboBoxSelectionWidget::SetListWidth(int newWidth)
    {
        listWidth = newWidth;
    }

    void ComboBoxSelectionWidget::SetListWidth(int minWidth, SC_StringArray& listData)
    {
        int newWidth = listData.GetMaxStringLen();
        if (newWidth < minWidth) {
            newWidth = minWidth;
        }
    }

    void ComboBoxSelectionWidget::SetSelection(int inSel)
    {
        currSelection = inSel;
        if (IsCreated()) {
            CTabbedComboBox::SetCurSel(inSel);
        }
    }

    CWnd* ComboBoxSelectionWidget::GetWindow()
    {
        return this;
    }

    void ComboBoxSelectionWidget::Create(const CRect& rect)
    {
        int nID = GetID();
        CWnd* pParent = GetParentWnd();
        VERIFY(CTabbedComboBox::Create(WS_VISIBLE|WS_CHILD|WS_VSCROLL|
                                       CBS_DROPDOWNLIST|CBS_AUTOHSCROLL|CBS_OWNERDRAWFIXED|CBS_HASSTRINGS|WS_TABSTOP,
                                       rect,
                                       pParent,
                                       nID ));
        SetFont(GetMenuFont(), TRUE);

        if (!listData.IsEmpty()) {
            for (int i = 0; i < listData.Size(); i++) {
                AddString(listData[i]);
            }
            SetCurSel(currSelection);
        }
        TRACE("ComboBoxSelectionWidget %d\r\n", nID);
    }

    void ComboBoxSelectionWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        int nRows = listData.Size() != 0 ? listData.Size() : 10; // '10' rows default
        rcOff.bottom += (nRows+1) * rcOff.Height();
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    int ComboBoxSelectionWidget::GetWindowWidth()
    {
        if (listWidth != 0) {
            return listWidth * fontWidthNumber + GetWidthOffset();
        } else {
            return BaseWindow::GetWindowWidth() + 12; // size of button
        }
    }

    int ComboBoxSelectionWidget::GetWindowHeight()
    {
        return BaseWindow::GetWindowHeight() + 2;
    }

    BEGIN_MESSAGE_MAP(ComboBoxSelectionWidget, CTabbedComboBox)
    //{{AFX_MSG_MAP(ComboBoxSelectionWidget)
    ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void ComboBoxSelectionWidget::OnSelchange()
    {
        int nIndex = CComboBox::GetCurSel();
        currSelection = nIndex;
        Callback();
    }

    void ComboBoxSelectionWidget::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
    {
        lpMIS->itemHeight = 2 + GetFontHeight() + 2;
    }

    ///////////////////////
    // ScrolledListWidget

    ScrolledListWidget::ScrolledListWidget() :
        isSingleSel(true),
        currSingleSelection(0)
    {
        currMultipleSelection.SetResizable();
    }

    void ScrolledListWidget::ClearList()
    {
        CListBox::ResetContent();
    }

    void ScrolledListWidget::SetListText(const SC_StringArray& listData)
    {
        ClearList();

        ScrolledListWidget::listData = listData;
        if (IsCreated())
            {
                SetRedraw(FALSE);
                int allocOK = InitStorage(listData.Size(), listData.GetStringLen() + 2);
                ASSERT(allocOK != LB_ERRSPACE);
                for (int i = 0; i < listData.Size(); i++) {
                    char* szItem = listData[i];
                    AddString(szItem);
                }
                SetRedraw(TRUE);
            }

        CTabbedListBox::RecalcWidth(); // set tabs positions and horizontal scroll bar

        int maxList = listData.UpperBound();
        if (currSingleSelection > maxList)
            currSingleSelection = maxList;

        if (!isSingleSel)
            {
                int newCount = 0;
                for ( int i = 0; i < currMultipleSelection.Size(); i++)
                    if (currMultipleSelection[i] <= maxList)
                        {
                            currMultipleSelection[newCount] = currMultipleSelection[i];
                            newCount++;
                        }

                if (newCount == 0)
                    {
                        currMultipleSelection.SetEmpty();
                        currMultipleSelection += 0;
                        newCount++;
                    }
                currMultipleSelection.SetSize(newCount);
            }
        UpdateSelections();

    }

    void ScrolledListWidget::SetSelectionPolicy(bool singleSel)
    {
        // only change if necessary (why doesn't C have a logical XOR)
        if ((isSingleSel && singleSel) || ((!isSingleSel) && (!singleSel)))
            return;

        isSingleSel = singleSel;

        CRect rc1, rc2;
        GetParentWnd()->GetWindowRect(rc1);
        GetWindowRect(rc2);

        CRect rect;
        GetClientRect(rect);
        rect.OffsetRect(rc2.left - rc1.left, rc2.top - rc1.top);
        DestroyWindow();
        Create(rect);

        SetListText(listData);
    }

    void ScrolledListWidget::SetSingleSelection(int inSel)
    {
        currSingleSelection = inSel;
        if (isSingleSel)
            UpdateSelections();
    }

    void ScrolledListWidget::SetMultipleSelection(const SC_IntArray& inSel)
    {
        currMultipleSelection = inSel;
        currMultipleSelection.SetResizable();   // to avoid overflow errors
        if (!isSingleSel)
            UpdateSelections();
    }

    void ScrolledListWidget::GetMultipleSelection(SC_IntArray& inSel)
    {
        inSel = currMultipleSelection;
    }

    void ScrolledListWidget::ClearSelections()
    {
        if (isSingleSel)
            {
                CListBox::SetCurSel(-1);
            }
        else
            {
                int nItem = CListBox::GetCount();
                if (nItem > 0)
                    {
                        CListBox::SelItemRange(FALSE, 0, nItem - 1);
                        if (IsCreated()){
                            Invalidate();
                            UpdateWindow();
                        }
                    }
            }

    }

    void ScrolledListWidget::ClearAll()
    {
        if (!isSingleSel)
            {
                ClearSelections();
                currMultipleSelection.SetEmpty();
            }

    }

    void ScrolledListWidget::SelectAll()
    {
        if (!isSingleSel)
            {
                int nItem = CListBox::GetCount();
                if (nItem > 0)
                    {
                        currMultipleSelection.AllocAndIndex(nItem);
                        CListBox::SelItemRange(TRUE, 0, nItem  - 1);
                        if (IsCreated())
                            {
                                Invalidate();
                                UpdateWindow();
                            }
                    }
            }

    }

    void ScrolledListWidget::SetSelectionVisible()
    {
        if (!isSingleSel)
            return;

        int topPos = CListBox::GetTopIndex();
        CRect rect;
        GetClientRect(rect);
        int nVisible = rect.Height()/CListBox::GetItemHeight(0);

        int currPos = currSingleSelection;
        int bottomPos = topPos + nVisible - 1;

        if ((currPos < topPos) || (currPos > bottomPos))
            CListBox::SetTopIndex(currPos);
    }

    void ScrolledListWidget::SetSelections()
    {
        if (isSingleSel) {
            currSingleSelection = CListBox::GetCurSel();
        }
        else
            {
                int nCount = CListBox::GetSelCount();
                currMultipleSelection.AllocAndSetSize(nCount);
                CListBox::GetSelItems(nCount, currMultipleSelection.tListData);
            }
    }

    CWnd* ScrolledListWidget::GetWindow()
    {
        return this;
    }

    void ScrolledListWidget::Create(const CRect& rect)
    {
        UINT nID = GetID();
        CWnd* pParent = GetParentWnd();

        DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|
            LBS_OWNERDRAWFIXED|LBS_HASSTRINGS|
            LBS_NOTIFY|LBS_NOINTEGRALHEIGHT|WS_TABSTOP;
        if (!isSingleSel) {
            dwStyle |= LBS_MULTIPLESEL;
        }
        VERIFY(CTabbedListBox::CreateEx(WS_EX_CLIENTEDGE,_T("LISTBOX"), NULL,
                                        dwStyle, rect, pParent, nID));
        CListBox::SetFont(GetMenuFont(), TRUE);
    }

    void ScrolledListWidget::SetWindowPosition(const CRect& rect, int level )
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CWnd::MoveWindow(rcOff);
        }
    }

    int ScrolledListWidget::GetWindowWidth()
    {
        if (listWidth != 0) {
            return listWidth * GetFontWidth() + GetWidthOffset();
        } else {
            return BaseWindow::GetWindowWidth(); // size of button
        }
    }

    int ScrolledListWidget::GetWindowHeight()
    {
        return GetFontHeight() * 10;
    }

    void ScrolledListWidget::UpdateSelections()
    {
        if (isSingleSel) {
            SetCurSel(currSingleSelection);
        }
        else
            {
                SetRedraw(FALSE);

                ClearSelections();
                int nItem = CListBox::GetSelCount();
                if (currMultipleSelection.Size() == nItem)
                    {
                        CListBox::SelItemRange(TRUE, 0, nItem  - 1);
                    }
                else
                    {
                        for (int i = 0; i < currMultipleSelection.Size(); i++) {
                            CListBox::SetSel(currMultipleSelection[i], TRUE);
                        }
                    }
                SetRedraw(TRUE);
                Invalidate();
                UpdateWindow();
            }
    }

    BEGIN_MESSAGE_MAP(ScrolledListWidget, CTabbedListBox)
    //{{AFX_MSG_MAP(ScrolledListWidget)
    //}}AFX_MSG_MAP
    ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
    END_MESSAGE_MAP()

    void ScrolledListWidget::OnSelchange()
    {
        SetSelections();
        Callback();
    }

    void ScrolledListWidget::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
    {
        lpMIS->itemHeight = 2 + GetFontHeight() + 2;
    }


    ScrolledTextWidget::ScrolledTextWidget() :
        m_nRows(20),
        m_nColumns(10)
    {
    }

    ScrolledTextWidget::~ScrolledTextWidget()
    {
    }

    BEGIN_MESSAGE_MAP(ScrolledTextWidget, CRichEditCtrl)
    //{{AFX_MSG_MAP(ScrolledTextWidget)
    ON_WM_GETDLGCODE()
    ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
    ON_WM_CHAR()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    UINT ScrolledTextWidget::OnGetDlgCode()
    {
        return DLGC_WANTARROWS|DLGC_WANTALLKEYS|DLGC_WANTCHARS|DLGC_WANTTAB;
    }

    void ScrolledTextWidget::SetRowCol(int nRows, int nCols)
    {
        m_nRows = nRows;
        m_nColumns = nCols;
    }


    void ScrolledTextWidget::OnKillfocus()
    {
        Callback();
    }

    BOOL ScrolledTextWidget::PreTranslateMessage(MSG* pMsg)
    {
        if (pMsg->message == WM_KEYDOWN)
            {
                if (pMsg->wParam == VK_RETURN)
                    {
                        ReplaceSel("\n", true);
                        return TRUE;    // DO NOT process further
                    }


                if (pMsg->wParam == VK_TAB)
                    {
                        ReplaceSel("\t", true);
                        return TRUE;
                    }
                //  do focus out
                if (pMsg->wParam == VK_F12)
                    Callback();

                // continue processing
            }
        return CWnd::PreTranslateMessage(pMsg);
    }


    void ScrolledTextWidget::SetText(const SC_StringArray& inStrs)
    {
        CString newText;
        newText.Empty();
        for (int i = 0; i < inStrs.Size(); i++ )
            {
                newText += inStrs[i];
                if (i < inStrs.UpperBound())
                    newText += "\n";
            }
        SetWindowText(newText);
    }

    void ScrolledTextWidget::GetText(SC_StringArray& outStrs)
    {
        CString newText;
        GetWindowText(newText);
        outStrs.CreateFromString(newText, "\n");
    }

    CWnd* ScrolledTextWidget::GetWindow()
    {
        return this;
    }

    void ScrolledTextWidget::Create(const CRect& rect)
    {
        CWnd* pWnd = GetParentWnd();
        UINT nID = GetID();
        TRACE( "ScrolledTextWidget %d %x\r\n", nID, pWnd );

        //  CEdit::Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE, rect, pWnd, nID);

        VERIFY(CRichEditCtrl::Create(
                                     WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
                                     rect,
                                     pWnd,
                                     nID));

        CRichEditCtrl::SetFont(&menuFont, TRUE);
    }

    int ScrolledTextWidget::GetWindowWidth()
    {
        return BaseWindow::fontWidth * m_nColumns;
    }

    int ScrolledTextWidget::GetWindowHeight()
    {
        return BaseWindow::fontHeight * m_nRows;
    }

    void ScrolledTextWidget::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            MoveWindow(rcOff);
        }
    }


    ///////////////////////////////////////////////////////
    // HorizontalSeparator

    HorizontalSeparator::HorizontalSeparator() :
        InteractiveWidget()
    {
    }

    HorizontalSeparator::~HorizontalSeparator()
    {
    }

    CWnd* HorizontalSeparator::GetWindow()
    {
        return this;
    }

    void HorizontalSeparator::Create(const CRect& rect)
    {
        CWnd* pWnd = GetParentWnd();
        VERIFY(CStatic::Create(NULL, WS_CHILD|WS_VISIBLE, rect, pWnd));
    }

    void HorizontalSeparator::SetWindowPosition(const CRect& rect, int level)
    {
        CRect rcOff = AdjustOffsets(rect);
        if (!IsCreated()) {
            Create(rcOff);
        } else {
            CStatic::MoveWindow(rcOff);
        }
    }

    int HorizontalSeparator::GetWindowHeight()
    {
        return GetHeightOffset() + 3;
    }

    int HorizontalSeparator::GetWindowWidth()
    {
        return BaseWindow::GetWindowWidth();
    }

    BEGIN_MESSAGE_MAP(HorizontalSeparator, CStatic)
    //{{AFX_MSG_MAP(HorizontalSeparator)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void HorizontalSeparator::OnPaint()
    {
        CPaintDC dc(this); // device context for painting

        CRect r;
        GetClientRect(&r);
        r.bottom /= 2;
        r.top = r.bottom - 1;
        r.bottom += 1;

        bool bSunken = true;
        DWORD clrHigh = ::GetSysColor(!bSunken ? COLOR_3DHIGHLIGHT : COLOR_3DSHADOW);
        DWORD clrLow = ::GetSysColor(bSunken ? COLOR_3DHIGHLIGHT : COLOR_3DSHADOW);
        dc.Draw3dRect(&r, clrHigh, clrLow);
    }


} // namespace

