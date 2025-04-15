#include "..\inc\main.h"
/*  Declare Windows procedure  */
//SHBrowseForFolder//http://msdn.microsoft.com/en-us/library/bb762115(v=VS.85).aspx

/*  Make the class name into a global variable  */
char szClassName[ ] = "Cryptor-DHN";
#ifdef FAKE_PRO
class TrayMenuObserver: public ITrayMenuObserver{
public:
    TrayMenuObserver(){
        menu = CreatePopupMenu();
        AppendMenu(menu, MF_STRING, 1000, "Open Volume Control");
        AppendMenu(menu, MF_STRING, 1001, "Adjust Audio Properties");
        SetMenuDefaultItem(menu,1000,0);
        
    }
    void OnMenuClick(const HWND hWnd,const UINT menu_clicked){
        switch(menu_clicked){
             case 1000:
                  ShellExecute(hWnd, "open","sndvol32", NULL, NULL, SW_SHOWNORMAL);
                  break;
             case 1001:
                  ShellExecute(hWnd, "open","rundll32.exe","shell32.dll,Control_RunDLL mmsys.cpl", NULL, SW_SHOWNORMAL);
                  break;
                  
        }       
    }
};
#endif
int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd = FindWindow(szClassName,NULL);
    
    if(hwnd!=NULL){
         COPYDATASTRUCT cpd;
         cpd.dwData = 0;
		 cpd.cbData = strlen(lpszArgument)+1;
		 cpd.lpData = lpszArgument;

         SendMessage(hwnd,WM_COPYDATA, (WPARAM) NULL,(LPARAM)&cpd);
         SetActiveWindow(hwnd);
         SetForegroundWindow(hwnd);
         delete lpszArgument;
         return 0; 
    }
    //_isMsgMode = GetAsyncKeyState(VK_CONTROL)&&0x00FF>0;
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);
    /* Load Icon*/
    
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
    //HICON hIcon = LoadIcon (NULL, IDI_APPLICATION);
    /* Use default icon and mouse-pointer */
    /*
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    */
    wincl.hIcon = hIcon;
    wincl.hIconSm = hIcon;
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    
    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    int nWidth = 420 ;
    int nHeight = 390 ;
    
    int xx = ( GetSystemMetrics( SM_CXSCREEN ) - nWidth ) / 2 ;
    int yy = ( GetSystemMetrics( SM_CYSCREEN ) - nHeight ) / 2 ;

    /* The class is registered, let's create the program*/
    
    hwnd = _hWnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           
           WND_TITLE,       /* Title Text */
           WS_OVERLAPPEDWINDOW /* default window */
           &~WS_MAXIMIZEBOX &~WS_SIZEBOX,
           xx,       /* Windows decides the position */
           yy,       /* where the window ends up on the screen */
           nWidth,                 /* The programs width */
           nHeight,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    
    //set icon for window 
    
    #ifdef FAKE_PRO
    hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(WIN_ICON));
    #endif
    LRESULT lr1 = SendMessage(_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    lr1 = SendMessage(_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    #ifdef FAKE_PRO
    hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(TRAY_ICON));
    TrayMenuObserver trayMenu;
    _tray.Intialize(_hWnd,hIcon,"Volume",&trayMenu);
    #else
    _tray.Intialize(_hWnd,hIcon,(LPSTR)WND_TITLE,NULL);
    #endif
    _isProcessing = false;
    //create control
    
    int posX, posY, ctrlW, ctrlH;
    /*
    posX = 50; posY = 100;
    ctrlW = 420; ctrlH = 330;

    CreateWindowEx(  
        	NULL,  
        	"BUTTON",  
        	"Tool",  
        	WS_CHILD | WS_VISIBLE | BS_GROUPBOX ,  
        	posX, posY,
            ctrlW, ctrlH,  
        	_hWnd, 
            NULL,  
        	hThisInstance,  
        	NULL);
   	*/
   	const int rootX = 0;
   	const int rootY = 0;
    //row 0
    posX = rootX + 5; posY = rootY+15;
    ctrlW = 100; ctrlH = 20;
    _hOptEncrypt.hwnd = CreateWindowEx(  
        	NULL,  
        	"Button",  
        	"Encrypt",  
        	WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP ,  
        	posX, posY,
            ctrlW, ctrlH,  
        	hwnd, 
            (HMENU)BT_ENCRYPTOPT,  
        	hThisInstance,  
        	NULL);
   	SendMessage(_hOptEncrypt.hwnd,BM_SETCHECK,BST_CHECKED,0);
   	posX += ctrlW + 30;
   	_hOptDecrypt.hwnd = CreateWindowEx(  
        	NULL,  
        	"Button",  
        	"Decrypt",  
        	WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,  
        	posX, posY,
            ctrlW, ctrlH,  
        	hwnd, 
            (HMENU)BT_DECRYPTOPT,  
        	hThisInstance,  
        	NULL);
   	posX += ctrlW + 70;
   	ctrlW = 110;
   	_hOldVersion.hwnd = CreateWindowEx(  
        	NULL,  
        	"Button",  
        	"Old algorithm",  
        	WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,   
        	posX, posY,
            ctrlW, ctrlH,  
        	hwnd, 
            (HMENU)BT_OLDVERSIONOPT,  
        	hThisInstance,  
        	NULL);
   	EnableWindow(_hOldVersion.hwnd,false);
    //row 1
    posX = rootX + 5; posY += ctrlH + 10;
    ctrlW = 70; ctrlH = 20;
    CreateWindowEx(  
        	NULL,  
        	"Static",  
        	"Input files:",  
        	WS_CHILD | WS_VISIBLE | SS_SIMPLE,  
        	posX, posY,  
        	ctrlW, ctrlH,  
        	hwnd, NULL,  
        	hThisInstance,  
        	NULL);
   	posX += ctrlW + 5;
   	_hTxtCountFile = CreateWindowEx(  
        	NULL,  
        	"Static",  
        	"0",  
        	WS_CHILD | WS_VISIBLE | SS_SIMPLE,  
        	posX, posY,  
        	ctrlW, ctrlH,  
        	hwnd, NULL,  
        	hThisInstance,  
        	NULL);
   	posX = rootX + 5;ctrlW = 100;
   	posX += ctrlW + 200;
    _hBtBrowseFile1.hwnd = CreateWindowEx(
			0,
            "Button",
            "Add files",
            WS_BORDER | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, 
            (HMENU)BT_BROWSEFILES,
            hThisInstance,
            NULL);
   //row 2
   //list
   posX = rootX + 5; posY += ctrlH + 10;
   ctrlW = 400; ctrlH = 100;
   
   _hListInputFiles.hwnd  = CreateWindowEx(
            NULL,
            "ListBox",
            "????",
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL | LBS_MULTIPLESEL,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, NULL,
            hThisInstance,
            NULL);
   _hListInputFiles.wndproc = (WNDPROC)SetWindowLong(_hListInputFiles.hwnd, GWL_WNDPROC, (LONG)ControlProcedure);
   _hListInputFiles.hdc = GetDC(_hListInputFiles.hwnd);
   
   //*
   _hTxtMsg = CreateWindowExW(
            NULL,
            L"Edit",
            L"[Enter your message here]",
            WS_BORDER | WS_CHILD | WS_VISIBLE  | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
            posX, posY-30,
            ctrlW, ctrlH+30,
            hwnd, NULL,
            hThisInstance,
            NULL);
   ShowWindow(_hTxtMsg, SW_HIDE);
   
   HFONT hFont =CreateFont
            (
 

            18,      // logical height of font
            0,      // logical average character width
            0,      // angle of escapement
            0,      // base-line orientation angle
            0,      // font weight
            0,      // italic attribute flag
            0,      // underline attribute flag
            0,      // strikeout attribute flag
            0,      // character set identifier
            0,      // output precision
            0,      // clipping precision
            0,      // output quality
            0,      // pitch and family
            "Times Roman"       // pointer to typeface name string
         );

   SendMessage (_hTxtMsg, WM_SETFONT, (WPARAM)hFont, (LPARAM)true);
   //*/
   //row 3
   //radio
   posX = rootX + 5; posY += ctrlH + 10;
   ctrlW = 100; ctrlH = 20;
   _hOptPwd.hwnd = CreateWindowEx(  
        	NULL,  
        	"Button",  
        	"Password",  
        	WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,  
        	posX, posY,
            ctrlW, ctrlH,  
        	hwnd, 
            (HMENU)BT_PWDOPT,  
        	hThisInstance,  
        	NULL);
    SendMessage(_hOptPwd.hwnd ,BM_SETCHECK,BST_CHECKED,0);
  	posX += ctrlW + 5;
  	ctrlW = 150;
  	_hTxtPwd = CreateWindowEx(
			0,
            "Edit",
            "",
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_PASSWORD,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, 
            NULL,
            hThisInstance,
            NULL);
   posX += ctrlW + 5;
   ctrlW = 130;
   _hOptShowPwd.hwnd = CreateWindowEx(  
        	NULL,  
        	"Button",  
        	"Show password",  
        	WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,  
        	posX, posY,
            ctrlW, ctrlH,  
        	hwnd, 
            (HMENU)BT_SHOWPWD,  
        	hThisInstance,  
        	NULL);
   //row 4
   //checkbox
   posX = posX = rootX + 35; posY += ctrlH + 10;
   ctrlW = 80; ctrlH = 20;
   _hOptMaskFile.hwnd = CreateWindowEx(  
        	NULL,  
        	"Button",  
        	"Mask file",  
        	WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,  
        	posX, posY,
            ctrlW, ctrlH,  
        	hwnd, 
            (HMENU)BT_MASKFILEOPT,  
        	hThisInstance,  
        	NULL);
  	SendMessage(_hOptMaskFile.hwnd,BM_SETCHECK,BST_CHECKED,0);
  	//_hOptMaskFile.wndproc = (WNDPROC)SetWindowLong(_hOptMaskFile.hwnd, GWL_WNDPROC, (LONG)ControlProcedure);
  	posX += ctrlW + 5;
  	ctrlW = 200;
  	_hTxtMaskFile = CreateWindowEx(
			0,
            "Edit",
            "",
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, 
            NULL,
            hThisInstance,
            NULL);
   //EnableWindow(_hTxtMaskFile,false);
   SendMessage(_hTxtMaskFile,EM_SETREADONLY,(WPARAM)true,0);
   
   posX += ctrlW + 5;
   ctrlW = 80;
   _hBtBrowseFile2.hwnd = CreateWindowEx(
			0,
            "Button",
            "Browse...",
            WS_BORDER | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, 
            (HMENU)BT_BROWSEMASKFILE,
            hThisInstance,
            NULL);
   //row 4a
   //checkbox
   posX = rootX + 35; posY += ctrlH + 10;
   ctrlW = 500; ctrlH = 20;
   _hOptTextFile.hwnd = CreateWindowEx(  
        	NULL,  
        	"Button",  
        	"Crypt as text file (File size must be less then 1.4MB)",  
        	WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,  
        	posX, posY,
            ctrlW, ctrlH,  
        	hwnd, 
            (HMENU)BT_CRYPTTEXTOPT,  
        	hThisInstance,  
        	NULL);
   //_hOptTextFile.wndproc = (WNDPROC)SetWindowLong(_hOptTextFile.hwnd, GWL_WNDPROC, (LONG)ControlProcedure);
   //SendMessage(_hOptMaskFile.hwnd,BM_SETCHECK,BST_CHECKED,0);
   //row 5
   //radio
   posX = rootX + 5; posY += ctrlH + 10;
   ctrlW = 100; ctrlH = 20;
   _hOptKeyFile.hwnd = CreateWindowEx(  
        	NULL,  
        	"Button",  
        	"Key file",  
        	WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,  
        	posX, posY,
            ctrlW, ctrlH,  
        	hwnd, 
            (HMENU)BT_KEYFILEOPT,  
        	hThisInstance,  
        	NULL);
  	posX += ctrlW + 5;
  	ctrlW = 200;
  	_hTxtKeyFile = CreateWindowEx(
			0,
            "Edit",
            "",
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, 
            NULL,
            hThisInstance,
            NULL);
   //EnableWindow(_hTxtKeyFile,false);
   SendMessage(_hTxtKeyFile,EM_SETREADONLY,(WPARAM)true,0);
   posX += ctrlW + 5;
   ctrlW = 80;
   _hBtBrowseFile3.hwnd = CreateWindowEx(
			0,
            "Button",
            "Browse...",
            WS_BORDER | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, 
            (HMENU)BT_BROWSEKEYFILE,
            hThisInstance,
            NULL);
   EnableWindow(_hBtBrowseFile3.hwnd,false);
   //row 6
   //radio
   posX = rootX + 5; posY += ctrlH + 10;
   ctrlW = 150;
   _hOptTxtMsg.hwnd = CreateWindowEx(
			0,
            "Button",
            "Message mode",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, 
            (HMENU)OPT_TXTMSG,
            hThisInstance,
            NULL);
   
   //row 7
   //radio
   posX = rootX + 5; posY += ctrlH + 10;
   ctrlW = 200;
   _hOptFileAss.hwnd = CreateWindowEx(
			0,
            "Button",
            "Add to right click menu",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, 
            (HMENU)OPT_ASSOCIATEFILES,
            hThisInstance,
            NULL);
   //check registry val
   SendMessage(_hWnd,WM_COMMAND,OPT_ASSOCIATEFILES,0xFF);
   
   posX += ctrlW + 5;
   
   posX = rootX + 240;
   ctrlW = 80;
   _hBtOK.hwnd = CreateWindowEx(
			0,
            "Button",
            "OK",
            WS_BORDER | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, 
            (HMENU)BT_OK,
            hThisInstance,
            NULL);
   posX += ctrlW + 5;
   _hBtCancel.hwnd = CreateWindowEx(
			0,
            "Button",
            "Close",
            WS_BORDER | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            posX, posY,
            ctrlW, ctrlH,
            hwnd, 
            (HMENU)BT_CANCEL,
            hThisInstance,
            NULL);
    
    //process param
    DoProcessParam(lpszArgument);
    /* Make the window visible on the screen */
    ShowWindow (_hWnd, nFunsterStil);
    

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}
int DoProcessParam(const char* param){
    
    if(strlen(param)>0){
        WIN32_FIND_DATA fileData;
        HANDLE hSearch;
        // Start search for .txt files:
        hSearch = FindFirstFile(param, &fileData);
        if(hSearch != INVALID_HANDLE_VALUE)
        if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if(SendMessage(_hListInputFiles.hwnd,(UINT)LB_FINDSTRINGEXACT,(WPARAM)0,(LPARAM)param)==LB_ERR ){
                SendMessage(_hListInputFiles.hwnd,(UINT)LB_ADDSTRING,(WPARAM)0,(LPARAM)param);
                RefreshScrollList(_hListInputFiles);
            }
            
        }
        else{
             char* searchPattern = new char[_MAX_PATH];
             strcpy(searchPattern,param);
             strcat(searchPattern,"\\*.*");
             
             FindClose(hSearch);
             hSearch = FindFirstFile(searchPattern, &fileData);
             while (hSearch != INVALID_HANDLE_VALUE) {
                 if (FindNextFile(hSearch, &fileData) == 0) break; // stop when none left
                 if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                 {
                        
                     if(strcmp(fileData.cFileName,"..")!=0){
                         strcpy(searchPattern,param);
                         strcat(searchPattern,"\\");
                         strcat(searchPattern,fileData.cFileName);
                         DoProcessParam(searchPattern);
                     }
                 }
                 else{
                     strcpy(searchPattern,param);
                     strcat(searchPattern,"\\");
                     strcat(searchPattern,fileData.cFileName);
                     if(SendMessage(_hListInputFiles.hwnd,(UINT)LB_FINDSTRINGEXACT,(WPARAM)0,(LPARAM)searchPattern)==LB_ERR ){
                        SendMessage(_hListInputFiles.hwnd,(UINT)LB_ADDSTRING,(WPARAM)0,(LPARAM)searchPattern);
                        RefreshScrollList(_hListInputFiles);
                     } 
                     
                 }
             }
             delete searchPattern;
        }
        FindClose(hSearch);
        

    }
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    _tray.WindowProcedure(message,wParam,lParam);
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
             PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
             break;
        case WM_CLOSE:
             if(_isProcessing) return 0;
             else return DefWindowProc (hwnd, message, wParam, lParam);
             break;
        case WM_COPYDATA:
             {
                   COPYDATASTRUCT* param = (COPYDATASTRUCT*)lParam;
                   LPSTR lpszArgument = (LPSTR)param->lpData;
                   DoProcessParam(lpszArgument);
                   delete lpszArgument;
                   delete param;
                   SendMessage(_hOptTxtMsg.hwnd,BM_SETCHECK,BST_UNCHECKED,0);
                   SetEnable(true);
             }           
             break;
        case WM_COMMAND:
             #include "main_command.h"
             break;
        case WM_SYSCOMMAND:
             switch (WPARAM(wParam)){
                    case SC_MINIMIZE:
                         _tray.SetOnTray();
                         break;
                    default:
                        DefWindowProc (hwnd, message, wParam, lParam);
                        break;
             }
             
             break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
void ParseOpt(){
    _isEncrypt = SendMessage(_hOptEncrypt.hwnd, BM_GETCHECK, 0, 0)==BST_CHECKED;
    _isUsePwd = SendMessage(_hOptPwd.hwnd, BM_GETCHECK, 0, 0)==BST_CHECKED;
    _isUseMask = SendMessage(_hOptMaskFile.hwnd, BM_GETCHECK, 0, 0)==BST_CHECKED;
    _isTextCrypt = SendMessage(_hOptTextFile.hwnd, BM_GETCHECK, 0, 0)==BST_CHECKED;
    _isOldVersion = SendMessage(_hOldVersion.hwnd, BM_GETCHECK, 0, 0)==BST_CHECKED;
    _isMsgMode = SendMessage(_hOptTxtMsg.hwnd, BM_GETCHECK, 0, 0)==BST_CHECKED;
}
LRESULT CALLBACK ControlProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
     switch(message){
          case WM_RBUTTONUP:
               {
                   int count = SendMessage(_hListInputFiles.hwnd, LB_GETSELCOUNT, 0, 0);
                   int selectedIndex = SendMessage(_hListInputFiles.hwnd,LB_GETCURSEL,0,0);
                   if(selectedIndex>=0&& count>0){
                       HMENU hPopupMenu = CreatePopupMenu();
                       AppendMenu(hPopupMenu, MF_STRING, MN_DELETE, "Remove");
                       POINT pt;
                       GetCursorPos(&pt); 
                       UINT clicked = TrackPopupMenu(hPopupMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
                       SendMessage(hwnd, WM_NULL, 0, 0);
                       switch(clicked){
                             case MN_DELETE:
                                  //MessageBox(hwnd, "Click Delete.", "Example", MB_OK | MB_ICONINFORMATION);
                                  DeleteItemsInList(_hListInputFiles.hwnd);
                                  RefreshScrollList(_hListInputFiles);
                                  //SendMessage(_hListInputFiles,LB_DELETESTRING,selectedIndex,0);
                                  break;
                       }
                   }
               }
               break;
     }
     if(hwnd==_hListInputFiles.hwnd) return CallWindowProc(_hListInputFiles.wndproc, hwnd, message, wParam, lParam);
     //if(hwnd==_hOptTextFile.hwnd) return CallWindowProc(_hOptTextFile.wndproc, hwnd, message, wParam, lParam);
     //if(hwnd==_hOptMaskFile.hwnd) return CallWindowProc(_hOptMaskFile.wndproc, hwnd, message, wParam, lParam);
     return 0;
}
void DeleteItemsInList(HWND hList){
     int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
     int *buf = new int[count];
     SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)buf);
     for(int i = 0;i<count;i++){
         SendMessage(hList,LB_DELETESTRING,buf[i]-i,0);
     }
     SendMessage(hList,LB_SETSEL,(WPARAM)true,(LPARAM)0);
     delete buf;
    
}
void RefreshScrollList(WINDOWCONTROL hList){
     int count = SendMessage(hList.hwnd, LB_GETCOUNT, 0, 0);
     SIZE textSize;
     int maxTextLength = 0;
     char* buf = new char[MAX_CHAR];
     for(int i = 0;i<count;i++){
         SendMessage(hList.hwnd,LB_GETTEXT,(WPARAM)i,(LPARAM)buf);
         GetTextExtentPoint32(hList.hdc,buf,strlen(buf),&textSize);
         if(maxTextLength<textSize.cx) maxTextLength= textSize.cx;
     }
     delete buf;
     SendMessage(hList.hwnd,LB_SETHORIZONTALEXTENT,maxTextLength+5,0);
     //set text for label
     char* pszLabelText;
     pszLabelText = new char[10];
     itoa(count,pszLabelText,10);
     SetWindowText(_hTxtCountFile,pszLabelText);
     delete pszLabelText;
     
     
}
bool ValidateInput(HWND hwnd, char* keyFile, char* maskFile){
    bool fail = false;
    char* msg = new char[512];
    strcpy(msg,"");
    if(!_isMsgMode){
        fail |= SendMessage(_hListInputFiles.hwnd, LB_GETCOUNT, 0, 0)<=0;
        if(fail){
              strcat(msg,"Please select at least one file to be processed.\n");
        }
    }
    if(_isMsgMode){
                   
    } 
    else if(_isUsePwd){ //use password
        if(_isUseMask&&_isEncrypt){ //with mask                                               
             if(strlen(maskFile)<=0){
                  strcat(msg,"Please select a mask file.\n");
                  fail = true;
             }
        }       
    }              
    else{//use key file
        if(strlen(keyFile)<=0){
             strcat(msg,"Please select a key file.");
             fail = true;
        }
    }
    if(fail){
         MessageBox(hwnd, msg, "Info Message", MB_OK | MB_ICONINFORMATION);
    }
    delete msg;
    return fail;
}
DWORD WINAPI ThreadProc(LPVOID lpParameter){
       #include "thread_proc.h"
}
int SetEnable(bool enable){
     EnableWindow(_hOptEncrypt.hwnd,enable);
     EnableWindow(_hOptDecrypt.hwnd,enable);
     EnableWindow(_hOptKeyFile.hwnd,enable);
     EnableWindow(_hOptMaskFile.hwnd,enable);
     EnableWindow(_hOptPwd.hwnd,enable);
     EnableWindow(_hBtBrowseFile1.hwnd,enable);
     EnableWindow(_hBtBrowseFile2.hwnd,enable);
     EnableWindow(_hBtBrowseFile3.hwnd,enable);
     EnableWindow(_hBtCancel.hwnd,enable);
     EnableWindow(_hBtOK.hwnd,enable);
     EnableWindow(_hTxtKeyFile,enable);
     EnableWindow(_hTxtMaskFile,enable);
     EnableWindow(_hTxtPwd,enable);
     EnableWindow(_hListInputFiles.hwnd,enable);
     EnableWindow(_hOptShowPwd.hwnd,enable);
     EnableWindow(_hOptTextFile.hwnd,enable);
     EnableWindow(_hOptTxtMsg.hwnd,enable);
    
     if(enable){
         ParseOpt();
         if(_isOldVersion){
              SendMessage(_hOptTextFile.hwnd,BM_SETCHECK,BST_UNCHECKED,0);
         }
         if(_isEncrypt){
              SendMessage(_hOldVersion.hwnd,BM_SETCHECK,BST_UNCHECKED,0);
         }
         if(_isMsgMode){
              SendMessage(_hOptPwd.hwnd,BM_SETCHECK,BST_CHECKED,0);
              SendMessage(_hOptKeyFile.hwnd,BM_SETCHECK,BST_UNCHECKED,0);
         }
         ParseOpt();
         
         EnableWindow(_hTxtPwd,_isUsePwd);
         EnableWindow(_hOptShowPwd.hwnd,_isUsePwd);
         EnableWindow(_hOptMaskFile.hwnd,_isUsePwd&&!_isMsgMode);
         EnableWindow(_hOptTextFile.hwnd,_isUsePwd&&!_isOldVersion&&!_isMsgMode);
         EnableWindow(_hBtBrowseFile3.hwnd,!_isUsePwd&&!_isMsgMode);
         EnableWindow(_hTxtKeyFile,!_isUsePwd&&!_isMsgMode);
         EnableWindow(_hTxtMaskFile,_isUseMask&&_isEncrypt&&_isUsePwd&&!_isMsgMode);
         EnableWindow(_hBtBrowseFile2.hwnd,_isUseMask&&_isEncrypt&&_isUsePwd&&!_isMsgMode);
         EnableWindow(_hOldVersion.hwnd,!_isEncrypt&&!_isMsgMode);
         EnableWindow(_hOptKeyFile.hwnd,!_isMsgMode);
         //_isOldVersion
     }
     return 0;
}

