#pragma once
#include "hdrSet.h"

#define MAX_REGISTRY_KEY_LENGTH		256
#define MAX_VALUE_NAME_LENGTH		16383
HKEY OpenKey(HKEY hKey, const wchar_t* SubKeyName);
bool CloseKey(HKEY hKey);
typedef struct RegistryDB {
	unsigned int num;
	char **strg;
}RegDB;

RegDB *dbSetup();