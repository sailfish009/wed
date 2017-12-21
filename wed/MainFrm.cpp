#include "stdafx.h"
#include "MainFrm.h"
#include <string>

 CMainFrame *CMainFrame::this_ptr = nullptr;

void CMainFrame::write()
{
}

void CMainFrame::SetStatusBar(std::wstring str)
{
  ::SetWindowText(m_hWndStatusBar, str.c_str());
}
