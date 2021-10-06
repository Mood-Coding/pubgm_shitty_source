#include "pch.h"
#include "ProcessManager.h"

ProcessManager* g_pPM = new ProcessManager;

ProcessManager::ProcessManager() {}

DWORD GetPID(std::wstring processName)
{
	DWORD pidWithHighestThreadCount = 0;
	DWORD highestThreadCount = 0;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnap, &pe);

	while (Process32Next(hSnap, &pe))
	{
		if ((_tcsicmp(pe.szExeFile, processName.c_str()) == 0) && (pe.cntThreads > highestThreadCount))
		{
			highestThreadCount = pe.cntThreads;
			pidWithHighestThreadCount = pe.th32ProcessID;
		}
	}

	return pidWithHighestThreadCount;
}

// Enable/disable privilege routine
bool ProcessManager::SetPrivilege(LPCWSTR lpszPrivilege, BOOL bEnablePrivilege) {
	TOKEN_PRIVILEGES priv = { 0,0,0,0 };
	HANDLE hToken = NULL;
	LUID luid = { 0,0 };
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		if (hToken)
			CloseHandle(hToken);
		return false;
	}
	if (!LookupPrivilegeValueW(0, lpszPrivilege, &luid)) {
		if (hToken)
			CloseHandle(hToken);
		return false;
	}
	priv.PrivilegeCount = 1;
	priv.Privileges[0].Luid = luid;
	priv.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : SE_PRIVILEGE_REMOVED;
	if (!AdjustTokenPrivileges(hToken, false, &priv, 0, 0, 0)) {
		if (hToken)
			CloseHandle(hToken);
		return false;
	}
	if (hToken)
		CloseHandle(hToken);
	return true;
}

bool ProcessManager::Init()
{
	std::cout << "[PM]\n";

	// Get SeDebugPrivilege
	bool bStatus = SetPrivilege(L"SeDebugPrivilege", TRUE);

	if (bStatus)
	{
		std::cout << "The SeDebugPrivilege was enabled!" << std::endl;
	}
	else
	{
		std::cout << "Can't get SeDebugPrivilege! Error: " << GetLastError() << '\n';
		return 0;
	}

	//get target process pid
	PID = GetPID(emuProcessName);
	if (PID)
	{
		std::cout << "Target PID: " << std::dec << PID << std::endl;
	}
	else
	{
		std::wcout << L"Not found process: " << emuProcessName.c_str() << std::endl;
		return false;
	}

	// Get handle of target process to read/ write memory
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, PID);

	if (hProcess)
	{
		std::cout << "Opened target process handle:" << std::hex << hProcess << '\n';
	}
	else
	{
		std::cout << "Couldn't open process. Please Run as administrator permission. Error: " << GetLastError() << '\n';
		return false;
	}

	return true;
}