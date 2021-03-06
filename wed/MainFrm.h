//========================================================================
// wed - simple win32 text editor written in c++ (WTL) 
//------------------------------------------------------------------------
// Copyright (c) 2017-2018 Ji Wong Park <sailfish009@gmail.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

// WTL cpp
///////////////////////////////////////////////////////////////////////////////
#include <atlframe.h>
#include <atlsplit.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include "resource.h"
#include "AboutDlg.h"
#include "wedView.h"
///////////////////////////////////////////////////////////////////////////////

#define SHOW_MENU   0

#include <string>

class CMainFrame : 
public CFrameWindowImpl<CMainFrame>, 
public CUpdateUI<CMainFrame>,
public CMessageFilter, public CIdleHandler
{
public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  static CMainFrame *this_ptr;
  CWedView m_view;
  CCommandBarCtrl m_CmdBar;

  virtual BOOL PreTranslateMessage(MSG* pMsg)
  {
    if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
    return TRUE;

    return m_view.PreTranslateMessage(pMsg);
  }

  virtual BOOL OnIdle()
  {
    UIUpdateToolBar();
    return FALSE;
  }

  BEGIN_UPDATE_UI_MAP(CMainFrame)
  UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
  UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
  END_UPDATE_UI_MAP()

  BEGIN_MSG_MAP(CMainFrame)
  MESSAGE_HANDLER(WM_CREATE, OnCreate)
  MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
  COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
  COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
  COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
  COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
  COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
  CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
  CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
  END_MSG_MAP()

  // Handler prototypes (uncomment arguments if needed):
  //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
  //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

  void SetStatusBar(std::wstring strText);

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
  {
    // create command bar window
    #if SHOW_MENU
    HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
    // attach menu
    m_CmdBar.AttachMenu(GetMenu());
    // load command bar images
    m_CmdBar.LoadImages(IDR_MAINFRAME);
    #endif
    // remove old menu
    SetMenu(NULL);

    // disable window resize
    ModifyStyle(WS_THICKFRAME, 0);
    ModifyStyle(WS_MAXIMIZEBOX, 0);

    #if SHOW_MENU
    HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

    CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
    AddSimpleReBarBand(hWndCmdBar);
    AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
    #endif

    CreateSimpleStatusBar();
    m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

    #if SHOW_MENU
    UIAddToolBar(hWndToolBar);
    #endif
    UISetCheck(ID_VIEW_TOOLBAR, 1);
    UISetCheck(ID_VIEW_STATUS_BAR, 1);

    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    // IMPLEMENTATION
    MoveWindow(0, 0, 640, 480);
    CenterWindow();
    this_ptr = this;

    return 0;
  }

  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
  {
    // unregister message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);

    bHandled = FALSE;
    return 1;
  }

  LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  {
    PostMessage(WM_CLOSE);
    return 0;
  }

  LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  {
    // TODO: add code to initialize document
    return 0;
  }

  LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  {
    static BOOL bVisible = TRUE;	// initially visible
    bVisible = !bVisible;
    CReBarCtrl rebar = m_hWndToolBar;
    int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
    rebar.ShowBand(nBandIndex, bVisible);
    UISetCheck(ID_VIEW_TOOLBAR, bVisible);
    UpdateLayout();
    return 0;
  }

  LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  {
    BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
    ::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
    UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
    UpdateLayout();
    return 0;
  }

  LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  {
    CAboutDlg dlg;
    dlg.DoModal();
    return 0;
  }
};
