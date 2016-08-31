#include "VMprocess.h"
#include "vmReg.h"
#include "DBase.h"

int main() {
	/* First Step. Checking Process List */
	HKEY hKey = NULL;
	int serviceCheck = CheckProcessList();
	if (serviceCheck == 0) {
		printf("[*] Message : VM Services detected!!\n");
	}
	/* Second Step, Checking Registery from VMware System */
	// dbSetup();
}