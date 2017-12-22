#include "stdafx.h"
#include "wedView.h"
#include "MainFrm.h"

CH ch = { 0 };
std::list <std::list<CH>> line_array;

POINT CWedView::p = { 0 };                                                          // current position
int CWedView::line_n = 0;
int CWedView::char_w = 0;
BOOL CWedView::line_changed = 0;
BOOL CWedView::wed_mode = 0;                                             // 0: edit mode,  1: save mode
HFONT CWedView::m_font;

LRESULT CWedView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  HBRUSH brush  = CreateSolidBrush(BACKGROUND);
  SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG)brush);

  m_font = CreateFont(18, 0, 0, 0, FW_LIGHT, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,  DEFAULT_QUALITY, DEFAULT_PITCH, 
    L"Dejavu Sans Mono");
    //L"Source Sans Pro");

  if (m_font) SetFont(m_font);

  CClientDC pDC(m_hWnd);
  pDC.SelectFont(m_font);
  pDC.GetTextMetrics(&m_tm);
  char_x = m_tm.tmAveCharWidth;
  char_y = m_tm.tmHeight;

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
    HideCaret();

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
    SetCaretPos(p.x, p.y*char_y);
    ShowCaret();
  }  
  break;

  // enter
  case 0x0D:   
    HideCaret();
    p.x = 0;  p.y += 1;
    if (p.y > line_n) { line_array.push_back(line); line_n = p.y; line.clear(); }
    else 
    { 
      std::list<CH> nline;  
      size_t size = line_array.size();
      for (size_t j = 0; j < size; ++j) 
      {
        if (j >= (size_t)p.y) 
        { 
          std::list<std::list<CH>>::iterator it = std::next(line_array.begin(), j);
          for (auto i = it->begin(); i != it->end(); i++) { CH ch = (*i); p.x += ch.w;  RECT rect = { ch.x, ch.y*char_y, ch.x + ch.w, ch.y*char_y + char_y };  InvalidateRect(&rect); }
        }
      }
      std::list <std::list<CH>>::iterator it = std::next(line_array.begin(), p.y);
      line_array.insert(it, nline);

      size = line_array.size();
      for (size_t j = 0; j < size; ++j)
      {
        if (j > (size_t)p.y)
        {
          p.x = 0;
          for (auto i = it->begin(); i != it->end(); i++) 
          { 
            CH ch = (*i); 
            CClientDC pDC(m_hWnd);
            pDC.SelectFont(m_font);
            pDC.SetTextColor(FONTCOLOR);
            pDC.SetBkColor(BACKGROUND);
            ch.x = p.x, ch.y = p.y, ch.c = wParam, ch.w = char_w;
            pDC.TextOut(ch.x, ch.y*char_y, (LPCTSTR)&ch.c);
            line.push_back(ch);
            line_changed = 1;
            p.x += ch.w;
          }
        }
      }
      p.x = 0;
    }
    SetCaretPos(p.x, p.y*char_y);
    ShowCaret();
    break;

  // ESC
  case 0x1B:   
  { 
    wed_mode = !wed_mode;

    if ((line_array.size() == 0) || (line_array.size() < (size_t)(p.y + 1))) line_array.push_back(line);
    std::list <std::list<CH>>::iterator it = std::next(line_array.begin(), p.y);
    if (line_changed) { line_changed = 0; it->swap(line); }

    switch (wed_mode)
    {
    case 0:  { std::string str = "sample.txt"; file_work(1, str, &line_array); }  break;       // file read     // hide_console(); 
    default: { std::string str = "sample.txt";  file_work(0, str, &line_array); } break;    // file write    // show_console(); 
    }
  }  
  break;

  default:  
  {
    HideCaret();
    CClientDC pDC(m_hWnd);
    pDC.SelectFont(m_font);
    pDC.GetCharWidth32((UINT)wParam, (UINT)wParam, &char_w);
    pDC.SetTextColor(FONTCOLOR);
    pDC.SetBkColor(BACKGROUND);
    ch.x = p.x, ch.y = p.y, ch.c = wParam, ch.w = char_w;
    pDC.TextOut(p.x, p.y*char_y, (LPCTSTR)&wParam); 
    line.push_back(ch);
    line_changed = 1;
    p.x += char_w;
    SetCaretPos(p.x, p.y*char_y);
    ShowCaret();
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
    HideCaret(); 
    int char_w = 0;
    for (auto i = line.begin(); i != line.end(); i++) if (p.x == (*i).x) { char_w = (*i).w; break; }
    p.x -= char_w;
    SetCaretPos(p.x, p.y*char_y); 
    ShowCaret(); 
  }
    break;

  case VK_RIGHT:    
  {
    HideCaret(); 
    int char_w = 0;
    for (auto i = line.begin(); i != line.end(); i++) if (p.x == (*i).x) { char_w = (*i).w; break; }
    p.x += char_w;
    SetCaretPos(p.x, p.y*char_y); 
    ShowCaret(); 
  }
    break;

  case VK_UP:         
  { 
    if (p.y == 0)           return 1; 
    HideCaret();  
    if ( line_array.size() < (size_t)(p.y + 1)) line_array.push_back(line);
    std::list <std::list<CH>>::iterator it = std::next(line_array.begin(), p.y);
    if (line_changed) { line_changed = 0; it->swap(line); }
    p.y -= 1;  
    SetCaretPos(p.x, p.y*char_y); 
    ShowCaret(); 
  }  
  break;

  case VK_DOWN:  
  { 
    if (p.y == line_n) return 1; 
    HideCaret();  
    if ( line_array.size() < (size_t)(p.y + 1)) line_array.push_back(line);
    std::list <std::list<CH>>::iterator it = std::next(line_array.begin(), p.y);
    if (line_changed) { line_changed = 0; it->swap(line); }
    p.y += 1; 
    SetCaretPos(p.x, p.y*char_y); 
    ShowCaret(); 
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

LRESULT CWedView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  return 0;
}

LRESULT CWedView::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  ::CreateCaret(m_hWnd, (HBITMAP)0, 1, char_y);
  SetCaretPos(p.x, p.y*char_y);
  ShowCaret();
  return 0;
}
