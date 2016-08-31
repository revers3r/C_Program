#pragma once
#include "VMprocess.h"
#include "hdrSet.h"
#include "vmReg.h"
#include "encrypt.h"

RegDB *parseDatabase(HKEY RootKey);
typedef struct DatabaseFmt {
	char Signature[6];
	int NumberOfSection;
	int eachSection_Size[4];
	char **Data;
}db_fmt, *pdb_fmt;