#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "user32.lib")

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	int nResult = MessageBox(NULL, "DLL Injected!",
	                         "Life, finds a way.",
							 MB_ICONINFORMATION|MB_OK);
	Sleep(50000);
	return TRUE;
}
