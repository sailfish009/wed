#include "stdafx.h"
#include "wedView.h"
#include "MainFrm.h"
#include <atldlgs.h>
#include <algorithm>

#include <experimental/filesystem> // C++-standard header file name  
#include <filesystem> // Microsoft-specific implementation header file name  
namespace fs = std::experimental::filesystem::v1;        // vs2015
//namespace fs = std::filesystem;                                       // vs2017

BOOL dirpath(char(&dir_path)[256]);

void CWedView::build_work()
{
  build_worker = std::thread([=] { read_dir(); }); build_worker.detach();
}

void CWedView::read_dir()
{
  char dir_path[256] = { 0 };
  if (dirpath(dir_path))
  {
    for (auto & f : fs::directory_iterator(dir_path))
      if (f.path().extension().string()== ".cpp")
      {
        // build some cpp files within wed
        printf("[%s]\n", f.path().filename().string().c_str());
      }
    CMainFrame::this_ptr->SetStatusBar(L"build done");  Sleep(500); CMainFrame::this_ptr->SetStatusBar(L"");
  }
}

BOOL dirpath(char(&dir_path)[256])
{
  CFolderDialog  dlg;
  dlg.m_bi.lpszTitle = L"select directory to build";
  if (dlg.DoModal() == IDOK)
  {
    std::wstring directory = dlg.GetFolderPath();
    std::string str(directory.begin(), directory.end());
    if (str.length() > 256)  return FALSE;
    strncpy_s(dir_path, str.c_str(), str.length());
    return TRUE;
  }
  return FALSE;
}