#include "stdafx.h"
#include "wedView.h"
#include <list>
#include <memory>

static int line_n = 0;
static int char_w = 0;
static long count_x[2048] = { 0 };
static long char_x;
static long char_y;
static POINT p = { 0 };     //current position

typedef struct
{
  long x;                              // character x position
  long y;                              // character y position
  UINT8 c;                           // character content
  UINT8 w;                          // character width
} CH ;

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
  printf("x: %d  y: %d\n", char_x, char_y);
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
    if (count_x[p.y] < 1) return 1;
    --count_x[p.y];

    if ( (line_array.size() == 0)  || (line_array.size() < (p.y +1) ) )
    {
      line_array.push_back(line);
      line.clear();
    }

    int count = 0;
    std::list <std::list<CH>>::iterator it = std::next(line_array.begin(), p.y);
    for (auto i = it->begin(); i != it->end(); i++)
    {
      if (count >= count_x[p.y])
      {
        CH ch = (*i);
        p.x -= ch.w;
        RECT rect = { ch.x, ch.y*char_y, ch.x + ch.w, ch.y*char_y + char_y };
        InvalidateRect(&rect);
      }
      ++count;
    }

    if (count_x[p.y])
    {
      std::list<CH>::iterator line_a = it->begin();
      std::list<CH>::iterator line_b = it->begin();
      std::advance(line_a, count_x[p.y]);
      std::advance(line_b, count_x[p.y] + 1);
      it->erase(line_a, line_b);
    }
    else
    {
      line.clear();
      line_array.clear();
      p.x = 0;
    }

    printf("[%d]\n", p.x);

  }  
  break;

  // enter
  case 0x0D:   
  { 
    p.x = 0;
    p.y += 1;
    if (p.y > line_n)
    {
      line_array.push_back(line);
      line_n = p.y;
      line.clear();
      printf("line_array: %d\n", line_array.size());
    }
    else
    {

    }
  }  
  break;

  // ESC
  case 0x1B:   
  { 
  }  
  break;

  default:  
  {
    m_hdc = GetDC();
    GetCharWidth32(m_hdc, (UINT)wParam, (UINT)wParam, &char_w);
    ch.x = p.x, ch.y = p.y, ch.c = wParam, ch.w = char_w;
    ReleaseDC(m_hdc);
    CClientDC pDC(m_hWnd);
    pDC.TextOut(p.x, p.y*char_y, (LPCTSTR)&wParam); 
    line.push_back(ch);
    p.x += char_w;
    ++count_x[p.y];
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
    if (p.x == 0)              return 1;
    p.x -= 1;
    break;

  case VK_RIGHT:    
    if (p.x == count_x[p.y]) return 1;
    p.x += 1;
    break;

  case VK_UP:
    if (p.y == 0)              return 1;
    p.y -= 1;
    break;

  case VK_DOWN:
    if (p.y == line_n)    return 1;
    p.y += 1;
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
