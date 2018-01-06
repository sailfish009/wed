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
    size_t size = ptr->size();
    switch(size)
    {
    case 0:break;
    case 1:
    {
      llt it = n(ptr->begin(), 0);
      for (auto i = it->begin(); i != it->end(); i++)
      {
        CH ch = (*i);
        RECT rect = { ch.x, ch.y*char_y, ch.x + ch.w, ch.y*char_y + char_y };  InvalidateRect(&rect);
      }
      ptr->clear();
    }
    break;

    default:
    {
      llt it_a = n(ptr->begin(), 0);
      llt it_b = n(ptr->begin(), size-1);

      auto i_a = it_a->begin();
      auto i_b = it_b->end(); --i_b;
      RECT rect = { (*i_a).x, (*i_a).y*char_y, (*i_b).x + (*i_b).w, (*i_b).y*char_y + char_y };  InvalidateRect(&rect);
      ptr->clear();
    }
      break;
    }
    
    p.x = p.y = 0;

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
        p.y += 1;
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

