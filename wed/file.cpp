#include "stdafx.h"
#include "wedView.h"
#include "MainFrm.h"
#include <algorithm>

void CWedView::file_work(UINT8 mode, std::string &str, std::list<std::list<CH>> *ptr)
{
  switch (mode)
  {
  case 0:   file_worker = std::thread([=] { write_file(str, ptr); }); file_worker.detach(); break;
  default: file_worker = std::thread([=] { read_file(str, ptr); });  file_worker.detach();  break;
  }
}

void CWedView::write_file(const std::string &str, std::list<std::list<CH>> *ptr)
{
  FILE *file = nullptr;
  fopen_s(&file, str.c_str(), "wb");
  size_t line_size = ptr->size();
  for (size_t j = 0; j < line_size; ++j)
  {
    std::list<std::list<CH>>::iterator it = std::next(ptr->begin(), j);
    for (auto i = it->begin(); i != it->end(); ++i)  fwrite(&(*i).c, 1, 1, file);
    fprintf(file, "\r\n");
  }
  fclose(file);

  CMainFrame::this_ptr->SetStatusBar(L"file saved");
  Sleep(500);
  CMainFrame::this_ptr->SetStatusBar(L"");
}

void CWedView::read_file(const std::string &str, std::list<std::list<CH>> *ptr)
{
  FILE *file = nullptr;
  fopen_s(&file, str.c_str(), "rb");

  fclose(file);
}
