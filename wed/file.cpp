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
#include <atldlgs.h>
#include <algorithm>

BOOL filepath(char(&file_path)[256]);

void CWedView::file_work(UINT8 mode, cla *ptr)
{
  switch (mode)
  {
  case 0:   file_worker = std::thread([=] { write_file(ptr); }); file_worker.detach(); break;
  default: file_worker = std::thread([=] { read_file(ptr); });  file_worker.detach();  break;
  }
}

void CWedView::write_file( cla *ptr)
{
  FILE *file = nullptr;
  char file_path[256] = { 0 };
  if (filepath(file_path))
  {
    save();
    fopen_s(&file, file_path, "wb");
    size_t line_size = ptr->size();
    for (size_t j = 0; j < line_size; ++j)
    {
      llt it = n(ptr->begin(), j);
      for (auto i = it->begin(); i != it->end(); ++i) if((*i).w) fwrite(&(*i).c, 1, 1, file);
      fprintf(file, "\r\n");
    }
    fclose(file);
    CMainFrame::this_ptr->SetStatusBar(L"file saved");  Sleep(500); CMainFrame::this_ptr->SetStatusBar(L"");
  }
}

void CWedView::read_file(cla *ptr)
{
  char file_path[256] = { 0 };
  if (filepath(file_path))
  {
    p.x = p.y = 0;
    ptr->clear();
    clear_screen();

    UINT8 buffer[512] = { 0 };
    FILE *file = nullptr;
    fopen_s(&file, file_path, "rb");

    CClientDC pDC(m_hWnd);
    for(;;)
    {
      memset(buffer, 0, 512);
      if (fgets((char *)buffer, 512, file))
      {
        buffer[strcspn((char*)buffer, "\r\n")] = 0;
        line.clear();
        p.x = 0;
        for (int i = 0; i < 512; ++i)
        {
          WPARAM wp = buffer[i];
          if (wp == 0) break;
          CH ch = { p.x,  p.y, buffer[i], (UINT8)char_w };
          pDC.SelectFont(m_font);
          pDC.GetCharWidth32((UINT)wp, (UINT)wp, &char_w);
          pDC.SetTextColor(FONTCOLOR);
          pDC.SetBkColor(BACKGROUND);
          pDC.TextOut(p.x, p.y*char_y, (LPCTSTR)&wp);
          p.x += char_w&0xFF;
          line.push_back(ch);
        }
        ptr->push_back(line);
        ++p.y;
      }
      else
        break;
    }
    fclose(file);
    line_n = p.y;
    p.x = p.y = 0;
  }
}

BOOL filepath(char(&file_path)[256])
{
  WCHAR filter[] = L"All Files(*.*)|*.*||";
  CFileDialog fdlg(TRUE, L"*", L"*", OFN_HIDEREADONLY, filter);
  if (IDOK == fdlg.DoModal())
  {
    std::wstring wstr = &fdlg.m_ofn.lpstrFile[0];
    std::string str(wstr.begin(), wstr.end());
    if (str.length() > 256)  return FALSE;
    strncpy_s(file_path, str.c_str(), str.length());
    return TRUE;
  }
  return FALSE;
}

