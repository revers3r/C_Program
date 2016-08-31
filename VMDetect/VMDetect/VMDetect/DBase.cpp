#include "DBase.h"

RegDB *parseDatabase(HKEY RootKey) {
	RegDB *Reg = (RegDB *)malloc(sizeof(RegistryDB));
	DatabaseFmt *dbf = (DatabaseFmt *)malloc(sizeof(DatabaseFmt));
	FILE *fp = fopen("RegistryDB.dbase", "rb");
	if (fp == NULL) {
		error_log			"[*] Can't Open Database File..\n"			error_end
			return NULL;
	}
	fseek(fp, 0, SEEK_SET);
	fread(dbf->Signature, sizeof(char), 5, fp);					// Read Signature
	printf("%s\n", dbf->Signature);
	getchar();
	return Reg;
}