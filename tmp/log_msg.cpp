#include "log_msg.h"
void Set_Error_Fmt(
	_In_ uint32_t level,
	_In_z_ const char* function,
	_In_z_ const char* fmt,
	_In_ ...
) {
	if (fmt == NULL) return;
	char buffer[2048];
	size_t szBuffer = sizeof(buffer);
	char* pos = buffer;
	va_list args;

	StringCbPrintfExA(pos, szBuffer, &pos, &szBuffer, 0, "%s", "[*] ERROR ");
	va_start(args, fmt);

	HRESULT hRes = StringCbVPrintfExA(
		pos, szBuffer, &pos, &szBuffer, 0, fmt, args);
	if (hRes != S_OK) {
		StringCbPrintfExA(
			pos, szBuffer,
			&pos, &szBuffer,
			0, "invalid parameters"
		);
	}
	va_end(args);
	StringCbPrintfExA(pos, szBuffer, &pos, &szBuffer, 0, "\n");
	OutputDebugStringA(buffer);
}