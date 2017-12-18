#include "stdafx.h"
#include "wedView.h"
#include <string>

long g_posx = 0;

LRESULT CWedView::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  //printf("[wParam: 0x%02x]\n", wParam);
  g_posx += 10;
  POINT p = { g_posx,10 };
  print((UINT8&)wParam, p);
  return 0;
}

LRESULT CWedView::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CPaintDC dc(m_hWnd);
  return 0;
}

void CWedView::print(UINT8 &w, POINT &p)
{
  std::wstring str{ w };
  CClientDC pDC(m_hWnd);
  pDC.TextOut(p.x, p.y, str.c_str());
}