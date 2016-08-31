#include "my_util.h"

char *MvCharTMultiByte(LPCWCH str) {
	char ctemp[2048] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, str, MAX_PATH, ctemp, MAX_PATH, NULL, NULL);
	return ctemp;
}

wchar_t *MvMultiByteTChar(char *str) {
	wchar_t temp[2048] = { 0 };
	int nLen = MultiByteToWideChar(CP_ACP, 0, str, strlen(str), NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str), temp, nLen);

	return temp;
}