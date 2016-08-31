#pragma once
#include "hdrSet.h"

char *EncryptData(
	LPTSTR DecryptData,
	LPTSTR szPassword
);

void HandleError(
	LPTSTR psz,
	int nErrorNumber
);

char *DecryptData(
	LPTSTR EncryptData,
	LPTSTR szPassword
);