#include "pch.h"
#include "MemoryManager.h"

MemoryManager* g_pMM = new MemoryManager();

bool MemoryManager::Init()
{
	std::cout << "[MM]\n";

	// Get SeDebugPrivilege
	bool bStatus{ SetPrivilege(L"SeDebugPrivilege", TRUE) };
	if (bStatus)
	{
		std::cout << "The SeDebugPrivilege was enabled!" << std::endl;
	}
	else
	{
		std::cout << "<SetPrivilege> Can't get SeDebugPrivilege! Error: " << GetLastError() << '\n';
		return false;
	}

	// Get emulator PID
	PID = GetPID(emuProcName);
	if (PID)
	{
		std::cout << "Emulator PID: " << std::dec << PID << std::endl;
	}
	else
	{
		std::wcout << L"Not found process: " << emuProcName.c_str() << std::endl;
		return false;
	}

	// Get handle of emulator process to read/ write memory
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, PID);

	if (hProcess)
	{
		std::cout << "Opened emulator process handle:" << std::hex << hProcess << '\n';
	}
	else
	{
		std::cout << "<OpenProcess> Couldn't open process. Please Run as administrator permission. Error: " << GetLastError() << '\n';
		return false;
	}

	/*if (!(LoadDriver() && StartDriver()))
		return false;*/
	
	bStatus = ConnectToDriver("\\Device\\KProcessHacker2");
	if (m_hDriver && bStatus)
	{
		std::cout << "Connected to driver 0x" << std::hex << m_hDriver << '\n';
	}
	else
	{
		std::cout << "<ConnectToDriver> Can't connect to driver" << std::endl;
		return false;
	}

	// Turn off SeDebugPrivilege
	SetPrivilege(L"SeDebugPrivilege", FALSE);

	return true;
}

bool MemoryManager::ConnectToDriver(std::string m_strDeviceName)
{
	std::wstring strObjectName;
	strObjectName.assign(m_strDeviceName.begin(), m_strDeviceName.end());
	UNICODE_STRING objectName;

	HANDLE hNtDriver;
	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK isb;

	RtlInitUnicodeString(&objectName, strObjectName.c_str());
	InitializeObjectAttributes(&objectAttributes, &objectName, FILE_NON_DIRECTORY_FILE, NULL, NULL);

	NTSTATUS status = NtOpenFile(&hNtDriver, FILE_GENERIC_READ | FILE_GENERIC_WRITE, &objectAttributes, &isb, FILE_SHARE_READ, FILE_NON_DIRECTORY_FILE);
	if (status == STATUS_SUCCESS)
	{
		m_hDriver = hNtDriver;
		return true;
	}
	else
	{
		std::cout << "<NtOpenFile> Error: 0x" << std::hex << status << std::endl;
		return false;
	}	
}

void MemoryManager::readMemory(PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize)
{
	struct {
		HANDLE ProcessHandle;
		PVOID BaseAddress;
		PVOID Buffer;
		SIZE_T BufferSize;
		PSIZE_T NumberOfBytesRead;
	} input { hProcess, BaseAddress, Buffer, BufferSize, NumberOfBytes };

	IO_STATUS_BLOCK ioStatusBlock{ 0 };

	NtDeviceIoControlFile(m_hDriver, NULL, NULL, NULL, &ioStatusBlock, MM_READVIRTUALMEMORY, &input, sizeof(input), nullptr, 0UL);
}

bool MemoryManager::search(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr,/* BOOL bIsCurrProcess, int iSearchMode,*/ std::vector<DWORD_PTR>& vRet)
{
	MEMORY_BASIC_INFORMATION	mbi;
	std::vector<MEMORY_REGION> m_vMemoryRegion;
	mbi.RegionSize = 0x400;
	DWORD dwAddress = dwStartAddr;

	MEMORY_REGION memSectorList[1000];

	int memSectorIndex = 0;
	while (VirtualQueryEx(hProcess, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < dwEndAddr) && ((dwAddress + mbi.RegionSize) > dwAddress)) {
		if ((mbi.State == MEM_COMMIT) &&
			((mbi.Protect & PAGE_GUARD) == 0) &&
			(mbi.Protect != PAGE_NOACCESS) &&
			((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE)
			)
		{
			MEMORY_REGION mData = { 0 };
			mData.dwBaseAddr = (DWORD_PTR)mbi.BaseAddress;
			mData.dwMemorySize = mbi.RegionSize;
			m_vMemoryRegion.push_back(mData);
			memSectorList[memSectorIndex] = mData;
			memSectorIndex++;
		}
		dwAddress = (DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize;
	}

	//int memSectorCount = memSectorIndex;
	memSectorIndex = 0;
	DWORD_PTR curAddr = dwStartAddr;
	while (curAddr < dwEndAddr) {
		VirtualQueryEx(hProcess, (LPCVOID)curAddr, &mbi, sizeof(mbi));
		long regionSizeOrg = mbi.RegionSize;
		long regionSize = mbi.RegionSize;
		if (regionSize > 10) {
			BYTE* pCurrMemoryData = new BYTE[regionSize];
			ZeroMemory(pCurrMemoryData, regionSize);
			readMemory((PVOID)curAddr, (PVOID*)pCurrMemoryData, regionSize);
			DWORD_PTR dwOffset = 0;
			int iOffset{ find(pCurrMemoryData, regionSize, bSearchData, nSearchSize) };
			while (iOffset != -1) {
				dwOffset += iOffset;
				vRet.push_back(dwOffset + curAddr);
				dwOffset += nSearchSize;
				iOffset = find(pCurrMemoryData + dwOffset, regionSize - dwOffset - nSearchSize, bSearchData, nSearchSize);
			}
			delete[] pCurrMemoryData;
		}
		++memSectorIndex;
		curAddr = curAddr + (DWORD_PTR)regionSizeOrg;
		continue;
	}
	return TRUE;
}


int MemoryManager::find(BYTE* buffer, int dwBufferSize, BYTE* bstr, DWORD dwStrLen) {
	if (dwBufferSize < 0) {
		return -1;
	}
	DWORD  i, j;
	for (i = 0; i < dwBufferSize; ++i)
	{
		for (j = 0; j < dwStrLen; ++j)
		{
			if (buffer[i + j] != bstr[j] && bstr[j] != '?')
				break;
		}

		if (j == dwStrLen)
			return i;
	}
	return -1;
}


FTransform MemoryManager::ftRead(DWORD base) {
	readMemory((PVOID) base, &_ft, sizeof(_ft));
	return _ft;
}

bool MemoryManager::LoadDriver()
{
	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };

	// Establishes a connection to the service control manager on the specified computer.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		std::cout << "<OpenSCManagerW> Error: " << std::dec << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Get driver path
	if (Utils::is_file_exist("kph.sys"))
	{
		// Get executable path
		wchar_t buffer_exe_path[512];
		GetModuleFileNameW(NULL, buffer_exe_path, 512);
		std::wstring exe_path(buffer_exe_path);

		// Remove executable name and append driver name
		m_DriverPath = exe_path.substr(0, exe_path.find_last_of('\\')) + L"\\kph.sys";
	}
	else
	{
		std::cout << "<LoadDriver> Can't file kph.sys driver!\n";
		goto CLEANUP;
	}

	// Trying to get the handle of the service if it exist
	hService = OpenServiceW(hSCManager, L"KPH", SERVICE_CHANGE_CONFIG | SERVICE_START | SERVICE_STOP);
	
	// The driver service has been created before.
	if (hService)
	{
		std::cout << "The driver service is already created!\n";
		m_bLoadedService = true;
	}
	// The service of the driver isn't exist so we will create a new one
	else
	{
		hService = CreateServiceW(hSCManager, L"KPH", L"KPH", SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, m_DriverPath.c_str(), NULL, NULL, NULL, NULL, NULL);

		// Can't create service for the driver
		if (hService)
		{
			std::cout << "Created driver service\n";
			m_bLoadedService = true;
		}
		else
		{
			std::cout << "<CreateServiceW> Error: " << std::dec << GetLastError() << '\n';
			goto CLEANUP;
		}
	}
	
CLEANUP:
	if (hService != NULL)
		CloseServiceHandle(hService);

	if (hSCManager != NULL)
		CloseServiceHandle(hSCManager);

	return m_bLoadedService;
}

bool MemoryManager::StartDriver()
{
	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };

	// Establishes a connection to the service control manager on the specified computer.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		std::cout << "<OpenSCManagerW> Error: " << std::dec << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Open an existing driver service
	hService = OpenServiceW(hSCManager, L"KPH", SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP);
	if (!hService)
	{
		std::cout << "<OpenServiceW> Error: " << std::dec << GetLastError() << '\n';
		goto CLEANUP;
	}

	if (StartServiceW(hService, NULL, NULL))
	{
		std::cout << "Started the driver service\n";
		m_bStartedService = true;
	}
	else
	{
		DWORD err{ GetLastError() };

		if (err == ERROR_INVALID_IMAGE_HASH)
		{
			std::cout << "<StartService> Cannot Verify Digital Signature\n";
			goto CLEANUP;
		}

		if (err == ERROR_SERVICE_ALREADY_RUNNING)
		{
			std::cout << "<StartService> The driver service is already running!\n";
			m_bStartedService = true;
			goto CLEANUP;
		}

		// Cannot create a file when that file already exists.
		if (err == 0xb7)
		{
			std::cout << "<StartService> There is a running driver service with different name\n";
			m_bUsingAnotherService = true;
			goto CLEANUP;
		}

		// There is an another error
		std::cout << "<StartService> Error: " << std::dec << err << '\n';
		goto CLEANUP;
	}

CLEANUP:
	if (hService != NULL)
		CloseServiceHandle(hService);

	if (hSCManager != NULL)
		CloseServiceHandle(hSCManager);

	return m_bStartedService || m_bUsingAnotherService;
}

void MemoryManager::StopDriver()
{
	// The driver service created by cheat is not running so we don't need to stop it
	if (m_bUsingAnotherService || !m_bStartedService)
		return;

	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };
	SERVICE_STATUS proc{ 0 };

	// Establishes a connection to the service control manager on the specified computer.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		std::cout << "<OpenSCManagerW> Error: " << std::dec << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Open an existing driver service
	hService = OpenServiceW(hSCManager, L"KPH", SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP);
	if (!hService)
	{
		std::cout << "<OpenServiceW> Error: " << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Send stop control code to driver service
	if (ControlService(hService, SERVICE_CONTROL_STOP, &proc))
	{
		std::cout << "Stopped driver service\n";
		m_bStoppedService = true;
	}
	else
		std::cout << "<ControlService> Error: " << std::dec << GetLastError() << '\n';

CLEANUP:
	if (hSCManager)
		CloseServiceHandle(hSCManager);

	if (hService)
		CloseServiceHandle(hService);
}

void MemoryManager::UnloadDriver()
{
	if (!m_bLoadedService)
		return;

	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };

	// Establishes a connection to the service control manager on the specified computer.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		std::cout << "<OpenSCManagerW> Error: " << std::dec << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Open an existing driver service
	hService = OpenServiceW(hSCManager, L"KPH", SERVICE_CHANGE_CONFIG | DELETE | SERVICE_STOP);
	if (!hService)
	{
		std::cout << "<OpenServiceW> Error: " << std::dec << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Marks driver service for deletion.
	// The driver service is not removed until all open handles to the service
	// have been closed by CloseServiceHandle + CloseHandle.
	if (DeleteService(hService))
		std::cout << "Deleted driver service\n";
	else
		std::cout << "<DeleteService> Error: " << std::dec << GetLastError() << '\n';

CLEANUP:
	if (hSCManager)
		CloseServiceHandle(hSCManager);

	if (hService)
		CloseServiceHandle(hService);

	if (m_hDriver)
		CloseHandle(m_hDriver);
}

// Enable/disable privilege routine
bool MemoryManager::SetPrivilege(LPCWSTR lpszPrivilege, BOOL bEnablePrivilege)
{
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

DWORD MemoryManager::GetPID(const std::wstring& processName)
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