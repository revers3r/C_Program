#pragma once
#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>
#include <stdio.h>
#include <sal.h>
#include <crtdbg.h>
#include <stdarg.h>
#include <conio.h>
#include <strsafe.h>
#include <intsafe.h>
#include <stdint.h>
#include <comdef.h>
#include <iostream>
#include <string>
#include <math.h>
#include <wincrypt.h>

#include "log_msg.h"
#define uint32_t unsigned _int32

#pragma warning(disable:4996)
#pragma warning(disable:4005)
#pragma comment (lib, "advapi32")

/* Encrypting Section */
#define KEY_LENGTH				0x00800000
#define ENCRYPT_ALGORITHM		CALG_RC4
#define ENCRYPT_BLOCK_SIZE		8