#include<Windows.h>
#include<cstring>
#include<iostream>
#include<string>
#include<cctype>
#include<CommCtrl.h>
#include<wincrypt.h>
#include<cstdlib>
#include<fstream>
#include<winnt.h>
#include<memory>
#include<WinUser.h>
#include<stddef.h>
#include<gdiplus.h>
#include "SetCenter.h"
#include "md5.h"

#pragma warning(disable:4996)

#define fbutton 501
#define xbutton 502
#define obutton 506
#define STATIC1 503
#define STATIC2 504
#define STATIC3 505
#define FILENAME 700
#define FILESIZE 701
#define FORMAT 1001
#define FORMAT2 1002
#define LIST 7000
#define DOSSHOW 7003
#define DOSHEADER 2001
#define FILEHEADER 2002
#define OPTIONALHEADER 2003
#define SECTIONHEADER 2004
#define HEADER2 3000
#define NAME2 3001
#define OFFSET2 3002
#define VALUE2 3003

IMAGE_DOS_HEADER gImageDosHeader;
IMAGE_NT_HEADERS gImageNtHeader;
IMAGE_OPTIONAL_HEADER gImageOptionalHeader;
PIMAGE_SECTION_HEADER g_pImageSectionHeader;

OPENFILENAME OFN;
char str[65535];
char lpstrFile[MAX_PATH]="";
FILE *fp, *f;
HWND statusbar, Fbutton, Xbutton, hStatic1, hFName, 
	hStatic2, hSize, hStatic3, hCheckSum, OButton, hFormat, hFormat2;
HWND NewWindow, hList, hDosShow, hHeader, hName, hValue, hOffset;
HWND hHeader2, hOffset2, hName2, hValue2;
POINT point;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass=TEXT("PEViewer");
HWND selectbutton;
std::string bin2hex(const std::string&);
std::string md5(const std::string strMd5);
BOOL DeleteFileOptionAll();
BOOL DeleteObjectAll();
BOOL Echo_Dos_Header();
BOOL Echo_Nt_Header();
BOOL Echo_Optional_Header();
BOOL Echo_Section_Header();
LRESULT EditColor(HDC, HWND);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);
	
	hWnd = CreateWindow(lpszClass, lpszClass, CW_USEDEFAULT,CW_USEDEFAULT, CW_USEDEFAULT, 520,
		310, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	CenterWindow(GetDesktopWindow());
	RECT rc;
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	GetWindowRect(hWnd, &rc);
	SetWindowPos(hWnd, 0, (screenWidth - rc.right)/2, (screenHeight - rc.bottom)/2,
		0, 0, SWP_NOZORDER|SWP_NOSIZE);
	while(GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	HBITMAP hBit, hBit2;
	BITMAP bit;
	HDC hmem;
	char *hash = (char *)malloc(sizeof(char)*33);
	char *str = (char *)malloc(sizeof(char)*40000);
	char format[6];
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	std::string::size_type pos = std::string(path).find_last_of("\\/");
	std::string finpath = std::string(path).substr(0, pos) + "\\background.jpg";
	static int selectnum;
	
	char *format2 = (char *)malloc(sizeof(char)*70);
	char strList[128];
	memset(format2, 0, sizeof(char)*70);
	std::string hextmp;
	switch(iMessage) {
	case WM_CREATE:{
		if(FindWindow("OLLYDBG", 0)) {
			exit(0);
		} else if(FindWindow("idaq", 0)) {
			exit(0);
		}
		statusbar = CreateWindow(TEXT("edit"), TEXT("https:\/\/www.facebook.com\/profile.php?id=100007043104885"), WS_BORDER | WS_CHILD | WS_VISIBLE | ES_READONLY,
			120, 290, 400, 20, hWnd, (HMENU)NULL, NULL, NULL);
		Fbutton = CreateWindow(TEXT("BUTTON"), TEXT("File"), WS_BORDER | WS_CHILD | WS_VISIBLE | BS_PUSHLIKE, 
			439, 0, 60, 21, hWnd, (HMENU)fbutton, NULL, NULL);
		Xbutton = CreateWindow(TEXT("BUTTON"), TEXT("X"), WS_BORDER | WS_CHILD | WS_VISIBLE | BS_PUSHLIKE | BS_AUTOCHECKBOX, 
			498, 0, 21, 21, hWnd, (HMENU)xbutton, NULL, NULL);
		return 0;
				   }
	case WM_CTLCOLOREDIT: {
		LRESULT r;
		if(r=EditColor((HDC)wParam, (HWND)lParam)) return r;
		break;
						  }
	case WM_RBUTTONDOWN: {
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);
		HMENU hPopupMenu = CreatePopupMenu();
		InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, DOSHEADER, TEXT("Dos Header"));
		InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, FILEHEADER, TEXT("Nt Header"));
		InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, OPTIONALHEADER, TEXT("Optional Header"));
		ClientToScreen(hWnd, &point);
		SetForegroundWindow(hWnd);
		TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, point.x, point.y, 0, hWnd, NULL);
		return 0;
						 }
	case WM_COMMAND: {
		if(LOWORD(wParam) == fbutton) {
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWnd;
			OFN.lpstrFilter = "Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0";
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = 256;
			OFN.lpstrInitialDir="c:\\";
			if (GetOpenFileName(&OFN) != 0) {
				DeleteFileOptionAll();
				DeleteObjectAll();
				fp = fopen(OFN.lpstrFile, "rb");
				f = fopen(OFN.lpstrFile, "r");
				fseek(fp, 0, SEEK_END);
				fgets(format, 6, f); 
				hextmp = md5(OFN.lpstrFile);
				hash = const_cast<char *>(hextmp.c_str());
				if(strstr(format, "MZ") != NULL) {
					format2 = "PE(Supported)";
					Echo_Dos_Header();
					Echo_Nt_Header();
					Echo_Optional_Header();
					Echo_Section_Header();
				} else if(strstr(format, "ELF") != NULL) {
					format2 = "ELF(Unsupported: To be added)";
				}
				hStatic1 = CreateWindow(TEXT("static"), TEXT("File name"), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 
					10, 100, 100, 20, hWnd, (HMENU)STATIC1, NULL, NULL);
				hFName = CreateWindow(TEXT("static"), TEXT(OFN.lpstrFile), WS_CHILD | WS_VISIBLE | WS_BORDER, 
					110, 100, 400, 20, hWnd, (HMENU)FILENAME, NULL, NULL);
				hStatic2 = CreateWindow(TEXT("static"), TEXT("Size"), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 
					10, 120, 100, 20, hWnd, (HMENU)STATIC2, NULL, NULL);
				hSize = CreateWindow(TEXT("static"), TEXT(std::to_string(ftell(fp)).c_str()), WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 120, 400, 20, hWnd, (HMENU)FILESIZE,
					NULL, NULL);
				hStatic3 = CreateWindow(TEXT("static"), TEXT("Hash"), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 
					10, 140, 100, 20, hWnd, (HMENU)STATIC3, NULL, NULL);
				hCheckSum = CreateWindow(TEXT("static"), TEXT(hash), WS_CHILD | WS_VISIBLE | WS_BORDER, 
					110, 140, 400, 20, hWnd, (HMENU)FILESIZE, NULL, NULL);
				hFormat = CreateWindow(TEXT("static"), TEXT("Format"), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 
					10, 160, 100, 20, hWnd, (HMENU)FORMAT, NULL, NULL);
				hFormat2 = CreateWindow(TEXT("static"), TEXT(format2), WS_CHILD | WS_VISIBLE | WS_BORDER, 
					110, 160, 400, 20, hWnd, (HMENU)FORMAT2, NULL, NULL);
				SetTextColor((HDC)Fbutton, RGB(100, 100, 100));
			}
			return 0;
		} else if(LOWORD(wParam) == xbutton) {
			PostQuitMessage(0);
		} else if(LOWORD(wParam) == DOSHEADER) {
				DeleteObjectAll();
				hList = CreateWindow(TEXT("listbox"), NULL, 
					WS_CHILD | WS_VISIBLE | LBS_STANDARD, 10, 190, 200, 96, 
					hWnd, (HMENU)LIST, g_hInst, NULL);
				hHeader = CreateWindow(TEXT("static"), TEXT("Header"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 190, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hName = CreateWindow(TEXT("static"), TEXT("Name"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 212, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hOffset = CreateWindow(TEXT("static"), TEXT("Offset"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 234, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hValue = CreateWindow(TEXT("static"), TEXT("Value"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 256, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hHeader2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 190, 215, 20,
					hWnd, (HMENU)HEADER2, g_hInst, NULL);
				hName2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 212, 215, 20,
					hWnd, (HMENU)NAME2, g_hInst, NULL);
				hOffset2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 234, 215, 20,
					hWnd, (HMENU)OFFSET2, g_hInst, NULL);
				hValue2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 256, 215, 20,
					hWnd, (HMENU)VALUE2, g_hInst, NULL);
				SetWindowText(hHeader2, TEXT("DOS_HEADER"));
				char dosarr[] = "e_magic;"
					"e_cblp;"
					"e_cp;"
					"e_crlc;"
					"e_cparhdr;"
					"e_minalloc;"
					"e_maxalloc;"
					"e_ss;"
					"e_sp;"
					"e_csum;"
					"e_ip;"
					"e_cs;"
					"e_lfarlc;"
					"e_ovno;"
					"e_oemid;"
					"e_oeminfo;"
					"e_lfanew;";
				const char coln[] = ";";
				char *token = NULL;
				token = strtok(dosarr, coln);
				while(token != NULL) {
					SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)token);
					token = strtok(NULL, coln);
				}
				return 0;
		} else if(LOWORD(wParam) == FILEHEADER) {
				DeleteObjectAll();
				hList = CreateWindow(TEXT("listbox"), NULL, 
					WS_CHILD | WS_VISIBLE | LBS_STANDARD, 10, 190, 200, 96, 
					hWnd, (HMENU)LIST, g_hInst, NULL);
				hHeader = CreateWindow(TEXT("static"), TEXT("Header"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 190, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hName = CreateWindow(TEXT("static"), TEXT("Name"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 212, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hOffset = CreateWindow(TEXT("static"), TEXT("Offset"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 234, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hValue = CreateWindow(TEXT("static"), TEXT("Value"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 256, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hHeader2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 190, 215, 20,
					hWnd, (HMENU)HEADER2, g_hInst, NULL);
				hName2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 212, 215, 20,
					hWnd, (HMENU)NAME2, g_hInst, NULL);
				hOffset2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 234, 215, 20,
					hWnd, (HMENU)OFFSET2, g_hInst, NULL);
				hValue2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 256, 215, 20,
					hWnd, (HMENU)VALUE2, g_hInst, NULL);
				SetWindowText(hHeader2, TEXT("NT_HEADER"));
				char dosarr[] = "Signature;"
					"Machine;"
					"NumberOfSections;"
					"TimeDateStamp;"
					"PointerToSymbolTable;"
					"NumberOfSymbols;"
					"SizeOfOptionalHeader;"
					"Characteristics;";
				const char coln[] = ";";
				char *token = NULL;
				token = strtok(dosarr, coln);
				while(token != NULL) {
					SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)token);
					token = strtok(NULL, coln);
				}
				return 0;
		} else if(LOWORD(wParam) == OPTIONALHEADER) {
				DeleteObjectAll();
				hList = CreateWindow(TEXT("listbox"), NULL, 
					WS_CHILD | WS_VISIBLE | LBS_STANDARD, 10, 190, 200, 96, 
					hWnd, (HMENU)LIST, g_hInst, NULL);
				hHeader = CreateWindow(TEXT("static"), TEXT("Header"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 190, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hName = CreateWindow(TEXT("static"), TEXT("Name"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 212, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hOffset = CreateWindow(TEXT("static"), TEXT("Offset"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 234, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hValue = CreateWindow(TEXT("static"), TEXT("Value"),
					WS_CHILD | WS_VISIBLE | SS_CENTER, 230, 256, 60, 20,
					hWnd, NULL, g_hInst, NULL);
				hHeader2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 190, 215, 20,
					hWnd, (HMENU)HEADER2, g_hInst, NULL);
				hName2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 212, 215, 20,
					hWnd, (HMENU)NAME2, g_hInst, NULL);
				hOffset2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 234, 215, 20,
					hWnd, (HMENU)OFFSET2, g_hInst, NULL);
				hValue2 = CreateWindow(TEXT("static"), NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 295, 256, 215, 20,
					hWnd, (HMENU)VALUE2, g_hInst, NULL);
				SetWindowText(hHeader2, TEXT("OPTIONAL_HEADER"));
				char dosarr[] = "Magic;"
					"MajorLinkerVersion;"
					"MinorLinkerVersion;"
					"SizeOfCode;"
					"SizeOfInitializedData;"
					"SizeOfUninitializedData;"
					"AddressOfEntryPoint;"
					"BaseOfCode;"
					"BaseOfdata;"
					"ImageBase;"
					"SectionAlignment;"
					"FileAlignment;"
					"MajorOperatingSystemVersion;"
					"MinorOperatingSystemVersion;"
					"MajorImageVersion;"
					"MinorImageVersion;"
					"MajorSubsystemVersion;"
					"MinorSubsystemVersion;"
					"Win32VersionValue;"
					"SizeOfImage;"
					"SizeOfHeaders;"
					"CheckSum;"
					"Subsystem;"
					"DllCharacteristics;"
					"SizeOfStackReserve;"
					"SizeOfStackCommint;"
					"SizeOfHeapReserve;"
					"SizeOfHeapCommit;"
					"LoaderFlags;"
					"NumberOfRvaAndSizes;";
				const char coln[] = ";";
				char *token = NULL;
				token = strtok(dosarr, coln);
				while(token != NULL) {
					SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)token);
					token = strtok(NULL, coln);
				}
				return 0;
					 }
					 }
	case LIST: {
		int offset = 0;
		char *offset2 = (char *)malloc(128);
		memset(offset2, 0, 128);
		int value = 0;
		char *value2 = (char *)malloc(128);
		memset(value2, 0, 128);
		if(HIWORD(wParam) == LBN_SELCHANGE) {
			memset(strList, 0, 128);
			selectnum = SendMessage(hList, LB_GETCURSEL, 0, 0);
			SendMessage(hList, LB_GETTEXT, selectnum, (LPARAM)strList);
			SetWindowText(hName2, strList);
			if(strcmp(strList, "e_magic") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_magic);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_magic;
				sprintf(value2, "0x%04X [MZ]", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_cblp") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_cblp);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_cblp;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_cp") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_cp);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_cp;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_crlc") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_crlc);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_crlc;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_cparhdr") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_cparhdr);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_cparhdr;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_minalloc") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_minalloc);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_minalloc;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_maxalloc") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_maxalloc);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_maxalloc;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_ss") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_ss);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_ss;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_sp") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_sp);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_sp;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_csum") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_csum);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_csum;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_ip") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_ip);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_ip;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_cs") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_cs);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_cs;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_lfarlc") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_lfarlc);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_lfarlc;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_ovno") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_ovno);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_ovno;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_oemid") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_oemid);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_oemid;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_oeminfo") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_oeminfo);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_oeminfo;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "e_lfanew") == 0) {
				offset = offsetof(IMAGE_DOS_HEADER, e_lfanew);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageDosHeader.e_lfanew;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "Signature") == 0) {
				offset = offsetof(IMAGE_NT_HEADERS, Signature);
				sprintf(offset2, "%08d", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.Signature;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "Machine") == 0) {
				offset = offsetof(IMAGE_FILE_HEADER, Machine) + (gImageDosHeader.e_lfanew + sizeof(DWORD));
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.FileHeader.Machine;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "NumberOfSections") == 0) {
				offset = offsetof(IMAGE_FILE_HEADER, NumberOfSections) + (gImageDosHeader.e_lfanew + sizeof(DWORD));
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.FileHeader.NumberOfSections;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "TimeDateStamp") == 0) {
				offset = offsetof(IMAGE_FILE_HEADER, TimeDateStamp) + (gImageDosHeader.e_lfanew + sizeof(DWORD));
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.FileHeader.TimeDateStamp;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "PointerToSymbolTable") == 0) {
				offset = offsetof(IMAGE_FILE_HEADER, PointerToSymbolTable) + (gImageDosHeader.e_lfanew + sizeof(DWORD));
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.FileHeader.PointerToSymbolTable;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "NumberOfSymbols") == 0) {
				offset = offsetof(IMAGE_FILE_HEADER, NumberOfSymbols) + (gImageDosHeader.e_lfanew + sizeof(DWORD));
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.FileHeader.NumberOfSymbols;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SizeOfOptionalHeader") == 0) {
				offset = offsetof(IMAGE_FILE_HEADER, SizeOfOptionalHeader) + (gImageDosHeader.e_lfanew + sizeof(DWORD));
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.FileHeader.SizeOfOptionalHeader;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "Characteristics") == 0) {
				offset = offsetof(IMAGE_FILE_HEADER, Characteristics) + (gImageDosHeader.e_lfanew + sizeof(DWORD));
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.FileHeader.Characteristics;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "Magic") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, Magic) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.Magic;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "MajorLinkerVersion") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, MajorLinkerVersion) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.MajorLinkerVersion;
				sprintf(value2, "0x%02X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "MinorLinkerVersion") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, MinorLinkerVersion) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.MinorLinkerVersion;
				sprintf(value2, "0x%02X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SizeOfCode") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, SizeOfCode) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.SizeOfCode;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SizeOfInitializedData") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, SizeOfInitializedData) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.SizeOfInitializedData;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SizeOfUninitializedData") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, SizeOfUninitializedData) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.SizeOfUninitializedData;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "AddressOfEntryPoint") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, AddressOfEntryPoint) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.AddressOfEntryPoint;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "BaseOfCode") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, BaseOfCode) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.BaseOfCode;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "BaseOfData") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, BaseOfData) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.BaseOfData;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "ImageBase") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, ImageBase) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.ImageBase;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SectionAlignment") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, SectionAlignment) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.SectionAlignment;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "FileAlignment") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, FileAlignment) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.FileAlignment;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "MajorOperatingSystemVersion") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, MajorOperatingSystemVersion) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.MajorOperatingSystemVersion;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "MinorOperatingSystemVersion") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, MinorOperatingSystemVersion) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.MinorOperatingSystemVersion;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "MajorImageVersion") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, MajorImageVersion) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.MajorImageVersion;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "MinorImageVersion") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, MinorImageVersion) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.MinorImageVersion;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "MajorSubsystemVersion") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, MajorSubsystemVersion) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.MajorSubsystemVersion;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "MinorSubsystemVersion") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, MinorSubsystemVersion) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.MinorSubsystemVersion;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "Win32VersionValue") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, Win32VersionValue) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.Win32VersionValue;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SizeOfImage") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, SizeOfImage) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.SizeOfImage;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SizeOfHeaders") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, SizeOfHeaders) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.SizeOfHeaders;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "CheckSum") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, CheckSum) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.CheckSum;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "Subsystem") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, Subsystem) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.Subsystem;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "DllCharacteristics") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, DllCharacteristics) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.DllCharacteristics;
				sprintf(value2, "0x%04X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SizeOfStackReserve") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, SizeOfStackReserve) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.SizeOfStackReserve;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SizeOfStackCommit") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, SizeOfStackCommit) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.SizeOfStackCommit;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SizeOfHeapReserve") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, SizeOfHeapReserve) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.SizeOfHeapReserve;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "SizeOfHeapCommit") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, SizeOfHeapCommit) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.SizeOfHeapCommit;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "LoaderFlags") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, LoaderFlags) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.LoaderFlags;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			} else if(strcmp(strList, "NumberOfRvaAndSizes") == 0) {
				offset = offsetof(IMAGE_OPTIONAL_HEADER, NumberOfRvaAndSizes) + (gImageDosHeader.e_lfanew + sizeof(DWORD)) + sizeof(IMAGE_FILE_HEADER);
				sprintf(offset2, "%08X", offset);
				SetWindowText(hOffset2, offset2);
				value = gImageNtHeader.OptionalHeader.NumberOfRvaAndSizes;
				sprintf(value2, "0x%08X", value);
				SetWindowText(hValue2, value2);
			}
		} 
		break;
			   }
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(::DefWindowProc(hWnd, iMessage, wParam, lParam));
}
std::string bin2hex(const std::string& input) {
	std::string res;
	const char hex[] = "0123456789ABCDEF";
	for(auto sc : input) {
		unsigned char c = static_cast<unsigned char>(sc);
		res += hex[c >> 4];
		res += hex[c & 0xf];
	}
	return res;
}
std::string md5(const std::string strMd5) {
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)strMd5.c_str(), strMd5.length());
	md5_finish(&state, digest);

	for(di = 0; di < 16; ++di) {
		sprintf(hex_output + di * 2, "%02x", digest[di]);
	}
	return hex_output;
}

BOOL DeleteFileOptionAll() {
	DestroyWindow(hStatic1);
	DestroyWindow(hFName);
	DestroyWindow(hStatic2);
	DestroyWindow(hSize);
	DestroyWindow(hStatic3);
	DestroyWindow(hCheckSum);
	DestroyWindow(hFormat);
	DestroyWindow(hFormat2);
	return true;
}

BOOL DeleteObjectAll() {
	DestroyWindow(hList);
	DestroyWindow(hHeader);
	DestroyWindow(hHeader2);
	DestroyWindow(hName);
	DestroyWindow(hName2);
	DestroyWindow(hOffset);
	DestroyWindow(hOffset2);
	DestroyWindow(hValue);
	DestroyWindow(hValue2);
	return true;
}

BOOL Echo_Dos_Header() {
	fseek(fp, 0, SEEK_SET);
	if( fread(&gImageDosHeader, sizeof(IMAGE_DOS_HEADER), 1, fp) == 1 ){
		return 1;
	} else {
		return 0;
	}
}

BOOL Echo_Nt_Header() {
	fseek(fp, gImageDosHeader.e_lfanew, SEEK_SET);
	if( fread(&gImageNtHeader, sizeof(IMAGE_NT_HEADERS), 1, fp) == 1 ){
		return 1;
	} else {
		return 0;
	}
}

BOOL Echo_Optional_Header() {
	fseek(fp, (sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS)), SEEK_SET);
	if( fread(&gImageOptionalHeader, sizeof(IMAGE_OPTIONAL_HEADER), 1, fp) == 1 ){
		return 1;
	} else {
		return 0;
	}
}

BOOL Echo_Section_Header()
{
	int nCount;
	g_pImageSectionHeader = (PIMAGE_SECTION_HEADER)malloc(sizeof(IMAGE_SECTION_HEADER) * gImageNtHeader.FileHeader.NumberOfSections);
	fseek(fp, sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_OPTIONAL_HEADER) ,SEEK_SET);

	if(fread(g_pImageSectionHeader, sizeof(IMAGE_SECTION_HEADER), gImageNtHeader.FileHeader.NumberOfSections, fp) == gImageNtHeader.FileHeader.NumberOfSections){
		return 1;
	} else {
		return 0;
	}
}

LRESULT EditColor(HDC hdc, HWND Ctrl) {
	if(Ctrl == statusbar) {
		SetBkMode(hdc, TRANSPARENT);
		SetBkColor(hdc, RGB(128, 128, 128));
		return (LRESULT)GetStockObject(GRAY_BRUSH);
	}
	return 0;
}