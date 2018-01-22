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

#include "stdafx.h"
#include "wedView.h"
#include "MainFrm.h"

cla LA; // Line Array

POINT CWedView::p = { 0 }; // current position
int CWedView::line_n = 0;
int CWedView::char_w = 0;
BOOL CWedView::line_changed = 0;
BOOL CWedView::wed_mode = 1; // 0: edit mode,  1: save mode
HFONT CWedView::m_font=nullptr;

LRESULT CWedView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  HBRUSH brush  = CreateSolidBrush(BACKGROUND);
  SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG)brush);
  m_font = CreateFont(18, 0, 0, 0, FW_LIGHT, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,  DEFAULT_QUALITY, DEFAULT_PITCH, 
    L"Dejavu Sans Mono");  //L"Source Sans Pro");
  if (m_font) SetFont(m_font);

  CClientDC pDC(m_hWnd);
  pDC.SelectFont(m_font);
  pDC.GetTextMetrics(&m_tm);
  char_x = m_tm.tmAveCharWidth;
  char_y = m_tm.tmHeight;
  ::CreateCaret(m_hWnd, (HBITMAP)0, 1, char_y);
  return 0;
}

LRESULT CWedView::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
  {
    HideCaret();
    // CTRL + o: open,  +s: save,  +b: build, +c: console
    switch (wParam) {case 0x0F: file_work(1, &LA); break; case 0x13: file_work(0, &LA); break; case 0x02: build_work(); break; case 0x03: show_console(); break; }
    return 1;
  }

  switch (wed_mode)
  {
    // Edit mode
    //////////////////////////////////////////////////////////////////////////////////////
  case 0:
  {
    switch (wParam)
    {
      // backspace
    case 0x08:
    {
      if (p.x < 1) return 1;
      HideCaret();
      llt it = n(LA.begin(), p.y);  lt line_a = it->begin();  lt line_b = it->begin();  int e_n = it->size() - 1;
      int count = 0;
      for (auto i = it->begin(); i != it->end(); i++)
      {
        if (count >= e_n) { CH ch = (*i); p.x -= ch.w;  RECT rect = { ch.x, ch.y*char_y, ch.x + ch.w, ch.y*char_y + char_y };  InvalidateRect(&rect); }
        ++count;
      }
      if (p.x > 0) { pos(line_a, e_n); pos(line_b, e_n + 1); it->erase(line_a, line_b); line = (*it); }
      else { line.clear(); LA.erase(it); p.x = 0; }
      SetCaretPos(p.x, p.y*char_y);
      ShowCaret();
    }
    break;

    // enter
    case 0x0D:
      HideCaret();
      p.x = 0;  p.y += 1;
      if (p.y > line_n)
      {
        line_n = p.y;
        if (line.size() == 0) { ch.x = 0, ch.y = p.y - 1, ch.c = ch.w = 0;  line.push_back(ch); LA.push_back(line); }
        line.clear();
      }
      else
      {
        cl nline;
        size_t size = LA.size();
        for (size_t j = 0; j < size; ++j) if (j >= (size_t)p.y)  clear_line(j);
        llt it = n(LA.begin(), p.y);
        LA.insert(it, nline);

        size = LA.size();
        for (size_t j = 0; j < size; ++j)
          if (j >(size_t)p.y)
          {
            it = n(LA.begin(), j);
            for (auto i = it->begin(); i != it->end(); i++) { (*i).y = j;  drawtext((*i), NULL); }
          }
        p.x = 0;
      }
      SetCaretPos(p.x, p.y*char_y);
      ShowCaret();
      break;

      // ESC
    case 0x1B: { wed_mode = !wed_mode; save();} break;

    default:
    {
      HideCaret();
      drawtext(ch, wParam);
      line.push_back(ch);
      if ((LA.size() == 0) || (LA.size() < (size_t)(line_n + 1))) { LA.push_back(line); }
      else { llt it = n(LA.begin(), p.y); (*it) = line; }
      line_changed = 1;
      p.x += char_w;
      SetCaretPos(p.x, p.y*char_y);
      ShowCaret();
    }
    break;
    }

  }
    break;
    ////////////////////////////////////////END//////////////////////////////////////////


    // Save mode
    //////////////////////////////////////////////////////////////////////////////////////
  default:
  {
    HideCaret();
    switch (wParam) { case 0x69: wed_mode = 0; break; case 0x68: key_left();break; case 0x6C: key_right(); break; case 0x6B: key_up(); break; case 0x6A: key_down(); break; }
    ShowCaret();
  }
    break;
    ////////////////////////////////////////END//////////////////////////////////////////
  }

  return 0;
}

LRESULT CWedView::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
  switch (wParam) { case VK_LEFT: key_left(); break;  case VK_RIGHT: key_right(); break; case VK_UP: key_up(); break; case VK_DOWN: key_down();break; }
  return 0;
}

void CWedView::drawtext(CH& c, const WPARAM& w, const LPARAM& l)
{
  CClientDC pDC(m_hWnd);
  pDC.SelectFont(m_font);
  pDC.SetTextColor(FONTCOLOR);
  pDC.SetBkColor(BACKGROUND);

  switch (w)
  {
  case NULL: { WPARAM wp = c.c;  if (l == NULL)   pDC.TextOut(c.x, c.y *char_y, (LPCTSTR)&wp);  else  pDC.TextOut(c.x, (c.y + l) *char_y, (LPCTSTR)&wp); }  break;

  default:
  {
    pDC.GetCharWidth32((UINT)w, (UINT)w, &char_w);
    c.x = p.x, c.y = p.y, c.c = w, c.w = char_w;
    pDC.TextOut(c.x, c.y*char_y, (LPCTSTR)&w);
  }
    break;
  }
}

void CWedView::key_up()
{
  if (p.y == 0)           return;
  else if (p.y == first_line)
  {
    --first_line;
    --last_line;
    --p.y;
    clear_screen();
    long  save_pos_x = 0;
    size_t size = LA.size();
    llt it = n(LA.begin(), first_line);
    auto pos_x = it->end();
    pos(pos_x, -1);
    save_pos_x = (*pos_x).x + (*pos_x).w;
	  for (size_t j = first_line; j < (size_t)(first_line + SCREEN_LINE + 1); ++j)
	  {
		  it = n(LA.begin(), j);
		  for (auto i = it->begin(); i != it->end(); i++) { drawtext((*i), NULL, (-1)*first_line); }
	  }
    SetCaretPos(save_pos_x, 0);
  }
  else
  {
    llt it = n(LA.begin(), p.y-first_line);
    p.y -= 1;
    it = n(LA.begin(), p.y);
    lt line_a = it->begin();
    int line_size = it->size();
    if (line_size)
    {
      int e_n = line_size - 1;
      pos(line_a, e_n);
      p.x = (*line_a).x + (*line_a).w;
    }
    else p.x = 0;
    SetCaretPos(p.x, (p.y-first_line)*char_y);
  }
}

void CWedView::key_down()
{
  if (p.y == line_n) return;
  else if (p.y == last_line)
  {
    ++first_line;
    ++last_line;  
    ++p.y;
    clear_screen();
    size_t init_pos = p.y - SCREEN_LINE;
    long  save_pos_x = 0;
    size_t size = LA.size();
    if (size > (size_t)p.y)
    {
      for (size_t j = init_pos; j < (init_pos + SCREEN_LINE +1); ++j)
      {
        llt it = n(LA.begin(), j);
        for (auto i = it->begin(); i != it->end(); i++) { save_pos_x = (*i).x +(*i).w; drawtext((*i), NULL, (-1) * init_pos); }
      }
      SetCaretPos(save_pos_x, SCREEN_LINE*char_y);
    }
    else
    {
      for (size_t j = init_pos; j < size; ++j)
      {
        llt it = n(LA.begin(), j);
        for (auto i = it->begin(); i != it->end(); i++) { save_pos_x = (*i).x+(*i).w; drawtext((*i), NULL, (-1) * init_pos); }
      }
      SetCaretPos(save_pos_x, (size -1)*char_y);
    }
  }
  else
  {
    if ( LA.size() ==  (size_t)(p.y +1) ) return; 
    ++p.y;
    llt it = n(LA.begin(), p.y);
    lt line_a = it->begin();
    int line_size = it->size();
    if (line_size)
    {
      pos(line_a, line_size - 1);
      p.x = (*line_a).x + (*line_a).w;
    }
    else p.x = 0;
    SetCaretPos(p.x, p.y*char_y);
  }
}

void CWedView::key_right()
{
  int char_w = 0;
  for (auto i = line.begin(); i != line.end(); i++) if (p.x == (*i).x) { char_w = (*i).w; break; }
  p.x += char_w;  SetCaretPos(p.x, p.y*char_y);
}

void CWedView::key_left()
{
  if (p.x == 0)  return;
  int char_w = 0;
  for (auto i = line.begin(); i != line.end(); i++) if (p.x == (*i).x) { char_w = (*i).w; break; }
  p.x -= char_w;  SetCaretPos(p.x, p.y*char_y);
}

void CWedView::save()
{
  if ((LA.size() == 0) || (LA.size() < (size_t)(line_n + 1)))   LA.push_back(line);
  if (line_changed)                                                                 { line_changed = 0; llt it = n(LA.begin(), p.y); it->swap(line); }
}

LRESULT CWedView::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CMainFrame::this_ptr->SetStatusBar(L"");
  SetCaretPos(p.x, p.y*char_y);
  ShowCaret();
  return 0;
}
