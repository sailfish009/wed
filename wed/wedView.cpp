#include "stdafx.h"
#include "wedView.h"
#include <string>

static POINT p = { 0 };
static int char_w = 0;
static long char_x;
static long char_y;
static long array_c = 0;
static int array_w[1024] = { 0 };


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
    if (array_c < 0) return 1;
    long tmp_char_w = array_w[--array_c];
    p.x -= tmp_char_w;
    RECT rect = { p.x, p.y, p.x + tmp_char_w, p.y + char_y }; 
    InvalidateRect(&rect); 
  }  
  break;

  default:  
  {
    if (array_c < 0) array_c = 0;
    m_hdc = GetDC();
    GetCharWidth32(m_hdc, (UINT)wParam, (UINT)wParam, &char_w);
    CClientDC pDC(m_hWnd);
    pDC.TextOut(p.x, p.y, (LPCTSTR)&wParam);
    ReleaseDC(m_hdc);
    p.x += array_w[array_c++] = char_w;
  }
    break;
  }
  printf("%d\n", array_c);
  return 0;
}

LRESULT CWedView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CPaintDC dc(m_hWnd);
  return 0;
}



