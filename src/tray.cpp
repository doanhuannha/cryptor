#include "..\inc\tray.h"
void WindowTray::Intialize(const HWND hWnd,const HICON hIcon,const LPSTR pszTooltip,ITrayMenuObserver* observer){
   _minimized = false;
   _hWnd = hWnd;
   _hIcon = hIcon;
   _pszTooltip = pszTooltip;
   _observer = observer;
}
void WindowTray::SetOnTray(){
    if (_minimized){ 
       Restore();
       return;
    }
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA); //this helps the OS determine stuff. (I have no idea, but it is necessary.
    nid.hWnd = _hWnd; //the hWnd and uID members allow the OS to uniquely identify your icon. One window (the hWnd) can have more than one icon, as long as they have unique uIDs.
    nid.uID = WindowTray_ID; //sorry, had forgotten this in my original example. but without, the function probably wouldn't work
    nid.uFlags = //some flags that determine the tray's behavior:
        NIF_ICON //we're adding an icon
        | NIF_MESSAGE //we want the tray to send a message to the window identified by hWnd when something happens to our icon (see uCallbackMesage member below).
        | NIF_TIP; //our icon has a tooltip.
    nid.uCallbackMessage = WindowTray_COMMAND; //this message must be handled in hwnd's window procedure. more info below.
    nid.hIcon = _hIcon,
    strcpy(nid.szTip, _pszTooltip); //this string cannot be longer than 64 characters including the NULL terminator (which is added by default to string literals).
    //There are some more members of the NOTIFYICONDATA struct that are for advanced features we aren't using. See sources below for MSDN docs if you want to use balloon tips (only Win2000/XP).
    Shell_NotifyIcon(NIM_ADD, &nid);
    ShowWindow(_hWnd, SW_HIDE);
    _minimized = true;                                                 
}
void WindowTray::Restore(){
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = _hWnd;
    nid.uID = WindowTray_ID;
    Shell_NotifyIcon(NIM_DELETE, &nid);
    ShowWindow(_hWnd, SW_SHOW);
    SetActiveWindow(_hWnd);
    SetForegroundWindow(_hWnd);
    _minimized = false; 
}
void WindowTray::WindowProcedure(UINT message, WPARAM wParam, LPARAM lParam){
    if(message==WM_TASKBARCREATED && _minimized) SetOnTray();
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
             if(_minimized) Restore();
             break;
        case WindowTray_COMMAND:
             {
                if (wParam != WindowTray_ID) return; //if it's not the icon we planted, then go away
                if (lParam == WM_LBUTTONDBLCLK) //the mouse button has been released. It's time to re-maximize our window. This is basically done using the reverse of the minimizing process. MAKE SURE you look for WM_LBUTTONUP and not WM_LBUTTONDOWN. Most tray icons handle the release, so if your icon disappears on mousedown, the next icon will get notified of a mouseup. This is not what you want to happen.
                {
                    #ifdef FAKE_PRO
                    if(GetAsyncKeyState(VK_CONTROL)&&0x00FF>0){
                         Restore();
                    }
                    else{
                         //ShellExecute(_hWnd, "open","rundll32.exe","shell32.dll,Control_RunDLL mmsys.cpl", NULL, SW_SHOWNORMAL);
                         ShellExecute(_hWnd, "open","sndvol32", NULL, NULL, SW_SHOWNORMAL);
                    }
                    #else
                    Restore();
                    #endif
                }
                if (lParam == WM_RBUTTONUP) //time to display a menu.
                {
                    HMENU trayMenu = NULL;
                    if(_observer==NULL){
                        trayMenu = CreatePopupMenu(); //create our menu. You'll want to error-check this, because if it fails the next few functions may produce segmentation faults, and your menu won't work.
                        AppendMenu(trayMenu, MF_STRING, WindowTray_EXIT, "Exit");
                        
                    }
                    else{
                         trayMenu = _observer->menu;
                    }
                    POINT pt;
                    GetCursorPos(&pt); 
                    SetForegroundWindow(_hWnd); //even though the window is hidden, we must set it to the foreground window because of popup-menu peculiarities. See the Remarks section of the MSDN page for TrackPopupMenu.
                    UINT clicked = TrackPopupMenu(trayMenu, TPM_RETURNCMD | TPM_NONOTIFY /*don't send me WM_COMMAND messages about this window, instead return the identifier of the clicked menu item*/, pt.x, pt.y, 0, _hWnd, NULL); //display the menu. you MUST #include <windowsx.h> to use those two macros.
                    SendMessage(_hWnd, WM_NULL, 0, 0); //send benign message to window to make sure the menu goes away.
                    if(_observer){
                         _observer->OnMenuClick(_hWnd, clicked);
                    }
                    else
                    switch(clicked){
                         case WindowTray_EXIT:
                              SendMessage(_hWnd, WM_DESTROY, 0, 0);
                              break;
                    }
                    
                }
             }
             break;
    }
    
}
