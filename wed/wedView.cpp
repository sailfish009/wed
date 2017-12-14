#include "stdafx.h"
#include "wedView.h"


LRESULT CWedView::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  // TODO: Add your message handler code here and/or call default

  printf("[wParam: 0x%02x]\n", wParam);

#if 0
  switch (wParam)
  {
  }
#endif

  return 0;
}
