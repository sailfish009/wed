#include "stdafx.h"
#include "MainFrm.h"
#include <string>

void CMainFrame::write()
{
}

void CMainFrame::SetStatusText(std::wstring strText)
{
  ::SetWindowText(m_hWndStatusBar, strText.c_str());
}
