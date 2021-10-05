#include "pch.h"
#include "MemoryManager.h"

MemoryManager* g_pMM = new MemoryManager();

bool MemoryManager::Init(HANDLE hTargetProcess, DWORD dwTargetProcessPID)
{
	std::cout << "[MM]\n";

	if (!LoadDriver())
		return false;

	bool bStatus = ConnectToDriver_fix("\\Device\\KProcessHacker2");

	// If not connected yet: exit. 
	if (m_hDriver && bStatus)
	{
		std::cout << "Connected to driver 0x" << std::hex << m_hDriver << ": SUCCESS" << std::endl;
	}
	else
	{
		std::cout << "Can't connect to driver" << std::endl;
		return false;
	}
	
	processHandle = hTargetProcess;
	processId = dwTargetProcessPID;

	return true;
}

bool MemoryManager::ConnectToDriver_fix(std::string m_strDeviceName)
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

	NtDeviceIoControlFile(m_hDriver, nullptr, nullptr, nullptr, &ioStatusBlock, MM_READVIRTUALMEMORY, &input, sizeof(input), nullptr, 0);
}


bool MemoryManager::search(BYTE* bSearchData, int nSearchSize, DWORD_PTR dwStartAddr, DWORD_PTR dwEndAddr, BOOL bIsCurrProcess, int iSearchMode, std::vector<DWORD_PTR>& vRet)
{
	MEMORY_BASIC_INFORMATION	mbi;
	std::vector<MEMORY_REGION> m_vMemoryRegion;
	mbi.RegionSize = 0x400;
	DWORD dwAddress = dwStartAddr;

	MEMORY_REGION memSectorList[1000];

	int memSectorIndex = 0;
	while (VirtualQueryEx(processHandle, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < dwEndAddr) && ((dwAddress + mbi.RegionSize) > dwAddress)) {
		if (
			(mbi.State == MEM_COMMIT) &&
			((mbi.Protect & PAGE_GUARD) == 0) &&
			(mbi.Protect != PAGE_NOACCESS) &&
			((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE)
			) {
			MEMORY_REGION mData = { 0 };
			mData.dwBaseAddr = (DWORD_PTR)mbi.BaseAddress;
			mData.dwMemorySize = mbi.RegionSize;
			m_vMemoryRegion.push_back(mData);
			memSectorList[memSectorIndex] = mData;
			memSectorIndex++;
		}
		dwAddress = (DWORD)mbi.BaseAddress + mbi.RegionSize;
	}

	int memSectorCount = memSectorIndex;
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
			int iOffset = find(pCurrMemoryData, regionSize, bSearchData, nSearchSize);
			while (iOffset != -1) {
				dwOffset += iOffset;
				vRet.push_back(dwOffset + curAddr);
				dwOffset += nSearchSize;
				iOffset = find(pCurrMemoryData + dwOffset, regionSize - dwOffset - nSearchSize, bSearchData, nSearchSize);
			}
			delete[] pCurrMemoryData;
		}
		memSectorIndex++;
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
	for (i = 0; i < dwBufferSize; i++) {
		for (j = 0; j < dwStrLen; j++) {
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
	if (Utils::is_file_exist("kph.sys"))
	{
		// Get exe path without process name
		wchar_t buffer_exe_path[MAX_PATH];
		GetModuleFileNameW(NULL, buffer_exe_path, sizeof(buffer_exe_path));

		std::wstring exe_path(buffer_exe_path);

		m_DriverPath = exe_path.substr(0, exe_path.find_last_of('\\') + 1) + L"\\kph.sys";
	}
	else
	{
		std::cout << "<!> Can't file kph.sys file!\n";
		return false;
	}

	// Establishes a connection to the service control manager on the specified computer.
	// Opens the specified service control manager database.
	SC_HANDLE hSCM{ OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS) };
	if (hSCM == NULL)
	{
		std::cout << "<!> Can't create SCManager\n";

		DWORD error{ GetLastError() };

		if (error == ERROR_ACCESS_DENIED)
			std::cout << "<!> The requested access was denied\n";	

		if (error == ERROR_DATABASE_DOES_NOT_EXIST)
			std::cout << "<!> The specified database does not exist\n";

		return false;
	}

	// Trying to get the handle of the service if it exist
	SC_HANDLE hService{ OpenServiceW(hSCM, L"KPH", STANDARD_RIGHTS_REQUIRED | SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS | SERVICE_START | SERVICE_STOP) };
	if (hService)
	{
		std::cout << "The driver service is already created!\n";
	}
	else
	{
		// The service of the driver isn't exist so we create new one
		hService = CreateServiceW(
			hSCM,
			L"KPH",
			L"KPH", 
			SERVICE_ALL_ACCESS | SERVICE_START | DELETE | SERVICE_STOP,
			SERVICE_KERNEL_DRIVER, 
			SERVICE_DEMAND_START, 
			SERVICE_ERROR_IGNORE,
			m_DriverPath.c_str(), 
			NULL,
			NULL, 
			NULL, 
			NULL, 
			NULL
		);

		// Can't create service for the driver
		if (hService == NULL)
		{
			DWORD err{ GetLastError() };

			if (err == ERROR_ACCESS_DENIED)
				std::cout << "<!> The handle to the SCM database does not have the SC_MANAGER_CREATE_SERVICE access right\n";

			if (err == ERROR_DUPLICATE_SERVICE_NAME)
				std::cout << "<!> The display name already exists in the service control manager database either as a service name or as another display name\n";

			if (err == ERROR_INVALID_HANDLE)
				std::cout << "<!> The handle to the specified service control manager database is invalid\n";

			if (err == ERROR_SERVICE_EXISTS)
				std::cout << "<!> The specified service already exists in this database\n";

			if (err == ERROR_SERVICE_MARKED_FOR_DELETE)
				std::cout << "<!> The specified service already exists in this database and has been marked for deletion\n";

			CloseServiceHandle(hSCM);

			return false;
		}
	}

	if (StartServiceW(hService, NULL, NULL))
	{
		std::cout << "Started the driver service!\n";
	}
	else
	{
		DWORD err{ GetLastError() };

		if (err == ERROR_INVALID_IMAGE_HASH)
		{
			std::cout << "<!> Cannot Verify Digital Signature Re-enable DSEFix\n";

			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);

			return false;
		}

		if (err == ERROR_SERVICE_ALREADY_RUNNING)
		{
			std::cout << "The driver service is already running!\n";

			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);

			return true;
		}

		// Can't start the driver service because it's disabled
		// So we need to change the service state to start by the service control manager
		if (err == ERROR_SERVICE_DISABLED)
		{
			std::cout << "Driver service is disabled\n";

			bool bStatus = ChangeServiceConfigW(
				hService,
				SERVICE_NO_CHANGE,
				SERVICE_DEMAND_START,
				SERVICE_NO_CHANGE,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL
			);

			if (bStatus)
			{
				std::cout << "Changed driver service state!\n";

				// Start the driver service again
				if (StartServiceW(hService, NULL, NULL))
				{
					std::cout << "Started the driver service!\n";
				}
				else
				{
					std::cout << "Can't start the driver service!\n";

					CloseServiceHandle(hService);
					CloseServiceHandle(hSCM);

					return false;
				}
			}
			else
			{
				std::cout << "<!> Can't change driver service state!\n";

				CloseServiceHandle(hService);
				CloseServiceHandle(hSCM);

				return false;
			}
		}

		if (err == 0xC1)
		{
			std::cout << "<!> The driver path is invalid!\n";

			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);

			return false;
		}

		std::cout << "<!> Start service error: " << err << '\n';

		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);

		return false;
	}
	
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	return true;
}