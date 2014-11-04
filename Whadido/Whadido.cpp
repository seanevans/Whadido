// Whadido.cpp : Defines the entry point for the console application.
// 

#include "stdafx.h"
#include "Windows.h"
//#include "Windowsx.h"
#include <stdio.h> 

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

int CaptureAnImage(POINT pt)
{
	HWND hWnd = WindowFromPoint(pt);
	HDC hdcScreen;
	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;

	// Retrieve the handle to a display device context for the client 
	// area of the window. 
	hdcScreen = GetDC(NULL);
	hdcWindow = GetDC(hWnd);

	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMemDC = CreateCompatibleDC(hdcWindow);

	if (!hdcMemDC)
	{
		MessageBox(hWnd, L"CreateCompatibleDC has failed", L"Failed", MB_OK);
		goto done;
	}

	// Get the client area for size calculation
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	//This is the best stretch mode
	SetStretchBltMode(hdcWindow, HALFTONE);

	//The source DC is the entire screen and the destination DC is the current window (HWND)
	/*if (!StretchBlt(hdcWindow,
		0, 0,
		rcClient.right, rcClient.bottom,
		hdcScreen,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		SRCCOPY))
	{
		MessageBox(hWnd, L"StretchBlt has failed", L"Failed", MB_OK);
		goto done;
	}*/

	// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

	if (!hbmScreen)
	{
		MessageBox(hWnd, L"CreateCompatibleBitmap Failed", L"Failed", MB_OK);
		goto done;
	}

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMemDC, hbmScreen);

	// Bit block transfer into our compatible memory DC.
	if (!BitBlt(hdcMemDC,
		0, 0,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		hdcWindow,
		0, 0,
		SRCCOPY))
	{
		MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
		goto done;
	}

	// Get the BITMAP from the HBITMAP
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
	// have greater overhead than HeapAlloc.
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char *lpbitmap = (char *)GlobalLock(hDIB);

	// Gets the "bits" from the bitmap and copies them into a buffer 
	// which is pointed to by lpbitmap.
	GetDIBits(hdcWindow, hbmScreen, 0,
		(UINT)bmpScreen.bmHeight,
		lpbitmap,
		(BITMAPINFO *)&bi, DIB_RGB_COLORS);

	// A file is created, this is where we will save the screen capture.
	HANDLE hFile = CreateFile(L"captureqwsx.bmp",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;

	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = 0x4D42; //BM   

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	//Close the handle for the file that was created
	CloseHandle(hFile);

	//Clean up
done:
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);
	ReleaseDC(hWnd, hdcWindow);

	return 0;
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		if (wParam == WM_RBUTTONDOWN) printf("right mouse down [%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		if (wParam == WM_RBUTTONUP) printf("right mouse up[%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		if (wParam == WM_LBUTTONDOWN) printf("left mouse down[%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		if (wParam == WM_LBUTTONUP)
		{
			POINT pt = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt;
			printf("left mouse up[%d][%d]\n", pt.x, pt.y);
			CaptureAnImage(pt);

		}
		if (wParam == WM_MBUTTONDOWN) printf("middle mouse down [%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		if (wParam == WM_MBUTTONUP) printf("middle mouse up [%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		//if (wParam == WM_MOUSEMOVE) printf("mouse move [%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		if (wParam == WM_MOUSEWHEEL) printf("mouse vertical wheel [%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		if (wParam == WM_MOUSEHWHEEL) printf("mouse horizontal wheel [%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		//How to we hook into mice with thumb buttons?
	}
	return CallNextHookEx(0, nCode, wParam, lParam);
}

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		Beep(750, 300);
		return FALSE;

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		Beep(600, 200);
		printf("Ctrl-Close event\n\n");
		return(TRUE);

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		Beep(900, 200);
		printf("Ctrl-Break event\n\n");
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		Beep(1000, 200);
		printf("Ctrl-Logoff event\n\n");
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		Beep(750, 500);
		printf("Ctrl-Shutdown event\n\n");
		return FALSE;

	default:
		return FALSE;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		printf("\nThe Control Handler is installed.\n");
		printf("\n -- Now try pressing Ctrl+C or Ctrl+Break, or");
		printf("\n    try logging off or closing the console...\n");
		printf("\n(...waiting in a loop for events...)\n\n");

		HHOOK mousehook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0);
		while (CtrlHandler) {
			MSG msg;
			if (GetMessage(&msg, 0, 0, 0)) {
				printf("msg recvd\n");
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	else
	{
		printf("\nERROR: Could not set control handler");
		return 1;
	}

	
	return 0;
}



