// wedView.h : interface of the CWedView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <list>
#include <thread>

using CH =
struct
{
  long x;                        // character x position
  long y;                        // character y position
  UINT8 c;                    // character content
  UINT8 w;                   // character width
};

class CWedView : public CWindowImpl<CWedView>
{
public:
  DECLARE_WND_CLASS(NULL)

  BOOL PreTranslateMessage(MSG* pMsg)
  {
    pMsg;
    return FALSE;
  }

  BEGIN_MSG_MAP(CWedView)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_CHAR, OnChar)
    MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
  END_MSG_MAP()

  //Handler prototypes (uncomment arguments if needed):
  //LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
  //LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  //LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

  LRESULT show_console();
  static void hide_console();

  std::thread file_worker;
  void file_work(UINT8 mode, std::string &str, std::list<std::list<CH>> *ptr);
  void write_file(const std::string &str, std::list<std::list<CH>> *ptr);
  void read_file(std::list<std::list<CH>> *ptr);

  HDC m_hdc;
  TEXTMETRIC m_tm;
  long char_x;
  long char_y;
  std::list<CH> line;

  static POINT p;                                                                // current position
  static int line_n;
  static int char_w;
  static BOOL line_changed;
  static BOOL wed_mode;                                             // 0: edit mode,  1: save mode

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
