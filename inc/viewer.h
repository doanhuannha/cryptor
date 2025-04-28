#ifndef JPEG_VIEWER_H
#define JPEG_VIEWER_H

#include <windows.h>
#include "cryptor.h"
#define EYE_ICON       0x102
class ImageViewer {
public:
    Intialize(HWND hWnd, char** files, int fileCount, char* pwd,bool oldCrypt);
	int WindowProc(HWND hwnd,UINT msg, WPARAM wParam, LPARAM lParam);
private:
	void Show(unsigned char* jpegData, int jpegSize, const char* title);
    void Show();
    void ToggleFullscreen(HWND hwnd);
    void Next(bool reverse);
    HINSTANCE _hThisInstance;
    bool _isFullscreen;
	RECT _windowedRect; // Saves window position & size
	HWND _hWnd;
	char** _files;
	int _fileCount;
	char _pwd[128];
	bool _oldCrypt;
	int _curImgIndex;
	
	static HBITMAP s_hBitmap;
	static HBITMAP LoadImageFromMemory(const unsigned char* data, const int size);
	/*
	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWindowProc(HWND hwnd,UINT msg, WPARAM wParam, LPARAM lParam);
	*/
};

#endif

