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

static bool b_console = false;
static HWND console_hwnd = nullptr;
static int c_char_w = 0;

cl cline;
static long x = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_CLOSE:
    DestroyWindow(hwnd);
    break;

  case WM_CHAR:
    switch (wParam) 
    { 
      // Backspace
    case 0x08:  
      {
        if (x < 1) return 1;
        auto i = cline.end(); --i;
        CH ch = (*i); x -= ch.w;  RECT rect = { ch.x, 0, ch.x + ch.w, 24 };  ::InvalidateRect(console_hwnd, &rect, TRUE);
        cline.pop_back();
      }
      break; 

      // ESC
    case 0x1B: CWedView::hide_console();  break; 

      // Enter
    case 0x0D:
      for (auto i = cline.begin(); i != cline.end(); i++)
      {
         CH ch = (*i); x -= ch.w;  RECT rect = { ch.x, 0, ch.x + ch.w, 24 };  ::InvalidateRect(console_hwnd, &rect, TRUE); 
      }
      x = 0;
      cline.clear();
      break;

    default:
    {
      CClientDC pDC(console_hwnd);
      pDC.SelectFont(CWedView::m_font);
      pDC.GetCharWidth32((UINT)wParam, (UINT)wParam, &c_char_w);
      pDC.SetTextColor(FONTCOLOR);
      pDC.SetBkColor(BACKGROUND);
      CH ch = { x, 0,  (UINT8)wParam, (UINT8)c_char_w };
      pDC.TextOut(x, 0, (LPCTSTR)&wParam);
      cline.push_back(ch);
      x += c_char_w;
    }
      break;
    }
    break;

  case WM_RBUTTONDOWN:
    if (console_hwnd != nullptr)
    {
      POINT p;
      GetCursorPos(&p);
      HMENU menu = CreatePopupMenu();
      InsertMenu(menu, 0, MF_BYPOSITION | MF_STRING, 1, _T("exit console"));
      SetForegroundWindow(hwnd);
      DWORD select = ::TrackPopupMenu(menu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD, p.x, p.y, 0, hwnd, 0);

      switch (select)
      {
      // select : 0 Cancel Menu
      case 1:  if (b_console == true) { b_console = false;  CWedView::hide_console();  }    break;
      }
      DestroyMenu(menu);
    }
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

HWND CreateConsoleWindow(HWND hwnd, RECT rect)
{
  WNDCLASSEX wc;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = (HINSTANCE)::GetModuleHandle(NULL);
  wc.hIcon = NULL;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); 
  //wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255)); 
  wc.lpszMenuName = NULL;
  wc.lpszClassName = _T("console");
  wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  RegisterClassEx(&wc);

  return CreateWindowEx(NULL, TEXT("console"),    TEXT(""),    WS_POPUP | WS_VISIBLE|WS_BORDER,
    rect.left,    rect.top,    rect.right - rect.left,    rect.bottom - rect.top,    hwnd, NULL, wc.hInstance, 0);
}

LRESULT CWedView::show_console()
{
  RECT rect = { 0 };
  GetWindowRect(&rect);
  rect.top = rect.bottom + 1;
  rect.bottom = rect.top + 24;
  if (console_hwnd == nullptr)  console_hwnd = CreateConsoleWindow(m_hWnd, rect);
  else ::MoveWindow(console_hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);

  if (b_console == false)
  {
    b_console = true;
    ::ShowWindow(console_hwnd,  SW_SHOW);
  }
  return 0;
}

void CWedView::hide_console()
{
  if (b_console == true)
  {
    b_console = false;
    ::ShowWindow(console_hwnd, SW_HIDE);
  }
}
