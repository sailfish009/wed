#include "stdafx.h"
#include "wedView.h"
#include "MainFrm.h"

static int line_n = 0;
static int char_w = 0;
static long char_x;
static long char_y;
static POINT p = { 0 };                                                                      //current position

static BOOL line_changed = 0;
static BOOL wed_mode = 0;                                                        // 0: edit mode,  1: save mode

CH ch = { 0 };
std::list<CH> line;
std::list <std::list<CH>> line_array;

LRESULT CWedView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  m_hdc = GetDC();
  GetTextMetrics(m_hdc, &m_tm);
  ReleaseDC(m_hdc);
  char_x = m_tm.tmAveCharWidth;
  char_y = m_tm.tmHeight;
  CreateCaret((HBITMAP)1);
  return 0;
}

LRESULT CWedView::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  //printf("0x%02x\n", wParam);
  switch (wParam)
  {
    // backspace
  case 0x08:   
  {
    if (p.x < 1) return 1;

    if ( (line_array.size() == 0)  || (line_array.size() < (size_t)(p.y +1) ) ) line_array.push_back(line);
    std::list <std::list<CH>>::iterator it = std::next(line_array.begin(), p.y);
    if (line_changed) { line_changed = 0; it->swap(line); }

    int count = 0;
    std::list<CH>::iterator line_a = it->begin();
    std::list<CH>::iterator line_b = it->begin();
    int e_n = it->size() -1;
    for (auto i = it->begin(); i != it->end(); i++)
    {
      if (count >= e_n) { CH ch = (*i); p.x -= ch.w;  RECT rect = { ch.x, ch.y*char_y, ch.x + ch.w, ch.y*char_y + char_y };  InvalidateRect(&rect);  }
      ++count;
    }
    if (p.x > 0) { std::advance(line_a, e_n); std::advance(line_b, e_n + 1); it->erase(line_a, line_b); line = (*it); }
    else { line.clear(); line_array.erase(it); p.x = 0; }
  }  
  break;

  // enter
  case 0x0D:   
    p.x = 0;  p.y += 1;
    if (p.y > line_n) { line_array.push_back(line); line_n = p.y; line.clear(); }
    break;

  // ESC
  case 0x1B:   
  { 
    wed_mode = !wed_mode;
    switch (wed_mode)
    {
    // file read     // hide_console(); 
    case 0:   break;       
    // file write    // show_console(); 
    default:  
    {
      if ((line_array.size() == 0) || (line_array.size() < (size_t)(p.y + 1))) line_array.push_back(line);
      std::list <std::list<CH>>::iterator it = std::next(line_array.begin(), p.y);
      if (line_changed) { line_changed = 0; it->swap(line); }
      std::string str = "sample.txt";  file_work(0, str, &line_array); 
    }  
      break;    
    }

  }  
  break;

  default:  
  {
    m_hdc = GetDC();
    GetCharWidth32(m_hdc, (UINT)wParam, (UINT)wParam, &char_w);
    ReleaseDC(m_hdc);
    ch.x = p.x, ch.y = p.y, ch.c = wParam, ch.w = char_w;
    CClientDC pDC(m_hWnd);
    pDC.TextOut(p.x, p.y*char_y, (LPCTSTR)&wParam); 
    line.push_back(ch);
    line_changed = 1;
    p.x += char_w;
  }
    break;
  }
  return 0;
}

LRESULT CWedView::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  switch (wParam)
  {
  case VK_LEFT:       
  {
    if (p.x == 0)  return 1;
    int char_w = 0;
    for (auto i = line.begin(); i != line.end(); i++) if (p.x == (*i).x) { char_w = (*i).w; break; }
    p.x -= char_w;
  }
    break;

  case VK_RIGHT:    
  {
    int char_w = 0;
    for (auto i = line.begin(); i != line.end(); i++) if (p.x == (*i).x) { char_w = (*i).w; break; }
    p.x += char_w;
  }
    break;

  case VK_UP:         { if (p.y == 0) return 1;    p.y -= 1; }           break;
  case VK_DOWN:  { if (p.y == line_n) return 1;  p.y += 1; }  break;
  }
  return 0;
}

LRESULT CWedView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CPaintDC dc(m_hWnd);
  return 0;
}

LRESULT CWedView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  return 0;
}
