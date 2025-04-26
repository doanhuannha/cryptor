#include "..\inc\viewer.h"
#include <gdiplus.h>
#include <ole2.h> // For IStream and CreateStreamOnHGlobal

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "ole32.lib")

using namespace Gdiplus;

HBITMAP ImageViewer::s_hBitmap;

ImageViewer::Intialize(HWND hWnd ,char** files, int fileCount, char* pwd, bool oldCrypt) {
	
	_hPWnd = hWnd;
	ShowWindow(_hWnd, SW_MAXIMIZE);
	_hWnd = NULL;
	_isFullscreen = false;
	
	_oldCrypt = oldCrypt;
	_curImgIndex = -1;
	_fileCount = fileCount;
	_files = new char*[fileCount];
	for(int i=0;i<fileCount;i++){
		_files[i] = new char[_MAX_PATH];
		strcpy(_files[i], files[i]);
	}
	strcpy(_pwd, pwd);
	//_shown = false;
	Show();

}

HBITMAP ImageViewer::LoadImageFromMemory(const unsigned char* data, const int size) {
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiPlusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiPlusStartupInput, NULL);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!hMem) return NULL;

    void* pMem = GlobalLock(hMem);
    memcpy(pMem, data, size);
    GlobalUnlock(hMem);

    IStream* pStream = NULL;
    if (CreateStreamOnHGlobal(hMem, TRUE, &pStream) != S_OK)
        return NULL;
	
    Bitmap* bitmap = Bitmap::FromStream(pStream);
    HBITMAP hBmp = NULL;

    if (bitmap && bitmap->GetLastStatus() == Ok) bitmap->GetHBITMAP(Color(0, 0, 0), &hBmp);
        

    delete bitmap;
    pStream->Release();
    GdiplusShutdown(gdiplusToken);
    return hBmp;
}

int ImageViewer::WindowProc(HWND hwnd,UINT msg, WPARAM wParam, LPARAM lParam) {
	_hWnd = hwnd;
	//if(!_shown) return DefWindowProc(hwnd, msg, wParam, lParam); 
    switch (msg) {
    	case WM_KEYDOWN:
	        {
	        	switch (wParam) {
				    case VK_F11:
				        ToggleFullscreen(hwnd);
				        break;
				
				    case VK_ESCAPE:
				        if (_isFullscreen)
				            ToggleFullscreen(hwnd);
				        else
				            DestroyWindow(hwnd);
				        break;
				
				    case VK_LEFT:
				        Next(true);
				        break;
				
				    case VK_RIGHT:
				        Next(false);
						break;
				}
			}
	        return 0;
		case WM_SIZE:
            // Invalidate the window so WM_PAINT gets triggered after resize
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        case WM_ERASEBKGND:
            // Let WM_PAINT fully handle painting (optional optimization)
            return 1;
        
		case WM_PAINT: {
        	
            if (s_hBitmap) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);

                // Clear background (black)
                RECT rect;
                GetClientRect(hwnd, &rect);
                HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 0)); // change color here
                FillRect(hdc, &rect, bgBrush);
                DeleteObject(bgBrush);

                // Create memory DC for the bitmap
                HDC hMemDC = CreateCompatibleDC(hdc);
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, s_hBitmap);

                // Get image size
                BITMAP bmp;
                GetObject(s_hBitmap, sizeof(BITMAP), &bmp);
                int imgW = bmp.bmWidth;
                int imgH = bmp.bmHeight;

                // Get window size
                int winW = rect.right - rect.left;
                int winH = rect.bottom - rect.top;

                // Compute aspect-ratio-fit size
                float imgAspect = (float)imgW / imgH;
                float winAspect = (float)winW / winH;

                int drawW, drawH;
                if (winAspect > imgAspect) {
                    drawH = winH;
                    drawW = (int)(imgAspect * winH);
                } else {
                    drawW = winW;
                    drawH = (int)(winW / imgAspect);
                }

                int x = (winW - drawW) / 2;
                int y = (winH - drawH) / 2;

                // Draw scaled bitmap
                SetStretchBltMode(hdc, HALFTONE);
                StretchBlt(hdc, x, y, drawW, drawH, hMemDC, 0, 0, imgW, imgH, SRCCOPY);

                // Cleanup
                SelectObject(hMemDC, hOldBitmap);
                DeleteDC(hMemDC);
                EndPaint(hwnd, &ps);
            }
            return 0;
        }
        case WM_DESTROY:
            if (s_hBitmap) {
                DeleteObject(s_hBitmap);
                s_hBitmap = NULL;
            }
            for (int i = 0; i < _fileCount; i++) {
			    delete[] _files[i];
			    _files[i] = NULL;
			}
			delete[] _files;
			_files = NULL;
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
void ImageViewer::Show(){
	Next(false);
}
void ImageViewer::Next(bool reverse){
	
	if(reverse) _curImgIndex--;
	else _curImgIndex++;
	
	if(_curImgIndex==-1){
		_curImgIndex = 0;
		return;	
	} 
	if(_curImgIndex==_fileCount){
		_curImgIndex = _fileCount-1;
		return;	
	}
	
	DWORD dataSize = 0;
	char outFileName[_MAX_PATH];

	const char* workingFile = _files[_curImgIndex];
	unsigned char* fileData = NULL;
	if(_oldCrypt) fileData = splitEmbedDataOld(workingFile, _pwd, false, dataSize, outFileName);
	else fileData = splitEmbedData(workingFile, _pwd,false, dataSize, outFileName);
	if(fileData!=NULL)
	{
		Show(fileData, dataSize, outFileName);
	}
	
}
void ImageViewer::Show(unsigned char* jpegData, int jpegSize, const char* title) {
	
	if (s_hBitmap) {
        DeleteObject(s_hBitmap);
        s_hBitmap = NULL;
    }
    
    s_hBitmap = LoadImageFromMemory(jpegData, jpegSize);
    
    if (!s_hBitmap){
    	MessageBox(NULL, "Invalid image", "Info Example", MB_OK | MB_ICONINFORMATION);
    	return;
	}
	SetWindowText(_hWnd, title);
	InvalidateRect(_hWnd, NULL, TRUE);
	UpdateWindow(_hWnd);
	
}
void ImageViewer::ToggleFullscreen(HWND hwnd) {
    static DWORD savedStyle;
    static DWORD savedExStyle;

    if (!_isFullscreen) {
        // Save current window info
        GetWindowRect(hwnd, &_windowedRect);
        savedStyle = GetWindowLong(hwnd, GWL_STYLE);
        savedExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

        // Remove borders and go topmost
        SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);
        SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);

        // Force fullscreen dimensions
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);

        SetWindowPos(hwnd, HWND_TOPMOST,
                     0, 0, screenW, screenH,
                     SWP_SHOWWINDOW | SWP_FRAMECHANGED);
        _isFullscreen = true;
    } else {
        // Restore style and position
        SetWindowLong(hwnd, GWL_STYLE, savedStyle);
        SetWindowLong(hwnd, GWL_EXSTYLE, savedExStyle);

        SetWindowPos(hwnd, HWND_NOTOPMOST,
                     _windowedRect.left, _windowedRect.top,
                     _windowedRect.right - _windowedRect.left,
                     _windowedRect.bottom - _windowedRect.top,
                     SWP_SHOWWINDOW | SWP_FRAMECHANGED);
        _isFullscreen = false;
    }
}
