#include "VMprocess.h"
#include "vmdb.h"
#include "hdrSet.h"

void PrintProcessList
(
	DWORD processid
) {
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ, FALSE, processid);
	TCHAR Buffer[MAX_PATH] = { 0 };
	//char ctemp[MAX_PATH];
	if (handle) {
		GetModuleFileNameEx(handle, 0, Buffer, MAX_PATH);
		//WideCharToMultiByte(CP_ACP, 0, Buffer, MAX_PATH, ctemp, MAX_PATH, NULL, NULL);
		CloseHandle(handle);
		_tprintf(_T("[*] %s [pid : %d]\n"), Buffer, processid);
	}
}

unsigned int CheckProcessList() {
	DWORD aProcess[1024], cbNeeded, cProcesses;
	unsigned int i;
	if (!EnumProcesses(aProcess, sizeof(aProcess), &cbNeeded)) {
		error_log	"Enumerate Process is failed!!\n"		error_end
		_exit(0);
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for (int i = 0; i < cProcesses; i++) {
		int flag = Filter(aProcess[i]);
		if (flag == 1) {
			return 0;
		}
	}
	return 1;
}

int Filter(
	DWORD pid
) {
	vmdb *vdb = (vmdb *)malloc(sizeof(vmdb));
	TCHAR sProcessName[MAX_PATH] = { 0 };
	char ctemp[MAX_PATH] = { 0 };

	vdb = SetVariable();
	HANDLE hHandle = OpenProcess(
		PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ, FALSE, pid);
	if (hHandle) {
		GetModuleFileNameEx(hHandle, 0, sProcessName, MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, sProcessName, MAX_PATH, ctemp, MAX_PATH, NULL, NULL);
		CloseHandle(hHandle);
		for (int i = 0; i < vdb->num; i++) {
			if (strstr(ctemp, vdb->vm_data[i])) {
				free(vdb);
				return 1;
			}
		}
	}
	free(vdb);
	return 0;
}

vmdb *SetVariable() {
	vmdb *db = (vmdb *)malloc(sizeof(vmdb));
	char **strg;
	FILE *fp = fopen("ftr.db", "r");
	char *str = (char *)malloc(sizeof(char) * 256);
	if (fp == NULL) {
		error_log		"[*] Can't Find Database File\n"		error_end
			_exit(0);
	}
	memset(str, 0, 256);
	db->num = getNumberOfData("ftr.db");
	if (strg = (char **)malloc(sizeof(char *) * db->num)) {
		for (int i = 0; i < db->num; i++) {
			strg[i] = (char *)malloc(sizeof(char) * 256);
			fgets(strg[i], 256, fp);
		}
	}
	db->vm_data = strg;
	fclose(fp);
	return db;
}

int getNumberOfData(char *dbPath) {
	FILE *fp = fopen(dbPath, "r");
	int ch = 0;
	unsigned int line = 0;
	if (fp == NULL) {
		error_log		"[*] Can't Find Database File\n"		error_end
			_exit(0);
	}
	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '\n')
			line++;
	}
	fclose(fp);
	return line;
}