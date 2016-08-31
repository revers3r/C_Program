#include<iostream>
#include<string>
#include<Windows.h>

#pragma warning(disable:4996)

BOOL CenterWindow(HWND hWndWindow) {
	HWND hWndParent;
	RECT rectWindow, rectParent;

	if((hWndParent = GetParent(hWndWindow)) != NULL) {
		GetWindowRect(hWndWindow, &rectWindow);
		GetWindowRect(hWndParent, &rectParent);

		int nWidth = rectWindow.right - rectWindow.left;
		int nHeight = rectWindow.bottom - rectWindow.top;

		int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
		int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;

		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (nX < 0) nX = 0;
		if (nY < 0) nY = 0;
		if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
		if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

		MoveWindow(hWndWindow, nX, nY, nWidth, nHeight, FALSE);

		return TRUE;
	}
	return FALSE;
}