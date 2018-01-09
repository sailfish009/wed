// wedView.h : interface of the CWedView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <list>
#include <thread>

#define RED_A  250
#define GREEN_A 250
#define BLUE_A 250
#define FONTCOLOR  (RGB(RED_A,GREEN_A,BLUE_A))

#define RED_B 0
#define GREEN_B 0
#define BLUE_B 0
#define BACKGROUND  (RGB(RED_B,GREEN_B,BLUE_B))

#define SCREEN_LINE 22

#define ALIAS_TEMPLATE_FUNCTION(highLevelF, lowLevelF) \
template<typename... Args> \
inline auto highLevelF(Args&&... args) -> decltype(lowLevelF(std::forward<Args>(args)...)) \
{ \
    return lowLevelF(std::forward<Args>(args)...); \
}

using CH =
struct
{
  long x;                        // character x position
  long y;                        // character y position
  UINT8 c;                    // character content
  UINT8 w;                   // character width
};

using cl = std::list<CH>;                             // Character List
using cla = std::list<std::list<CH>>;        // Character List Array

using lt = std::list<CH>::iterator;                             // List iterator
using llt = std::list<std::list<CH>>::iterator;         // List List iterator

ALIAS_TEMPLATE_FUNCTION(pos, std::advance)
ALIAS_TEMPLATE_FUNCTION(n, std::next)

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
  void file_work(UINT8 mode, cla *ptr);
  void write_file(cla *ptr);
  void read_file(cla *ptr);

  std::thread build_worker;
  void build_work();
  void read_dir();

  static HFONT m_font;
  TEXTMETRIC m_tm;
  long char_x;
  long char_y;

  CH ch = { 0 };
  cl line;
  long first_line = 0;
  long last_line = SCREEN_LINE;
  UINT8 m_buffer[512] = { 0 };
  void drawtext(CH& c, const WPARAM& w, const LPARAM& l=NULL);
  void clear_line(const long& line_pos);
  void clear_screen();

  void key_up();
  void key_down();
  void key_right();
  void key_left();
  void save();

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
