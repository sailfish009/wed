#include "stdafx.h"
#include "wedView.h"
#include <string>

static POINT p = { 0 };
static int char_w = 0;
static DWORD char_x;
static DWORD char_y;


LRESULT CWedView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  m_hdc = GetDC();
  GetTextMetrics(m_hdc, &m_tm);
  ReleaseDC(m_hdc);
  char_x = m_tm.tmAveCharWidth;
  char_y = m_tm.tmHeight;

  printf("x: %d  y: %d\n", char_x, char_y);
  return 0;
}

LRESULT CWedView::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  switch (wParam)
  {
  case 8:   
  { 
    p.x -= char_w; 
    RECT rect = { p.x, p.y, p.x + char_x, p.y + char_y }; 
    InvalidateRect(&rect); 
  }  
  break;

  default:  
  {
    m_hdc = GetDC();
    GetCharWidth32(m_hdc, (UINT)wParam, (UINT)wParam, &char_w);
    CClientDC pDC(m_hWnd);
    pDC.TextOut(p.x, p.y, (LPCTSTR)&wParam);
    ReleaseDC(m_hdc);
    p.x += char_w;
  }
    break;
  }
  return 0;
}

LRESULT CWedView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CPaintDC dc(m_hWnd);
  return 0;
}



