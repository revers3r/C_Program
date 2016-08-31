#include "vmReg.h"
#include "VMprocess.h"

HKEY OpenKey(HKEY hKey, const wchar_t* SubKeyName) {
	HKEY hSubKey = NULL;
	REGSAM sam = KEY_ALL_ACCESS;
	DWORD ret = RegOpenKeyExW(hKey, SubKeyName, 0, sam, &hSubKey);

	if (ret != ERROR_SUCCESS) {
		error_log	"RegOpenKeyExW(%ws) fail.., ret = %u", SubKeyName, ret		error_end
		return NULL;
	}
	return hSubKey;
}

bool CloseKey(HKEY hKey) {
	DWORD ret = RegCloseKey(hKey);
	if (ret != ERROR_SUCCESS) {
		error_log		"RegCloseKey(hKey) is fail.."		error_end
			return false;
	}
	return true;
}

void dbSetup() {
	RegDB *regist;
	HKEY key = HKEY_CLASSES_ROOT;
	regist = parseDatabase(key);
	for (int i = 0; i < regist->num; i++) {
		if (OpenKey(key, MvMultiByteTChar(regist->strg[i]))) {

		}
	}
}