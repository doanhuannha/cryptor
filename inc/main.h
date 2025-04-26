#include <windows.h>

#include "tray.h"

#define APP_ICON        0x101
#define EYE_ICON       0x102
#define WIN_ICON        0x103
#define TRAY_ICON        0x104
#define BCKIMG_BMP       0x105

#define BT_OK 0x200
#define BT_CANCEL 0x201
#define BT_BROWSEFILES 0x202
#define BT_BROWSEKEYFILE 0x203
#define BT_BROWSEMASKFILE 0x204
#define BT_KEYFILEOPT 0x205
#define BT_PWDOPT 0x206
#define BT_MASKFILEOPT 0x207
#define BT_ENCRYPTOPT 0x208
#define BT_DECRYPTOPT 0x209
#define BT_SHOWPWD 0x20A
#define BT_CRYPTTEXTOPT 0x20B
#define BT_OLDVERSIONOPT 0x20C
#define BT_VIEW 0x20D

#define OPT_ENCRYPT 0x301
#define OPT_DECRYPT 0x302
#define OPT_CRYPTMODE_PWD 0x303
#define OPT_CRYPTMODE_MASK 0x304

#define OPT_ASSOCIATEFILES 0x305
#define OPT_TXTMSG 0x306




#define MN_DELETE 0x401

#ifdef FAKE_PRO
#define WND_TITLE "My Computer"
#define WND_TITLE_PROCESS "My Computer - Processing..."
#else
#define WND_TITLE "DHN Cryptor v2.2 (05/2025)"
#define WND_TITLE_PROCESS "DHN Cryptor v2.2 (05/2025) - Processing..."
#endif

#include "..\inc\viewer.h"

struct ImageViewerParam {
    unsigned char* fileData;
    DWORD dataSize;
    char* fileName;
    
};

struct WINDOWCONTROL{
      HWND hwnd; 
      WNDPROC wndproc;
      HDC hdc;
};
WindowTray _tray;

WINDOWCONTROL _hBtBrowseFile1;
WINDOWCONTROL _hBtBrowseFile2;
WINDOWCONTROL _hBtBrowseFile3;
WINDOWCONTROL _hBtOK;
WINDOWCONTROL _hBtView;
WINDOWCONTROL _hBtCancel;
WINDOWCONTROL _hListInputFiles;

WINDOWCONTROL _hOptPwd;
WINDOWCONTROL _hOptShowPwd;
WINDOWCONTROL _hOptKeyFile;
WINDOWCONTROL _hOptMaskFile;
WINDOWCONTROL _hOptTextFile;
WINDOWCONTROL _hOptEncrypt;
WINDOWCONTROL _hOptDecrypt;
WINDOWCONTROL _hOptFileAss;
WINDOWCONTROL _hOptTxtMsg;
WINDOWCONTROL _hOldVersion;

HWND _hTxtKeyFile;
HWND _hTxtMaskFile;
HWND _hTxtPwd;
HWND _hTxtMsg;

HWND _hTxtCountFile;

bool _isEncrypt;
bool _isUsePwd;
bool _isUseMask;
bool _isTextCrypt;
bool _isOldVersion;
bool _isMsgMode;
bool _isView;

bool _isProcessing;

HWND _hWnd;
HINSTANCE _hThisInstance;
ImageViewer _viewer;

void ParseOpt();
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ImageWindowProc (HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK ControlProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void DeleteItemsInList(HWND hList);
void RefreshScrollList(WINDOWCONTROL hList);
bool ValidateInput(HWND hwnd, char* keyFile, char* maskFile);
DWORD WINAPI ThreadProc(LPVOID lpParameter);
int DoProcessParam(const char* param);
int SetEnable(bool enable);

char szClassName[] = "Cryptor-DHN";
