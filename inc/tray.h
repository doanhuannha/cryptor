#include <windows.h>
#include "config.h"

#ifndef RC_INVOKED //variable definitions and function prototypes just confuse the resource compiler

const int WM_TASKBARCREATED = RegisterWindowMessage("TaskbarCreated");

#endif // 8: #ifndef RC_INVOKED

#define WindowTray_ID 1
#define WindowTray_EXIT 2
#define WindowTray_COMMAND (WM_USER+0)
//const UINT WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
class ITrayMenuObserver{
	public:
		virtual void OnMenuClick(const HWND,const UINT) = 0;
		HMENU menu;
};
class WindowTray{
public:
       void Intialize(const HWND, const HICON,const LPSTR,ITrayMenuObserver*);
       void SetOnTray();
       void Restore();
       void WindowProcedure(UINT, WPARAM, LPARAM);
       inline bool IsOnTray(){return _minimized;};

private:
       HWND _hWnd;
       HICON _hIcon;
       LPSTR _pszTooltip;
       bool _minimized;
       ITrayMenuObserver* _observer ; 
};
