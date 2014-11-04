// Whadido.cpp : Defines the entry point for the console application.
// 

#include "stdafx.h"
#include "Windows.h"
//#include "Windowsx.h"
#include <stdio.h> 

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		if (wParam == WM_RBUTTONDOWN) printf("right mouse down [%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		if (wParam == WM_RBUTTONUP) printf("right mouse up[%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		if (wParam == WM_LBUTTONDOWN) printf("left mouse down[%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
		if (wParam == WM_LBUTTONUP) printf("left mouse up[%d][%d]\n", reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.x, reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt.y);
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



