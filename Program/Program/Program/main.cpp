#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>
#include <tchar.h>

void PrintProcessList(DWORD processid);

int main()
{
	DWORD aProcess[1024], cbNeeded, cProcesses;
	unsigned int i;
	if (!EnumProcesses(aProcess, sizeof(aProcess), &cbNeeded)) {
		printf("Enumerate Process is failed!!\n");
		_exit(0);
	}

	cProcesses = cbNeeded / sizeof(DWORD);
	for (i = 0; i < cProcesses; i++) {
		if (aProcess[i] != 0) {
			PrintProcessList(aProcess[i]);
		}
	}
	getchar();
}

void PrintProcessList(DWORD processid) {
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