#pragma once
#include "hdrSet.h"

#define LOG_ERROR		3
void Set_Error_Fmt(
	_In_ uint32_t level,
	_In_z_ const char* function,
	_In_z_ const char* fmt,
	_In_ ...
);;
#define error_log		Set_Error_Fmt(LOG_ERROR, __FUNCTION__, 
#define error_end		);