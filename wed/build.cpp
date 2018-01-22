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

#include <experimental/filesystem> // C++-standard header file name  
#include <filesystem> // Microsoft-specific implementation header file name  
namespace fs = std::experimental::filesystem::v1;        // vs2015
//namespace fs = std::filesystem;                        // vs2017

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