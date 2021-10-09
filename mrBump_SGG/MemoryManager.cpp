#include "pch.h"
#include "MemoryManager.h"

MemoryManager* g_pMM = new MemoryManager();

bool MemoryManager::Init()
{
	std::cout << "[MM]\n";

	if (!(LoadDriver() && StartDriver()))
		false;
	
	bool bStatus{ ConnectToDriver("\\Device\\KProcessHacker2") };
	if (m_hDriver && bStatus)
	{
		std::cout << "Connected to driver 0x" << std::hex << m_hDriver << '\n';
	}
	else
	{
		std::cout << "Can't connect to driver" << std::endl;
		return false;
	}
	
	processHandle = g_pPM->hProcess;

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
	std::cout << "Connect driver status: 0x" << std::hex << status << std::endl;
	if (status == STATUS_SUCCESS)
	{
		m_hDriver = hNtDriver;
		return true;
	}
	else
	{
		return false;
	}	
}


void MemoryManager::readMemory(PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize) {
	struct {
		HANDLE ProcessHandle;
		PVOID BaseAddress;
		PVOID Buffer;
		SIZE_T BufferSize;
		PSIZE_T NumberOfBytesRead;
	} input = { processHandle, BaseAddress, Buffer, BufferSize, NumberOfBytes };

	IO_STATUS_BLOCK ioStatusBlock;

	NtDeviceIoControlFile(m_hDriver, nullptr, nullptr, nullptr, &ioStatusBlock, MM_READVIRTUALMEMORY, &input, sizeof(input), nullptr, 0UL);
}


bool MemoryManager::search(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr,/* BOOL bIsCurrProcess, int iSearchMode,*/ std::vector<DWORD_PTR>& vRet)
{
	MEMORY_BASIC_INFORMATION	mbi;
	std::vector<MEMORY_REGION> m_vMemoryRegion;
	mbi.RegionSize = 0x400;
	DWORD dwAddress = dwStartAddr;

	MEMORY_REGION memSectorList[1000];

	int memSectorIndex = 0;
	while (VirtualQueryEx(processHandle, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < dwEndAddr) && ((dwAddress + mbi.RegionSize) > dwAddress)) {
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
		VirtualQueryEx(processHandle, (LPCVOID)curAddr, &mbi, sizeof(mbi));
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
	for (i = 0; i < dwBufferSize; i++)
	{
		for (j = 0; j < dwStrLen; j++)
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
	bool bLoad = false;
	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };

	// Establishes a connection to the service control manager on the specified computer.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		std::cout << "<OpenSCManagerW> Error: " << GetLastError() << '\n';
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
	hService = OpenServiceW(hSCManager, L"KPH", STANDARD_RIGHTS_REQUIRED | SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS | SERVICE_START | SERVICE_STOP);
	
	// The driver service has been created before.
	if (hService)
	{
		std::cout << "The driver service is already created!\n";
		bLoad = true;
	}
	// The service of the driver isn't exist so we will create a new one
	else
	{
		hService = CreateServiceW(hSCManager, L"KPH", L"KPH", SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, m_DriverPath.c_str(), NULL, NULL, NULL, NULL, NULL);

		// Can't create service for the driver
		if (hService)
		{
			bLoad = true;
			std::cout << "Created driver service\n";
		}
		else
		{
			std::cout << "<CreateServiceW> Error: " << GetLastError() << '\n';
			goto CLEANUP;
		}
	}
	
CLEANUP:
	if (hService != NULL)
		CloseServiceHandle(hService);

	if (hSCManager != NULL)
		CloseServiceHandle(hSCManager);

	return bLoad;
}

bool MemoryManager::StartDriver()
{
	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };
	bool bStart{ false };

	// Establishes a connection to the service control manager on the specified computer.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		std::cout << "<OpenSCManagerW> Error: " << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Open an existing driver service
	hService = OpenServiceW(hSCManager, L"KPH", SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP);
	if (!hService)
	{
		std::cout << "<OpenServiceW> Error: " << GetLastError() << '\n';
		goto CLEANUP;
	}

	if (StartServiceW(hService, NULL, NULL))
	{
		bStart = true;
		std::cout << "Started the driver service\n";
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
			bStart = true;
			goto CLEANUP;
		}

		// Cannot create a file when that file already exists.
		if (err == 0xb7)
		{
			std::cout << "<StartService> There is a running driver service with different name\n";
			m_bUsingAnotherDriverService = true;
			bStart = true;
			goto CLEANUP;
		}

		// There is an another error
		std::cout << "<StartService> Error: " << err << '\n';
		goto CLEANUP;
	}

CLEANUP:
	if (hService != NULL)
		CloseServiceHandle(hService);

	if (hSCManager != NULL)
		CloseServiceHandle(hSCManager);

	return bStart;
}

bool MemoryManager::StopDriver()
{
	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };
	SERVICE_STATUS proc{ 0 };
	bool bStopped{ false };

	// Establishes a connection to the service control manager on the specified computer.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		std::cout << "<OpenSCManagerW> Error: " << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Open an existing driver service
	hService = OpenServiceW(hSCManager, L"KPH", SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP);
	if (!hService)
	{
		std::cout << "<OpenServiceW> Error: " << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Sends stop control code to driver service
	if (ControlService(hService, SERVICE_CONTROL_STOP, &proc))
	{
		std::cout << "Stopped driver service\n";
		bStopped = true;
	}
	else
	{
		DWORD error{ GetLastError() };
		std::cout << "<ControlService> Error: " << error << '\n';
	}

CLEANUP:
	if (hSCManager)
		CloseServiceHandle(hSCManager);

	if (hService)
		CloseServiceHandle(hService);

	return bStopped;
}

bool MemoryManager::UnloadDriver()
{
	SC_HANDLE hSCManager{ 0 };
	SC_HANDLE hService{ 0 };
	bool bUnload{ false };

	// Establishes a connection to the service control manager on the specified computer.
	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		std::cout << "<OpenSCManagerW> Error: " << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Open an existing driver service
	hService = OpenServiceW(hSCManager, L"KPH", SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP);
	if (!hService)
	{
		std::cout << "<OpenServiceW> Error: " << GetLastError() << '\n';
		goto CLEANUP;
	}

	// Marks driver service for deletion.
	// The driver service is not removed until
	// all open handles to the service have been closed
	// by CloseServiceHandle + CloseHandle.
	if (DeleteService(hService))
	{
		std::cout << "Deleted driver service\n";
		bUnload = true;
	}
	else
	{
		std::cout << "<DeleteService> Error: " << GetLastError << '\n';
	}

CLEANUP:
	if (hSCManager)
		CloseServiceHandle(hSCManager);

	if (hService)
		CloseServiceHandle(hService);

	if (m_hDriver)
		CloseHandle(m_hDriver);

	return bUnload;
}