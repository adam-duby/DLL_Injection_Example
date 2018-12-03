#include <stdio.h>
#include <windows.h>
#include <TlHelp32.h>
#include <string.h>
#pragma comment(lib, "user32.lib")
#define WIN32_DEFAULT_LIBS

DWORD GetPID()
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (strcmp(entry.szExeFile, "Message.exe") == 0)
			{
				CloseHandle(snapshot); 
				return entry.th32ProcessID;
			}
		}
	}
	CloseHandle(snapshot);
	return 0;
}
/*
INT WINAPI  wWinMain(HINSTANCE hInst,
					HINSTANCE hPrevInst,
					LPWSTR lpCmdLine,
					INT nShowCmd)
					*/
int main(int argc, char* argv[])
{
	char* buffer = "inject.dll";
	// Get process handle passing in the process ID.
	int procID = GetPID();
	printf("\t[Info]:    PID: %d\n", procID);
	if (procID == 0)
		ExitProcess(0);
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	if(process == NULL)
		printf("\t[Error]:   Process with PID %d not found.\n", procID);
	
	// Get address of the LoadLibrary function.
	LPVOID addr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	if(addr == NULL)
		printf("\t[Error]:   Failed to get address of LoadLibraryA.\n");
	
	// Allocate new memory region inside the process's address space.
	LPVOID arg = (LPVOID)VirtualAllocEx(process, NULL, strlen(buffer), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(arg == NULL)
		printf("\t[Error]:   VirtualAllocEx failed.\n");
	
	// Write the argument to LoadLibraryA to the process's newly allocated memory region.
	int n = WriteProcessMemory(process, arg, buffer, strlen(buffer), NULL);
	if(n == 0)
		printf("\t[Error]:   Failed to write to process address space.\n");
	
	// Inject DLL into the process's address space.
	HANDLE threadID = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)addr, arg, NULL, NULL);
	if(threadID == NULL)
		printf("\t[Error]:   Remote thread not created.\n");
	else
		printf("\t[Success]: Remote thread created.\n");
	
	CloseHandle(process);
	ExitProcess(0);
}