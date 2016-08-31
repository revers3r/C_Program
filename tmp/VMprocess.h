#pragma once
#include "hdrSet.h"
#include "vmdb.h"

void PrintProcessList(DWORD);
unsigned int CheckProcessList();

int Filter(DWORD);
vmdb *SetVariable();
int getNumberOfData(char *);